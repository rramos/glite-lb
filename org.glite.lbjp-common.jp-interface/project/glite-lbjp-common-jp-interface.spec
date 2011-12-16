Summary: Virtual package for development with gLite L&B/JP interface library
Name: glite-lbjp-common-jp-interface
Version: @MAJOR@.@MINOR@.@REVISION@
Release: @AGE@%{?dist}
Url: @URL@
License: Apache Software License
Vendor: EMI
Group: Development/Libraries
BuildRequires: chrpath
BuildRequires: cppunit-devel
BuildRequires: glite-jobid-api-c-devel
BuildRequires: glite-lbjp-common-db-devel
BuildRequires: glite-lbjp-common-trio-devel
BuildRequires: libtool
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lbjp-common.jp-interface/%{version}/src/%{name}-@VERSION@.src.tar.gz


%description
This is a virtual package providing runtime and development files for gLite
L&B/JP interface library.


%package -n lib%{name}
Summary: @SUMMARY@
Group: System Environment/Libraries


%description -n lib%{name}
@DESCRIPTION@


%package -n %{name}-devel
Summary: Development files for gLite L&B/JP interface library
Group: Development/Libraries
Requires: lib%{name}%{?_isa} = %{version}-%{release}
Provides: %{name}%{?_isa} = %{version}-%{release}


%description -n %{name}-devel
This package contains development libraries and header files for gLite L&B/JP
interface library.


%prep
%setup -q


%build
/usr/bin/perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=/usr --libdir=%{_lib} --project=emi --module lbjp-common.jp-interface
make


%check
make check


%install
rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*.a' -exec rm -rf {} \;
find $RPM_BUILD_ROOT -name '*' -print | xargs -I {} -i bash -c "chrpath -d {} > /dev/null 2>&1" || echo 'Stripped RPATH'

%clean
rm -rf $RPM_BUILD_ROOT


%post -n lib%{name} -p /sbin/ldconfig


%postun -n lib%{name} -p /sbin/ldconfig


%files -n lib%{name}
%defattr(-,root,root)
/usr/%{_lib}/libglite_jp_common.so.@MAJOR@.@MINOR@.@REVISION@
/usr/%{_lib}/libglite_jp_common.so.@MAJOR@


%files -n %{name}-devel
%defattr(-,root,root)
%dir /usr/include/glite
%dir /usr/include/glite/jp
/usr/include/glite/jp/types.h
/usr/include/glite/jp/indexdb.h
/usr/include/glite/jp/attr.h
/usr/include/glite/jp/file_plugin.h
/usr/include/glite/jp/context.h
/usr/include/glite/jp/type_plugin.h
/usr/include/glite/jp/backend.h
/usr/include/glite/jp/known_attr.h
/usr/include/glite/jp/builtin_plugins.h
/usr/%{_lib}/libglite_jp_common.so


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@%{?dist}
- automatically generated package
