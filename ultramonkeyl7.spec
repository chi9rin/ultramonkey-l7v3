%define l7vs_moddir	%{_libdir}/l7vs
%define l7vs_logdir	%{_localstatedir}/log/l7vs
%define l7vs_includedir %{_includedir}/l7vs
%define l7vsadm_sockdir	%{_localstatedir}/run/l7vs
%define l7vs_maxvs	64

Summary: The Layer-7 Virtual Server
Name: ultramonkeyl7
Version: 3.1.0
Release: devel%{?dist}
License: LGPLv2.1
Group: System Environment/Daemons
URL: http://sourceforge.jp/projects/ultramonkey-l7/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildRequires: glib2-devel
AutoReqProv: no
Requires: boost >= 1.41.0
Requires: log4cxx >= 0.10.0
Requires: apr
Requires: apr-util
Requires: openssl
Requires: net-snmp
Requires: perl
Requires: perl-libwww-perl
Requires: perl-Crypt-SSLeay
Requires: perl-Net-SSLeay
Requires: perl-IO-Socket-SSL
Requires: perl-IO-Socket-INET6

%define hb2_tempdir	/usr/share/doc/%{name}-%{version}-%{release}/heartbeat-ra
%define mibs_tempdir	/usr/share/doc/%{name}-%{version}-%{release}/mibs
%define moduledevel_tempdir	/usr/share/doc/%{name}-%{version}-%{release}/moduledevel

%description
Layer-7 load balancing daemon

%package devel
Summary:        Header files for UltraMonkeyl7's module
Group:          Development/Libraries
Requires:       %{name} = %{version}-%{release}

%description devel
This is the development package that provides header files
for UltraMonkeyl7's module.

%prep
%setup -q

%build
%configure
make

%install
rm -rf ${RPM_BUILD_ROOT}

mkdir -p ${RPM_BUILD_ROOT}%{_sbindir}
mkdir -p -m 755 ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs
mkdir -p ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/sslproxy
mkdir -p ${RPM_BUILD_ROOT}%{_sysconfdir}/ha.d/conf
mkdir -p ${RPM_BUILD_ROOT}%{_sysconfdir}/rc.d/init.d
mkdir -p -m 755 ${RPM_BUILD_ROOT}%{l7vs_moddir}
mkdir -p ${RPM_BUILD_ROOT}%{l7vs_logdir}
mkdir -p ${RPM_BUILD_ROOT}%{l7vsadm_sockdir}
mkdir -p ${RPM_BUILD_ROOT}%{hb2_tempdir}
mkdir -p ${RPM_BUILD_ROOT}%{mibs_tempdir}
mkdir -p ${RPM_BUILD_ROOT}%{moduledevel_tempdir}
mkdir -p ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample
mkdir -p ${RPM_BUILD_ROOT}%{l7vs_includedir}

# bin
install -c -m 755 -D l7vsd/src/l7vsd ${RPM_BUILD_ROOT}%{_sbindir}/l7vsd
install -c -m 755 -D l7vsd/src/l7vsadm ${RPM_BUILD_ROOT}%{_sbindir}/l7vsadm
install -c -m 755 -D l7directord/l7directord ${RPM_BUILD_ROOT}%{_sbindir}/l7directord

# lib
install -c -m 755 -D l7vsd/module/protocol/.libs/libprotomod_sslid.so ${RPM_BUILD_ROOT}%{l7vs_moddir}/protomod_sslid.so
install -c -m 755 -D l7vsd/module/protocol/.libs/libprotomod_ip.so ${RPM_BUILD_ROOT}%{l7vs_moddir}/protomod_ip.so
install -c -m 755 -D l7vsd/module/protocol/.libs/libprotomod_sessionless.so ${RPM_BUILD_ROOT}%{l7vs_moddir}/protomod_sessionless.so
install -c -m 755 -D l7vsd/module/schedule/.libs/libsched_lc.so ${RPM_BUILD_ROOT}%{l7vs_moddir}/sched_lc.so
install -c -m 755 -D l7vsd/module/schedule/.libs/libsched_rr.so ${RPM_BUILD_ROOT}%{l7vs_moddir}/sched_rr.so
install -c -m 755 -D l7vsd/module/schedule/.libs/libsched_wrr.so ${RPM_BUILD_ROOT}%{l7vs_moddir}/sched_wrr.so

