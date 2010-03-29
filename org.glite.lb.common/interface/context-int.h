#ifndef GLITE_LB_CONTEXT_INT_H
#define GLITE_LB_CONTEXT_INT_H

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


#include "glite/security/glite_gss.h"

#ifdef BUILDING_LB_COMMON
#include "jobstat.h"
#include "query_rec.h"
#include "lb_plain_io.h"
#include "authz.h"
#include "connpool.h"
#include "padstruct.h"
#else
#include "glite/lb/jobstat.h"
#include "glite/lb/query_rec.h"
#include "glite/lb/lb_plain_io.h"
#include "glite/lb/authz.h"
#include "glite/lb/connpool.h"
#include "glite/lb/padstruct.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define EDG_WLL_SEQ_NULL 	"UI=000000:NS=0000000000:WM=000000:BH=0000000000:JSS=000000:LM=000000:LRMS=000000:APP=000000:LBS=000000"
#define EDG_WLL_SEQ_PBS_NULL 	"TIMESTAMP=00000000000000:POS=0000000000:EV.CODE=000:SRC=?" 
#define EDG_WLL_SEQ_CONDOR_NULL EDG_WLL_SEQ_PBS_NULL
#define EDG_WLL_SEQ_SIZE      	(sizeof(EDG_WLL_SEQ_NULL))
#define EDG_WLL_SEQ_PBS_SIZE	(sizeof(EDG_WLL_SEQ_PBS_NULL))
#define EDG_WLL_SEQ_CONDOR_SIZE EDG_WLL_SEQ_PBS_SIZE
#define EDG_WLL_SEQ_FORMAT_PRINTF	"UI=%06d:NS=%010d:WM=%06d:BH=%010d:JSS=%06d:LM=%06d:LRMS=%06d:APP=%06d:LBS=%06d"
#define EDG_WLL_SEQ_FORMAT_SCANF	"UI=%d:NS=%d:WM=%d:BH=%d:JSS=%d:LM=%d:LRMS=%d:APP=%d:LBS=%d"
#define EDG_WLL_SEQ_FORMAT_NUMBER	9

typedef struct _edg_wll_SeqCode {
	unsigned int	type;				/* seq code type    */
	unsigned int	c[EDG_WLL_SOURCE__LAST];	/* glite seq. code  */
	char		pbs[EDG_WLL_SEQ_PBS_SIZE];	/* PBS seq. code    */
				/* 0-24 TIMESTAMP=YYYYMMDDHHMMSS: */
				/* 25-39 POS=%010u: */
				/* 40-51 EV.CODE=%03d: */
				/* 53-56 SRC=%c */
	char		condor[EDG_WLL_SEQ_CONDOR_SIZE];			
} edg_wll_SeqCode;

/* non-gsi one-element analogy of connPool for L&B Proxy server */
glite_lb_padded_struct(_edg_wll_ConnProxy,12,
	edg_wll_PlainConnection	conn;
	char   *buf;
	size_t  bufSize;
	size_t  bufUse;
)
typedef struct _edg_wll_ConnProxy  edg_wll_ConnProxy;


