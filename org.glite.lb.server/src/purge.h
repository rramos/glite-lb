#ifndef __EDG_WORKLOAD_LOGGING_COMMON_PURGE_H__
#define __EDG_WORKLOAD_LOGGING_COMMON_PURGE_H__

/** Server side implementation
 *  besides output to the SSL stream (in the context) it may produce
 *  the server-side dump files
 */
int edg_wll_PurgeServer(
	edg_wll_Context ctx,
	const edg_wll_PurgeRequest *request
);

/** LB Proxy purge implementation
 *	it gives no output - purge only one job from LB Proxy DB
 */
int edg_wll_PurgeServerProxy(
	edg_wll_Context ctx,
	edg_wlc_JobId job
);

#define		FILE_TYPE_ANY		""
#define		FILE_TYPE_PURGE		"purge"
#define		FILE_TYPE_DUMP		"dump"
#define		FILE_TYPE_LOAD		"load"

extern int edg_wll_CreateTmpFileStorage(
	edg_wll_Context ctx,
	char *prefix,
	char **fname
);

extern int edg_wll_CreateFileStorageFromTmp(
	edg_wll_Context ctx,
	char *tmp_type,
	char *file_type,
	char **fname
);

extern int edg_wll_CreateFileStorage(
	edg_wll_Context ctx,
	char *file_type,
	char *prefix,
	char **fname
);

#define edg_wll_CreateTmpDumpFile(ctx, f)	edg_wll_CreateTmpFileStorage(ctx,ctx->dumpStorage,f)
#define edg_wll_CreateTmpPurgeFile(ctx, f)	edg_wll_CreateTmpFileStorage(ctx,ctx->purgeStorage,f)

#define edg_wll_CreateDumpFileFromTmp(ctx, f, f2)	\
			edg_wll_CreateFileStorageFromTmp(ctx, f, FILE_TYPE_DUMP, f2)
#define edg_wll_CreatePurgeFileFromTmp(ctx, f, f2)	\
			edg_wll_CreateFileStorageFromTmp(ctx, f, FILE_TYPE_PURGE, f2)

#define edg_wll_CreateDumpFile(ctx, f)		edg_wll_CreateFileStorage(ctx,FILE_TYPE_DUMP,NULL,f)
#define edg_wll_CreatePurgeFile(ctx, f)		edg_wll_CreateFileStorage(ctx,FILE_TYPE_PURGE,NULL,f)

#endif