# initscripts
install -c -m 755 -D l7directord/init.d/l7directord ${RPM_BUILD_ROOT}%{_sysconfdir}/rc.d/init.d/l7directord
install -c -m 755 -D l7vsd/init.d/l7vsd ${RPM_BUILD_ROOT}%{_sysconfdir}/rc.d/init.d/l7vsd

# config
install -c -m 644 -D doc/conf/l7vs.cf ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/l7vs.cf
install -c -m 644 -D doc/conf/l7directord.cf.sample ${RPM_BUILD_ROOT}%{_sysconfdir}/ha.d/conf/l7directord.cf.sample
install -c -m 644 -D doc/conf/sslproxy.target.cf ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/sslproxy/sslproxy.target.cf
install -c -m 644 -D doc/sslfiles/root.pem ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/sslproxy/root.pem
install -c -m 644 -D doc/sslfiles/server.pem ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/sslproxy/server.pem
install -c -m 644 -D doc/sslfiles/dh512.pem ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/sslproxy/dh512.pem
install -c -m 644 -D doc/sslfiles/passwd.txt ${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs/sslproxy/passwd.txt

# heartbeat2 config
install -c -m 644 -D doc/heartbeat-ra/logd.cf ${RPM_BUILD_ROOT}%{hb2_tempdir}/logd.cf
install -c -m 644 -D doc/heartbeat-ra/ha.cf ${RPM_BUILD_ROOT}%{hb2_tempdir}/ha.cf
install -c -m 600 -D doc/heartbeat-ra/authkeys ${RPM_BUILD_ROOT}%{hb2_tempdir}/authkeys
install -c -m 600 -D doc/heartbeat-ra/cib.xml-sample ${RPM_BUILD_ROOT}%{hb2_tempdir}/cib.xml
install -c -m 755 -D doc/heartbeat-ra/L7vsd ${RPM_BUILD_ROOT}%{hb2_tempdir}/L7vsd
install -c -m 755 -D doc/heartbeat-ra/L7directord ${RPM_BUILD_ROOT}%{hb2_tempdir}/L7directord
install -c -m 755 -D doc/heartbeat-ra/VIPcheck ${RPM_BUILD_ROOT}%{hb2_tempdir}/VIPcheck

# mib file
install -c -m 644 -D doc/mibs/ULTRAMONKEY-L7-MIB.txt ${RPM_BUILD_ROOT}%{mibs_tempdir}/ULTRAMONKEY-L7-MIB.txt

# header for devel
install -c -m 644 -D l7vsd/include/protocol_module_base.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/protocol_module_base.h
install -c -m 644 -D l7vsd/include/schedule_module_base.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/schedule_module_base.h
install -c -m 644 -D l7vsd/include/module_base.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/module_base.h
install -c -m 644 -D l7vsd/include/utility.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/utility.h
install -c -m 644 -D l7vsd/include/logger.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/logger.h
install -c -m 644 -D l7vsd/include/logger_enum.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/logger_enum.h
install -c -m 644 -D l7vsd/include/trapmessage.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/trapmessage.h
install -c -m 644 -D l7vsd/include/error_code.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/error_code.h
install -c -m 644 -D l7vsd/include/atomic.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/atomic.h
install -c -m 644 -D l7vsd/include/wrlock.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/wrlock.h
install -c -m 644 -D l7vsd/include/replication.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/replication.h
install -c -m 644 -D l7vsd/include/realserver.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/realserver.h
install -c -m 644 -D l7vsd/include/realserver_element.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/realserver_element.h
install -c -m 644 -D l7vsd/include/endpoint.h ${RPM_BUILD_ROOT}%{l7vs_includedir}/endpoint.h
install -c -m 644 -D doc/moduledevel/sample/protocol/README ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/README
install -c -m 644 -D doc/moduledevel/sample/protocol/Makefile.am ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/Makefile.am
install -c -m 644 -D doc/moduledevel/sample/protocol/configure.in ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/configure.in
install -c -m 644 -D doc/moduledevel/sample/protocol/http_protocol_module_base.cpp ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/http_protocol_module_base.cpp
install -c -m 644 -D doc/moduledevel/sample/protocol/http_protocol_module_base.h ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/http_protocol_module_base.h
install -c -m 644 -D doc/moduledevel/sample/protocol/protocol_module_simple.cpp ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/protocol_module_simple.cpp
install -c -m 644 -D doc/moduledevel/sample/protocol/protocol_module_simple.h ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/protocol/protocol_module_simple.h
install -c -m 644 -D doc/moduledevel/sample/schedule/README ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/schedule/README
install -c -m 644 -D doc/moduledevel/sample/schedule/Makefile.am ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/schedule/Makefile.am
install -c -m 644 -D doc/moduledevel/sample/schedule/configure.in ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/schedule/configure.in
install -c -m 644 -D doc/moduledevel/sample/schedule/schedule_module_rnd.cpp ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/schedule/schedule_module_rnd.cpp
install -c -m 644 -D doc/moduledevel/sample/schedule/schedule_module_rnd.h ${RPM_BUILD_ROOT}%{moduledevel_tempdir}/sample/schedule/schedule_module_rnd.h

%clean
rm -rf $RPM_BUILD_ROOT

%post
chkconfig --add l7vsd
chkconfig --add l7directord

%preun
if [ $1 = 0 ]; then
	/sbin/service l7directord stop > /dev/null 2>&1
	/sbin/service l7vsd stop > /dev/null 2>&1
	chkconfig --del l7vsd
	chkconfig --del l7directord
fi

%files
%defattr(-, root, root, 0755)
%{_sbindir}/*
%{l7vs_moddir}/*
%{_sysconfdir}/rc.d/init.d/*
%dir %{l7vs_moddir}
%dir %{l7vs_logdir}
%config(noreplace) %{_sysconfdir}/l7vs/*
%config(noreplace) %{_sysconfdir}/ha.d/*
%dir %{_sysconfdir}/l7vs
%dir %{_sysconfdir}/ha.d
%dir %{l7vsadm_sockdir}
%dir %{hb2_tempdir}
%config(noreplace) %{hb2_tempdir}/*
%dir %{mibs_tempdir}
%config(noreplace) %{mibs_tempdir}/*

%files devel
%defattr(-, root, root, 0755)
%dir %{_includedir}/l7vs
%{_includedir}/l7vs/protocol_module_base.h
%{_includedir}/l7vs/schedule_module_base.h
%{_includedir}/l7vs/module_base.h
%{_includedir}/l7vs/utility.h
%{_includedir}/l7vs/logger.h
%{_includedir}/l7vs/logger_enum.h
%{_includedir}/l7vs/trapmessage.h
%{_includedir}/l7vs/error_code.h
%{_includedir}/l7vs/atomic.h
%{_includedir}/l7vs/wrlock.h
%{_includedir}/l7vs/replication.h
%{_includedir}/l7vs/realserver.h
%{_includedir}/l7vs/realserver_element.h
%{_includedir}/l7vs/endpoint.h
%dir %{moduledevel_tempdir}
%{moduledevel_tempdir}/*

%changelog
* Sat Sep 8 2012 HIBARI Michiro  <l05102@shibaura-it.ac.jp> 3.1.0-devel
- Update for 3.1.0-devel
- Change spec file for generate devel package.

* Fri Aug 31 2012 Hiroaki Nakano  <nakano.hiroaki@nttcom.co.jp> 3.0.4-2
- Update for 3.0.4-2

* Mon Aug 6 2012 Takeda Kenji <takeda.kenji@nttcom.co.jp> 3.0.4-1
- Update for 3.0.4-1

* Wed Aug 1 2012 Hiroaki Nakano <nakano.hiroaki@nttcom.co.jp> 3.0.4-rc1
- Update for 3.0.4-rc1

* Thu Dec 20 2011 HIBARI Michiro <hibari.michirou@nttcom.co.jp> 3.0.3-1
- Update for 3.0.3-1

* Thu Feb 10 2011 HIBARI Michiro <hibari.michirou@nttcom.co.jp> 3.0.2-1
- Update for 3.0.2-1

* Mon Jan 24 2011 HIBARI Michiro <hibari.michirou@nttcom.co.jp> 3.0.1-1
- Update for 3.0.1-1

* Thu Jan 14 2010 TATEISHI Katsuyuki <kt@wheel.jp> 3.0.0-1
- Update for 3.0.0-1

* Sat Aug 22 2009 TATEISHI Katsuyuki <kt@wheel.jp> 2.1.3-0
- Update for 2.1.3-0

* Thu Aug  6 2009 TATEISHI Katsuyuki <kt@wheel.jp> 2.1.2-2atfix
- rpmlint free