/* !!! if adding something malloc-able, update edg_wll_FreeContext too !!! */
glite_lb_padded_struct(_edg_wll_Context,150,
/// XXX: branch value: glite_lb_padded_struct(_edg_wll_Context,120,
/* Error handling */
	int		errCode;	/* recent error code */
	char 		*errDesc;	/* additional error description */

/* server part */

	void		*dbctx;
	int		dbcaps;
	edg_wll_Connections	*connections;
	edg_wll_Connections	*connNotif;		/* holds connections from notif-interlogger */
	edg_wll_ConnProxy	*connProxy;		/* holds one plain connection */

	edg_wll_QueryRec	**job_index;
	void		*job_index_cols;
	
	time_t		peerProxyValidity;
	char		*peerName;
	char		*serverIdentity;		/* DN of server certificate or "anonymous" */
	edg_wll_VomsGroups	vomsGroups;
	int		allowAnonymous;
	int		noAuth;		/* if set, you can obtain info about events 	*/
					/* and jobs not belonging to you 		*/
	int		noIndex;	/* don't enforce indices */
	int		rgma_export;
	int		strict_locking;	/* lock jobs for storing event too */

	int             is_V21;         /* true if old (V21) request arrived */
	int		isProxy;	/* LBProxy */
	int		serverRunning;	/* bkserver runs server service */
	int		proxyRunning;	/* bkserver runs proxy service */

/* server limits */
	int		softLimit;
	int		hardJobsLimit;
	int		hardEventsLimit;

	time_t		notifDuration;
	
/* purge and dump files storage */
	char		*dumpStorage;
	char		*purgeStorage;

/* maildir location */
	char	*jpreg_dir;

/* flag for function store_event
 * if set then event are loaded from dump file
 */
	int		event_load;

/* address and port we are listening at */
	char		*srvName;
	unsigned int	srvPort;
	
/* other client stuff */
	int		notifSock;		/* default client socket	*
					   	 * for receiving notifications  */   
	
/* user settable parameters */
	char		*p_host;
	edg_wll_Source	p_source;
	char		*p_instance;
	enum edg_wll_Level	p_level;
	char		*p_destination;
	int		p_dest_port;
	char		*p_lbproxy_store_sock;
	char		*p_lbproxy_serve_sock;
	char		*p_user_lbproxy;
	struct timeval	p_log_timeout;
	struct timeval	p_sync_timeout;
	struct timeval	p_query_timeout;
	struct timeval	p_notif_timeout;
	struct timeval	p_tmp_timeout;
	char		*p_query_server;
	int		p_query_server_port;
	int		p_query_server_override;
	int		p_query_events_limit;
	int		p_query_jobs_limit;
	edg_wll_QueryResults	p_query_results;
	char		*p_notif_server;
	int		p_notif_server_port;
	char		*p_proxy_filename;
	char		*p_cert_filename;
	char		*p_key_filename;
	time_t		purge_timeout[EDG_WLL_NUMBER_OF_STATCODES];
/* producer part */
	glite_jobid_t	p_jobid;
	edg_wll_SeqCode	p_seqcode;
	int		count_statistics;

	int		greyjobs;
	char		**fqans; /* null-terminated list of peer's VOMS FQANs */
	edg_wll_QueryRec	**notif_index;
	void		*notif_index_cols;
	time_t		notifDurationMax;

	char		**super_users;

	time_t          rssTime;
	_edg_wll_authz_policy	authz_policy;
)

/* to be used internally: set, update and and clear the error information in 
 * context, the desc string (if provided) is strdup()-ed
 *
 * all return the error code for convenience (usage in return statement) */


extern int edg_wll_SetError(
	edg_wll_Context,	/* context */
	int,			/* error code */
	const char *		/* error description */
);

extern int edg_wll_SetErrorGss(
	edg_wll_Context,
	const char *,
	edg_wll_GssStatus *
);

/** update errDesc and errCode */
extern int edg_wll_UpdateError(
	edg_wll_Context,	/* context */
	int,			/* error code */
	const char *		/* error description */
);

/** set errCode to 0, free errDesc */
extern int edg_wll_ResetError(edg_wll_Context);

/** retrieve standard error text wrt. code
 * !! does not allocate memory */
extern const char *edg_wll_GetErrorText(int);

extern int edg_wll_ContextReopen(edg_wll_Context);

extern int edg_wll_SetSequenceCode(edg_wll_Context, const char *, int);
extern int edg_wll_IncSequenceCode(edg_wll_Context ctx);

extern void edg_wll_FreeParams(edg_wll_Context context);

#ifdef __cplusplus
}
#endif

#endif /* GLITE_LB_CONTEXT_INT_H */