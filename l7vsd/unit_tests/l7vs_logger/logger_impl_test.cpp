
#include <iostream>
#include "../../../l7vsd/src/logger_impl.cpp"
#include <boost/test/unit_test.hpp>
#include <log4cxx/appender.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/hierarchy.h>
#include <dlfcn.h>
#include <limits.h>
#include "parameter_stub/parameter.h"
#include "logger_enum.h"
#include <boost/test/included/unit_test.hpp>


using namespace boost::unit_test_framework;

using namespace l7vs;

extern void* _start;    //開始位置
extern void* _etext;    //終了位置

#define BUF_LEN (4096)

typedef	std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
		category_level_map_type;


int resetConfigurationCalled = 0;

int getRootLoggerCalled = 0;
bool getRootLoggerReturnNull = false;

int patternLayoutCtorCalled = 0;
void* patternLayoutCreated = 0;
bool patternLayoutNewFail = false;

int fileAppenderCtorCalled = 0;
void* fileAppenderCreated = 0;
bool fileAppenderNewFail = false;
std::string fileAppenderCtorArgFilename = "";
log4cxx::LayoutPtr fileAppenderCtorArgLayout = 0;
int fileAppenderSetFileCalled = 0;
std::string fileAppenderSetFileArgFilename = "";
bool fileAppenderSetFileArgAppend = false;
bool fileAppenderSetFileArgBufferedIO = false;
size_t fileAppenderSetFileArgBufferSize = 0;
void* fileAppenderSetFileArgPool = 0;
bool fileAppenderSetFileException = false;

int poolCtorCalled = 0;
void* poolCreated = 0;

int syslogAppenderCtorCalled = 0;
void* syslogAppenderCreated = 0;
bool syslogAppenderNewFail = false;
log4cxx::LayoutPtr syslogAppenderCtorArgLayout = 0;
int syslogAppenderCtorArgFacility = 0;

int loggerAddAppenderCalled = 0;
log4cxx::AppenderPtr loggerAddAppenderArg[2] = {0, 0};

int loggerForcedLogCalled = 0;
log4cxx::LevelPtr loggerForcedLogArgLevel = 0;
std::string loggerForcedLogArgMessage = "";
char loggerForcedLogArgFile[256];
int loggerForcedLogArgLine = 0;
bool loggerForcedLogEnabled = true;
bool loggerForcedLogException = false;

int loggerGetLoggerCalled = 0;
std::string loggerGetLoggerArgName = "";
bool loggerGetLoggerReturnNull = false;

int fixedWindowCtorCalled = 0;
void* fixedWindowCreated = 0;
bool fixedWindowNewFail = false;

int sizeBaseCtorCalled = 0;
void* sizeBaseCreated = 0;
bool sizeBaseNewFail = false;

int strictPolicyCtorCalled = 0;
void* strictPolicyCreated = 0;
bool strictPolicyNewFail = false;

int timeSizePolicyCtorCalled = 0;
void* timeSizePolicyCreated = 0;
bool timeSizePolicyNewFail = false;

int rollingAppenderCtorCalled = 0;
void* rollingAppenderCreated = 0;
bool rollingAppenderNewFail = false;

int loggerGetLevelCalled = 0;
int loggerSetLevelCalled = 0;

int snmpChangeLevelCalled = 0;
int snmpChangeLevelArgCat = 0;
int snmpChangeLevelArgLevel = 0;

int snmpGetLevelCalled = 0;
LOG_CATEGORY_TAG snmpGetLevelArgCat;
LOG_LEVEL_TAG snmpGetLevelReturns;

int appenderSkeletonSetLayoutCalled = 0;
void* appenderSkeletonSetLayoutArgLayout = 0;

int rollingAppenderSkeletonActivateOptionsCalled = 0;

int hierarchySetConfiguredCalled = 0;
/*
extern "C"
{
void resetConfiguration_() {
	resetConfigurationCalled = 1;
	((void (*)()) dlsym(RTLD_NEXT, "_ZN7log4cxx10LogManager18resetConfigurationEv"))();
}

log4cxx::LoggerPtr getRootLogger_() {
	getRootLoggerCalled = 1;
	if (getRootLoggerReturnNull) {
		return 0;
	}
	return ((log4cxx::LoggerPtr (*)()) dlsym(RTLD_NEXT, "_ZN7log4cxx6Logger13getRootLoggerEv"))();
}

log4cxx::PatternLayout* patternLayoutCtor1_(log4cxx::PatternLayout* ts, const std::string& pattern) {
	patternLayoutCtorCalled = 1;
	patternLayoutCreated = ts;
	return ((log4cxx::PatternLayout* (*)(log4cxx::PatternLayout*, const std::string&)) dlsym(RTLD_NEXT, "_ZN7log4cxx13PatternLayoutC1ERKSs"))(ts, pattern);
}

log4cxx::FileAppender* fileAppenderCtor_(log4cxx::FileAppender* ts, const log4cxx::LayoutPtr& layout, const std::string& filename) {
	++fileAppenderCtorCalled;
	fileAppenderCtorArgFilename = filename;
	fileAppenderCtorArgLayout = layout;
	fileAppenderCreated = ts;
	return ((log4cxx::FileAppender* (*)(log4cxx::FileAppender*, const log4cxx::LayoutPtr&, const std::string&)) dlsym(RTLD_NEXT, "_ZN7log4cxx12FileAppenderC1ERKNS_7helpers10ObjectPtrTINS_6LayoutEEERKSs"))(ts, layout, filename);
}

void fileAppenderSetFile_(log4cxx::FileAppender* ts, log4cxx::LogString& file, bool append, bool bufferedIO, size_t bufferSize, log4cxx::helpers::Pool& p) {
	++fileAppenderSetFileCalled;
	fileAppenderSetFileArgFilename = file;
	fileAppenderSetFileArgAppend = append;
	fileAppenderSetFileArgBufferedIO = bufferedIO;
	fileAppenderSetFileArgBufferSize = bufferSize;
	fileAppenderSetFileArgPool = &p;
	if (fileAppenderSetFileException) {
		throw log4cxx::helpers::IOException();
	}
	((void (*)(log4cxx::FileAppender*, log4cxx::LogString&, bool, bool, size_t, log4cxx::helpers::Pool&)) dlsym(RTLD_NEXT, "_ZN7log4cxx12FileAppender7setFileERKSsbbmRNS_7helpers4PoolE"))(ts, file, append, bufferedIO, bufferSize, p);
}

log4cxx::net::SyslogAppender* syslogAppenderCtor_(log4cxx::net::SyslogAppender* ts, const log4cxx::LayoutPtr& layout, int syslogFacility) {
	++syslogAppenderCtorCalled;
	syslogAppenderCtorArgLayout = layout;
	syslogAppenderCtorArgFacility = syslogFacility;
	syslogAppenderCreated = ts;
	return ((log4cxx::net::SyslogAppender* (*)(log4cxx::net::SyslogAppender*, const log4cxx::LayoutPtr&, int)) dlsym(RTLD_NEXT, "_ZN7log4cxx3net14SyslogAppenderC1ERKNS_7helpers10ObjectPtrTINS_6LayoutEEEi"))(ts, layout, syslogFacility);
}

void loggerAddAppender_(log4cxx::Logger* ts, const log4cxx::AppenderPtr& newAppender) {
	if (2 > loggerAddAppenderCalled) {
		loggerAddAppenderArg[loggerAddAppenderCalled] = newAppender;
	}
	++loggerAddAppenderCalled;
	((void (*)(log4cxx::Logger*, const log4cxx::AppenderPtr)) dlsym(RTLD_NEXT, "_ZN7log4cxx6Logger11addAppenderERKNS_7helpers10ObjectPtrTINS_8AppenderEEE"))(ts, newAppender);
}

log4cxx::helpers::Pool* poolCtor_(log4cxx::helpers::Pool* ts) {
	++poolCtorCalled;
	poolCreated = ts;
	return ((log4cxx::helpers::Pool* (*)(log4cxx::helpers::Pool*)) dlsym(RTLD_NEXT, "_ZN7log4cxx7helpers4PoolC1Ev"))(ts);
}

void loggerForcedLog_(log4cxx::Logger* ts, const log4cxx::LevelPtr& level, const std::string& message, const log4cxx::spi::LocationInfo location) {
	++loggerForcedLogCalled;
	loggerForcedLogArgLevel = level;
	loggerForcedLogArgMessage = message;
	strncpy(loggerForcedLogArgFile, location.getFileName(), sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = location.getLineNumber();
	if (loggerForcedLogException) {
		throw log4cxx::helpers::IOException();
	}
	if (loggerForcedLogEnabled) {
		((void (*)(log4cxx::Logger*, const log4cxx::LevelPtr&, const std::string&, const log4cxx::spi::LocationInfo&)) dlsym(RTLD_NEXT, "_ZNK7log4cxx6Logger9forcedLogERKNS_7helpers10ObjectPtrTINS_5LevelEEERKSsRKNS_3spi12LocationInfoE"))(ts, level, message, location);
	}
}

log4cxx::LoggerPtr loggerGetLogger_(const char* name) {
	++loggerGetLoggerCalled;
	loggerGetLoggerArgName = name;
	if (loggerGetLoggerReturnNull) {
		return 0;
	}
	return ((log4cxx::LoggerPtr (*)(const std::string&)) dlsym(RTLD_NEXT, "_ZN7log4cxx6Logger9getLoggerERKSs"))(name);
}

log4cxx::rolling::FixedWindowRollingPolicy* fixedWindowCtor_(log4cxx::rolling::FixedWindowRollingPolicy* ts) {
	++fixedWindowCtorCalled;
	fixedWindowCreated = ts;
	return ((log4cxx::rolling::FixedWindowRollingPolicy* (*)(log4cxx::rolling::FixedWindowRollingPolicy*)) dlsym(RTLD_NEXT, "_ZN7log4cxx7rolling24FixedWindowRollingPolicyC1Ev"))(ts);
}

log4cxx::rolling::SizeBasedTriggeringPolicy* sizeBaseCtor_(log4cxx::rolling::SizeBasedTriggeringPolicy* ts) {
	++sizeBaseCtorCalled;
	sizeBaseCreated = ts;
	return ((log4cxx::rolling::SizeBasedTriggeringPolicy* (*)(log4cxx::rolling::SizeBasedTriggeringPolicy*)) dlsym(RTLD_NEXT, "_ZN7log4cxx7rolling25SizeBasedTriggeringPolicyC1Ev"))(ts);
}

log4cxx::rolling::RollingFileAppender* rollingAppenderCtor_(log4cxx::rolling::RollingFileAppender* ts) {
	++rollingAppenderCtorCalled;
	rollingAppenderCreated = ts;
	return ((log4cxx::rolling::RollingFileAppender* (*)(log4cxx::rolling::RollingFileAppender*)) dlsym(RTLD_NEXT, "_ZN7log4cxx7rolling19RollingFileAppenderC1Ev"))(ts);
}

log4cxx::LevelPtr loggerGetLevel_(log4cxx::Logger* ts) {
	++loggerGetLevelCalled;
	return ((log4cxx::LevelPtr (*)(log4cxx::Logger*)) dlsym(RTLD_NEXT, "_ZNK7log4cxx6Logger8getLevelEv"))(ts);
}

void loggerSetLevel_(log4cxx::Logger* ts, const log4cxx::LevelPtr& level) {
	++loggerSetLevelCalled;
	((void (*)(log4cxx::Logger*, const log4cxx::LevelPtr&)) dlsym(RTLD_NEXT, "_ZN7log4cxx6Logger8setLevelERKNS_7helpers10ObjectPtrTINS_5LevelEEE"))(ts, level);
}

void appenderSkeletonSetLayout_(log4cxx::AppenderSkeleton* ts, const log4cxx::LayoutPtr& layout1) {
	++appenderSkeletonSetLayoutCalled;
	appenderSkeletonSetLayoutArgLayout = layout1;
	((void (*)(log4cxx::AppenderSkeleton*, const log4cxx::LayoutPtr&)) dlsym(RTLD_NEXT, "_ZN7log4cxx16AppenderSkeleton9setLayoutERKNS_7helpers10ObjectPtrTINS_6LayoutEEE"))(ts, layout1);
}

void rollingAppenderSkeletonActivateOptions_(log4cxx::rolling::RollingFileAppenderSkeleton* ts, log4cxx::helpers::Pool& p) {
	++rollingAppenderSkeletonActivateOptionsCalled;
	((void (*)(log4cxx::rolling::RollingFileAppenderSkeleton*, log4cxx::helpers::Pool&)) dlsym(RTLD_NEXT, "_ZN7log4cxx7rolling27RollingFileAppenderSkeleton15activateOptionsERNS_7helpers4PoolE"))(ts, p);
}

void hirarchySetConfigured_(log4cxx::Hierarchy* ts, bool configured) {
	++hierarchySetConfiguredCalled;
	((void (*)(log4cxx::Hierarchy*, bool)) dlsym(RTLD_NEXT, "_ZN7log4cxx9Hierarchy13setConfiguredEb"))(ts, configured);
	
}

void _ZN7log4cxx10LogManager18resetConfigurationEv() __attribute__((weak,alias("resetConfiguration_")));

log4cxx::LoggerPtr _ZN7log4cxx6Logger13getRootLoggerEv() __attribute__((weak,alias("getRootLogger_")));

log4cxx::PatternLayout* _ZN7log4cxx13PatternLayoutC1ERKSs(log4cxx::PatternLayout* ts, const std::string& pattern) __attribute__((weak,alias("patternLayoutCtor1_")));

log4cxx::FileAppender* _ZN7log4cxx12FileAppenderC1ERKNS_7helpers10ObjectPtrTINS_6LayoutEEERKSs(log4cxx::FileAppender* ts, const log4cxx::LayoutPtr& layout, const std::string& filename) __attribute__((weak,alias("fileAppenderCtor_")));

void _ZN7log4cxx12FileAppender7setFileERKSsbbmRNS_7helpers4PoolE(log4cxx::LogString& file, bool append, bool bufferedIO, size_t bufferSize, log4cxx::helpers::Pool& p) __attribute__((weak,alias("fileAppenderSetFile_")));

log4cxx::net::SyslogAppender* _ZN7log4cxx3net14SyslogAppenderC1ERKNS_7helpers10ObjectPtrTINS_6LayoutEEEi(log4cxx::net::SyslogAppender* ts, const log4cxx::LayoutPtr& layout, int syslogFacility) __attribute__((weak,alias("syslogAppenderCtor_")));

void _ZN7log4cxx6Logger11addAppenderERKNS_7helpers10ObjectPtrTINS_8AppenderEEE(log4cxx::Logger* ts, const log4cxx::AppenderPtr& newAppender) __attribute__((weak,alias("loggerAddAppender_")));

log4cxx::helpers::Pool* _ZN7log4cxx7helpers4PoolC1Ev() __attribute__((weak,alias("poolCtor_")));

void _ZNK7log4cxx6Logger9forcedLogERKNS_7helpers10ObjectPtrTINS_5LevelEEERKSsRKNS_3spi12LocationInfoE(log4cxx::Logger* ts, const log4cxx::LevelPtr& level, const std::string& message, const log4cxx::spi::LocationInfo& location) __attribute__((weak,alias("loggerForcedLog_")));

log4cxx::LoggerPtr _ZN7log4cxx6Logger9getLoggerEPKc(const char* name) __attribute__((weak,alias("loggerGetLogger_")));


log4cxx::rolling::FixedWindowRollingPolicy* _ZN7log4cxx7rolling24FixedWindowRollingPolicyC1Ev(log4cxx::rolling::FixedWindowRollingPolicy* ts) __attribute__((weak,alias("fixedWindowCtor_")));

log4cxx::rolling::SizeBasedTriggeringPolicy* _ZN7log4cxx7rolling25SizeBasedTriggeringPolicyC1Ev(log4cxx::rolling::SizeBasedTriggeringPolicy* ts) __attribute__((weak,alias("sizeBaseCtor_")));

log4cxx::rolling::RollingFileAppender* _ZN7log4cxx7rolling19RollingFileAppenderC1Ev(log4cxx::rolling::RollingFileAppender* ts) __attribute__((weak,alias("rollingAppenderCtor_")));

log4cxx::LevelPtr _ZNK7log4cxx6Logger8getLevelEv(log4cxx::Logger* ts) __attribute__((weak,alias("loggerGetLevel_")));

void _ZN7log4cxx6Logger8setLevelERKNS_7helpers10ObjectPtrTINS_5LevelEEE(log4cxx::Logger* ts, const log4cxx::LevelPtr& level) __attribute__((weak,alias("loggerSetLevel_")));


void _ZN7log4cxx16AppenderSkeleton9setLayoutERKNS_7helpers10ObjectPtrTINS_6LayoutEEE(log4cxx::AppenderSkeleton* ts, const log4cxx::LayoutPtr& layout1) __attribute__((weak,alias("appenderSkeletonSetLayout_")));

void _ZN7log4cxx7rolling27RollingFileAppenderSkeleton15activateOptionsERNS_7helpers4PoolE(log4cxx::rolling::RollingFileAppenderSkeleton* ts, log4cxx::helpers::Pool& p) __attribute__((weak,alias("rollingAppenderSkeletonActivateOptions_")));

void _ZN7log4cxx9Hierarchy13setConfiguredEb(log4cxx::Hierarchy* ts, bool configured) __attribute__((weak,alias("hirarchySetConfigured_")));


}

int l7vs_snmpbridge_change_loglevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level)
{
	++snmpChangeLevelCalled;
	snmpChangeLevelArgCat = cat;
	snmpChangeLevelArgLevel = level;
	return 0;
}

LOG_LEVEL_TAG l7vs_snmpbridge_get_loglevel( LOG_CATEGORY_TAG cat )
{
	++snmpGetLevelCalled;
	snmpGetLevelArgCat = cat;
	return snmpGetLevelReturns;
}



void* operator new(size_t size) {
	if (patternLayoutNewFail && sizeof(log4cxx::PatternLayout) == size) {
		throw std::bad_alloc();
	}
	else if (fileAppenderNewFail && sizeof(log4cxx::FileAppender) == size) {
		throw std::bad_alloc();
	}
	else if (syslogAppenderNewFail && sizeof(log4cxx::net::SyslogAppender) == size) {
		throw std::bad_alloc();
	}
	else if (fixedWindowNewFail && sizeof(log4cxx::rolling::FixedWindowRollingPolicy) == size) {
		throw std::bad_alloc();
	}
	else if (sizeBaseNewFail && sizeof(log4cxx::rolling::SizeBasedTriggeringPolicy) == size) {
		throw std::bad_alloc();
	}
	else if (strictPolicyNewFail && sizeof(log4cxx::rolling::StrictTimeBasedRollingPolicy) == size) {
		throw std::bad_alloc();
	}
	else if (timeSizePolicyNewFail && sizeof(log4cxx::rolling::TimeAndSizeBasedRollingPolicy) == size) {
		throw std::bad_alloc();
	}
	else if (rollingAppenderNewFail && sizeof(log4cxx::rolling::RollingFileAppender) == size) {
		throw std::bad_alloc();
	}
	
	return malloc(size);
}

void operator delete(void* pv) {
	free(pv);
}
*/
//template <typename T> T& operator++( T& value ){
//	value = static_cast<T>( value+1 );
//	return value;
//}

