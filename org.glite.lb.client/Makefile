# Default values
top_srcdir=..
stagedir=.
globalprefix=glite
lbprefix=lb
package=glite-lb-client
version=0.0.0

nothrflavour=gcc32
thrflavour=gcc32pthr

CC:=gcc
CXX:=g++

-include Makefile.inc
-include ../project/version.properties

version=${module.version}

# We must follow major version changes of org.glite.lb.common due to
# binary compatibility. However, we may live a life of our own, changing our
# major version more frequently. This variable specifies how many steps ahead
# we are (the number can be even negative).

VERSION_AHEAD=-3

VPATH=${top_srcdir}/src:${top_srcdir}/interface:${top_srcdir}/test:${top_srcdir}/examples:${top_srcdir}/doc
AT3=${stagedir}/sbin/glite-lb-at3
GENSAM=${top_srcdir}/examples/gen_sample_job
CHECK_VERSION:=VERSION=${version} VERSION_AHEAD=${VERSION_AHEAD} perl ${stagedir}/sbin/glite-lb-check_version.pl

SUFFIXES = .T .l

l_SRC = \
	chkpt.l \
	cleared.l \
	done.l \
	done_dag.l \
	done_subjob.l \
	ready.l \
	ready_dag.l \
	ready_subjob.l \
	running.l \
	running_dag.l \
	running_subjob.l \
	scheduled.l \
	scheduled_dag.l \
	scheduled_subjob.l \
	submitted.l \
	submitted_dag.l \
	submitted_subjob.l \
	waiting.l \
	waiting_dag.l \
	waiting_subjob.l \
	failed_dag.l \
	failed_subjob.l \
	aborted.l \
	cancelled.l \
	cream_registered.l \
	cream_pending.l \
	cream_idle.l \
	cream_running.l \
	cream_reallyrunning.l \
	cream_done.l \
	cream_failed.l \
	submitted_ft_reg.l \
	running_ft.l \
	done_ft.l \
	aborted_ft.l \

# TODO: missing resubmission_deep
#	shallow_resub_complex.l shallow_resub_simple.l shallow_resub_simple2.l \
#	resubmission.l resubmitted.l


sh_PROGS = $(l_SRC:.l=.sh)

ifdef DEBUG
	DEBUG:=-g -O0 -Wall -DEDG_WLL_LOG_STUB
else
	DEBUG:=-g -O0 -Wall
endif

ifdef LB_STANDALONE
        LB_STANDALONE_FLAGS:=-DLB_STANDALONE
endif

ifdef LB_PERF
	LB_PERF_FLAGS:=-DLB_PERF
	LB_PERF_TOOLS:=perftest_logjobs perftest_jobreg
endif
COMMON_LIB:=-lglite_lb_common_${nothrflavour}
COMMON_LIB_THR:=-lglite_lb_common_${thrflavour}
TRIO_LIB:=-lglite_lbu_trio

CFLAGS:=${DEBUG} \
	-I. \
	-I${top_srcdir}/src -I${top_srcdir}/interface \
	-I${stagedir}/include \
	${COVERAGE_FLAGS} \
	-DBUILDING_LB_CLIENT \
	-D_GNU_SOURCE ${LB_STANDALONE_FLAGS} ${LB_PERF_FLAGS}

CXXFLAGS:=${CFLAGS}

EXT_LIB:=

TEST_LIBS:=-L${cppunit_prefix}/${libdir} -lcppunit
TEST_INC:=-I${cppunit_prefix}/include

LDFLAGS:=-L${stagedir}/${libdir} ${COVERAGE_FLAGS}

COMPILE:=libtool --mode=compile ${CC} ${CFLAGS}
CXXCOMPILE:=libtool --mode=compile ${CXX} ${CXXFLAGS}
LINK:=libtool --mode=link ${CC} ${LDFLAGS} 
LINKXX:=libtool --mode=link ${CXX} ${LDFLAGS}
INSTALL?=libtool --mode=install install

LIBOBJS:=connection.o consumer.o notification.o prod_proto.o \
	producer.o uiwrap.o statistics.o

PLUSOBJS:=Event.o Job.o JobStatus.o Notification.o ServerConnection.o stat_fields.o

HDRS:=consumer.h notification.h statistics.h prod_proto.h connection.h \
	Job.h Notification.h ServerConnection.h stat_fields.h
