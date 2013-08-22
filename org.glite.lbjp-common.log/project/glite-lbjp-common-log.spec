%{!?_pkgdocdir: %global _pkgdocdir %{_docdir}/%{name}-%{version}}

Name:           glite-lbjp-common-log
Version:        @MAJOR@.@MINOR@.@REVISION@
Release:        @AGE@%{?dist}
Summary:        @SUMMARY@

Group:          System Environment/Libraries
License:        ASL 2.0
Url:            @URL@
Vendor:         EMI
Source:         http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lbjp-common.log/%{version}/src/%{name}-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildRequires:  libtool
BuildRequires:  log4c-devel
BuildRequires:  perl
BuildRequires:  perl(Getopt::Long)
BuildRequires:  perl(POSIX)
BuildRequires:  pkgconfig

%description
@DESCRIPTION@

%package        devel
Summary:        Development files for gLite L&B/JP common log module
Group:          Development/Libraries
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
This package contains development libraries and header files for gLite L&B/JP
common log module.


%prep
%setup -q


%build
perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=%{_prefix} --libdir=%{_lib} --docdir=%{_pkgdocdir} --project=emi --module lbjp-common.log
CFLAGS="%{?optflags}" LDFLAGS="%{?__global_ldflags}" make


%check
CFLAGS="%{?optflags}" LDFLAGS="%{?__global_ldflags}" make check


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
install -m 0644 LICENSE project/ChangeLog $RPM_BUILD_ROOT%{_pkgdocdir}
find $RPM_BUILD_ROOT -name '*.la' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*.a' -exec rm -rf {} \;

%clean
rm -rf $RPM_BUILD_ROOT


%post -p /sbin/ldconfig


%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
%dir %{_pkgdocdir}/
%dir %{_sysconfdir}/glite-lb
%config(noreplace) %{_sysconfdir}/glite-lb/log4crc
%doc %{_pkgdocdir}/ChangeLog
%doc %{_pkgdocdir}/LICENSE
%doc %{_pkgdocdir}/README
%doc %{_pkgdocdir}/log4crc.example-debugging
%doc %{_pkgdocdir}/log4crc.example-production
%{_libdir}/libglite_lbu_log.so.1
%{_libdir}/libglite_lbu_log.so.1.*

%files devel
%defattr(-,root,root)
%dir %{_includedir}/glite
%dir %{_includedir}/glite/lbu
%{_includedir}/glite/lbu/log.h
%{_libdir}/libglite_lbu_log.so


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@
- automatically generated package