//l7vs::Parameter l7vs::Parameter::instance;

class LoggerImplTest : public l7vs::LoggerImpl
{
public:
	LoggerImplTest() {}
    
	virtual const log4cxx::LevelPtr toLevel(LOG_LEVEL_TAG level) { return LoggerImpl::toLevel(level); }
    
//	virtual LOG_LEVEL_TAG toLevelTag(log4cxx::LevelPtr level) { return LoggerImpl::toLevelTag(level); }
    
	virtual bool init() { return LoggerImpl::init(); }
    
	virtual void errorConf(unsigned int message_id, const std::string& message, const char* file, int line)
		{ LoggerImpl::errorConf(message_id, message, file, line); }
    
	virtual void loadConf() { return LoggerImpl::loadConf(); }
    
	bool getInitialized() { return initialized; }
    
	void setInitialized(bool f) { initialized = f; }
    
	std::string getLogFilename() { return normal_log_property.log_filename_value; }
    
	LOG_ROTATION_TAG getRotation() { return normal_log_property.rotation_value; }
    
	unsigned int getMaxBackupIndex() { return normal_log_property.max_backup_index_value; }
    
	unsigned long long getMaxFileSize() { return normal_log_property.max_file_size_value; }
    
	LOG_ROTATION_TIMING_TAG getRotationTiming() { return normal_log_property.rotation_timing_value; }
    
	std::string getRotationTimingValue() { return normal_log_property.rotation_timing_value_value; }
    
	int getLevelTable(int key) { return levelTable[key]; }
    
	const char *getCategoryTable(LOG_CATEGORY_TAG cat) {
		return category_name_map[cat].c_str(); 
	}
    
	LOG_CATEGORY_TAG getLoggerCategory(const char *key) { return name_category_map[std::string(key)]; }
    
	LOG_LEVEL_TAG getLoggerCategoryLevel(LOG_CATEGORY_TAG key) { return category_level_map[key]; }
    
	category_level_map_type getLoggerCategoryMapType() { return category_level_map; }
    
//	LOG_MODULE_TAG getLoggerProcess() { return loggerProcess; }
    
	const char* getHostname() { return hostname.c_str(); }
    
	void putLogFatal(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char* file, int line)
		{ LoggerImpl::putLogFatal(cat, message_id, message, file, line); }
        void logic_error( const unsigned int logno, const std::string& str, const char* file , const unsigned int line)
        { LoggerImpl::logic_error( logno, str, file , line); }

};

void logger_impl_constructor_test()
{
	BOOST_MESSAGE( "----- logger_impl_constructor_test start -----" );

	LoggerImplTest implTest;

        // unit_test[1]  constructor call(member variable initialize 1)
	//＊initalizedが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getInitialized(), false);
	//＊logfilenameが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getLogFilename(), "");
	//＊rotationが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getRotation(), LOG_ROT_SIZE);
	//＊maxBackupIndexが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getMaxBackupIndex(), 0U);
	//＊maxFileSizeが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getMaxFileSize(), 0U);
	//＊rotationTimingが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getRotationTiming(), LOG_TIM_YEAR);
	//＊rotationTimingValueが初期化されていること
	BOOST_CHECK_EQUAL(implTest.getRotationTimingValue(), "");

        // unit_test[2]  constructor call(member variable initialize 2)
	//＊levelTableが初期化されていること
        int loglevel = log4cxx::Level::DEBUG_INT;
	BOOST_CHECK_EQUAL(implTest.getLevelTable(LOG_LV_NONE), loglevel);
	loglevel = log4cxx::Level::DEBUG_INT;	
	BOOST_CHECK_EQUAL(implTest.getLevelTable(LOG_LV_DEBUG), loglevel);
	loglevel = log4cxx::Level::INFO_INT;	
	BOOST_CHECK_EQUAL(implTest.getLevelTable(LOG_LV_INFO), loglevel);
	loglevel = log4cxx::Level::WARN_INT;	
	BOOST_CHECK_EQUAL(implTest.getLevelTable(LOG_LV_WARN), loglevel);
	loglevel = log4cxx::Level::ERROR_INT;	
	BOOST_CHECK_EQUAL(implTest.getLevelTable(LOG_LV_ERROR), loglevel);
	loglevel = log4cxx::Level::FATAL_INT;	
	BOOST_CHECK_EQUAL(implTest.getLevelTable(LOG_LV_FATAL), loglevel);

	//＊logger_categoryが初期化されていること
#if defined(LOGGER_PROCESS_VSD)
        // unit_test[3]  constructor call(member variable initialize 3)
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_network"), LOG_CAT_L7VSD_NETWORK);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_network_qos"), LOG_CAT_L7VSD_NETWORK_QOS);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_network_bandwidth"), LOG_CAT_L7VSD_NETWORK_BANDWIDTH);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_network_num_connection"), LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_network_access"), LOG_CAT_L7VSD_NETWORK_ACCESS);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_mainthread"), LOG_CAT_L7VSD_MAINTHREAD);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_virtualservice"), LOG_CAT_L7VSD_VIRTUALSERVICE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_virtualservice_thread"), LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_session"), LOG_CAT_L7VSD_SESSION);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_session_thread"), LOG_CAT_L7VSD_SESSION_THREAD);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_realserver"), LOG_CAT_L7VSD_REALSERVER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_sorryserver"), LOG_CAT_L7VSD_SORRYSERVER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_module"), LOG_CAT_L7VSD_MODULE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_replication"), LOG_CAT_L7VSD_REPLICATION);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_replication_sendthread"), LOG_CAT_L7VSD_REPLICATION_SENDTHREAD);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_parameter"), LOG_CAT_L7VSD_PARAMETER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_logger"), LOG_CAT_L7VSD_LOGGER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_command"), LOG_CAT_L7VSD_COMMAND);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_start_stop"), LOG_CAT_L7VSD_START_STOP);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_system"), LOG_CAT_L7VSD_SYSTEM);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_system_memory"), LOG_CAT_L7VSD_SYSTEM_MEMORY);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_system_endpoint"), LOG_CAT_L7VSD_SYSTEM_ENDPOINT);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_system_signal"), LOG_CAT_L7VSD_SYSTEM_SIGNAL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_system_environment"), LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_snmpbridge"), LOG_CAT_L7VSD_SNMPBRIDGE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_protocol"), LOG_CAT_PROTOCOL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsd_schedule"), LOG_CAT_SCHEDULE);


        // unit_test[4]  constructor call(member variable initialize 4)
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_NETWORK), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_NETWORK_QOS), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_NETWORK_BANDWIDTH), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_NETWORK_ACCESS), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_MAINTHREAD), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SESSION), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SESSION_THREAD), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_REALSERVER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SORRYSERVER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_MODULE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_REPLICATION), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_REPLICATION_SENDTHREAD), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_PARAMETER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_LOGGER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_COMMAND), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_START_STOP), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SYSTEM), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SYSTEM_MEMORY), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SYSTEM_ENDPOINT), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SYSTEM_SIGNAL), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_SNMPBRIDGE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_PROTOCOL), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SCHEDULE), LOG_LV_DEBUG);
	
        // unit_test[5]  constructor call(member variable initialize 5)
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_NETWORK), "l7vsd_network");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_NETWORK_QOS), "l7vsd_network_qos");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_NETWORK_BANDWIDTH), "l7vsd_network_bandwidth");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION), "l7vsd_network_num_connection");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_NETWORK_ACCESS), "l7vsd_network_access");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_MAINTHREAD), "l7vsd_mainthread");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE), "l7vsd_virtualservice");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD), "l7vsd_virtualservice_thread");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SESSION), "l7vsd_session");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SESSION_THREAD), "l7vsd_session_thread");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_REALSERVER), "l7vsd_realserver");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SORRYSERVER), "l7vsd_sorryserver");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_MODULE), "l7vsd_module");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_REPLICATION), "l7vsd_replication");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_REPLICATION_SENDTHREAD), "l7vsd_replication_sendthread");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_PARAMETER), "l7vsd_parameter");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_LOGGER), "l7vsd_logger");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_COMMAND), "l7vsd_command");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_START_STOP), "l7vsd_start_stop");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SYSTEM), "l7vsd_system");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SYSTEM_MEMORY), "l7vsd_system_memory");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SYSTEM_ENDPOINT), "l7vsd_system_endpoint");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SYSTEM_SIGNAL), "l7vsd_system_signal");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT), "l7vsd_system_environment");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSD_SNMPBRIDGE), "l7vsd_snmpbridge");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_PROTOCOL), "l7vsd_protocol");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SCHEDULE), "l7vsd_schedule");