GEN_HDRS:=Event.h JobStatus.h producer.h

LIBTHROBJS:=${LIBOBJS:.o=.thr.o}
LIBLOBJS:=${LIBOBJS:.o=.lo}
LIBTHRLOBJS:=${LIBOBJS:.o=.thr.lo}

PLUSTHROBJS:=${PLUSOBJS:.o=.thr.o}
PLUSLOBJS:=${PLUSOBJS:.o=.lo}
PLUSTHRLOBJS:=${PLUSOBJS:.o=.thr.lo}

LIB:=libglite_lb_client_${nothrflavour}.la
THRLIB:=libglite_lb_client_${thrflavour}.la

PLUSLIB:=libglite_lb_clientpp_${nothrflavour}.la
THRPLUSLIB:=libglite_lb_clientpp_${thrflavour}.la

TOOLS:=${LB_PERF_TOOLS}
EXAMPLES_SRC:=log_usertag_proxy.c job_log.c job_reg.c feed_shark.c query_ext.c query_seq_code.c stats.c abort_job.c change_acl.c stresslog.c flood_proxy.c dagids.c stress_context.c parse_eventsfile.c test_changed_jdl.c done_failed_events.c
EXAMPLES:=${EXAMPLES_SRC:.c=}

# TODO: migrate them here from branch_RC31_3
# EXAMPLES_PLUS_SRC:=indexed_attrs_plus.cpp job_status_plus.cpp query_events_plus.cpp listener.cpp user_jobs_plus.cpp job_log_plus.cpp notify_plus.cpp
# EXAMPLES_PLUS:=${EXAMPLES_PLUS_SRC:.cpp=}

EXAMPLES_CL_SRC:=user_jobs.c job_status.c multiple_user_jobs.c
EXAMPLES_CL:=${EXAMPLES_CL_SRC:.c=}

EXAMPLES_CL_THR_SRC:=user_jobs_threaded.c
EXAMPLES_CL_THR:=${EXAMPLES_CL_THR_SRC:.c=}

MAN_GZ:=glite-lb-logevent.1.gz glite-lb-notify.1.gz
MAN8_GZ:=glite-lb-dump.8.gz glite-lb-load.8.gz
MAN = $(MAN_GZ:.gz=)
MAN8 = $(MAN8_GZ:.gz=)

PLUS_EXTRA_LIB:=-lglite_jobid


# version_info=-version-info `echo ${version} | cut -d. -f1,2 | tr . :`

# counted minor versions: 
offset=0

version_info:=-version-info ${shell \
	perl -e '$$,=":"; @F=split "\\.","${version}"; print $$F[0]+$$F[1]+${offset},$$F[2],$$F[1]' }

ifdef LB_STANDALONE
compile all: generate ${LIB} ${THRLIB} ${TOOLS} logevent notify register_sandbox examples ${MAN_GZ} ${MAN8_GZ}
else
compile all: check_version generate ${LIB} ${THRLIB} ${PLUSLIB} ${THRPLUSLIB} ${TOOLS} logevent notify register_sandbox examples ${MAN_GZ} ${MAN8_GZ}
endif

generate: ${GEN_HDRS}

default: all

${LIB}: ${LIBOBJS}
	${LINK} ${version_info} -o $@ ${LIBLOBJS} -rpath ${PREFIX}/${libdir} \
			${COMMON_LIB} ${TRIO_LIB} \
			-lglite_security_gss_${nothrflavour}

${THRLIB}: ${LIBTHROBJS}
	${LINK} ${version_info} -o $@ ${LIBTHRLOBJS} -rpath ${PREFIX}/${libdir} \
			${COMMON_LIB_THR} ${TRIO_LIB} \
			-lglite_security_gss_${thrflavour}

${PLUSLIB}: ${PLUSOBJS} ${LIB}
	${LINKXX} ${version_info} -o $@ ${PLUSLOBJS} -rpath ${PREFIX}/${libdir} ${LIB} ${PLUS_EXTRA_LIB}

${THRPLUSLIB}: ${PLUSTHROBJS} ${THRLIB}
	${LINKXX} ${version_info} -o $@ ${PLUSTHRLOBJS} -rpath ${PREFIX}/${libdir} ${THRLIB} ${PLUS_EXTRA_LIB}

