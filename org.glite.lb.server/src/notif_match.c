#ident "$Header$"
/*
Copyright (c) Members of the EGEE Collaboration. 2004-2010.
See http://www.eu-egee.org/partners for details on the copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>

#include "glite/lb/context-int.h"
#include "glite/lbu/trio.h"
#include "glite/lbu/log.h"

#include "lb_authz.h"
#include "lb_xml_parse.h"
#include "query.h"
#include "il_notification.h"
#include "db_supp.h"
#include "index.h"
#include "authz_policy.h"
#include "get_events.h"
#include "server_stats.h"
#include "jobstat.h"

static int notif_match_conditions(edg_wll_Context,const edg_wll_JobStat *,const edg_wll_JobStat *,const char *, int flags);

int edg_wll_NotifExpired(edg_wll_Context,const char *);

int edg_wll_NotifMatch(edg_wll_Context ctx, const edg_wll_JobStat *oldstat, const edg_wll_JobStat *stat)
{
	edg_wll_NotifId		nid = NULL;
	char	*jobq,*ju = NULL,*jobc[6];
	glite_lbu_Statement	jobs = NULL;
	int	ret,flags;
	size_t i;
	time_t	expires,now = time(NULL);
	
	char *cond_where = NULL;
	char *cond_and_where = NULL;
	edg_wll_JobStat newstat = *stat; // shallow copy
	edg_wll_JobStat* statcopy[2][2][2];
	int m, a, h;
	edg_wll_Event* events = NULL;

	memset(statcopy, 0, sizeof(statcopy));

	edg_wll_ResetError(ctx);

	if (ctx->notif_index) {
		edg_wll_IColumnRec *notif_index_cols = ctx->notif_index_cols;

		for (i=0; notif_index_cols[i].qrec.attr; i++) {
			char	*val = NULL;

			if (notif_index_cols[i].qrec.attr != EDG_WLL_QUERY_ATTR_JDL_ATTR) {
				val = edg_wll_StatToSQL(stat,notif_index_cols[i].qrec.attr);
				assert(val != (char *) -1);
			}
			else { // Special Treatment for JDL attributes
				val = edg_wll_JDLStatToSQL(stat,notif_index_cols[i].qrec);
			} 

			if (val) {
				char	*aux;
				if (!cond_where) cond_where = strdup("");
				trio_asprintf(&aux, "%s or %s = %s",cond_where,
						notif_index_cols[i].colname,val);
				free(cond_where);
				cond_where = aux;
				free(val);
			}
			else if (notif_index_cols[i].qrec.attr == EDG_WLL_QUERY_ATTR_JDL_ATTR) {
				char	*aux;
				if (!cond_and_where) cond_and_where = strdup("");
				trio_asprintf(&aux, "%s AND %s is NULL",cond_and_where,
						notif_index_cols[i].colname);
				free(cond_and_where);
				cond_and_where = aux;
				free(val);
			}
		}
	}

	if ( (ret = edg_wll_NotifIdCreate(ctx->srvName, ctx->srvPort, &nid)) )
	{
		edg_wll_SetError(ctx, ret, "edg_wll_NotifMatch()");
		goto err;
	}

	trio_asprintf(&jobq,
		"select distinct n.notifid,n.destination,n.valid,u.cert_subj,n.conditions,n.flags "
		"from notif_jobs j,users u,notif_registrations n "
		"where j.notifid=n.notifid and n.userid=u.userid "
		"   and (j.jobid = '%|Ss' or j.jobid = '%|Ss' %s) %s",
		ju = edg_wlc_JobIdGetUnique(stat->jobId),NOTIF_ALL_JOBS,cond_where ? cond_where : "",cond_and_where ? cond_and_where : "");

	free(ju); ju = NULL;
	free(cond_where); cond_where = NULL;
	free(cond_and_where); cond_and_where = NULL;
	glite_common_log_msg(LOG_CATEGORY_LB_SERVER_DB, LOG_PRIORITY_DEBUG, jobq);

	if (edg_wll_ExecSQL(ctx,jobq,&jobs) < 0) goto err;

	statcopy[0][0][0]=&newstat;
			
	while ((ret = edg_wll_FetchRow(ctx,jobs,sizeof(jobc)/sizeof(jobc[0]),NULL,jobc)) > 0) {
		flags = atoi(jobc[5]);
		if (now > (expires = glite_lbu_StrToTime(jobc[2]))) {
			edg_wll_NotifExpired(ctx,jobc[0]);
			glite_common_log(LOG_CATEGORY_LB_SERVER, LOG_PRIORITY_DEBUG, "[%d] NOTIFY:%s expired at %s UTC", 
				getpid(),jobc[0],asctime(gmtime(&expires)));
		}
		else if (notif_match_conditions(ctx,oldstat,&newstat,jobc[4],flags))
		{
			char	*errt, *errd;
			char	*dest;
			int	authz_flags;

		        ret = edg_wll_NotifCheckAuthz(ctx, &newstat, flags, jobc[3], &authz_flags);
		        if (ret != 1) {     
		                char *ju;
                		glite_common_log(LOG_CATEGORY_CONTROL, LOG_PRIORITY_INFO,
		                                 "[%d] authorization failed when sending notification for job %s",
                                		 getpid(),
                		                 ju = glite_jobid_getUnique(newstat.jobId));
		                free(ju);
				for (i=0; i<sizeof(jobc)/sizeof(jobc[0]); i++) free(jobc[i]);
				continue;
        		}

			m = (authz_flags & STATUS_FOR_MONITORING) == STATUS_FOR_MONITORING;
			a = (authz_flags & READ_ANONYMIZED) == READ_ANONYMIZED;
			h = (flags & EDG_WLL_NOTIF_HISTORY) == EDG_WLL_NOTIF_HISTORY;

			if (!statcopy[m][a][h]) {
				statcopy[m][a][h]=(edg_wll_JobStat*)calloc(sizeof(edg_wll_JobStat),1);
				edg_wll_CpyStatus(&newstat, statcopy[m][a][h]); 

				glite_common_log(LOG_CATEGORY_LB_SERVER, LOG_PRIORITY_DEBUG, "NOTIFY: Populating status copy for flag combination [%d][%d][%d]", m, a, h);

			        if (authz_flags & STATUS_FOR_MONITORING)
			                blacken_fields(statcopy[m][a][h], authz_flags);
			        if (authz_flags & READ_ANONYMIZED)
			                anonymize_stat(ctx, statcopy[m][a][h]);

				if (flags & EDG_WLL_NOTIF_HISTORY) {
					if (!events) {
						glite_common_log(LOG_CATEGORY_LB_SERVER, LOG_PRIORITY_DEBUG, "NOTIFY: extracting event history for job %s", jobc[0]);
						if (!(events = fetch_history(ctx, statcopy[m][a][h]))) {
							edg_wll_Error(ctx, &errt, &errd);
							glite_common_log(LOG_CATEGORY_LB_SERVER, LOG_PRIORITY_ERROR, "NOTIFY: query events for %s failed, %s: %s", jobc[0], errt, errd);
							free(errt);
							free(errd);
							edg_wll_ResetError(ctx);
							continue;
						}
					}
					collate_history(ctx, statcopy[m][a][h], events, authz_flags);
				}
			}

			glite_common_log(LOG_CATEGORY_LB_SERVER, LOG_PRIORITY_DEBUG, "NOTIFY: %s, job %s", jobc[0], ju = edg_wlc_JobIdGetUnique(newstat.jobId));
			free(ju); ju = NULL;

			dest = jobc[1];
			
			if (   edg_wll_NotifIdSetUnique(&nid, jobc[0]) )
			{
				free(dest);
				goto err;
			}
			/* XXX: only temporary hack!!!
			 */
			ctx->p_instance = strdup("");
			if ( edg_wll_NotifJobStatus(ctx, nid, dest, jobc[3], atoi(jobc[5]), expires, *statcopy[m][a][h], authz_flags) )
			{
				for (i=0; i<sizeof(jobc)/sizeof(jobc[0]); i++) free(jobc[i]);
				goto err;
			}
		}

		if (! strncmp(jobc[1], "x-msg", 5))
                        edg_wll_ServerStatisticsIncrement(ctx, SERVER_STATS_NOTIF_MSG_SENT);
                else
                        edg_wll_ServerStatisticsIncrement(ctx, SERVER_STATS_NOTIF_LEGACY_SENT);
		
		for (i=0; i<sizeof(jobc)/sizeof(jobc[0]); i++) free(jobc[i]);
	}
	if (ret < 0) goto err;
	
