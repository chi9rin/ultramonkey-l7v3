%define l7vs_moddir	%{_libdir}/l7vs
%define l7vs_logdir	%{_localstatedir}/log/l7vs
%define l7vsadm_sockdir	%{_localstatedir}/run/l7vs
%define l7vs_maxvs	64

Summary: The Layer-7 Virtual Server
Name: ultramonkeyl7
Version: 3.0.0
Release: 1
License: LGPLv2
Group: System Environment/Daemons
URL: http://ultramonkey-l7.sourceforge.jp/
Source0: %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
BuildRequires: glib2-devel
AutoReqProv: no

%define hb2_tempdir	/usr/share/doc/%{name}-%{version}-%{release}/heartbeat-ra

%description
Layer-7 load balancing daemon

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

%changelog
* Thu Jan 14 2010 TATEISHI Katsuyuki <kt@wheel.jp> 3.0.0-1
- Update for 3.0.0-1

* Sat Aug 22 2009 TATEISHI Katsuyuki <kt@wheel.jp> 2.1.3-0
- Update for 2.1.3-0

* Thu Aug  6 2009 TATEISHI Katsuyuki <kt@wheel.jp> 2.1.2-2atfix
- rpmlint free
