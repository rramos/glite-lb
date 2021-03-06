# defaults
top_srcdir=..
stagedir=.
distdir=.
globalprefix=glite
lbprefix=lb
package=glite-lb-state-machine
PREFIX=/opt/glite
prefix=

default: all

-include Makefile.inc
-include ${top_srcdir}/project/version.properties
version=${module.version}

VPATH = ${top_srcdir}/src:${top_srcdir}/interface

CC=gcc
CXX=g++
AT3=${stagedir}${prefix}/bin/glite-lb-at3

SUFFIXES = .T 

SOURCES=\
	interface/*.h interface/*.xsd interface/*.xsd.T \
	project/.post* \
	src/*.c src/*.xsl \
	Makefile

ifneq (${classads_prefix},/usr)
	classadslib := -L${classads_prefix}/${libdir} -L${classads_prefix}/lib 
endif
ifneq (${expat_prefix},/usr)
	expatlib := -L${expat_prefix}/${libdir} -L${expat_prefix}/lib 
endif
CLASSADS_CFLAGS?=-I${classads_prefix}/include -I${classads_prefix}/include/classad
CLASSADS_LIBS?=${classadslib} -lclassad
EXPAT_CFLAGS?=
EXPAT_LIBS?=${expatlib} -lexpat
GLITE_LBJP_COMMON_GSS_CFLAGS?=`PKG_CONFIG_PATH=$$PKG_CONFIG_PATH:${PREFIX}${prefix}/${libdir}/pkgconfig pkg-config glite-security-gss --cflags`

DEBUG:=-g -O0 -Wall

CFLAGS:= \
	${DEBUG} \
	${CLASSADS_CFLAGS} \
	-I${stagedir}${prefix}/include -I${top_srcdir}/src -I. \
	-I${top_srcdir}/interface ${GLITE_LBJP_COMMON_GSS_CFLAGS} ${CFLAGS}

COMPILE:=libtool --mode=compile ${CC} ${CFLAGS}
LINK:=libtool --mode=link ${CC} -rpath ${stagedir}${prefix}/${libdir}  ${LDFLAGS} 
SOLINK:=libtool --mode=link ${CC} -module ${LDFLAGS} -rpath ${stagedir}${prefix}/${libdir}
LINKXX:=libtool --mode=link ${CXX} ${LDFLAGS} 
INSTALL:=libtool --mode=install install
LINKXX:=libtool --mode=link  ${CXX} -rpath ${stagedir}${prefix}/${libdir} ${LDFLAGS}
XSLTPROC:=xsltproc --novalid

ifeq (${thrflavour},)
default_flavour=
else
default_flavour=_${nothrflavour}
endif
COMMON_LIBS:= -L${stagedir}${prefix}/${libdir} -lglite_lb_common${default_flavour} -lglite_security_gss${default_flavour} -lglite_jobid
PLUGIN_LIBS:= -L${stagedir}${prefix}/${libdir} -lglite_lb_common${default_flavour}

PLUGIN_LOBJS:=seqcode_aux.lo process_event.lo process_event_pbs.lo process_event_condor.lo process_event_cream.lo lb_plugin.lo  process_event_file_transfer.lo process_event_file_transfer_collection.lo process_event_virtual_machine.lo
MACHINE_OBJS:=seqcode_aux.o process_event.o process_event_pbs.o process_event_condor.o process_event_cream.o process_event_file_transfer.o process_event_file_transfer_collection.o process_event_virtual_machine.o intjobstat_supp.o
MACHINE_LOBJS:=${MACHINE_OBJS:.o=.lo}

PLUGIN_LIB=glite_lb_plugin.la
MACHINE_LIB=libglite_lb_statemachine.la

XSD=lb-job-attrs2.xsd

all: compile

compile: ${MACHINE_LIB} ${PLUGIN_LIB} ${XSD}

# In order to use libtool versioning correcty, we must have:
#
# current = major + minor + offset
# revision = patch
# age = minor
#
# where offset is a sum of maximal released minor's of all previous major's
# 

# counted minors: 1.4
offset=4
version_info:=-version-info ${shell \
        perl -e '$$,=":"; @F=split /\./,"${version}"; print $$F[0]+$$F[1]+${offset},$$F[2],$$F[1]' }

${PLUGIN_LIB}: ${PLUGIN_LOBJS}
	${SOLINK} -o $@ ${PLUGIN_LOBJS} ${PLUGIN_LIBS}

${MACHINE_LIB}:  ${MACHINE_OBJS}
	${LINK} ${version_info} -o $@ ${MACHINE_LOBJS} ${COMMON_LIBS}
#	ar crv $@ ${MACHINE_OBJS}
#	-ranlib $@


doc:

stage: compile
	$(MAKE) install PREFIX=${stagedir}

install:
	-mkdir -p ${DESTDIR}${PREFIX}${prefix}/${libdir} ${DESTDIR}${PREFIX}${prefix}/interface ${DESTDIR}${PREFIX}${prefix}/include/glite/lb
	${INSTALL} -m 755 ${MACHINE_LIB} ${DESTDIR}${PREFIX}${prefix}/${libdir}
	${INSTALL} -m 755 ${PLUGIN_LIB} ${DESTDIR}${PREFIX}${prefix}/${libdir}
	install -m 644 ${top_srcdir}/interface/lb-job-attrs.xsd ${DESTDIR}${PREFIX}${prefix}/interface
	install -m 644 lb-job-attrs2.xsd ${DESTDIR}${PREFIX}${prefix}/interface
	install -m 644 ${top_srcdir}/interface/lb-job-record.xsd ${DESTDIR}${PREFIX}${prefix}/interface
	install -m 644 ${top_srcdir}/interface/intjobstat.h ${DESTDIR}${PREFIX}${prefix}/include/glite/lb
	install -m 644 ${top_srcdir}/interface/intjobstat_supp.h ${DESTDIR}${PREFIX}${prefix}/include/glite/lb
	install -m 644 ${top_srcdir}/interface/process_event.h ${DESTDIR}${PREFIX}${prefix}/include/glite/lb
	install -m 644 ${top_srcdir}/interface/seqcode_aux.h ${DESTDIR}${PREFIX}${prefix}/include/glite/lb
	install -m 644 job_attrs.h ${DESTDIR}${PREFIX}${prefix}/include/glite/lb
	install -m 644 job_attrs2.h ${DESTDIR}${PREFIX}${prefix}/include/glite/lb

clean:
	rm -rvf .libs *.o *.lo ${PLUGIN_LIB} ${MACHINE_LIB}
	rm -rfv job_attrs.h job_attrs2.h lb-job-attrs2.xsd glite-jpis-config.xml
	rm -rvf dist ${package}-*.tar.gz

distclean:
	rm -rvf project/changelog Makefile.inc *.spec debian/

check:
	@echo No tests yet

%.c: %.c.T
	rm -f $@
	${AT3} $< >$@ || rm -f $@
	chmod -w $@ >/dev/null

%.cpp: %.cpp.T
	rm -f $@
	${AT3} $< >$@ || rm -f $@
	chmod -w $@ >/dev/null

%.xsd: %.xsd.T
	rm -f $@
	${AT3} $< >$@ || rm -f $@
	chmod -w $@ >/dev/null

%.o %.lo: %.c
	${COMPILE} -c $<

lb_plugin.lo: lb_plugin.c job_attrs.h job_attrs2.h
	${COMPILE} -DPLUGIN_DEBUG -o $@ -c $<

job_attrs.h: lb-job-attrs.xsd job_attrs.xsl
	${XSLTPROC} ${top_srcdir}/src/job_attrs.xsl $< >$@ || ( rm -f $@; false)

job_attrs2.h: lb-job-attrs2.xsd job_attrs2.xsl
	${XSLTPROC} ${top_srcdir}/src/job_attrs2.xsl $< >$@ || ( rm -f $@; false)

glite-jpis-config.xml: lb-job-attrs2.xsd jpis_config.xsl
	${XSLTPROC} ${top_srcdir}/src/jpis_config.xsl lb-job-attrs2.xsd >$@ || ( rm -f $@; false)

.PHONY: default all compile doc stage clean distclean check dist distcheck
