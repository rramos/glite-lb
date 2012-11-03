Name:           emi-lb-nagios-plugins
Version:        @MAJOR@.@MINOR@.@REVISION@
Release:        @AGE@%{?dist}
Summary:        @SUMMARY@

Group:          System Environment/Daemons
License:        ASL 2.0
Url:            @URL@
Vendor:         EMI
Source:         http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lb.nagios/%{version}/src/%{name}-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildArch:      noarch
Requires:       glite-lb-client
Requires:       glite-lb-utils
Requires:       glite-lb-ws-test
Requires:       globus-proxy-utils
Provides:       glite-lb-nagios-plugins = %{name}-%{version}-%{release}

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
%doc project/ChangeLog
%dir /usr/libexec/
%dir /usr/libexec/grid-monitoring/
%dir /usr/libexec/grid-monitoring/probes/
%dir /usr/libexec/grid-monitoring/probes/emi.lb/
%dir /var/lib/grid-monitoring/
%dir /var/lib/grid-monitoring/emi.lb/
/usr/libexec/grid-monitoring/probes/emi.lb/LB-probe


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@%{?dist}
- automatically generated package
