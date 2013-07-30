AUTOMAKE_OPTIONS = foreign

UML7_VERSION = @ultramonkeyl7_version@
MODDEV_TEMPDIR = ${prefix}/share/doc/${UML7_VERSION}/moduledevel

install:
	install -c -m 644 -D ./README ${MODDEV_TEMPDIR}/sample/protocol/README
	install -c -m 644 -D ./Makefile.am.sample ${MODDEV_TEMPDIR}/sample/protocol/Makefile.am
	install -c -m 644 -D ./configure.in ${MODDEV_TEMPDIR}/sample/protocol/configure.in
	install -c -m 644 -D ./http_protocol_module_base.cpp ${MODDEV_TEMPDIR}/sample/protocol/http_protocol_module_base.cpp
	install -c -m 644 -D ./http_protocol_module_base.h ${MODDEV_TEMPDIR}/sample/protocol/http_protocol_module_base.h
	install -c -m 644 -D ./protocol_module_simple.cpp ${MODDEV_TEMPDIR}/sample/protocol/protocol_module_simple.cpp
	install -c -m 644 -D ./protocol_module_simple.h ${MODDEV_TEMPDIR}/sample/protocol/protocol_module_simple.h
