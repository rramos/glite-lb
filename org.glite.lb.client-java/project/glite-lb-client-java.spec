Name:           glite-lb-client-java
Version:        @MAJOR@.@MINOR@.@REVISION@
Release:        @AGE@%{?dist}
Summary:        @SUMMARY@

Group:          System Environment/Libraries
License:        ASL 2.0
Url:            @URL@
Vendor:         EMI
Source:         http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lb.client-java/%{version}/src/%{name}-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:  ant
%if 0%{?fedora}
BuildRequires:  axis
%else
BuildRequires:  axis1.4
%endif
BuildRequires:  chrpath
BuildRequires:  emi-trustmanager
BuildRequires:  emi-trustmanager-axis
BuildRequires:  glite-jobid-api-java
BuildRequires:  glite-lb-types
BuildRequires:  glite-lb-ws-interface
BuildRequires:  jakarta-commons-lang
BuildRequires:  java-devel
BuildRequires:  jpackage-utils
BuildRequires:  libtool
Requires:       glite-jobid-api-java
Requires:       jakarta-commons-lang
Requires:       jpackage-utils

%description
@DESCRIPTION@


%package        axis
Summary:        Axis 1.4 flavor of Java L&B client
Group:          System Environment/Libraries
Requires:       %{name} = %{version}-%{release}
Requires:       emi-trustmanager-axis
Requires:       emi-trustmanager
Requires:       glite-jobid-api-java
Requires:       jakarta-commons-lang
Requires:       jpackage-utils
%if 0%{?rhel} >= 6
BuildArch:      noarch
%endif

%description    axis
This package contains java L&B client library based on Axis 1.4.


%package        examples
Summary:        Java L&B client examples
Group:          Applications/Communications
Requires:       %{name} = %{version}-%{release}
Requires:       %{name}-axis
Requires:       emi-trustmanager-axis
Requires:       emi-trustmanager
Requires:       glite-jobid-api-java
Requires:       jpackage-utils
%if 0%{?rhel} >= 6
BuildArch:      noarch
%endif

%description    examples
This package contains java L&B client examples for Axis 1.4. For the
communication is used trustmanager or pure SSL.


%prep
%setup -q


%build
/usr/bin/perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=%{_prefix} --libdir=%{_lib} --project=emi --module lb.client-java --with-axis=/usr/local/axis1.4
CFLAGS="%{?optflags}" LDFLAGS="%{?__global_ldflags}" make


%check
CFLAGS="%{?optflags}" LDFLAGS="%{?__global_ldflags}" make check


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*.a' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*' -print | xargs -I {} -i bash -c "chrpath -d {} > /dev/null 2>&1" || echo 'Stripped RPATH'


%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
%doc LICENSE project/ChangeLog
%{_libdir}/libglite_lb_sendviasocket.so
%{_libdir}/libglite_lb_sendviasocket.so.0
%{_libdir}/libglite_lb_sendviasocket.so.0.0.0
%{_javadir}/%{name}.jar

%files axis
%defattr(-,root,root)
%{_javadir}/%{name}-axis.jar

%files examples
%defattr(-,root,root)
%{_javadir}/%{name}-examples.jar


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@
- automatically generated package