#elif defined(LOGGER_PROCESS_ADM)

        // unit_test[6]  constructor call(member variable initialize 6)
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_parse"), LOG_CAT_L7VSADM_PARSE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_operate"), LOG_CAT_L7VSADM_OPERATE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_config_result"), LOG_CAT_L7VSADM_CONFIG_RESULT);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_common"), LOG_CAT_L7VSADM_COMMON);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_logger"), LOG_CAT_L7VSADM_LOGGER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_parameter"), LOG_CAT_L7VSADM_PARAMETER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_module"), LOG_CAT_L7VSADM_MODULE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_protocol"), LOG_CAT_PROTOCOL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("l7vsadm_schedule"), LOG_CAT_SCHEDULE);


        // unit_test[7]  constructor call(member variable initialize 7)
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARSE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_OPERATE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_CONFIG_RESULT), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_LOGGER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARAMETER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_MODULE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_PROTOCOL), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SCHEDULE), LOG_LV_DEBUG);

        // unit_test[8]  constructor call(member variable initialize 8)
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARSE), "l7vsadm_parse");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_OPERATE), "l7vsadm_operate");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_CONFIG_RESULT), "l7vsadm_config_result");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON), "l7vsadm_common");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_LOGGER), "l7vsadm_logger");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARAMETER), "l7vsadm_parameter");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_L7VSADM_MODULE), "l7vsadm_module");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_PROTOCOL), "l7vsadm_protocol");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SCHEDULE), "l7vsadm_schedule");

#else

        // unit_test[10]  constructor call(member variable initialize 10)
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_start_stop"), LOG_CAT_SNMPAGENT_START_STOP);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_manager_receive"), LOG_CAT_SNMPAGENT_MANAGER_RECEIVE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_manager_send"), LOG_CAT_SNMPAGENT_MANAGER_SEND);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_l7vsd_receive"), LOG_CAT_SNMPAGENT_L7VSD_RECEIVE);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_l7vsd_send"), LOG_CAT_SNMPAGENT_L7VSD_SEND);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_logger"), LOG_CAT_SNMPAGENT_LOGGER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_parameter"), LOG_CAT_SNMPAGENT_PARAMETER);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_system"), LOG_CAT_SNMPAGENT_SYSTEM);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_system_memory"), LOG_CAT_SNMPAGENT_SYSTEM_MEMORY);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_system_endpoint"), LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_system_signal"), LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategory("snmpagent_system_environment"), LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT);

        // unit_test[11]  constructor call(member variable initialize 11)
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_START_STOP), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_MANAGER_RECEIVE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_MANAGER_SEND), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_SEND), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_LOGGER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_PARAMETER), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM_MEMORY), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT), LOG_LV_DEBUG);

        // unit_test[12]  constructor call(member variable initialize 12)
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_START_STOP), "snmpagent_start_stop");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_MANAGER_RECEIVE), "snmpagent_manager_receive");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_MANAGER_SEND), "snmpagent_manager_send");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), "snmpagent_l7vsd_receive");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_SEND), "snmpagent_l7vsd_send");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_LOGGER), "snmpagent_logger");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_PARAMETER), "snmpagent_parameter");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM), "snmpagent_system");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM_MEMORY), "snmpagent_system_memory");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT), "snmpagent_system_endpoint");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL), "snmpagent_system_signal");
	BOOST_CHECK_EQUAL(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT), "snmpagent_system_environment");

#endif

	//＊logger_processが初期化されていること
//#if defined(LOGGER_PROCESS_VSD)
//	BOOST_CHECK_EQUAL(implTest.getLoggerProcess(), LOG_MOD_L7VSD);
//#elif defined(LOGGER_PROCESS_ADM)
//	BOOST_CHECK_EQUAL(implTest.getLoggerProcess(), LOG_MOD_L7VSADM);
//#else
//	BOOST_CHECK_EQUAL(implTest.getLoggerProcess(), LOG_MOD_SNMPAGENT);
//#endif

	BOOST_MESSAGE( "----- logger_impl_constructor_test end -----" );
}

void logger_impl_get_instance_test()
{
	BOOST_MESSAGE( "----- logger_impl_get_instance_test start -----" );

        //＊複数回のgetInstanceを呼んで同じものがくるか
	l7vs::LoggerImpl& impl1 = l7vs::LoggerImpl::getInstance();
	void* first = &impl1;
	l7vs::LoggerImpl& impl2 = l7vs::LoggerImpl::getInstance();
	void* second = &impl2;
	 BOOST_CHECK_EQUAL( first, second );

	BOOST_MESSAGE( "----- logger_impl_get_instance_test end -----" );
}

void logger_impl_to_level_test()
{
	BOOST_MESSAGE( "----- logger_impl_to_level_test start -----" );

	LoggerImplTest implTest;

	//＊LOG_LV_NONEを指定した時にLevel::Debugが返るか
	BOOST_CHECK_EQUAL(implTest.toLevel(LOG_LV_NONE), log4cxx::Level::getDebug());
	//＊LOG_LV_DEBUGを指定した時にLevel::Debugが返るか
	BOOST_CHECK_EQUAL(implTest.toLevel(LOG_LV_DEBUG), log4cxx::Level::getDebug());
	//＊LOG_LV_INFOを指定した時にLevel::Infoが返るか
	BOOST_CHECK_EQUAL(implTest.toLevel(LOG_LV_INFO), log4cxx::Level::getInfo());
	//＊LOG_LV_WARNを指定した時にLevel::Warnが返るか
	BOOST_CHECK_EQUAL(implTest.toLevel(LOG_LV_WARN), log4cxx::Level::getWarn());
	//＊LOG_LV_ERRORを指定した時にLevel::Errorが返るか
	BOOST_CHECK_EQUAL(implTest.toLevel(LOG_LV_ERROR), log4cxx::Level::getError());
	//＊LOG_LV_FATALを指定した時にLevel::Fatalが返るか
	BOOST_CHECK_EQUAL(implTest.toLevel(LOG_LV_FATAL), log4cxx::Level::getFatal());

	BOOST_MESSAGE( "----- logger_impl_to_level_test end -----" );
}

/*
void logger_impl_to_level_tag_test()
{
	BOOST_MESSAGE( "----- logger_impl_to_level_tag_test start -----" );

	LoggerImplTest implTest;

	//＊Level::Debugを指定した時にLOG_LV_DEBUGが返るか
	BOOST_CHECK_EQUAL(implTest.toLevelTag(log4cxx::Level::getDebug()), LOG_LV_DEBUG);
	//＊Level::Infoを指定した時にLOG_LV_INFOが返るか
	BOOST_CHECK_EQUAL(implTest.toLevelTag(log4cxx::Level::getInfo()), LOG_LV_INFO);
	//＊Level::Warnを指定した時にLOG_LV_WARNが返るか
	BOOST_CHECK_EQUAL(implTest.toLevelTag(log4cxx::Level::getWarn()), LOG_LV_WARN);
	//＊Level::Errorを指定した時にLOG_LV_ERRORが返るか
	BOOST_CHECK_EQUAL(implTest.toLevelTag(log4cxx::Level::getError()), LOG_LV_ERROR);
	//＊Level::Fatalを指定した時にLOG_LV_FATALが返るか
	BOOST_CHECK_EQUAL(implTest.toLevelTag(log4cxx::Level::getFatal()), LOG_LV_FATAL);

	BOOST_MESSAGE( "----- logger_impl_to_level_tag_test end -----" );
}
*/

void logger_impl_init_test()
{
	BOOST_MESSAGE( "----- logger_impl_init_test start -----" );

	bool ret;

	LoggerImplTest implTest;

	//forceLogを行わない
	loggerForcedLogEnabled = false;

	//＊1回目のinitが成功すること
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);

	//＊2回目のinitの場合初期化処理は実行されずtrueが返却されること
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);

	//＊getRootLoggerを呼び出していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	getRootLoggerCalled = 0;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);
	BOOST_CHECK_EQUAL(getRootLoggerCalled, 1);

	//＊getRootLoggerがNULLを返したときにエラーとなること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	getRootLoggerReturnNull = true;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, false);
	getRootLoggerReturnNull = false;

	//＊PatternLayoutを生成していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	patternLayoutCtorCalled = 0;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);
	BOOST_CHECK_EQUAL(patternLayoutCtorCalled, 1);

	//＊PatternLayoutの生成が失敗した時エラーとなること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	patternLayoutNewFail = true;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, false);
	patternLayoutNewFail = false;


	//＊生成したPatternLayoutと"/dev/null"を引数としてFileAppenderを生成していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	fileAppenderCtorCalled = 0;
	fileAppenderCtorArgLayout = 0;
	fileAppenderCtorArgFilename = "";
	patternLayoutCreated = 0;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);
	BOOST_CHECK_EQUAL(fileAppenderCtorCalled, 1);
	BOOST_CHECK_EQUAL(fileAppenderCtorArgLayout, patternLayoutCreated);
	BOOST_CHECK_EQUAL(fileAppenderCtorArgFilename, LOGGER_NULL);

	//＊FileAppenderの生成が失敗した時にエラーとなること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	fileAppenderNewFail = true;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, false);
	fileAppenderNewFail = false;

	//＊FileAppender::setFileを呼び出していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	fileAppenderSetFileCalled = 0;
	fileAppenderSetFileArgFilename = "";
	fileAppenderSetFileArgAppend = false;
	fileAppenderSetFileArgBufferedIO = false;
	fileAppenderSetFileArgBufferSize = 0;
	poolCreated = 0;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgFilename, LOGGER_CONSOLE);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgAppend, true);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferedIO, true);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferSize, (size_t)LOGGER_DEFAULT_BUFFER_SIZE);
	
	//＊FileAppender::setFileの呼び出しで例外が発生した時にエラーとなること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	fileAppenderSetFileException = true;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, false);
	fileAppenderSetFileException = false;

	//＊生成したPatternLayoutとSyslogFacility="USER"を引数にSyslogAppenderを生成していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	syslogAppenderCtorCalled = 0;
	syslogAppenderCtorArgLayout = 0;
	syslogAppenderCtorArgFacility = 0;
	patternLayoutCreated = 0;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);
	BOOST_CHECK_EQUAL(syslogAppenderCtorCalled, 1);
	BOOST_CHECK_EQUAL(syslogAppenderCtorArgLayout, patternLayoutCreated);
	BOOST_CHECK_EQUAL(syslogAppenderCtorArgFacility, 8);

	//＊SyslogAppenderの生成が失敗した時にエラーとなること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	syslogAppenderNewFail = true;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, false);
	syslogAppenderNewFail = false;

	//＊RootLoggerに対して生成したFileAppenderとSyslogAppenderを追加していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);

	log4cxx::LogManager::resetConfiguration();
	loggerForcedLogEnabled = true;

	//ログレベルの設定を初期化していること
	log4cxx::LogManager::resetConfiguration();
	implTest.setInitialized(false);
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);

