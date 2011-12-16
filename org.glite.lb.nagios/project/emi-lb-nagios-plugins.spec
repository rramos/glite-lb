Summary: @SUMMARY@
Name: emi-lb-nagios-plugins
Version: @MAJOR@.@MINOR@.@REVISION@
Release: @AGE@%{?dist}
Url: @URL@
License: Apache Software License
Vendor: EMI
Group: System Environment/Daemons
BuildArch: noarch
Requires: glite-lb-client
Requires: glite-lb-utils
Requires: glite-lb-ws-test
Requires: globus-proxy-utils
Provides: glite-lb-nagios-plugins
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lb.nagios/%{version}/src/%{name}-@VERSION@.src.tar.gz


%description
@DESCRIPTION@


%prep
%setup -q


%build
/usr/bin/perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=/usr --libdir=%{_lib} --project=emi --module lb.nagios
make


%check
make check


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT


%post
chown nagios:nagios /usr/libexec/grid-monitoring/probes/emi.lb >/dev/null 2>&1 || :
exit 0


%files
%defattr(-,root,root)
%dir /usr/share/doc/%{name}-%{version}/
%dir /usr/libexec/
%dir /usr/libexec/grid-monitoring/
%dir /usr/libexec/grid-monitoring/probes/
%dir /usr/libexec/grid-monitoring/probes/emi.lb/
%dir /var/lib/grid-monitoring/
%dir /var/lib/grid-monitoring/emi.lb/
/usr/share/doc/%{name}-%{version}/package.summary
/usr/share/doc/%{name}-%{version}/ChangeLog
/usr/share/doc/%{name}-%{version}/package.description
/usr/libexec/grid-monitoring/probes/emi.lb/LB-probe


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@%{?dist}
- automatically generated package
