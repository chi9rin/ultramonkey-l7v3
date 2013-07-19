AUTOMAKE_OPTIONS = foreign

UML7_VERSION = @ultramonkeyl7_version@
MODDEV_TEMPDIR = @prefix@/share/doc/${UML7_VERSION}/moduledevel

install:
	install -c -m 644 -D ./README ${MODDEV_TEMPDIR}/sample/schedule/README
	install -c -m 644 -D ./Makefile.am.sample ${MODDEV_TEMPDIR}/sample/schedule/Makefile.am
	install -c -m 644 -D ./configure.in ${MODDEV_TEMPDIR}/sample/schedule/configure.in
	install -c -m 644 -D ./schedule_module_rnd.cpp ${MODDEV_TEMPDIR}/sample/schedule/schedule_module_rnd.cpp
	install -c -m 644 -D ./schedule_module_rnd.h ${MODDEV_TEMPDIR}/sample/schedule/schedule_module_rnd.h
