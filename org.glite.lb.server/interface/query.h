#ifndef GLITE_LB_QUERY_H
#define GLITE_LB_QUERY_H

#ident "$Header$"

#include "glite/jobid/cjobid.h"
#include "glite/lb/context.h"
#include "glite/lb/events.h"
#include "glite/lb/jobstat.h"
#include "glite/lb/query_rec.h"

int convert_event_head(edg_wll_Context,char **,edg_wll_Event *);
int check_strict_jobid(edg_wll_Context, const edg_wlc_JobId);
int match_status(edg_wll_Context, const edg_wll_JobStat *stat,const edg_wll_QueryRec **conditions);

#define NOTIF_ALL_JOBS	"all_jobs"

#endif /* GLITE_LB_QUERY_H */
