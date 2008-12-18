noinst_LIBRARIES= liblogger.a
AUTOMAKE_OPTIONS = foreign
		
liblogger_a_SOURCES	= \
			logger_enum.h \
			logger.h \
			logger_impl.h \
			logger.cpp \
			logger_impl.cpp \
			strict_time_based_rolling_policy.h \
			strict_time_based_rolling_policy.cpp \
			time_and_size_based_rolling_policy.h \
			time_and_size_based_rolling_policy.cpp

liblogger_a_CPPFLAGS	= -O2 -g -I../include -I../parameter \
			-DVERSION=\"$(VERSION)\"
