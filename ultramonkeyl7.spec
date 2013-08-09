%define l7vs_moddir	%{_libdir}/l7vs
%define l7vs_logdir	%{_localstatedir}/log/l7vs
%define l7vs_includedir %{_includedir}/l7vs
%define l7vsadm_sockdir	%{_localstatedir}/run/l7vs
%define l7vs_buffer_size 4096

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
Requires: apache-log4cxx >= 0.10.0
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

%define hb2_tempdir	/usr/share/doc/%{name}-%{version}/heartbeat-ra
%define mibs_tempdir	/usr/share/doc/%{name}-%{version}/mibs
%define moduledevel_tempdir	/usr/share/doc/%{name}-%{version}/moduledevel

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
./configure \
	--prefix=%{_prefix} \
	--sysconfdir=%{_sysconfdir} \
	--localstatedir=%{_localstatedir} \
	--libdir=%{_libdir}

make %{?_smp_mflags}

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
mkdir -p ${RPM_BUILD_ROOT}%{_mandir}/man8
mkdir -p ${RPM_BUILD_ROOT}%{moduledevel_tempdir}
mkdir -p ${RPM_BUILD_ROOT}%{_includedir}

%makeinstall \
	L7VS_CONF_DIR=${RPM_BUILD_ROOT}%{_sysconfdir}/l7vs \
	L7VS_MODDIR=${RPM_BUILD_ROOT}%{l7vs_moddir}

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
%dir %{_sysconfdir}/l7vs
%dir %{_sysconfdir}/ha.d
%config(noreplace) %{_sysconfdir}/l7vs/*
%config(noreplace) %{_sysconfdir}/ha.d/*
%dir %{l7vsadm_sockdir}
%dir %{_docdir}/%{name}-%{version}
%dir %{hb2_tempdir}
%doc %{hb2_tempdir}/*
%dir %{mibs_tempdir}
%doc %{mibs_tempdir}/*
%{_mandir}/man8/*
%config(noreplace) %{_sysconfdir}/logrotate.d/l7directord

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