err:
	for ( m = 0; m < 2 ; m++ )
		for ( a = 0; a < 2 ; a++ )
			for ( h = 0; h < 2 ; h++ )
				if (statcopy[m][a][h] && (m || a || h)) {
					edg_wll_FreeStatus(statcopy[m][a][h]);
					free(statcopy[m][a][h]);
				}
	free(ctx->p_instance); ctx->p_instance = NULL;
	if ( nid ) edg_wll_NotifIdFree(nid);
	free(jobq);
	glite_lbu_FreeStmt(&jobs);
	free(cond_where);
	free(cond_and_where);
	return edg_wll_Error(ctx,NULL,NULL);
}

int edg_wll_NotifExpired(edg_wll_Context ctx,const char *notif)
{
	char	*dn = NULL,*dj = NULL;

	trio_asprintf(&dn,"delete from notif_registrations where notifid='%|Ss'",notif);
	trio_asprintf(&dj,"delete from notif_jobs where notifid='%|Ss'",notif);
	glite_common_log_msg(LOG_CATEGORY_LB_SERVER_DB, LOG_PRIORITY_DEBUG, dn);
	glite_common_log_msg(LOG_CATEGORY_LB_SERVER_DB, LOG_PRIORITY_DEBUG, dj);

	if (edg_wll_ExecSQL(ctx,dn,NULL) < 0 ||
		edg_wll_ExecSQL(ctx,dj,NULL) < 0)
	{
		char	*et,*ed;
		edg_wll_Error(ctx,&et,&ed);

		glite_common_log(LOG_CATEGORY_CONTROL, LOG_PRIORITY_WARN, 
			"delete notification %s: %s (%s)", notif, et, ed);		
		free(et); free(ed);
	}

	free(dn);
	free(dj);
	return edg_wll_ResetError(ctx);
}