/*
#if defined(LOGGER_PROCESS_VSD)
	for (LOG_CATEGORY_TAG cat = implTest.getCategoryRangeStart(LOG_MOD_L7VSD); cat <= implTest.getCategoryRangeEnd(LOG_MOD_L7VSD); ++cat) {
#elif defined(LOGGER_PROCESS_ADM)
	for (LOG_CATEGORY_TAG cat = implTest.getCategoryRangeStart(LOG_MOD_L7VSADM); cat <= implTest.getCategoryRangeEnd(LOG_MOD_L7VSADM); ++cat) {
#else	//LOGGER_PROCESS_SNM
	for (LOG_CATEGORY_TAG cat = implTest.getCategoryRangeStart(LOG_MOD_SNMPAGENT); cat <= implTest.getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
#endif
		BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(cat))->getLevel(), log4cxx::Level::getInfo());
	}
*/
	BOOST_MESSAGE( "----- logger_impl_init_test end -----" );
}

void logger_impl_error_conf_test()
{
	BOOST_MESSAGE( "----- logger_error_conf_test end -----" );

	LoggerImplTest implTest;

	//forceLogを行わない
	loggerForcedLogEnabled = false;

#define TEST_MESSAGE "test_message"

	//＊resetConfigurationが呼ばれていること
	resetConfigurationCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(resetConfigurationCalled, 1);

	//＊getRootLoggerを呼び出していること
	getRootLoggerCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(getRootLoggerCalled, 1);

	//＊getRootLoggerがNULLを返したときにreturnしていること(その後のPatternLayoutのコンストラクタが呼ばれていないこと)
	getRootLoggerReturnNull = true;
	patternLayoutCtorCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(patternLayoutCtorCalled, 0);
	getRootLoggerReturnNull = false;
	
	//＊PatternLayoutを生成していること
	patternLayoutCtorCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(patternLayoutCtorCalled, 1);

	//＊PatternLayoutの生成が失敗した時エラーとなること(その後のSyslogAppenderのコンストラクタが呼ばれていないこと)
	patternLayoutNewFail = true;
	syslogAppenderCtorCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(syslogAppenderCtorCalled, 0);
	patternLayoutNewFail = false;
	
	//＊生成したPatternLayoutとSyslogFacility="USER"を引数にSyslogAppenderを生成していること
	syslogAppenderCtorCalled = 0;
	syslogAppenderCtorArgLayout = 0;
	syslogAppenderCtorArgFacility = 0;
	patternLayoutCreated = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(syslogAppenderCtorCalled, 1);
	BOOST_CHECK_EQUAL(syslogAppenderCtorArgLayout, patternLayoutCreated);
	BOOST_CHECK_EQUAL(syslogAppenderCtorArgFacility, 8);

	//＊SyslogAppenderの生成が失敗した時にエラーとなること(その後のFileAppenderのコンストラクタが呼ばれていないこと)
	syslogAppenderNewFail = true;
	fileAppenderCtorCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(fileAppenderCtorCalled, 0);
	syslogAppenderNewFail = false;

	//＊生成したPatternLayoutとLOGGER_CONSOLEを引数としてFileAppenderを生成していること
	fileAppenderCtorCalled = 0;
	fileAppenderCtorArgLayout = 0;
	fileAppenderCtorArgFilename = "";
	patternLayoutCreated = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(fileAppenderCtorCalled, 1);
	BOOST_CHECK_EQUAL(fileAppenderCtorArgLayout, patternLayoutCreated);
	BOOST_CHECK_EQUAL(fileAppenderCtorArgFilename, LOGGER_CONSOLE);

	//＊FileAppenderの生成が失敗した時にエラーとなること(その後のaddAppenderを呼んでいないこと)
	fileAppenderNewFail = true;
	loggerAddAppenderCalled = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 0);
	fileAppenderNewFail = false;

	//＊RootLoggerに対して生成したFileAppenderとSyslogAppenderを追加していること
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	implTest.errorConf(0, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);

	//＊logPutFatalが呼ばれていること(forcedLogが設定した引数で呼ばれていること)
	loggerForcedLogCalled = 0;
	loggerForcedLogArgLevel = 0;
	loggerForcedLogArgMessage = "";
	memset(loggerForcedLogArgFile, 0, sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = 0;
	implTest.errorConf(18, TEST_MESSAGE, __FILE__, 99);
	BOOST_CHECK_EQUAL(loggerForcedLogCalled, 1);
	BOOST_CHECK_EQUAL(loggerForcedLogArgLevel, log4cxx::Level::getFatal());

	char buf[256];
	snprintf(buf, sizeof(buf), "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_FATAL, LOG_CAT_L7VSD_LOGGER, 18, TEST_MESSAGE, implTest.getHostname());
	BOOST_CHECK(!strncmp(buf, loggerForcedLogArgMessage.c_str(), sizeof(buf)));
	
	BOOST_CHECK(!strncmp(loggerForcedLogArgFile, __FILE__, sizeof(loggerForcedLogArgFile)));
	BOOST_CHECK_EQUAL(loggerForcedLogArgLine, 99);

#undef TEST_MESSAGE

	//forceLog回避を解除
	loggerForcedLogEnabled = true;

	BOOST_MESSAGE( "----- logger_error_conf_test end -----" );
}

void logger_impl_error_conf_testi02()
{
	BOOST_MESSAGE( "----- logger_error_conf_test02 end -----" );

	LoggerImplTest implTest;
	std::string testMsg = "testMsg";

        // unit_test[1]  errorConf method call02
	implTest.errorConf( 1, testMsg, __FILE__, __LINE__);	

	BOOST_MESSAGE( "----- logger_error_conf_test02 end -----" );
}

void logger_impl_load_conf_test()
{
	BOOST_MESSAGE( "----- logger_impl_load_conf_test start -----" );

	LoggerImplTest implTest;
	bool ret;

	//forceLogを行わない
	loggerForcedLogEnabled = false;

	//init する
	ret = implTest.init();
	BOOST_CHECK_EQUAL(ret, true);

        Parameter param;

        std::ostringstream oss;

	size_t filesize;

        std::string no_thorw_error_msg = "no throw exception error!!";


#define TEST_LOG_FILE "l7vsd.log"

        // unit_test[1] loadConf method call
	//＊ファイル名を取得すること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getLogFilename(), TEST_LOG_FILE);

        // unit_test[2] loadConf method call
        try {
	//＊ファイル名が存在しない時にエラーとなること
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);
        } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not exist logfilename_key : l7vsd_log_filename");
        }


        // unit_test[3] loadConf method call
	//＊共通　ローテーションパターンを取得すること　サイズ
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotation(), LOG_ROT_SIZE);
	
        // unit_test[4] loadConf method call
	//＊共通　ローテーションパターンを取得すること　日付
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotation(), LOG_ROT_DATE);

        // unit_test[5] loadConf method call
	//＊共通　ローテーションパターンを取得すること　日付サイズ
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotation(), LOG_ROT_DATESIZE);

        // unit_test[6] loadConf method call
        try{
	//＊ローテーションパターンが「size」「date」「datesize」以外の時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "sizedate");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
        
	    BOOST_CHECK_EQUAL(ex.what(), "Invalid Log Rotation Setting : sizedate");

        }

        // unit_test[7] loadConf method call
        try{
	//＊ローテーションパターンが存在しない時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch ( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log Rotation Setting.");
        }

        // unit_test[8] loadConf method call
	//＊共通　最大履歴数を取得すること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getMaxBackupIndex(), 10U);


        // unit_test[9] loadConf method call
        try {
	//＊最大履歴数がUINT_MAXより大きい時エラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	std::ostringstream oss;
	oss << 1LLU + UINT_MAX;
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, oss.str());
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

	} catch( const std::logic_error& ex ) {

            BOOST_CHECK_EQUAL(ex.what(), "Invalid MaxBackupIndex Value : 4294967296.");

        }
        oss.str("");
	oss.clear(std::stringstream::goodbit);

        // unit_test[10] loadConf method call
        try {
	//＊最大履歴数がLOGGER_BACKUP_INDEX_LIMITより大きい時エラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	oss << LOGGER_BACKUP_INDEX_LIMIT + 1;
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, oss.str());
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {

            BOOST_CHECK_EQUAL(ex.what(), "Max Backup Index must at most 12.");

        }

	oss.str("");
	oss.clear(std::stringstream::goodbit);

        // unit_test[11] loadConf method call
        try {
	//＊最大履歴数がマイナスの時エラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "-10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {

            BOOST_CHECK_EQUAL(ex.what(), "Invalid MaxBackupIndex Value : -10.");

        }

        // unit_test[12] loadConf method call
        try {
	//＊最大履歴数が存在しない時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
        implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {

            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log MaxBackupIndex Setting.");

        }


        // unit_test[13] loadConf method call
	//＊サイズ　最大サイズを取得すること　単位なし
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getMaxFileSize(), 1000000U);

        // unit_test[14] loadConf method call
	//＊サイズ　最大サイズを取得すること　K単位
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000K");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getMaxFileSize(), (1000U * 1024));

        // unit_test[15] loadConf method call
	//＊サイズ　最大サイズを取得すること　M単位
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "100M");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getMaxFileSize(), (100U * 1024 * 1024));

        // unit_test[16] loadConf method call
	//＊サイズ　最大サイズを取得すること　G単位
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "2G");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getMaxFileSize(), (2U * 1024 * 1024 * 1024));


        // unit_test[17] loadConf method call
        try {
	//＊最大サイズがマイナスの時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "-100000");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
        }


        // unit_test[18] loadConf method call
        try {
	//＊最大サイズが制限値より少ない時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	oss << LOGGER_FILESIZE_LOWER_LIMIT - 1;
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, oss.str());
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "FileSize must at least 65535 bytes.");
        }

	oss.str("");
	oss.clear(std::stringstream::goodbit);


        // unit_test[19] loadConf method call
        try {
	//＊最大サイズがsize_t max より大きい時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	oss << 1LLU + ULONG_MAX;
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, oss.str());
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "FileSize must at least 65535 bytes.");
        }

	oss.str("");
	oss.clear(std::stringstream::goodbit);

        // unit_test[20] loadConf method call
        try {
	//＊K単位の指定で最大サイズを超えた時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	filesize = (ULONG_MAX / 1024) + 1;
	oss << filesize << "K";
	BOOST_MESSAGE("size=" << oss.str());
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, oss.str());
	implTest.loadConf();
	BOOST_MESSAGE("size=" << implTest.getMaxFileSize());
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
        }


	oss.str("");
	oss.clear(std::stringstream::goodbit);


        // unit_test[21] loadConf method call
        try {
	//＊M単位の指定で最大サイズを超えた時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	filesize = (ULONG_MAX / 1024 / 1024) + 1;
	oss << filesize << "M";
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, oss.str());
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
        }

	oss.str("");
	oss.clear(std::stringstream::goodbit);

        // unit_test[22] loadConf method call
        try{
	//＊G単位の指定で最大サイズを超えた時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	filesize = (ULONG_MAX / 1024 / 1024 / 1024) + 1;
	oss << filesize << "G";
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, oss.str());
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid FileSize Value.");
        }

	oss.str("");
	oss.clear(std::stringstream::goodbit);


        // unit_test[23] loadConf method call
        try {
	//＊最大サイズが存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY);
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log MaxFileSize Setting.");
        }

        // unit_test[24] loadConf method call
	//＊日付単位を取得すること 年
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTiming(), LOG_TIM_YEAR);

        // unit_test[25] loadConf method call
	//＊日付単位を取得すること 月
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "8 15:55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTiming(), LOG_TIM_MONTH);

        // unit_test[26] loadConf method call
	//＊日付単位を取得すること　週 
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "sun 15:55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTiming(), LOG_TIM_WEEK);

        // unit_test[27] loadConf method call
	//＊日付単位を取得すること　日 
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15:55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTiming(), LOG_TIM_DATE);

        // unit_test[28] loadConf method call
	//＊日付単位を取得すること　時 
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hour");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "55");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTiming(), LOG_TIM_HOUR);

        // unit_test[29] loadConf method call
        try {
	//＊日付単位が　「year」「month」「week」「date」「hour」以外のときエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hr");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "55");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Invalid Log RotationTiming Setting.");
        }


        // unit_test[30] loadConf method call
        try {
	//＊日付単位が存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Setting.");
        }


        // unit_test[31] loadConf method call
	//＊ローテーション日時を取得すること　年
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 01:31");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTimingValue(), "05050131");

        // unit_test[32] loadConf method call
        try {	
	//＊ローテーション日時(年)が不正な時　区切り文字無し　すべて無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05050131");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }


        // unit_test[33] loadConf method call
        try {
	//＊ローテーション日時(年)が不正な時　区切り文字無し　日付の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "0505 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[34] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　区切り文字無し　日付、時間の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/0501:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[35] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　区切り文字無し　時間の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 0131");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[36] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値無し　月が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "/05 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[37] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値無し　日が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/ 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[38] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値無し　時間が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 :31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }


        // unit_test[39] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値無し　分が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 01:");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }


        // unit_test[40] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値不正　月が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "a/05 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[41] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値不正　日が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/b 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[42] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 c:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[43] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値不正　分が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 01:d");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[44] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値範囲不正　月が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "13/05 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[45] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値範囲不正　日が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/32 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[46] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 24:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[47] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "05/05 01:60");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[48] loadConf method call
        try{
	//＊ローテーション日時(年)が不正な時　日付不正　存在しない日付
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "04/31 01:31");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Year Error.");
        }

        // unit_test[49] loadConf method call
        try{
	//＊ローテーション日時(年)が存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Year Setting.");
        }



        // unit_test[50] loadConf method call
	//＊ローテーション日時を取得すること　月
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 18:21");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTimingValue(), "151821");


        // unit_test[51] loadConf method call
        try{	
	//＊ローテーション日時(月)が不正な時　区切り文字無し　すべて無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "151821");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[52] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　区切り文字無し　日付、時間の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "1518:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[53] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　区切り文字無し　時間と分の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 1821");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[54] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値無し　日が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, " 18:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[55] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値無し　時間が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 :21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[56] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値無し　分が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 18:");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[57] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値不正　日が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "a 18:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[58] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 b:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[59] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値不正　分が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 18:c");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[60] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値範囲不正　日が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "0 18:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[61] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 -1:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[62] loadConf method call
        try{
	//＊ローテーション日時(月)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "15 18:-15");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Month Error.");
        }

        // unit_test[63] loadConf method call
        try{
	//＊ローテーション日時(月)が存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "month");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Month Setting.");
        }


        // unit_test[64] loadConf method call
	//＊ローテーション日時を取得すること　週
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon 23:59");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTimingValue(), "12359");		//0:日 1:月 2:火 3:水 4:木 5:金 6:土


        // unit_test[65] loadConf method call
        try{	
	//＊ローテーション日時(週)が不正な時　区切り文字無し　すべて無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon2359");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[66] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　区切り文字無し　週、時間の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon23:59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[67] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　区切り文字無し　時間と分の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon 2359");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }


        // unit_test[68] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値無し　週が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, " 23:59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[69] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値無し　時間が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon :59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[70] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値無し　分が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon 23:");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[71] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値不正　週が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "monday 23:59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[72] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon a:59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[73] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値不正　分が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon 23:b");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[74] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値範囲不正　日が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "0 18:21");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[75] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon 99:59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[76] loadConf method call
        try{
	//＊ローテーション日時(週)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "mon 23:100");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Week Error.");
        }

        // unit_test[77] loadConf method call
        try{
	//＊ローテーション日時(週)が存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "week");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Week Setting.");
        }

        // unit_test[78] loadConf method call
	//＊ローテーション日時を取得すること　日
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "12:00");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTimingValue(), "1200");

        // unit_test[79] loadConf method call
        try{	
	//＊ローテーション日時(日)が不正な時　区切り文字無し　時間と分の間の区切りが無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "1200");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[80] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値無し　時間が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, ":00");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[81] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値無し　分が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "12:");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[82] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "a:00");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[83] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値不正　分が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "12:b");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[84] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "-21:00");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[85] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値範囲不正　時間が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "12:-59");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Date Error.");
        }

        // unit_test[86] loadConf method call
        try{
	//＊ローテーション日時(日)が存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "date");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Date Setting.");
        }


        // unit_test[87] loadConf method call
	//＊ローテーション日時を取得すること　時
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hour");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "45");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(implTest.getRotationTimingValue(), "45");

        // unit_test[88] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値無し　分が無し
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hour");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Hour Error.");
        }

        // unit_test[89] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値不正　分が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hour");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "a");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Hour Error.");
        }

        // unit_test[90] loadConf method call
        try{
	//＊ローテーション日時(日)が不正な時　値範囲不正　分が不正
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hour");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "120");
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Parse Timing Hour Error.");
        }

        // unit_test[91] loadConf method call
        try{
	//＊ローテーション日時(日)が存在しなかった時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "hour");
	param.deleteStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);
	implTest.loadConf();
        BOOST_ERROR(no_thorw_error_msg);

        } catch( const std::logic_error& ex ) {
            BOOST_CHECK_EQUAL(ex.what(), "Not Exist Log RotaionTiming Hour Setting.");
        }


	//----applender追加テスト----
