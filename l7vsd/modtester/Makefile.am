AUTOMAKE_OPTIONS	= foreign
sbin_PROGRAMS		= modtester

modtester_CPPFLAGS		= \
			-O0 -g -Wall -Werror -pthread \
			-I../include \
			-DVERSION=\"$(VERSION)\" \
			-DMAX_BUFFER_SIZE=4096

modtester_SOURCES	=	\
			module_base.h \
			module_control_base.h \
			protocol_module_base.h \
			protocol_module_control.h \
			realserver.h \
			realserver_element.h \
			schedule_module_base.h \
			schedule_module_control.h \
			data_creater.h \
			../src/protocol_module_control.cpp \
			../src/schedule_module_control.cpp \
			random_device.cpp \
			data_creater.cpp \
			modtester.cpp

modtester_LDADD	= 	\
			../src/libl7vsd_logger.a \
			../src/libl7vsd_parameter.a \
			-lrt \
			-llog4cxx \
			-lboost_thread-gcc41-mt \
			-lboost_filesystem-gcc41-mt \
			-lboost_system-gcc41-mt \
			-lboost_unit_test_framework-gcc41-mt
