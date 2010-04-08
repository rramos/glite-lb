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

#include <getopt.h>
#include <stdsoap2.h>

#include "glite/security/glite_gsplugin.h"

#include "bk_ws_H.h"
#include "ws_fault.h"

#include "soap_version.h"
#if GSOAP_VERSION <= 20602
#define soap_call___lb__GetVersion soap_call___ns1__GetVersion
#endif

#include "LoggingAndBookkeeping.nsmap"

static struct option opts[] = {
	{"help",	0,	NULL,	'h'},
	{"server",	1,	NULL,	'm'},
};

static void usage(char *me)
{
	fprintf(stderr,"usage: %s [option]\n"
		"\t-h, --help      Shows this screen.\n"
		"\t-m, --server    BK server address:port.\n"
		, me);
}

int main(int argc,char** argv)
{
	glite_gsplugin_Context		gsplugin_ctx;
	struct soap			soap;
	struct _lbe__GetVersion		in;
	struct _lbe__GetVersionResponse	out;
	int				opt, err;
	char				*server = "http://localhost:9003/",
					*name = NULL;


	name = strrchr(argv[0],'/');
	if (name) name++; else name = argv[0];

	while ((opt = getopt_long(argc, argv, "hm:", opts, NULL)) != EOF) switch (opt)
	{
	case 'h': usage(name); return 0;
	case 'm': server = optarg; break;
	case '?': usage(name); return 1;
	}

    glite_gsplugin_init_context(&gsplugin_ctx);

	soap_init(&soap);
	soap_set_namespaces(&soap, namespaces);

	if ( soap_register_plugin_arg(&soap, glite_gsplugin, (void *)gsplugin_ctx) )
	{
		soap_print_fault(&soap, stderr);
		return 1;
	}

    memset(&in, 0, sizeof(in));
    memset(&out, 0, sizeof(out));
    switch (err = soap_call___lb__GetVersion(&soap, server, "", &in, &out))
	{
	case SOAP_OK: printf("Server version: %s\n", out.version); break;
	case SOAP_FAULT: 
	case SOAP_SVR_FAULT:
		{
		char	*et;
		int	err;

		err = glite_lb_FaultToErr(&soap,&et);
		fprintf(stderr,"%s: %s (%s)\n",argv[0],strerror(err),et);
		exit(1);
		}
	default: 
		printf("???, err=%d\n", err);
		soap_print_fault(&soap, stderr);
    }

    soap_end(&soap);
    soap_done(&soap);
    glite_gsplugin_free_context(gsplugin_ctx);

    return 0;
}