/*
	//resetConfigurationを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	resetConfigurationCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(resetConfigurationCalled, 1);

	//＊getRootLoggerを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	getRootLoggerCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(getRootLoggerCalled, 1);

	//＊getRootLoggerがNULLを返したときにエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	getRootLoggerReturnNull = true;
	implTest.loadConf();
	getRootLoggerReturnNull = false;
	
	//＊PatternLayoutを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	patternLayoutCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(patternLayoutCtorCalled, 1);

	//＊PatternLayoutの生成が失敗した時エラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	patternLayoutNewFail = true;
	syslogAppenderCtorCalled = 0;
	implTest.loadConf();
	patternLayoutNewFail = false;

	//----サイズローテーションの時----

	//＊サイズローテーションの時にfixedwindowrollingpolicyを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	fixedWindowCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(fixedWindowCtorCalled, 1);
	

	//＊fixedwindowrollingpolicyの生成に失敗した時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	fixedWindowNewFail = true;
	implTest.loadConf();
	fixedWindowNewFail = false;

	//＊fixedwindowrollingpolicyのsetminindexを1で呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::FixedWindowRollingPolicy*)fixedWindowCreated)->getMinIndex(), 1);

	//＊fixedwindowrollingpolicyのsetmaxindexをmaxbackupindexで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "5");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::FixedWindowRollingPolicy*)fixedWindowCreated)->getMaxIndex(), 5);

	//＊fixedwindowrollingpolicyのsetFilenamepatternにlogFilename + "." + LOGGER_FILE_PATTERNを設定していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	fixedWindowCreated = 0;
	implTest.loadConf();
	std::ostringstream fileoss;
	fileoss << TEST_LOG_FILE << "." << LOGGER_FILE_PATTERN;
	BOOST_CHECK_EQUAL(((log4cxx::rolling::FixedWindowRollingPolicy*)fixedWindowCreated)->getFileNamePattern(), fileoss.str());
	fileoss.str("");
	fileoss.clear(std::stringstream::goodbit);

	//＊サイズローテーションの時にsizebasedtriggeringpolicyを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	sizeBaseCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(sizeBaseCtorCalled, 1);

	//＊sizebasedtriggeringpolicyの生成に失敗したときエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	sizeBaseNewFail = true;
	implTest.loadConf();
	sizeBaseNewFail = false;

	//＊sizebasedtriggeringpolicyのsetmaxfilesizeにmaxfilesizeを設定していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	sizeBaseCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::SizeBasedTriggeringPolicy*)sizeBaseCreated)->getMaxFileSize(), 1000000U);

	//＊rollingfileappenderを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	rollingAppenderCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(rollingAppenderCtorCalled, 1);

	//＊rollingfileappenderの生成に失敗した時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	rollingAppenderNewFail = true;
	implTest.loadConf();
	rollingAppenderNewFail = false;


	//＊rollingfileappenderのlayoutに生成したlayoutを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	appenderSkeletonSetLayoutCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(appenderSkeletonSetLayoutCalled, 1);

	//＊rollingfileappenderのrollingpolicyに生成したfixedwindowrollingpolicyを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	fixedWindowCreated = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::RollingFileAppender*)rollingAppenderCreated)->getRollingPolicy(), fixedWindowCreated);

	//＊rollingfileappenderのtriggeringpolicyに生成したsizebasedtriggeringpolicyを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	sizeBaseCreated = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::RollingFileAppender*)rollingAppenderCreated)->getTriggeringPolicy(), sizeBaseCreated);

	//＊FileAppender::setFileを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	fileAppenderSetFileCalled = 0;
	fileAppenderSetFileArgFilename = "";
	fileAppenderSetFileArgAppend = false;
	fileAppenderSetFileArgBufferedIO = false;
	fileAppenderSetFileArgBufferSize = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(fileAppenderSetFileCalled, 2);

	BOOST_CHECK_EQUAL(fileAppenderSetFileArgFilename, TEST_LOG_FILE);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgAppend, true);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferedIO, false);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferSize, (size_t)LOGGER_DEFAULT_BUFFER_SIZE);

	//＊rollingfileappenderのActivateOptionsを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	rollingAppenderSkeletonActivateOptionsCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(rollingAppenderSkeletonActivateOptionsCalled, 1);


	//＊rootにrollingfileappenderを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 1);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], rollingAppenderCreated);

	//日付ローテーションの時

	//＊日付ローテーションの時にStrictTimeBasedRollingPolicyを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(strictPolicyCtorCalled, 1);

	//＊StrictTimeBasedRollingPolicyの生成に失敗した時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyNewFail = true;
	implTest.loadConf();
	strictPolicyNewFail = false;

	//＊StrictTimeBasedRollingPolicyのsetmaxindexを引数maxbackupindexで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "8");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::StrictTimeBasedRollingPolicy*)strictPolicyCreated)->getMaxIndex(), 8);

	//＊StrictTimeBasedRollingPolicyのsetFilenamepatternにlogFilename + "." + LOGGER_FILE_PATTERNを設定していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyCreated = 0;
	implTest.loadConf();
	fileoss << TEST_LOG_FILE << "." << LOGGER_FILE_PATTERN;
	BOOST_CHECK_EQUAL(((log4cxx::rolling::StrictTimeBasedRollingPolicy*)strictPolicyCreated)->getFileNamePattern(), fileoss.str());
	fileoss.str("");
	fileoss.clear(std::stringstream::goodbit);

	//＊StrictTimeBasedRollingPolicyのsetRotationTimingを引数rotationTimingで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "8");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::StrictTimeBasedRollingPolicy*)strictPolicyCreated)->getRotationTiming(), LOG_TIM_YEAR);

	//＊StrictTimeBasedRollingPolicyのsetRotationTimingValueを引数rotationTimingValueで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "8");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::StrictTimeBasedRollingPolicy*)strictPolicyCreated)->getRotationTimingValue(), "02081555");

	//＊rollingfileappenderを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	rollingAppenderCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(rollingAppenderCtorCalled, 1);

	//＊rollingfileappenderの生成に失敗した時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	rollingAppenderNewFail = true;
	implTest.loadConf();
	rollingAppenderNewFail = false;


	//＊rollingfileappenderのlayoutに生成したlayoutを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	appenderSkeletonSetLayoutCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(appenderSkeletonSetLayoutCalled, 1);


	//＊rollingfileappenderのrollingpolicyに生成したStrictTimeBasedRollingPolicyを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	strictPolicyCreated = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::RollingFileAppender*)rollingAppenderCreated)->getRollingPolicy(), strictPolicyCreated);

	//＊FileAppender::setFileを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	fileAppenderSetFileCalled = 0;
	fileAppenderSetFileArgFilename = "";
	fileAppenderSetFileArgAppend = false;
	fileAppenderSetFileArgBufferedIO = false;
	fileAppenderSetFileArgBufferSize = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(fileAppenderSetFileCalled, 2);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgFilename, TEST_LOG_FILE);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgAppend, true);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferedIO, false);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferSize, (size_t)LOGGER_DEFAULT_BUFFER_SIZE);

	//＊rollingfileappenderのActivateOptionsを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	rollingAppenderSkeletonActivateOptionsCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(rollingAppenderSkeletonActivateOptionsCalled, 1);

	//＊rootにrollingfileappenderを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "date");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 1);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], rollingAppenderCreated);


	//日付+サイズローテーションの時

	//＊日付＊サイズローテーションの時にTimeAndSizeBasedRollingPolicyを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(timeSizePolicyCtorCalled, 1);

	//＊TimeAndSizeBasedRollingPolicyの生成に失敗した時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyNewFail = true;
	implTest.loadConf();
	timeSizePolicyNewFail = false;

	//＊TimeAndSizeBasedRollingPolicyのsetmaxindexを引数maxbackupindexで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "7");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::TimeAndSizeBasedRollingPolicy*)timeSizePolicyCreated)->getMaxIndex(), 7);

	//＊TimeAndSizeBasedRollingPolicyのsetFilenamepatternにlogFilename + "." + LOGGER_FILE_PATTERNを設定していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCreated = 0;
	implTest.loadConf();
	fileoss << TEST_LOG_FILE << "." << LOGGER_FILE_PATTERN;
	BOOST_CHECK_EQUAL(((log4cxx::rolling::TimeAndSizeBasedRollingPolicy*)timeSizePolicyCreated)->getFileNamePattern(), fileoss.str());
	fileoss.str("");
	fileoss.clear(std::stringstream::goodbit);

	//＊TimeAndSizeBasedRollingPolicyのsetMaxFileSizeを引数maxFileSizeで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "12345678");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::TimeAndSizeBasedRollingPolicy*)timeSizePolicyCreated)->getMaxFileSize(), 12345678U);

	//＊TimeAndSizeBasedRollingPolicyのsetRotationTimingを引数rotationTimingで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::TimeAndSizeBasedRollingPolicy*)timeSizePolicyCreated)->getRotationTiming(), LOG_TIM_YEAR);

	//＊TimeAndSizeBasedRollingPolicyのsetRotationTimingValueを引数rotationTimingValueで呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::TimeAndSizeBasedRollingPolicy*)timeSizePolicyCreated)->getRotationTimingValue(), "02081555");

	//＊rollingfileappenderを生成していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	rollingAppenderCtorCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(rollingAppenderCtorCalled, 1);

	//＊rollingfileappenderの生成に失敗した時にエラーとなること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	rollingAppenderNewFail = true;
	implTest.loadConf();
	rollingAppenderNewFail = false;

	//＊rollingfileappenderのlayoutに生成したlayoutを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	appenderSkeletonSetLayoutCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(appenderSkeletonSetLayoutCalled, 1);

	//＊rollingfileappenderのrollingpolicyに生成したTimeAndSizeTimeBasedRollingPolicyを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	timeSizePolicyCreated = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(((log4cxx::rolling::RollingFileAppender*)rollingAppenderCreated)->getRollingPolicy(), timeSizePolicyCreated);

	//＊FileAppender::setFileを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	fileAppenderSetFileCalled = 0;
	fileAppenderSetFileArgFilename = "";
	fileAppenderSetFileArgAppend = false;
	fileAppenderSetFileArgBufferedIO = false;
	fileAppenderSetFileArgBufferSize = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(fileAppenderSetFileCalled, 2);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgFilename, TEST_LOG_FILE);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgAppend, true);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferedIO, false);
	BOOST_CHECK_EQUAL(fileAppenderSetFileArgBufferSize, (size_t)LOGGER_DEFAULT_BUFFER_SIZE);

	//＊rollingfileappenderのActivateOptionsを呼び出していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	rollingAppenderSkeletonActivateOptionsCalled = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(rollingAppenderSkeletonActivateOptionsCalled, 1);

	//＊rootにrollingfileappenderを追加していること
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "datesize");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY, "year");
	param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY, "2/8 15:55");
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	rollingAppenderCreated = 0;
	implTest.loadConf();
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 1);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], rollingAppenderCreated);
*/
/*
	//ログレベルの設定を取得していること
#if defined(LOGGER_PROCESS_VSD)
	for (LOG_CATEGORY_TAG cat = implTest.getCategoryRangeStart(LOG_MOD_L7VSD); cat <= implTest.getCategoryRangeEnd(LOG_MOD_L7VSD); ++cat) {
#elif defined(LOGGER_PROCESS_ADM)
	for (LOG_CATEGORY_TAG cat = implTest.getCategoryRangeStart(LOG_MOD_L7VSADM); cat <= implTest.getCategoryRangeEnd(LOG_MOD_L7VSADM); ++cat) {
#else	//LOGGER_PROCESS_SNM
	for (LOG_CATEGORY_TAG cat = implTest.getCategoryRangeStart(LOG_MOD_SNMPAGENT); cat <= implTest.getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
#endif
 		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
		param.setStringValue(PARAM_COMP_LOGGER, implTest.getCategoryTable(cat), "debug");
		implTest.loadConf();
		BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(cat))->getLevel(), log4cxx::Level::getDebug());

 		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
		param.setStringValue(PARAM_COMP_LOGGER, implTest.getCategoryTable(cat), "info");
		implTest.loadConf();
		BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(cat))->getLevel(), log4cxx::Level::getInfo());

 		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
		param.setStringValue(PARAM_COMP_LOGGER, implTest.getCategoryTable(cat), "warn");
		implTest.loadConf();
		BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(cat))->getLevel(), log4cxx::Level::getWarn());

 		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
		param.setStringValue(PARAM_COMP_LOGGER, implTest.getCategoryTable(cat), "error");
		implTest.loadConf();
		BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(cat))->getLevel(), log4cxx::Level::getError());

 		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY, TEST_LOG_FILE);
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY, "size");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY, "10");
		param.setStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY, "1000000");
		param.setStringValue(PARAM_COMP_LOGGER, implTest.getCategoryTable(cat), "fatal");
		implTest.loadConf();
		BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(cat))->getLevel(), log4cxx::Level::getFatal());
	}
*/
	loggerForcedLogEnabled = true;

