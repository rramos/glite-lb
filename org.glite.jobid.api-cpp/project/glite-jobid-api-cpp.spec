Summary: Virtual package for development with gLite jobid C++ API
Name: glite-jobid-api-cpp
Version: @MAJOR@.@MINOR@.@REVISION@
Release: @AGE@%{?dist}
Url: @URL@
License: Apache Software License
Vendor: EMI
Group: Development/Libraries
BuildRequires: glite-jobid-api-c-devel
BuildRequires: libtool
BuildRequires: cppunit-devel
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.jobid.api-c/%{version}/src/%{name}-@VERSION@.src.tar.gz


%description
This is a virtual package providing runtime and development files for gLite
jobid C++ API.


%package -n %{name}-devel
Summary: @SUMMARY@
Group: Development/Libraries
Requires: libglite-jobid-api-c%{?_isa}
Provides: %{name} = %{version}-%{release}
Obsoletes: glite-jobid-api-cpp <= 1.2.0


%description -n %{name}-devel
@DESCRIPTION@


%prep
%setup -q


%build
/usr/bin/perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=/usr --libdir=%{_lib} --project=emi --module jobid.api-cpp
make


%check
make check


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*.a' -exec rm -rf {} \;


%clean
rm -rf $RPM_BUILD_ROOT


%files -n %{name}-devel
%defattr(-,root,root)
%dir /usr/include/glite
%dir /usr/include/glite/jobid
/usr/include/glite/jobid/JobId.h


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@%{?dist}
- automatically generated package
