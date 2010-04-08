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


#include <stdlib.h>
#include <string.h>

#include <cclassad.h>

#include "jobstat.h"

static const struct timeval null_timeval = {0,0};


void edg_wll_FreeStatus(edg_wll_JobStat *stat)
{ 
  if (stat) {
	int 	i;
    
@@@{
        selectType $status '_common_';
        for (getFieldsOrdered $status) {
                my $f = selectField $status $_;
                my $ft = $f->{type};
                if ($ft eq 'jobid') {
                        gen "\tedg_wlc_JobIdFree(stat->$_);\n";
                }
                if ($ft eq 'string') {
                        gen "\tif (stat->$_ != NULL )    \tfree(stat->$_);\n";
                }
                if ($ft eq 'intlist') {
                        gen "\tif (stat->$_ != NULL )    \tfree(stat->$_);\n";
                }
                if ($ft eq 'strlist') {
                        gen "\tif (stat->$_ != NULL ) {\n";
			gen "\t\tfor (i=0; stat->$_\[i]; i++)\n";
			gen "\t\t\tfree(stat->$_\[i]);\n";
			gen "\t\tfree(stat->$_);\n";
			gen "\t}\n";
                }
                if ($ft eq 'taglist') {
                        gen "\tif (stat->$_ != NULL ) {\n";
			gen "\t\tfor (i=0; stat->$_\[i].tag; i++) {\n";
			gen "\t\t\tfree(stat->$_\[i].tag);\n";
			gen "\t\t\tfree(stat->$_\[i].value);\n";
			gen "\t\t}\n";
			gen "\t\tfree(stat->$_);\n";
			gen "\t}\n";
                }
                if ($ft eq 'stslist') {
                        gen "\tif (stat->$_ != NULL ) {\n";
			gen "\t\tfor (i=0; stat->$_\[i].state; i++)\n";
			gen "\t\t\tedg_wll_FreeStatus(&stat->$_\[i]);\n";
			gen "\t\tfree(stat->$_);\n";
			gen "\t}\n";
                }
                if ($ft eq 'cclassad') {
                        gen "\tif (stat->$_ != NULL )   { \tcclassad_delete(stat->$_); stat->$_=NULL; }\n";
                }
        }
@@@}
  }
}

edg_wll_JobStat *edg_wll_CpyStatus(const edg_wll_JobStat *src, edg_wll_JobStat *dest)
{
	int 	i;


	if ( !src || !dest )
		return NULL;

	edg_wll_InitStatus(dest);
    	dest->state = src->state;
@@@{
		selectType $status '_common_';
		for (getFieldsOrdered $status) {
			my $f = selectField $status $_;
			my $ft = $f->{type};
			if ($ft eq 'jobid') {
				gen "\tif ( edg_wlc_JobIdDup(src->$_, &(dest->$_)) ) goto err;\n";
			}
			if ($ft eq 'string') {
				gen "\tif ( src->$_ != NULL )\n";
				gen "\t\tif ( !(dest->$_ = strdup(src->$_)) ) goto err;\n";
			}
			if ($ft eq 'intlist') {
				gen "\tif ( src->$_ != NULL )\n\t{\n";
				gen "\t\ti = 1 + src->$_\[0];\n";
				gen "\t\tif ( !(dest->$_ = malloc(sizeof(*src->$_)*i)) ) goto err;\n";
				gen "\t\tmemcpy(dest->$_,src->$_,sizeof(*src->$_)*i);\n\t}\n";
			}
			if ($ft eq 'strlist') {
				gen "\tif ( src->$_ != NULL )\n\t{\n";
				gen "\t\tfor ( i = 0; src->$_\[i]; i++ ) ;\n";
				gen "\t\tif ( !(dest->$_ = malloc(sizeof(*src->$_)*(i+1))) ) goto err;\n";
				gen "\t\tfor ( i = 0; src->$_\[i]; i++ )\n";
				gen "\t\t\tif ( !(dest->$_\[i] = strdup(src->$_\[i])) ) goto err;\n";
				gen "\t\tdest->$_\[i] = NULL;\n\t}\n";
			}
			if ($ft eq 'stslist') {
				gen "\tif ( src->$_ != NULL )\n\t{\n";
				gen "\t\tfor ( i = 0; src->$_\[i].state; i++ ) ;\n";
				gen "\t\tif ( !(dest->$_ = malloc(sizeof(*src->$_)*(i+1))) ) goto err;\n";
				gen "\t\tfor ( i = 0; src->$_\[i].state; i++ )\n";
				gen "\t\t\tif ( !edg_wll_CpyStatus(&src->$_\[i], &dest->$_\[i]) ) goto err;\n";
				gen "\t\tdest->$_\[i].state = EDG_WLL_JOB_UNDEF;\n\t}\n";
			}
			if (($ft eq 'bool') or ($ft eq 'timeval') or ($ft eq 'logsrc')
				or ($ft eq 'port') or ($ft eq 'level') or ($ft eq 'int')) {
				gen "\tdest->$_ = src->$_;\n";
			}
			if ($ft eq 'taglist') {
				gen qq{
!	if (src->$_ != NULL) \{
!		for (i=0; src->$_\[i].tag; i++);
!		dest->$_ = malloc(sizeof(*src->$_)*(i+1));
!		for (i=0; src->$_\[i].tag; i++) \{
!			dest->$_\[i].tag = strdup(src->$_\[i].tag);
!			dest->$_\[i].value = strdup(src->$_\[i].value);
!		\}
!		dest->$_\[i].tag = NULL;
!        \}
				};
			}
		}
@@@}
    
  return dest;

err:
  edg_wll_FreeStatus(dest);
  return NULL;
}


