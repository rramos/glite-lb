Name:           glite-lb-client
Version:        @MAJOR@.@MINOR@.@REVISION@
Release:        @AGE@%{?dist}
Summary:        @SUMMARY@

Group:          System Environment/Libraries
License:        ASL 2.0
Url:            @URL@
Vendor:         EMI
Source:         http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lb.client/%{version}/src/%{name}-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:  classads
BuildRequires:  classads-devel
BuildRequires:  cppunit-devel
BuildRequires:  chrpath
BuildRequires:  glite-lb-types
BuildRequires:  glite-jobid-api-c-devel
BuildRequires:  glite-jobid-api-cpp-devel
BuildRequires:  glite-lb-common-devel
BuildRequires:  glite-lbjp-common-gss-devel
BuildRequires:  glite-lbjp-common-trio-devel
BuildRequires:  libtool
BuildRequires:  pkgconfig

%description
@DESCRIPTION@


%package        devel
Summary:        Development files for gLite L&B client library
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}
Requires:       glite-lb-common-devel
Requires:       glite-jobid-api-c-devel
Requires:       glite-jobid-api-cpp-devel

%description    devel
This package contains development libraries and header files for gLite L&B
client library.


%package        progs
Summary:        gLite L&B client programs and examples
Group:          System Environment/Base
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    progs
This package contains client programs and examples for gLite L&B.


%prep
%setup -q


%build
/usr/bin/perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=%{_prefix} --libdir=%{_lib} --project=emi --module lb.client
CFLAGS="%{?optflags}" LDFLAGS="%{?__global_ldflags}" make


%check
CFLAGS="%{?optflags}" LDFLAGS="%{?__global_ldflags}" make check


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
install -m 0644 LICENSE project/ChangeLog $RPM_BUILD_ROOT%{_docdir}/%{name}-%{version}
find $RPM_BUILD_ROOT -name '*.la' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*.a' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*' -print | xargs -I {} -i bash -c "chrpath -d {} > /dev/null 2>&1" || echo 'Stripped RPATH'


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
%dir %{_docdir}/%{name}-%{version}/
%{_libdir}/libglite_lb_client.so.14
%{_libdir}/libglite_lb_client.so.14.*
%{_libdir}/libglite_lb_clientpp.so.14
%{_libdir}/libglite_lb_clientpp.so.14.*
%{_docdir}/%{name}-%{version}/ChangeLog
%{_docdir}/%{name}-%{version}/LICENSE

%files devel
%defattr(-,root,root)
%dir %{_docdir}/%{name}-%{version}/examples/
%dir %{_datadir}/emi/
%dir %{_datadir}/emi/build/
%dir %{_datadir}/emi/build/m4/
%dir %{_includedir}/glite/
%dir %{_includedir}/glite/lb/
%{_includedir}/glite/lb/*.h
%{_libdir}/libglite_lb_client.so
%{_libdir}/libglite_lb_clientpp.so
%{_docdir}/%{name}-%{version}/examples/*
%{_datadir}/emi/build/m4/glite_lb.m4

%files progs
%defattr(-,root,root)
%dir %{_libdir}/glite-lb/
%dir %{_libdir}/glite-lb/examples/
%{_bindir}/glite-lb-logevent
%{_bindir}/glite-lb-notify
%{_bindir}/glite-lb-register_sandbox
%{_libdir}/glite-lb/examples/*
%{_docdir}/%{name}-%{version}/README-notify
%{_mandir}/man1/glite-lb-notify.1.gz
%{_mandir}/man1/glite-lb-logevent.1.gz


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@
- automatically generated package