#undef TEST_LOG_FILE
  
	BOOST_MESSAGE( "----- logger_impl_load_conf_test end -----" );
}

void logger_impl_get_log_level_test()
{
	BOOST_MESSAGE( "----- logger_impl_get_log_level_test start -----" );

	LoggerImplTest implTest;
	LOG_LEVEL_TAG logLevel;

#if defined(LOGGER_PROCESS_VSD)

        // unit_test[1] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_WARN);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getWarn());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_WARN);

        // unit_test[2] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getError());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_ERROR);

        // unit_test[3] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getFatal());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_FATAL);

        // unit_test[4] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getDebug());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_DEBUG);

        // unit_test[5] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_INFO);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_INFO);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getInfo());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_INFO);

#elif defined(LOGGER_PROCESS_ADM)


        // unit_test[6] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_PARSE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARSE), LOG_LV_WARN);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARSE))->getLevel(), log4cxx::Level::getWarn());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSADM_PARSE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_WARN);

        // unit_test[7] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_PARSE,LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARSE), LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARSE))->getLevel(), log4cxx::Level::getError());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSADM_PARSE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_ERROR);

        // unit_test[8] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_PARSE,LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARSE), LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARSE))->getLevel(), log4cxx::Level::getFatal());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSADM_PARSE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_FATAL);

        // unit_test[9] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_PARSE,LOG_LV_FATAL);
	implTest.setLogLevel(LOG_CAT_L7VSADM_PARSE,LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARSE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARSE))->getLevel(), log4cxx::Level::getDebug());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSADM_PARSE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_DEBUG);

        // unit_test[10] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_PARSE,LOG_LV_INFO);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_PARSE), LOG_LV_INFO);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_PARSE))->getLevel(), log4cxx::Level::getInfo());
	logLevel = implTest.getLogLevel(LOG_CAT_L7VSADM_PARSE);
	BOOST_CHECK_EQUAL(logLevel, LOG_LV_INFO);


#else

        // unit_test[11] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_SNMPAGENT_SYSTEM,LOG_LV_WARN);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM), LOG_LV_WARN);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM))->getLevel(), log4cxx::Level::getWarn());
        logLevel = implTest.getLogLevel(LOG_CAT_SNMPAGENT_SYSTEM);
        BOOST_CHECK_EQUAL(logLevel, LOG_LV_WARN);

        // unit_test[12] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_SNMPAGENT_SYSTEM,LOG_LV_ERROR);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM), LOG_LV_ERROR);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM))->getLevel(), log4cxx::Level::getError());
        logLevel = implTest.getLogLevel(LOG_CAT_SNMPAGENT_SYSTEM);
        BOOST_CHECK_EQUAL(logLevel, LOG_LV_ERROR);

        // unit_test[13] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_SNMPAGENT_SYSTEM,LOG_LV_FATAL);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM), LOG_LV_FATAL);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM))->getLevel(), log4cxx::Level::getFatal());
        logLevel = implTest.getLogLevel(LOG_CAT_SNMPAGENT_SYSTEM);
        BOOST_CHECK_EQUAL(logLevel, LOG_LV_FATAL);

        // unit_test[14] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_SNMPAGENT_SYSTEM,LOG_LV_DEBUG);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM), LOG_LV_DEBUG);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM))->getLevel(), log4cxx::Level::getDebug());
        logLevel = implTest.getLogLevel(LOG_CAT_SNMPAGENT_SYSTEM);
        BOOST_CHECK_EQUAL(logLevel, LOG_LV_DEBUG);

        // unit_test[15] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_SNMPAGENT_SYSTEM,LOG_LV_INFO);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_SYSTEM), LOG_LV_INFO);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_SYSTEM))->getLevel(), log4cxx::Level::getInfo());
        logLevel = implTest.getLogLevel(LOG_CAT_SNMPAGENT_SYSTEM);
        BOOST_CHECK_EQUAL(logLevel, LOG_LV_INFO);


#endif



	BOOST_MESSAGE( "----- logger_impl_get_log_level_test end -----" );
}


void logger_impl_set_get_log_level_all_test()
{
        BOOST_MESSAGE( "----- logger_impl_set_get_log_level_all_test start -----" );

        LoggerImplTest implTest;

        bool set_result;
	category_level_map_type category_map;

        category_level_list_type list;

	//set_result = implTest.setLogLevelAll(LOG_LV_NONE);
        //BOOST_CHECK_EQUAL(set_result,false);

        // unit_test[1] setLogLevelAll getLogLevelAll method call
	set_result = implTest.setLogLevelAll(LOG_LV_WARN);
        BOOST_CHECK_EQUAL(set_result,true);
	category_map = implTest.getLoggerCategoryMapType();
	BOOST_FOREACH( category_level_map_type::value_type& itr, category_map ){
		BOOST_CHECK_EQUAL(itr.second,LOG_LV_WARN);
	}
        list.clear();
        implTest.getLogLevelAll(list);
        BOOST_FOREACH( category_level_list_type::value_type& itr, list ){
                BOOST_CHECK_EQUAL(itr.second,LOG_LV_WARN);
        }

        // unit_test[2] setLogLevelAll getLogLevelAll method call
	set_result = implTest.setLogLevelAll(LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(set_result,true);
	category_map = implTest.getLoggerCategoryMapType();
	BOOST_FOREACH( category_level_map_type::value_type& itr, category_map ){
		BOOST_CHECK_EQUAL(itr.second,LOG_LV_ERROR);
	}
        list.clear();
        implTest.getLogLevelAll(list);
        BOOST_FOREACH( category_level_list_type::value_type& itr, list ){
                BOOST_CHECK_EQUAL(itr.second,LOG_LV_ERROR);
        }

        // unit_test[3] setLogLevelAll getLogLevelAll method call
	set_result = implTest.setLogLevelAll(LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(set_result,true);
	category_map = implTest.getLoggerCategoryMapType();
	BOOST_FOREACH( category_level_map_type::value_type& itr, category_map ){
		BOOST_CHECK_EQUAL(itr.second,LOG_LV_FATAL);
	}
        list.clear();
        implTest.getLogLevelAll(list);
        BOOST_FOREACH( category_level_list_type::value_type& itr, list ){
                BOOST_CHECK_EQUAL(itr.second,LOG_LV_FATAL);
        }

        // unit_test[4] setLogLevelAll getLogLevelAll method call
	set_result = implTest.setLogLevelAll(LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(set_result,true);
	category_map = implTest.getLoggerCategoryMapType();
	BOOST_FOREACH( category_level_map_type::value_type& itr, category_map ){
		BOOST_CHECK_EQUAL(itr.second,LOG_LV_DEBUG);
	}
        list.clear();
        implTest.getLogLevelAll(list);
        BOOST_FOREACH( category_level_list_type::value_type& itr, list ){
                BOOST_CHECK_EQUAL(itr.second,LOG_LV_DEBUG);
        }

        // unit_test[5] setLogLevelAll getLogLevelAll method call
	set_result = implTest.setLogLevelAll(LOG_LV_INFO);
	BOOST_CHECK_EQUAL(set_result,true);
	category_map = implTest.getLoggerCategoryMapType();
	BOOST_FOREACH( category_level_map_type::value_type& itr, category_map ){
		BOOST_CHECK_EQUAL(itr.second,LOG_LV_INFO);
	}
        list.clear();
        implTest.getLogLevelAll(list);
        BOOST_FOREACH( category_level_list_type::value_type& itr, list ){
                BOOST_CHECK_EQUAL(itr.second,LOG_LV_INFO);
        }



        BOOST_MESSAGE( "----- logger_impl_set_get_log_level_all_test end -----" );
}




void logger_impl_set_log_level_test()
{
	BOOST_MESSAGE( "----- logger_impl_set_log_level_test start -----" );

	LoggerImplTest implTest;


#if defined(LOGGER_PROCESS_VSD)
        // unit_test[1] setLogLevel method call
	//＊Logger::setLevelを呼び出してLevelを設定していること
        implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_WARN);	
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_WARN);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getWarn());

        // unit_test[2] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_ERROR);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_ERROR);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getError());

        // unit_test[3] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_FATAL);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_FATAL);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getFatal());

        // unit_test[4] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_DEBUG);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_DEBUG);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getDebug());

        // unit_test[5] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_INFO);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_INFO);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getInfo());

        // unit_test[6] setLogLevel method call
        implTest.setLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE,LOG_LV_WARN);
        BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSD_VIRTUALSERVICE), LOG_LV_WARN);
        BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSD_VIRTUALSERVICE))->getLevel(), log4cxx::Level::getWarn());