int edg_wll_InitStatus(edg_wll_JobStat *stat)
{
	if (!stat) return -1;
	
	stat->state = EDG_WLL_JOB_UNDEF;

@@@{
        selectType $status '_common_';
        for (getFieldsOrdered $status) {
                my $f = selectField $status $_;
                gen "\tstat->$_ = $f->{null};\n"
        }
@@@}

	return 0;
}

static const char * const statNames[] = {
	"Undefined",
@@@{
        for (getTypesOrdered $status) {
                gen "\t\"$_\",\n";
        }
@@@}
};

edg_wll_JobStatCode edg_wll_StringToStat(const char *name)

{
        unsigned int     i;

        for (i=0; i<sizeof(statNames)/sizeof(statNames[0]); i++)
                if (strcasecmp(statNames[i],name) == 0) return (edg_wll_JobStatCode) i;
        return (edg_wll_JobStatCode) -1; 
}

char *edg_wll_StatToString(edg_wll_JobStatCode statCode)
{
        if ((int)statCode < 0 || statCode >= sizeof(statNames)/sizeof(statNames[0])) return (char *) NULL; 
        return strdup(statNames[statCode]);
}

static const char * const jw_statNames[] = {
@@@{
                my $f = selectField $status jw_status;
                if ($f->{codes}) {
                        for (@{$f->{codes}}) {
                                my $lc = lc $_->{name};
				my $uc = ucfirst $lc;
                                gen qq{
!       "$uc",
}; 
                        } 
                }
@@@}
};

enum edg_wll_StatJw_status edg_wll_StringToJWStat(const char *name)
{
        unsigned int     i;

        for (i=0; i<sizeof(jw_statNames)/sizeof(jw_statNames[0]); i++)
                if (strcasecmp(jw_statNames[i],name) == 0) return (enum edg_wll_StatJw_status) i;
        return (enum edg_wll_StatJw_status) -1; 
}

char *edg_wll_JWStatToString(enum edg_wll_StatJw_status statCode)
{
        if ((int)statCode < 0 || statCode >= sizeof(jw_statNames)/sizeof(jw_statNames[0])) return (char *) NULL; 
        return strdup(jw_statNames[statCode]);
}

static const char * const cream_statNames[] = {
@@@{
                my $f = selectField $status cream_state;
                if ($f->{codes}) {
                        for (@{$f->{codes}}) {
                                my $lc = lc $_->{name};
				my $uc = ucfirst $lc;
                                gen qq{
!       "$uc",
}; 
                        } 
                }
@@@}
};

enum edg_wll_StatCream_state edg_wll_StringToCreamStat(const char *name)
{
        unsigned int     i;

        for (i=0; i<sizeof(cream_statNames)/sizeof(cream_statNames[0]); i++)
                if (strcasecmp(cream_statNames[i],name) == 0) return (enum edg_wll_StatCream_state) i;
        return (enum edg_wll_StatCream_state) -1; 
}

char *edg_wll_CreamStatToString(enum edg_wll_StatCream_state statCode)
{
        if ((int)statCode < 0 || statCode >= sizeof(cream_statNames)/sizeof(cream_statNames[0])) return (char *) NULL; 
        return strdup(cream_statNames[statCode]);
}



char *edg_wll_JDLField(edg_wll_JobStat *stat, const char *field_name)
{
	char *extr_val;

	if (stat->jdl_classad == NULL) { //classad had not been unparsed yet

		stat->jdl_classad = (struct classad*)cclassad_create(stat->jdl);

		/* XXX: Can't parse, likely due to filename 
		        which is logged first, than replaced by content */
		if (!stat->jdl_classad) return NULL;
	}

	if (cclassad_evaluate_to_string(stat->jdl_classad, field_name, &extr_val)) {
		return extr_val;
	}
	else 
		return NULL;
}

