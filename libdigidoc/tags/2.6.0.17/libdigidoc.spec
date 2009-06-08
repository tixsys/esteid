Name: libdigidoc
Summary: library for handling digitally signed documents
Version: 2.2.4
Release: 1 
License: LGPL
Group: Development/C
URL: http://www.openxades.org
Packager: Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
Source0: %{name}-%{version}.tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: openssl
Prereq: openssl
BuildPrereq: openssl

%description
This package provides a C library for handling digitally signed documents
that use the OpenXAdES format.

%prep
%setup 

%build
%configure --prefix=/usr --sysconfdir=/etc

%install
rm -fr %{buildroot}
make DESTDIR=$RPM_BUILD_ROOT install

%clean 
rm -fr %{buildroot}

%post
ln -sf /usr/lib/libdigidoc.so.2.0.1 /usr/lib/libdigidoc.so
ln -sf /usr/lib/libdigidoc.so.2.0.1 /usr/lib/libdigidoc.so.2

%preun

%postun

%files
%defattr(-,root,root) 
%{_bindir}/cdigidoc
%config %{_sysconfdir}/digidoc.conf

%{_includedir}/libdigidoc

%{_libdir}/libdigidoc*
%{_libdir}/pkgconfig/libdigidoc.pc
%{_datadir}/libdigidoc/certs
%{_datadir}/libdigidoc/DigiDoci-vorming-1.3.pdf
%{_datadir}/libdigidoc/DigiDocLib-2.2.1.pdf
%{_datadir}/libdigidoc/digidoc_1.3.xsd

%changelog
* Thu Jan 19 2006 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.2.4
* Wed Apr 20 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.1.11
* Thu Apr 07 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.1.9
* Thu Mar 31 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.1.8
* Sun Mar 27 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.1.7
* Fri Mar 25 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- fixed displaying debug info
- added possibility to send debug info to a log file
* Mon Mar 21  2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- fixed a hash code calculation problem
* Sun Mar 13 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- fixed some padding and interoperability problems
* Thu Mar 10 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.1.0
* Sun Feb 27 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to version 2.0.0
* Wed Feb 09 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- ported to RedHat Enterprise Server 4
* Thu Feb 03 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- some bugfixes
* Mon Jan 31 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- some bugfixes
* Sat Jan 15 2005 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to release 1.99.0
* Thu Dec 30 2004 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to release 1.98.0
* Tue Dec 14 2004 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to release 1.97.0
* Sun Nov 21 2004 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to release 1.95.0
* Fri Sep 10 2004 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- updated to release 1.92.0
* Sat Jun 12 2004 Veiko Sinivee <veiko.sinivee@solo.delfi.ee>
- creation of RPM spec

