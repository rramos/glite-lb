#include <stdsoap2.h>

#include "glite/security/glite_gsplugin.h"

#include "glite/lb/context-int.h"
#include "glite/lb/consumer.h"

#include "jobstat.h"
#include "query.h"
#include "bk_ws_H.h"
#include "get_events.h"
#include "ws_fault.h"
#include "ws_typeref.h"


int edgwll2__GetVersion(
        struct soap						   *soap,
		struct edgwll2__GetVersionResponse *out)
{
	out->version = strdup(VERSION);

	return SOAP_OK;
}

int edgwll2__JobStatus(
        struct soap						   *soap,
        char							   *jobid,
        struct edgwll__JobStatFlags		   *flags,
        struct edgwll2__JobStatusResponse  *out)
{
	edg_wll_Context		ctx = (edg_wll_Context) glite_gsplugin_get_udata(soap);
	edg_wlc_JobId		j;
	edg_wll_JobStat		s;


	if ( edg_wlc_JobIdParse(jobid, &j) )
	{
		edg_wll_SetError(ctx, EINVAL, jobid);
		edg_wll_ErrToFault(ctx, soap);
		return SOAP_FAULT;
	}

	if ( edg_wll_JobStatus(ctx, j, 0, &s) )
	{
		edg_wll_ErrToFault(ctx, soap);
		return SOAP_FAULT;
	}

	edg_wll_StatusToSoap(soap, &s, &(out->status));

	return SOAP_OK;
}

int edgwll2__QueryJobs(
	struct soap						   *soap,
	struct edgwll__QueryConditions	   *conditions,
	struct edgwll__JobStatFlags		   *flags,
	struct edgwll2__QueryJobsResponse  *out)
{
	edg_wll_Context		ctx = (edg_wll_Context) glite_gsplugin_get_udata(soap);
	edg_wlc_JobId	   *jobsOut = NULL;
	edg_wll_JobStat	   *statesOut = NULL;
	edg_wll_QueryRec  **qr = NULL;
	int					fl,
						i, j,
						ret = SOAP_FAULT;


	out->states = soap_malloc(soap, sizeof(*out->states));
	out->jobs = soap_malloc(soap, sizeof(*out->jobs));
	if ( !out->states || !out->jobs ) goto cleanup;
	memset(out->states, 0, sizeof(*(out->states)));
	memset(out->jobs, 0, sizeof(*(out->jobs)));

	edg_wll_ResetError(ctx);
	edg_wll_SoapToJobStatFlags(flags, &fl);
	if ( edg_wll_SoapToQueryCondsExt(conditions, &qr) ) {
		edg_wll_SetError(ctx, ENOMEM, "Couldn't create internal structures");
		goto cleanup;
	}
	if ( edg_wll_QueryJobsServer(ctx, (const edg_wll_QueryRec **)qr, fl,
					&jobsOut, &statesOut) ) goto cleanup;
	if ( edg_wll_JobsQueryResToSoap(soap, jobsOut, statesOut, out) ) goto cleanup;
	ret = SOAP_OK;

cleanup:
	if ( qr ) {
		for ( i = 0; qr[i]; i++ ) {
			for ( j = 0; qr[i][j].attr; j++ )
				edg_wll_QueryRecFree(&qr[i][j]);
			free(qr[i]);
		}
		free(qr);
	}
	if ( jobsOut ) {
		for ( i = 0; jobsOut[i]; i++ )
			edg_wlc_JobIdFree(jobsOut[i]);
		free(jobsOut);
	}
	if ( statesOut ) {
		for ( i = 0; statesOut[i].state; i++ )
			edg_wll_FreeStatus(&statesOut[i]);
		free(statesOut);
	}
	if ( ret == SOAP_FAULT ) edg_wll_ErrToFault(ctx, soap);

	return ret;
}

int edgwll2__UserJobs(
	struct soap						   *soap,
	struct edgwll2__UserJobsResponse   *out)
{
	out->jobs = NULL;
	out->states = NULL;

	return SOAP_OK;
}


/*
int edgwll2__QueryEvents(
	struct soap				*soap,
	struct edgwll__QueryConditions		*jc,
	struct edgwll__QueryConditions		*ec,
	struct edgwll2__QueryEventsResponse	*out)
{
	edg_wll_Context		ctx = (edg_wll_Context) glite_gsplugin_get_udata(soap);
	edg_wll_Event		*eventsOut = NULL;
	edg_wll_QueryRec	**qrj = NULL,
				**qre = NULL;

	int			fl,
				i, j,
				ret = SOAP_FAULT;


x	out->states = soap_malloc(soap, sizeof(*out->states));
x	out->jobs = soap_malloc(soap, sizeof(*out->jobs));
x	if ( !out->states || !out->jobs ) goto cleanup;
x	memset(out->states, 0, sizeof(*(out->states)));
x	memset(out->jobs, 0, sizeof(*(out->jobs)));

	edg_wll_ResetError(ctx);
	if ( edg_wll_SoapToQueryCondsExt(jc, &qrj) ) {
		edg_wll_SetError(ctx, ENOMEM, "Couldn't create internal structures");
		goto cleanup;
	}
	if ( edg_wll_SoapToQueryCondsExt(ec, &qre) ) {
		edg_wll_SetError(ctx, ENOMEM, "Couldn't create internal structures");
		goto cleanup;
	}

	if ( edg_wll_QueryEventsServer( ctx, ctx->noAuth, (const edg_wll_QueryRec **) jc, 
					(const edg_wll_QueryRec **) ec, &eventsOut) )
		goto cleanup;

	if ( edg_wll_EventsQueryResToSoap(soap, eventsOut, out) ) goto cleanup;
	ret = SOAP_OK;

cleanup:
	if ( qrj ) {
		for ( i = 0; qrj[i]; i++ ) {
			for ( j = 0; qrj[i][j].attr; j++ )
				edg_wll_QueryRecFree(&qrj[i][j]);
			free(qrj[i]);
		}
		free(qrj);
	}

	if ( qre ) {
		for ( i = 0; qre[i]; i++ ) {
			for ( j = 0; qre[i][j].attr; j++ )
				edg_wll_QueryRecFree(&qre[i][j]);
			free(qre[i]);
		}
		free(qr);
	}
	if ( eventsOut ) {
		for ( i = 0; eventsOut[i].type; i++ )
			edg_wll_FreeEvent(&eventsOut[i]);
		free(eventssOut);
	}
	if ( ret == SOAP_FAULT ) edg_wll_ErrToFault(ctx, soap);

	return ret;
}
*/