#elif defined(LOGGER_PROCESS_ADM)


        // unit_test[7] setLogLevel method call
        //＊Logger::setLevelを呼び出してLevelを設定していること
	implTest.setLogLevel(LOG_CAT_L7VSADM_COMMON,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_WARN);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON))->getLevel(), log4cxx::Level::getWarn());

        // unit_test[8] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_COMMON,LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON))->getLevel(), log4cxx::Level::getError());

        // unit_test[9] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_COMMON,LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON))->getLevel(), log4cxx::Level::getFatal());

        // unit_test[10] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_COMMON,LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON))->getLevel(), log4cxx::Level::getDebug());

        // unit_test[11] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_COMMON,LOG_LV_INFO);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_INFO);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON))->getLevel(), log4cxx::Level::getInfo());

        // unit_test[12] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_L7VSADM_COMMON,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_L7VSADM_COMMON), LOG_LV_WARN);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_L7VSADM_COMMON))->getLevel(), log4cxx::Level::getWarn());


#else

        // unit_test[13] setLogLevel method call
        //＊Logger::setLevelを呼び出してLevelを設定していること
	implTest.setLogLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_WARN);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE))->getLevel(), log4cxx::Level::getWarn());

        // unit_test[14] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_ERROR);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE))->getLevel(), log4cxx::Level::getError());

        // unit_test[15] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_FATAL);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE))->getLevel(), log4cxx::Level::getFatal());

        // unit_test[16] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_DEBUG);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE))->getLevel(), log4cxx::Level::getDebug());

        // unit_test[17] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_INFO);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE))->getLevel(), log4cxx::Level::getInfo());

        // unit_test[18] setLogLevel method call
	implTest.setLogLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,LOG_LV_WARN);
	BOOST_CHECK_EQUAL(implTest.getLoggerCategoryLevel(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE), LOG_LV_WARN);
	BOOST_CHECK_EQUAL(log4cxx::Logger::getLogger(implTest.getCategoryTable(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE))->getLevel(), log4cxx::Level::getWarn());


#endif



	BOOST_MESSAGE( "----- logger_impl_set_log_level_test end -----" );
}


