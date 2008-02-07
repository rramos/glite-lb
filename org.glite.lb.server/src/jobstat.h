#ifndef GLITE_LB_LBS_JOBSTAT_H
#define GLITE_LB_LBS_JOBSTAT_H

#ident "$Header$"

#include "glite/lb/jobstat.h"

/*
 * Internal representation of job state
 * (includes edg_wll_JobStat API structure)
 */

/* convention: revision X.XX - DESCRIPTION 			*/
/* where X.XX is version from indent + 1 (version after commit) */
/* and DESCRIPTION is short hit why version changed		*/

#define INTSTAT_VERSION "revision 1.31 - proxy merge"


// Internal error codes 

#define RET_FAIL        0
#define RET_OK          1
#define RET_FATAL       RET_FAIL
#define RET_SOON        2
#define RET_LATE        3
#define RET_BADSEQ      4
#define RET_SUSPECT     5
#define RET_IGNORE      6
#define RET_BADBRANCH   7
#define RET_GOODBRANCH  8
#define RET_TOOOLD      9
#define RET_UNREG	10
#define RET_INTERNAL    100


// shallow resubmission container - holds state of each branch
// (useful when state restore is needed after ReallyRunning event)
//
typedef struct _branch_state {
	int	branch;
	char	*destination;
	char	*ce_node;
	char	*jdl;
	/*!! if adding new field, modify also free_branch_state() */
} branch_state;


typedef struct _intJobStat {
		edg_wll_JobStat	pub;
		int		resubmit_type;
		char		*last_seqcode;
		char		*last_cancel_seqcode;
		char		*branch_tag_seqcode;		
		char		*last_branch_seqcode;
		char		*deep_resubmit_seqcode;
		branch_state	*branch_states;		// branch zero terminated array

		struct timeval	last_pbs_event_timestamp;
		int		pbs_reruning;		// true if rerun event arrived

		/*!! if adding new field, modify also destroy_intJobStat_extension() */
	} intJobStat;

typedef enum _edg_wll_PBSEventSource {
	EDG_WLL_PBS_EVENT_SOURCE_UNDEF = 0,
	EDG_WLL_PBS_EVENT_SOURCE_SCHEDULER,
	EDG_WLL_PBS_EVENT_SOURCE_SERVER,
	EDG_WLL_PBS_EVENT_SOURCE_MOM,
	EDG_WLL_PBS_EVENT_SOURCE_ACCOUNTING,
	EDG_WLL_PBS_EVENT_SOURCE__LAST
} edg_wll_PBSEventSource;

typedef enum _edg_wll_CondorEventSource {
	EDG_WLL_CONDOR_EVENT_SOURCE_UNDEF = 0,
	EDG_WLL_CONDOR_EVENT_SOURCE_COLLECTOR,
	EDG_WLL_CONDOR_EVENT_SOURCE_MASTER,
	EDG_WLL_CONDOR_EVENT_SOURCE_MATCH,
	EDG_WLL_CONDOR_EVENT_SOURCE_NEGOTIATOR,
	EDG_WLL_CONDOR_EVENT_SOURCE_SCHED,
	EDG_WLL_CONDOR_EVENT_SOURCE_SHADOW,
	EDG_WLL_CONDOR_EVENT_SOURCE_STARTER,
	EDG_WLL_CONDOR_EVENT_SOURCE_START,
	EDG_WLL_CONDOR_EVENT_SOURCE_JOBQUEUE,
	EDG_WLL_CONDOR_EVENT_SOURCE__LAST
} edg_wll_CondorEventSource;

typedef enum _subjobClassCodes {
	SUBJOB_CLASS_UNDEF = 0,
	SUBJOB_CLASS_RUNNING,
	SUBJOB_CLASS_DONE,
	SUBJOB_CLASS_ABORTED,
	SUBJOB_CLASS_CLEARED,
	SUBJOB_CLASS_REST
} subjobClassCodes;

int edg_wll_JobStatusServer(edg_wll_Context, glite_jobid_const_t, int, edg_wll_JobStat *);

void destroy_intJobStat(intJobStat *);
void destroy_intJobStat_extension(intJobStat *p);


int edg_wll_intJobStatus( edg_wll_Context, glite_jobid_const_t, int, intJobStat *, int);
edg_wll_ErrorCode edg_wll_StoreIntState(edg_wll_Context, intJobStat *, int);
edg_wll_ErrorCode edg_wll_StoreIntStateEmbryonic(edg_wll_Context, edg_wlc_JobId, char *icnames, char *values, glite_lbu_bufInsert *bi);
edg_wll_ErrorCode edg_wll_LoadIntState(edg_wll_Context , edg_wlc_JobId , int, intJobStat **);

edg_wll_ErrorCode edg_wll_StepIntState(edg_wll_Context ctx, edg_wlc_JobId job, edg_wll_Event *e, int seq, edg_wll_JobStat *stat_out);
edg_wll_ErrorCode edg_wll_StepIntStateParent(edg_wll_Context,edg_wlc_JobId,edg_wll_Event *,int,intJobStat *,edg_wll_JobStat *);



intJobStat* dec_intJobStat(char *, char **);
char *enc_intJobStat(char *, intJobStat* );

void write2rgma_status(edg_wll_JobStat *);
void write2rgma_chgstatus(edg_wll_JobStat *, char *);
char* write2rgma_statline(edg_wll_JobStat *);

int before_deep_resubmission(const char *, const char *);
int same_branch(const char *, const char *);
int component_seqcode(const char *a, edg_wll_Source index);
char * set_component_seqcode(char *s,edg_wll_Source index,int val);
int processEvent(intJobStat *, edg_wll_Event *, int, int, char **);
int processEvent_PBS(intJobStat *, edg_wll_Event *, int, int, char **);
int processEvent_Condor(intJobStat *, edg_wll_Event *, int, int, char **);

int add_stringlist(char ***, const char *);
int edg_wll_compare_seq(const char *, const char *);
int edg_wll_compare_pbs_seq(const char *,const char *);
#define edg_wll_compare_condor_seq edg_wll_compare_pbs_seq
edg_wll_PBSEventSource get_pbs_event_source(const char *pbs_seq_num);
edg_wll_CondorEventSource get_condor_event_source(const char *pbs_seq_num);

void init_intJobStat(intJobStat *p);

edg_wll_ErrorCode edg_wll_GetSubjobHistogram(edg_wll_Context, edg_wlc_JobId parent_jobid, int *hist);
edg_wll_ErrorCode edg_wll_StoreSubjobHistogram(edg_wll_Context, edg_wlc_JobId parent_jobid, intJobStat *ijs);

#endif /* GLITE_LB_LBS_JOBSTAT_H*/
