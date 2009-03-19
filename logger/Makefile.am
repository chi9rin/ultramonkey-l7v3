noinst_LIBRARIES= libl7vsd_logger.a libl7vsadm_logger.a libsnmpagent_logger.a libsslproxy_logger.a 
AUTOMAKE_OPTIONS = foreign
		
libl7vsd_logger_a_SOURCES	= \
			logger_enum.h \
			logger_rotation_enum.h \
			logger.h \
			logger_impl.h \
			logger.cpp \
			logger_impl.cpp \
			strict_time_based_rolling_policy.h \
			strict_time_based_rolling_policy.cpp \
			time_and_size_based_rolling_policy.h \
			time_and_size_based_rolling_policy.cpp

libl7vsd_logger_a_CPPFLAGS	= \
			-fno-strict-aliasing -O2 -g -Wall -Werror -pthread -I../include -I../parameter \
			-DLOGGER_PROCESS_VSD \
			-DVERSION=\"$(VERSION)\"

libl7vsadm_logger_a_SOURCES	= \
			logger_enum.h \
			logger_rotation_enum.h \
			logger.h \
			logger_impl.h \
			logger.cpp \
			logger_impl.cpp \
			strict_time_based_rolling_policy.h \
			strict_time_based_rolling_policy.cpp \
			time_and_size_based_rolling_policy.h \
			time_and_size_based_rolling_policy.cpp

libl7vsadm_logger_a_CPPFLAGS	= \
			-fno-strict-aliasing -O2 -g -Wall -Werror -pthread -I../include -I../parameter \
			-DLOGGER_PROCESS_ADM \
			-DVERSION=\"$(VERSION)\"

libsnmpagent_logger_a_SOURCES	= \
			logger_enum.h \
			logger_rotation_enum.h \
			logger.h \
			logger_impl.h \
			logger.cpp \
			logger_impl.cpp \
			strict_time_based_rolling_policy.h \
			strict_time_based_rolling_policy.cpp \
			time_and_size_based_rolling_policy.h \
			time_and_size_based_rolling_policy.cpp

libsnmpagent_logger_a_CPPFLAGS	= \
			-fno-strict-aliasing -O2 -g -Wall -Werror -pthread -I../include -I../parameter \
			-DLOGGER_PROCESS_SNM \
			-DVERSION=\"$(VERSION)\"

libsslproxy_logger_a_SOURCES	= \
			logger_enum.h \
			logger_rotation_enum.h \
			logger.h \
			logger_impl.h \
			logger.cpp \
			logger_impl.cpp \
			strict_time_based_rolling_policy.h \
			strict_time_based_rolling_policy.cpp \
			time_and_size_based_rolling_policy.h \
			time_and_size_based_rolling_policy.cpp

libsslproxy_logger_a_CPPFLAGS	= \
			-fno-strict-aliasing -O2 -g -Wall -Werror -pthread -I../include -I../parameter \
			-DLOGGER_PROCESS_SSL \
			-DVERSION=\"$(VERSION)\"