logevent: logevent.o args.o
	${LINK} -o $@ logevent.o args.o ${LIB} ${EXT_LIB} 

notify: notify.o
	${LINKXX} -o $@ notify.o ${PLUSLIB} ${EXT_LIB} ${PLUS_EXTRA_LIB}

register_sandbox: %: %.o
	${LINK} -o $@ $@.o ${LIB} ${EXT_LIB}

${TOOLS} ${EXAMPLES}: %: %.o
	${LINK} -o $@ $< ${LIB} ${EXT_LIB} 

${EXAMPLES_CL}: %: %.o
	${LINK} -o $@ $< ${LIB} ${COMMON_LIB} ${EXT_LIB} 

${EXAMPLES_CL_THR}: %: %.o
	${LINK} -o $@ $< ${THRLIB} ${COMMON_LIB_THR} ${EXT_LIB} 

${TOOLS}: ${LIB}

${PLUSOBJS}: %.o: %.cpp
	${CXXCOMPILE} -c $<

${PLUSTHROBJS}: %.thr.o: %.cpp
	${CXXCOMPILE} -o $@ -c $<

${EXAMPLES_PLUS}: ${PLUSLIB}

${EXAMPLES_PLUS}: %: %.o
	${LINKXX} -o $@ $< ${PLUSLIB}

${LIBOBJS}: %.o: %.c
	${COMPILE} -c $<

${LIBTHROBJS}: %.thr.o: %.c
	${COMPILE} -o $@ -c $<

${MAN_GZ}: ${MAN}
	rm -f ${MAN_GZ} ${MAN}
	cp $? .
	gzip -f $(notdir $?)

${MAN8_GZ}: ${MAN8}
	cp $? .
	gzip -f $(notdir $?)

perftest_logjobs.o: perftest_logjobs.c
	${CC} ${CFLAGS} -DLB_PERF_DROP -c $<

# catches $TOOLS and logevent compilation
%.o: %.c 
	${CC} ${CFLAGS} -c $<

%.h: %.h.T
	rm -f $@
	${AT3} $< >$@ || ( rm -f $@; false )
	chmod -w $@ >/dev/null

%.o: %.cpp
	${CXX} ${CFLAGS} ${GLOBUSINC} -c $<

%.c: %.c.T
	rm -f $@
	${AT3} $< >$@ || ( rm -f $@; false )
	chmod -w $@ >/dev/null

%.cpp: %.cpp.T
	rm -f $@
	${AT3} $< >$@ || ( rm -f $@; false )
	chmod -w $@ >/dev/null

%.sh: %.l gen_begin gen_sample_job
	rm -f $@
	-chmod +x ${GENSAM}
	$(GENSAM) $< >$@ || rm -f $@
	chmod -w,+x $@ > /dev/null

examples: ${EXAMPLES} ${EXAMPLES_PLUS} ${EXAMPLES_CL} ${EXAMPLES_CL_THR} ${sh_PROGS}

check: compile 
# shut up check.producer

check.producer: producer_test
	./producer_test

producer_test: producer_test.o prod_proto_test.o
	${LINKXX} -o $@ ${LIB} ${TEST_LIBS} $+ ${EXT_LIB}

producer_test.o: %.o: %.cpp
	${CXX} -c ${CXXFLAGS} ${TEST_INC} $<

man: ${MAN_GZ} ${MAN8_GZ}

stage:  compile
	$(MAKE) install PREFIX=${stagedir}

Event.h Event.cpp: EventAttrNames.pl

JobStatus.h JobStatus.cpp: StatusAttrNames.pl

#EventAttrNames.pl:
#	rm -f $@
#	ln -s ${stagedir}/share/perl/gLite/LB/$@ $@

dist: distsrc distbin

install:
	mkdir -p ${PREFIX}/bin
	mkdir -p ${PREFIX}/sbin
	mkdir -p ${PREFIX}/include/${globalprefix}/${lbprefix}
	mkdir -p ${PREFIX}/${libdir}
	mkdir -p ${PREFIX}/share/doc/${package}-${version}/examples/
	mkdir -p ${PREFIX}/share/man/man1
	mkdir -p ${PREFIX}/examples
	mkdir -p ${PREFIX}/share/man/man8
	mkdir -p ${PREFIX}/share/build/m4
ifdef LB_STANDALONE
	${INSTALL} -m 644 ${LIB} ${THRLIB} ${PREFIX}/${libdir}
