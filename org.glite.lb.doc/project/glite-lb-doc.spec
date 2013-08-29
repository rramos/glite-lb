%{!?_pkgdocdir: %global _pkgdocdir %{_docdir}/%{name}-%{version}}

Name:           glite-lb-doc
Version:        @MAJOR@.@MINOR@.@REVISION@
Release:        @AGE@%{?dist}
Summary:        @SUMMARY@

Group:          Documentation
License:        ASL 2.0
Url:            @URL@
Vendor:         EMI
Source:         http://eticssoft.web.cern.ch/eticssoft/repository/emi/emi.lb.doc/%{version}/src/%{name}-%{version}.tar.gz
BuildRoot:      %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

BuildArch:      noarch
BuildRequires:  glite-lb-types
BuildRequires:  perl
BuildRequires:  perl(Getopt::Long)
BuildRequires:  perl(POSIX)
%if 0%{?fedora} >= 9 || 0%{?rhel} >= 6
BuildRequires:  tex(latex)
%else
BuildRequires:  tetex-latex
%endif
%if 0%{?fedora} >= 18
BuildRequires:  tex(comment.sty)
BuildRequires:  tex(lastpage.sty)
BuildRequires:  tex(multirow.sty)
%endif

%description
@DESCRIPTION@


%prep
%setup -q


%build
perl ./configure --thrflavour= --nothrflavour= --root=/ --prefix=%{_prefix} --libdir=%{_lib} --docdir=%{_pkgdocdir} --project=emi --module lb.doc
# parallel build not supported (problems with pdflatex)
make


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT
install -m 0644 LICENSE project/ChangeLog $RPM_BUILD_ROOT%{_pkgdocdir}


%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-,root,root)
%dir %{_pkgdocdir}/
%dir %{_pkgdocdir}/examples/
%{_pkgdocdir}/examples/*
%{_pkgdocdir}/ChangeLog
%{_pkgdocdir}/LICENSE
%{_pkgdocdir}/README
%{_pkgdocdir}/LBAG.pdf
%{_pkgdocdir}/LBUG.pdf
%{_pkgdocdir}/LBDG.pdf
%{_pkgdocdir}/LBTG.pdf
%{_pkgdocdir}/LBTP.pdf


%changelog
* @SPEC_DATE@ @MAINTAINER@ - @MAJOR@.@MINOR@.@REVISION@-@AGE@
- automatically generated package
