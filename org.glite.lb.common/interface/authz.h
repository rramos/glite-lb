#ifndef _LB_AUTHZ_H
#define _LB_AUTHZ_H

#ident "$Header$"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _edg_wll_VomsGroup {
	char *vo;
	char *name;
} edg_wll_VomsGroup;

typedef struct _edg_wll_VomsGroups {
	size_t len;
	edg_wll_VomsGroup *val;
} edg_wll_VomsGroups;

#ifdef __cplusplus 
}
#endif

#endif