else
	${INSTALL} -m 644 ${LIB} ${THRLIB} ${PLUSLIB} ${THRPLUSLIB} ${PREFIX}/${libdir}
	${INSTALL} -m 644 ${LIB} ${THRLIB} ${PREFIX}/${libdir}
endif
	${INSTALL} -m 644 ${top_srcdir}/LICENSE ${PREFIX}/share/doc/${package}-${version}
	( cd ${top_srcdir}/project && ${INSTALL} -m 644 ChangeLog package.description package.summary ${PREFIX}/share/doc/${package}-${version} )
	${INSTALL} -m 644 ${top_srcdir}/doc/README-notify ${PREFIX}/share/doc/${package}-${version}
	(cd ${top_srcdir}/interface && ${INSTALL} -m 644 ${HDRS} ${PREFIX}/include/${globalprefix}/${lbprefix}) 
	${INSTALL} -m 644 ${GEN_HDRS} ${PREFIX}/include/${globalprefix}/${lbprefix} 
ifndef LB_STANDALONE
#	cp -r C CPP ${PREFIX}/share/doc/${package}-${version}
#       cp -r ${top_srcdir}/doc/api/{Makefile,api.tex,fig} ${PREFIX}/share/doc/${package}-${version}/api
endif
	for p in logevent notify register_sandbox; do \
		${INSTALL} -m 755 "$$p" "${PREFIX}/bin/glite-lb-$$p"; \
	done
	for p in ${TOOLS} ; do \
		${INSTALL} -m 755 "$$p" "${PREFIX}/sbin/glite-lb-$$p"; \
	done
# TODO: restructuring - do we really need binaries of the examples to be installed?
	for p in ${EXAMPLES} ${EXAMPLES_PLUS} ${EXAMPLES_CL} ${EXAMPLES_CL_THR} ${sh_PROGS} ; do \
		${INSTALL} -m 755 "$$p" "${PREFIX}/examples/glite-lb-$$p"; \
	done
	for p in ${EXAMPLES_SRC} ${EXAMPLES_PLUS_SRC} ${EXAMPLES_CL_SRC} ${EXAMPLES_CL_THR_SRC} ; do \
		${INSTALL} -m 644 "${top_srcdir}/examples/$$p" "${PREFIX}/share/doc/${package}-${version}/examples/"; \
	done
	${INSTALL} -m 644 ${top_srcdir}/examples/Makefile "${PREFIX}/share/doc/${package}-${version}/examples/"
	${INSTALL} -m 644 ${top_srcdir}/examples/README.examples "${PREFIX}/share/doc/${package}-${version}/examples/"
	${INSTALL} -m 755 ${top_srcdir}/src/export.sh "${PREFIX}/sbin/glite-lb-export.sh"
	${INSTALL} -m 755 ${top_srcdir}/examples/notify.pl ${PREFIX}/examples/glite-lb-notify.pl
	${INSTALL} -m 755 ${top_srcdir}/examples/sandbox_transfers.sh ${PREFIX}/examples/glite-lb-sandbox_transfers.sh
	${INSTALL} -m 644 ${MAN_GZ} ${PREFIX}/share/man/man1
	${INSTALL} -m 644 ${MAN8_GZ} ${PREFIX}/share/man/man8
	sed "s/%LIBDIR%/${libdir}/" ${top_srcdir}/m4/glite_lb_client.m4 > glite_lb_client.m4
	${INSTALL} -m 644 glite_lb_client.m4 ${PREFIX}/share/build/m4/

clean:
	rm -rvf *.o *.lo .libs lib* *.c *.cpp *.h *.dox producer_test notify C/ CPP/
	rm -rvf ${LIB} ${THRLIB} ${TOOLS} logevent  register_sandbox ${PLUSLIB} ${THRPLUSLIB} ${MAN_GZ} ${MAN8_GZ}
	rm -rvf ${EXAMPLES} ${EXAMPLES_CL} ${EXAMPLES_CL_THR} ${sh_PROGS}
	rm -f EventAttrNames.pl
	rm -rvf log.xml project/ rpmbuild/ RPMS/ tgz/

check_version:
	${CHECK_VERSION} ${stagedir}/include/glite/lb/common_version.h


.PHONY: default all compile examples check stage install clean check_version