static int notif_match_conditions(edg_wll_Context ctx,const edg_wll_JobStat *oldstat, const edg_wll_JobStat *stat,const char *cond, int flags)
{
	edg_wll_QueryRec	**c,**p;
	int			match = 0,i;

	if (!cond) return 1;

	if (!(flags & EDG_WLL_NOTIF_TERMINAL_STATES) || // Either there is no terminal flag
		((flags & EDG_WLL_NOTIF_TERMINAL_STATES) && (EDG_WLL_JOB_TERMINAL_STATE[stat->state]) && // Or the new state is terminal
			((stat->state!=EDG_WLL_JOB_PURGED) || !(EDG_WLL_JOB_TERMINAL_STATE[oldstat->state])))) { // And in case it is purged, it was not in a terminal state before

		if (parseJobQueryRec(ctx,cond,strlen(cond),&c)) {
			glite_common_log(LOG_CATEGORY_CONTROL, LOG_PRIORITY_ERROR, 
				"notif_match_conditions(): parseJobQueryRec failed");

			return 1;
		}

		match = match_status(ctx,oldstat,stat,(const edg_wll_QueryRec **) c);
		if ( c )
		{
			for (p = c; *p; p++) {
				for (i=0; (*p)[i].attr; i++)
					edg_wll_QueryRecFree((*p)+i);
				free(*p);
			}
			free(c);
		}
	}
	return match;
}

/* FIXME: does not favour any VOMS information in ACL
 * effective VOMS groups of the recipient are not available here, should be 
 * probably stored along with the registration.
 */
int edg_wll_NotifCheckAuthz(edg_wll_Context ctx,edg_wll_JobStat *stat,
			    int flags,const char *recip, int *authz_flags)
{
	int		ret;
	struct _edg_wll_GssPrincipal_data princ;
	edg_wll_Acl	acl = NULL;

	*authz_flags = 0;

	memset(&princ, 0, sizeof(princ));
	princ.name = (char *)recip;

	if (stat->acl) {
		acl =  calloc(1,sizeof *acl);
		ret = edg_wll_DecodeACL(stat->acl,&acl->value);
		if (ret) {
			free(acl);
			acl = NULL;
		}
	}

	ret = check_jobstat_authz(ctx, stat, flags, acl, &princ, authz_flags);
	if (acl)
		edg_wll_FreeAcl(acl);
	if (ret != 1)
		return ret;

	return ret;
}