void logger_impl_put_log_fatal_test()
{
	BOOST_MESSAGE( "----- logger_impl_put_log_fatal_test sta -----" );

	LoggerImplTest implTest;
	implTest.init();
	loggerForcedLogEnabled = false;

#define TEST_MESSAGE "test_message"

	//＊log4cxx::Logger::getLoggerを指定したカテゴリを引数に呼び出しいること
	loggerGetLoggerCalled = 0;
	loggerGetLoggerArgName = "";
	loggerGetLoggerReturnNull = false;
	implTest.putLogFatal(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerGetLoggerCalled, 1);
	BOOST_CHECK_EQUAL(loggerGetLoggerArgName, implTest.getCategoryTable(LOG_CAT_L7VSD_LOGGER));

//	getLoggerはNULLを返さない

//	//＊log4cxx::Logger::getLoggerがNULLを返した時にエラーとなりAppenderがSyslog, FileAppender(/dev/console)に切り替わること
//	loggerAddAppenderCalled = 0;
//	loggerAddAppenderArg[0] = 0;
//	loggerAddAppenderArg[1] = 0;
//	fileAppenderCreated = 0;
//	syslogAppenderCreated = 0;
//	loggerGetLoggerReturnNull = true;
//	implTest.putLogFatal(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
//	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
//	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
//	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);
//	loggerGetLoggerReturnNull = false;

	//＊logger::forcedLogを指定した引数で呼び出していること
	loggerForcedLogCalled = 0;
	loggerForcedLogArgLevel = 0;
	loggerForcedLogArgMessage = "";
	memset(loggerForcedLogArgFile, 0, sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = 0;
	implTest.putLogFatal(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, 88);
	BOOST_CHECK_EQUAL(loggerForcedLogCalled, 1);
	BOOST_CHECK_EQUAL(loggerForcedLogArgLevel, log4cxx::Level::getFatal());

	char buf[256];
	snprintf(buf, sizeof(buf), "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_FATAL, LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, implTest.getHostname());
	BOOST_CHECK(!strncmp(buf, loggerForcedLogArgMessage.c_str(), sizeof(buf)));
	
	BOOST_CHECK(!strncmp(loggerForcedLogArgFile, __FILE__, sizeof(loggerForcedLogArgFile)));
	BOOST_CHECK_EQUAL(loggerForcedLogArgLine, 88);

	//＊logger::forcedLogが例外を返した時にエラーとなりAppenderがSyslog, FileAppender(/dev/console)に切り替わること
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	loggerForcedLogException = true;
	implTest.putLogFatal(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);
	loggerForcedLogException = false;

	//＊長いメッセージを渡した時にBUF_LENで切り詰められた文字列を引数に渡していること
	std::string long_str;
	long_str.assign(10000, 'A');
	BOOST_MESSAGE(long_str);
	implTest.putLogFatal(LOG_CAT_L7VSD_LOGGER, 12, long_str, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerForcedLogArgMessage.length(), (std::string::size_type)(BUF_LEN - 1));


#undef TEST_MESSAGE

	BOOST_MESSAGE( "----- logger_impl_put_log_fatal_test end -----" );
}

void logger_impl_put_log_fatal_test02()
{
        BOOST_MESSAGE( "----- logger_impl_put_log_fatal_test02 sta -----" );

        LoggerImplTest implTest;
        implTest.init();
#define TEST_MESSAGE "test_message"

        // unit_test[1] putLogFatal method call02
        // プロトコルモジュール・スケジュールモジュール・それ以外のカテゴリのメッセージが
	// 同じフォーマットで出力されることを確認
	implTest.putLogFatal(LOG_CAT_L7VSD_LOGGER, 41, TEST_MESSAGE, __FILE__, __LINE__); 
	implTest.putLogFatal(LOG_CAT_PROTOCOL, 300068, TEST_MESSAGE, __FILE__, __LINE__); 
	implTest.putLogFatal(LOG_CAT_SCHEDULE, 300068, TEST_MESSAGE, __FILE__, __LINE__); 

#undef TEST_MESSAGE

        BOOST_MESSAGE( "----- logger_impl_put_log_fatal_test02 end -----" );
}


void logger_impl_put_log_error_test()
{
	BOOST_MESSAGE( "----- logger_impl_put_log_error_test sta -----" );

	LoggerImplTest implTest;
	implTest.init();
	loggerForcedLogEnabled = false;

#define TEST_MESSAGE "test_message"

	//＊log4cxx::Logger::getLoggerを指定したカテゴリを引数に呼び出しいること
	loggerGetLoggerCalled = 0;
	loggerGetLoggerArgName = "";
	loggerGetLoggerReturnNull = false;
	implTest.putLogError(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerGetLoggerCalled, 1);
	BOOST_CHECK_EQUAL(loggerGetLoggerArgName, implTest.getCategoryTable(LOG_CAT_L7VSD_LOGGER));

	//＊logger::forcedLogを指定した引数で呼び出していること
	loggerForcedLogCalled = 0;
	loggerForcedLogArgLevel = 0;
	loggerForcedLogArgMessage = "";
	memset(loggerForcedLogArgFile, 0, sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = 0;
	implTest.putLogError(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, 88);
	BOOST_CHECK_EQUAL(loggerForcedLogCalled, 1);
	BOOST_CHECK_EQUAL(loggerForcedLogArgLevel, log4cxx::Level::getError());

	char buf[256];
	snprintf(buf, sizeof(buf), "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_ERROR, LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, implTest.getHostname());
	BOOST_CHECK(!strncmp(buf, loggerForcedLogArgMessage.c_str(), sizeof(buf)));
	
	BOOST_CHECK(!strncmp(loggerForcedLogArgFile, __FILE__, sizeof(loggerForcedLogArgFile)));
	BOOST_CHECK_EQUAL(loggerForcedLogArgLine, 88);

	//＊logger::forcedLogが例外を返した時にエラーとなりAppenderがSyslog, FileAppender(/dev/console)に切り替わること
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	loggerForcedLogException = true;
	implTest.putLogError(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);
	loggerForcedLogException = false;

	//＊長いメッセージを渡した時にBUF_LENで切り詰められた文字列を引数に渡していること
	std::string long_str;
	long_str.assign(10000, 'A');
	BOOST_MESSAGE(long_str);
	implTest.putLogError(LOG_CAT_L7VSD_LOGGER, 12, long_str, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerForcedLogArgMessage.length(), (std::string::size_type)(BUF_LEN - 1));


#undef TEST_MESSAGE

	BOOST_MESSAGE( "----- logger_impl_put_log_error_test end -----" );
}

void logger_impl_put_log_error_test02()
{
        BOOST_MESSAGE( "----- logger_impl_put_log_error_test02 sta -----" );

        LoggerImplTest implTest;
        implTest.init();
#define TEST_MESSAGE "test_message"

        // unit_test[1] putLogError method call02
        // プロトコルモジュール・スケジュールモジュール・それ以外のカテゴリのメッセージが
        // 同じフォーマットで出力されることを確認
        implTest.putLogError(LOG_CAT_L7VSD_LOGGER, 41, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogError(LOG_CAT_PROTOCOL, 300068, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogError(LOG_CAT_SCHEDULE, 300068, TEST_MESSAGE, __FILE__, __LINE__);

#undef TEST_MESSAGE

        BOOST_MESSAGE( "----- logger_impl_put_log_error_test02 end -----" );
}



void logger_impl_put_log_warn_test()
{
	BOOST_MESSAGE( "----- logger_impl_put_log_warn_test sta -----" );

	LoggerImplTest implTest;
	implTest.init();
	loggerForcedLogEnabled = false;

#define TEST_MESSAGE "test_message"

	//＊log4cxx::Logger::getLoggerを指定したカテゴリを引数に呼び出しいること
	loggerGetLoggerCalled = 0;
	loggerGetLoggerArgName = "";
	loggerGetLoggerReturnNull = false;
	implTest.putLogWarn(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerGetLoggerCalled, 1);
	BOOST_CHECK_EQUAL(loggerGetLoggerArgName, implTest.getCategoryTable(LOG_CAT_L7VSD_LOGGER));
/*
	//＊logger::forcedLogを指定した引数で呼び出していること
	loggerForcedLogCalled = 0;
	loggerForcedLogArgLevel = 0;
	loggerForcedLogArgMessage = "";
	memset(loggerForcedLogArgFile, 0, sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = 0;
	implTest.putLogWarn(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, 88);
	BOOST_CHECK_EQUAL(loggerForcedLogCalled, 1);
	BOOST_CHECK_EQUAL(loggerForcedLogArgLevel, log4cxx::Level::getWarn());

	char buf[256];
	snprintf(buf, sizeof(buf), "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_WARN, LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, implTest.getHostname());
	BOOST_CHECK(!strncmp(buf, loggerForcedLogArgMessage.c_str(), sizeof(buf)));
	
	BOOST_CHECK(!strncmp(loggerForcedLogArgFile, __FILE__, sizeof(loggerForcedLogArgFile)));
	BOOST_CHECK_EQUAL(loggerForcedLogArgLine, 88);

	//＊logger::forcedLogが例外を返した時にエラーとなりAppenderがSyslog, FileAppender(/dev/console)に切り替わること
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	loggerForcedLogException = true;
	implTest.putLogWarn(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);
	loggerForcedLogException = false;

	//＊長いメッセージを渡した時にBUF_LENで切り詰められた文字列を引数に渡していること
	std::string long_str;
	long_str.assign(10000, 'A');
	BOOST_MESSAGE(long_str);
	implTest.putLogWarn(LOG_CAT_L7VSD_LOGGER, 12, long_str, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerForcedLogArgMessage.length(), (std::string::size_type)(BUF_LEN - 1));
*/

#undef TEST_MESSAGE

	BOOST_MESSAGE( "----- logger_impl_put_log_warn_test end -----" );
}

void logger_impl_put_log_warn_test02()
{
        BOOST_MESSAGE( "----- logger_impl_put_log_warn_test02 sta -----" );

        LoggerImplTest implTest;
        implTest.init();
#define TEST_MESSAGE "test_message"

        // unit_test[1] putLogError method call02
        // プロトコルモジュール・スケジュールモジュール・それ以外のカテゴリのメッセージが
        // 同じフォーマットで出力されることを確認
        implTest.putLogWarn(LOG_CAT_L7VSD_LOGGER, 41, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogWarn(LOG_CAT_PROTOCOL, 300068, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogWarn(LOG_CAT_SCHEDULE, 300068, TEST_MESSAGE, __FILE__, __LINE__);

#undef TEST_MESSAGE

        BOOST_MESSAGE( "----- logger_impl_put_log_warn_test02 end -----" );
}



void logger_impl_put_log_info_test()
{
	BOOST_MESSAGE( "----- logger_impl_put_log_info_test sta -----" );

	LoggerImplTest implTest;
	implTest.init();
	loggerForcedLogEnabled = false;

#define TEST_MESSAGE "test_message"

	//＊log4cxx::Logger::getLoggerを指定したカテゴリを引数に呼び出しいること
	loggerGetLoggerCalled = 0;
	loggerGetLoggerArgName = "";
	loggerGetLoggerReturnNull = false;
	implTest.putLogInfo(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerGetLoggerCalled, 1);
	BOOST_CHECK_EQUAL(loggerGetLoggerArgName, implTest.getCategoryTable(LOG_CAT_L7VSD_LOGGER));

	//＊logger::forcedLogを指定した引数で呼び出していること
	loggerForcedLogCalled = 0;
	loggerForcedLogArgLevel = 0;
	loggerForcedLogArgMessage = "";
	memset(loggerForcedLogArgFile, 0, sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = 0;
	implTest.putLogInfo(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, 88);
	BOOST_CHECK_EQUAL(loggerForcedLogCalled, 1);
	BOOST_CHECK_EQUAL(loggerForcedLogArgLevel, log4cxx::Level::getInfo());

	char buf[256];
	snprintf(buf, sizeof(buf), "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_INFO, LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, implTest.getHostname());
	BOOST_CHECK(!strncmp(buf, loggerForcedLogArgMessage.c_str(), sizeof(buf)));
	
	BOOST_CHECK(!strncmp(loggerForcedLogArgFile, __FILE__, sizeof(loggerForcedLogArgFile)));
	BOOST_CHECK_EQUAL(loggerForcedLogArgLine, 88);

	//＊logger::forcedLogが例外を返した時にエラーとなりAppenderがSyslog, FileAppender(/dev/console)に切り替わること
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	loggerForcedLogException = true;
	implTest.putLogInfo(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);
	loggerForcedLogException = false;

	//＊長いメッセージを渡した時にBUF_LENで切り詰められた文字列を引数に渡していること
	std::string long_str;
	long_str.assign(10000, 'A');
	BOOST_MESSAGE(long_str);
	implTest.putLogInfo(LOG_CAT_L7VSD_LOGGER, 12, long_str, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerForcedLogArgMessage.length(), (std::string::size_type)(BUF_LEN - 1));


#undef TEST_MESSAGE

	BOOST_MESSAGE( "----- logger_impl_put_log_info_test end -----" );
}

void logger_impl_put_log_info_test02()
{
        BOOST_MESSAGE( "----- logger_impl_put_log_info_test02 sta -----" );

        LoggerImplTest implTest;
        implTest.init();
#define TEST_MESSAGE "test_message"

        // unit_test[1] putLogInfo method call02
        // プロトコルモジュール・スケジュールモジュール・それ以外のカテゴリのメッセージが
        // 同じフォーマットで出力されることを確認
        implTest.putLogInfo(LOG_CAT_L7VSD_LOGGER, 41, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogInfo(LOG_CAT_PROTOCOL, 300068, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogInfo(LOG_CAT_SCHEDULE, 300068, TEST_MESSAGE, __FILE__, __LINE__);

#undef TEST_MESSAGE

        BOOST_MESSAGE( "----- logger_impl_put_log_info_test02 end -----" );
}


void logger_impl_put_log_debug_test()
{
	BOOST_MESSAGE( "----- logger_impl_put_log_debug_test sta -----" );

	LoggerImplTest implTest;
	implTest.init();
	loggerForcedLogEnabled = false;

#define TEST_MESSAGE "test_message"

	//＊log4cxx::Logger::getLoggerを指定したカテゴリを引数に呼び出しいること
	loggerGetLoggerCalled = 0;
	loggerGetLoggerArgName = "";
	loggerGetLoggerReturnNull = false;
	implTest.putLogDebug(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerGetLoggerCalled, 1);
	BOOST_CHECK_EQUAL(loggerGetLoggerArgName, implTest.getCategoryTable(LOG_CAT_L7VSD_LOGGER));

	//＊logger::forcedLogを指定した引数で呼び出していること
	loggerForcedLogCalled = 0;
	loggerForcedLogArgLevel = 0;
	loggerForcedLogArgMessage = "";
	memset(loggerForcedLogArgFile, 0, sizeof(loggerForcedLogArgFile));
	loggerForcedLogArgLine = 0;
	implTest.putLogDebug(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, 88);
	BOOST_CHECK_EQUAL(loggerForcedLogCalled, 1);
	BOOST_CHECK_EQUAL(loggerForcedLogArgLevel, log4cxx::Level::getDebug());

	char buf[256];
	snprintf(buf, sizeof(buf), "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_DEBUG, LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, implTest.getHostname());
	BOOST_CHECK(!strncmp(buf, loggerForcedLogArgMessage.c_str(), sizeof(buf)));
	
	BOOST_CHECK(!strncmp(loggerForcedLogArgFile, __FILE__, sizeof(loggerForcedLogArgFile)));
	BOOST_CHECK_EQUAL(loggerForcedLogArgLine, 88);

	//＊logger::forcedLogが例外を返した時にエラーとなりAppenderがSyslog, FileAppender(/dev/console)に切り替わること
	loggerAddAppenderCalled = 0;
	loggerAddAppenderArg[0] = 0;
	loggerAddAppenderArg[1] = 0;
	fileAppenderCreated = 0;
	syslogAppenderCreated = 0;
	loggerForcedLogException = true;
	implTest.putLogDebug(LOG_CAT_L7VSD_LOGGER, 12, TEST_MESSAGE, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerAddAppenderCalled, 2);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[0], syslogAppenderCreated);
	BOOST_CHECK_EQUAL(loggerAddAppenderArg[1], fileAppenderCreated);
	loggerForcedLogException = false;

	//＊長いメッセージを渡した時にBUF_LENで切り詰められた文字列を引数に渡していること
	std::string long_str;
	long_str.assign(10000, 'A');
	BOOST_MESSAGE(long_str);
	implTest.putLogDebug(LOG_CAT_L7VSD_LOGGER, 12, long_str, __FILE__, __LINE__);
	BOOST_CHECK_EQUAL(loggerForcedLogArgMessage.length(), (std::string::size_type)(BUF_LEN - 1));


#undef TEST_MESSAGE

	BOOST_MESSAGE( "----- logger_impl_put_log_debug_test end -----" );
}

void logger_impl_put_log_debug_test02()
{
        BOOST_MESSAGE( "----- logger_impl_put_log_debug_test02 sta -----" );

        LoggerImplTest implTest;
        implTest.init();
#define TEST_MESSAGE "test_message"

        // unit_test[1] putLogDebug method call02
        // プロトコルモジュール・スケジュールモジュール・それ以外のカテゴリのメッセージが
        // 同じフォーマットで出力されることを確認
        implTest.putLogDebug(LOG_CAT_L7VSD_LOGGER, 41, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogDebug(LOG_CAT_PROTOCOL, 300068, TEST_MESSAGE, __FILE__, __LINE__);
        implTest.putLogDebug(LOG_CAT_SCHEDULE, 300068, TEST_MESSAGE, __FILE__, __LINE__);

#undef TEST_MESSAGE

        BOOST_MESSAGE( "----- logger_impl_put_log_debug_test02 end -----" );
}


void logger_impl_logic_error_test()
{
        BOOST_MESSAGE( "----- logger_impl_logic_error_test start -----" );

        LoggerImplTest implTest;
        std::string ss = "testMsg";

        implTest.init();
        //implTest.putLogDebug( LOG_CAT_L7VSD_VIRTUALSERVICE, 41, ss, __FILE__, __LINE__ );

        // unit_test[1] logic_error method call

        try{
#if	defined(LOGGER_PROCESS_VSD)
	
        implTest.logic_error( 1, ss, __FILE__, __LINE__ );

#elif defined(LOGGER_PROCESS_ADM)

        implTest.logic_error( 1, ss, __FILE__, __LINE__ );

#elif defined(LOGGER_PROCESS_SNM)
	
        implTest.logic_error( 1, ss, __FILE__, __LINE__ );
	
#endif

	}catch(const std::exception& ex){
	    BOOST_CHECK_EQUAL( ex.what() , ss );
	}


        BOOST_MESSAGE( "----- logger_impl_logic_error_test end -----" );

}

/*
void logger_impl_get_categor_range_start_test()
{
	BOOST_MESSAGE( "----- logger_impl_get_category_range_start_test start -----" );

	LoggerImplTest implTest;
	
	//＊l7vsdのstartはLOG_CAT_L7VSD_NETWORKが返ってくるか
	BOOST_CHECK_EQUAL(implTest.getCategoryRangeStart(LOG_MOD_L7VSD), LOG_CAT_L7VSD_NETWORK);
	//＊l7vsadmのstartはLOG_CAT_L7VSADM_PARSEが返ってくるか
	BOOST_CHECK_EQUAL(implTest.getCategoryRangeStart(LOG_MOD_L7VSADM), LOG_CAT_L7VSADM_PARSE);
	//＊snmpagentのstartはLOG_CAT_SNMPAGENT_START_STOPが返ってくるか
	BOOST_CHECK_EQUAL(implTest.getCategoryRangeStart(LOG_MOD_SNMPAGENT), LOG_CAT_SNMPAGENT_START_STOP);

	BOOST_MESSAGE( "----- logger_impl_get_category_range_start_test end -----" );
}

void logger_impl_get_categor_range_end_test()
{
	BOOST_MESSAGE( "----- logger_impl_get_category_range_end_test start -----" );

	LoggerImplTest implTest;
	
	//＊l7vsdのendはLOG_CAT_L7VSD_PROTOCOLが返ってくるか
	BOOST_CHECK_EQUAL(implTest.getCategoryRangeEnd(LOG_MOD_L7VSD), LOG_CAT_L7VSD_MODULE);
	//＊l7vsadmのendはLOG_CAT_L7VSADM_MODULEが返ってくるか
	BOOST_CHECK_EQUAL(implTest.getCategoryRangeEnd(LOG_MOD_L7VSADM), LOG_CAT_L7VSADM_MODULE);
	//＊snmpagentのendはLOG_CAT_SNMPAGENT_L7VSD_SENDが返ってくるか
	BOOST_CHECK_EQUAL(implTest.getCategoryRangeEnd(LOG_MOD_SNMPAGENT), LOG_CAT_SNMPAGENT_PARAMETER);


	BOOST_MESSAGE( "----- logger_impl_get_category_range_end_test end -----" );
}
*/

test_suite*
init_unit_test_suite(int argc, char* argv[])
{

	test_suite* test = BOOST_TEST_SUITE("logger_test");
	test->add(BOOST_TEST_CASE(logger_impl_load_conf_test));
	test->add(BOOST_TEST_CASE(logger_impl_error_conf_testi02));
	test->add(BOOST_TEST_CASE(logger_impl_logic_error_test));
        test->add(BOOST_TEST_CASE(logger_impl_put_log_fatal_test02));
        test->add(BOOST_TEST_CASE(logger_impl_put_log_error_test02));
        test->add(BOOST_TEST_CASE(logger_impl_put_log_warn_test02));
        test->add(BOOST_TEST_CASE(logger_impl_put_log_info_test02));
        test->add(BOOST_TEST_CASE(logger_impl_put_log_debug_test02));
	test->add(BOOST_TEST_CASE(logger_impl_set_get_log_level_all_test));
	test->add(BOOST_TEST_CASE(logger_impl_set_log_level_test));
	test->add(BOOST_TEST_CASE(logger_impl_get_log_level_test));
	test->add(BOOST_TEST_CASE(logger_impl_constructor_test));

//	test->add(BOOST_TEST_CASE(logger_impl_init_test));
/*
	test->add(BOOST_TEST_CASE(logger_impl_constructor_test));
	test->add(BOOST_TEST_CASE(logger_impl_get_instance_test));
	test->add(BOOST_TEST_CASE(logger_impl_to_level_test));
//	test->add(BOOST_TEST_CASE(logger_impl_to_level_tag_test));
	test->add(BOOST_TEST_CASE(logger_impl_init_test));
	test->add(BOOST_TEST_CASE(logger_impl_error_conf_test));
	test->add(BOOST_TEST_CASE(logger_impl_load_conf_test));
	test->add(BOOST_TEST_CASE(logger_impl_get_log_level_test));
	test->add(BOOST_TEST_CASE(logger_impl_set_log_level_test));
	test->add(BOOST_TEST_CASE(logger_impl_put_log_fatal_test));
	test->add(BOOST_TEST_CASE(logger_impl_put_log_error_test));
	test->add(BOOST_TEST_CASE(logger_impl_put_log_warn_test));
	test->add(BOOST_TEST_CASE(logger_impl_put_log_info_test));
	test->add(BOOST_TEST_CASE(logger_impl_put_log_debug_test));
//	test->add(BOOST_TEST_CASE(logger_impl_get_categor_range_start_test));
//	test->add(BOOST_TEST_CASE(logger_impl_get_categor_range_end_test));
*/

	return test;


}

