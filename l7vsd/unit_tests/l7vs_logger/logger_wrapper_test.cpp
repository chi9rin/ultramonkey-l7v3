#include <iostream>
#include <string>
#include <boost/test/unit_test.hpp>
#include <errno.h>

#include "../logger_stub/logger.h"
//#include "../logger_stub/logger_impl.h"
#include "../../l7vsd/logger/logger_wrapper.h"

using namespace boost::unit_test_framework;
using namespace std;

l7vs::Logger	l7vs::Logger::instance;

//logger_get_log_level
void	wrapper_logger_get_log_level()
{
	LOG_LEVEL_TAG		loglevel;
	LOG_CATEGORY_TAG	logcat;

	//LoggerクラスのgetLogLevelメソッドをコールしていることを確認する
	//LoggerクラスのgetLogLevelメソッドが返した値を返すことを確認する
	logcat = LOG_CAT_L7VSD_NETWORK;

	l7vs::Logger::getInstance().RetLevel = LOG_LV_DEBUG;
	loglevel = logger_get_log_level( logcat );
	BOOST_CHECK_EQUAL( LOG_LV_DEBUG, loglevel );

	l7vs::Logger::getInstance().RetLevel = LOG_LV_INFO;
	loglevel = logger_get_log_level( logcat );
	BOOST_CHECK_EQUAL( LOG_LV_INFO, loglevel );

	l7vs::Logger::getInstance().RetLevel = LOG_LV_WARN;
	loglevel = logger_get_log_level( logcat );
	BOOST_CHECK_EQUAL( LOG_LV_WARN, loglevel );

	l7vs::Logger::getInstance().RetLevel = LOG_LV_ERROR;
	loglevel = logger_get_log_level( logcat );
	BOOST_CHECK_EQUAL( LOG_LV_ERROR, loglevel );

	l7vs::Logger::getInstance().RetLevel = LOG_LV_FATAL;
	loglevel = logger_get_log_level( logcat );
	BOOST_CHECK_EQUAL( LOG_LV_FATAL, loglevel );
}

//logger_set_log_level
void	wrapper_logger_set_log_level()
{
	int	retint = 0;

	LOG_LEVEL_TAG		loglevel = LOG_LV_INFO;
	LOG_CATEGORY_TAG	logcat = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;

	//LoggetクラスのsetLogLevelメソッドをコールしていることを確認する
	//LoggerクラスのsetLogLevelメソッドがtrueを返した時に0を返すことを確認する。(返り値の変換)
	l7vs::Logger::getInstance().RetBool = true;
	retint = logger_set_log_level( logcat, loglevel );
	BOOST_CHECK_EQUAL( 0, retint );

	//LoggerクラスのsetLogLevelメソッドがfalseを返した時に0を返すことを確認する。(返り値の変換)
	l7vs::Logger::getInstance().RetBool = false;
	retint = logger_set_log_level( logcat, loglevel );
	BOOST_CHECK_EQUAL( -1, retint );
}

#define	FILENAME "/var/log/l7vs/080201_01.log"

//logger_put_log_fatal
void	wrapper_logger_put_log_fatal()
{
	int	msg_id = 10;
	int	line = 100;
	string	message = "logmessage";

	LOG_CATEGORY_TAG	logcat = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;

	//LoggetクラスのputLogFatalメソッドをコールしていることを確認する
	logger_put_log_fatal( logcat, msg_id, FILENAME, line, message.c_str() );
}

//logger_put_log_error
void	wrapper_logger_put_log_error()
{
	int	msg_id = 10;
	int	line = 100;
	string	message = "logmessage";

	LOG_CATEGORY_TAG	logcat = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;

	//LoggetクラスのputLogErrorメソッドをコールしていることを確認する
	logger_put_log_error( logcat, msg_id, FILENAME, line, message.c_str() );
}

//logger_put_log_warn
void	wrapper_logger_put_log_warn()
{
	int	msg_id = 10;
	int	line = 100;
	string	message = "logmessage";

	LOG_CATEGORY_TAG	logcat = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;

	//LoggetクラスのputLogWarnメソッドをコールしていることを確認する
	logger_put_log_warn( logcat, msg_id, FILENAME, line, message.c_str() );
}

//logger_put_log_info
void	wrapper_logger_put_log_info()
{
	int	msg_id = 10;
	int	line = 100;
	string	message = "logmessage";

	LOG_CATEGORY_TAG	logcat = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;

	//LoggetクラスのputLogInfoメソッドをコールしていることを確認する
	logger_put_log_info( logcat, msg_id, FILENAME, line, message.c_str() );
}

//logger_put_log_debug
void	wrapper_logger_put_log_debug()
{
	int	msg_id = 10;
	int	line = 100;
	string	message = "logmessage";

	LOG_CATEGORY_TAG	logcat = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;

	//LoggetクラスのputLogDebugメソッドをコールしていることを確認する
	logger_put_log_debug( logcat, msg_id, FILENAME, line, message.c_str() );
}

//logger_get_category_range_start
void	wrapper_logger_get_category_range_start()
{
	LOG_CATEGORY_TAG	retcat;
	LOG_MODULE_TAG		module = LOG_MOD_L7VSD;

	//LoggetクラスのgetCategoryRangeStartメソッドをコールしていることを確認する
	//LoggerクラスのgetCategoryRangeStartメソッドが返した値を返すことを確認する
	l7vs::Logger::getInstance().RetCat = LOG_CAT_L7VSD_NETWORK;
	retcat = logger_get_category_range_start( module );
	BOOST_CHECK_EQUAL( LOG_CAT_L7VSD_NETWORK, retcat );
}

//logger_get_category_range_end
void	wrapper_logger_get_category_range_end()
{
	LOG_CATEGORY_TAG	retcat;
	LOG_MODULE_TAG		module = LOG_MOD_L7VSD;

	//LoggetクラスのgetCategoryRangeEndメソッドをコールしていることを確認する
	//LoggerクラスのgetCategoryRangeEndメソッドが返した値を返すことを確認する
	l7vs::Logger::getInstance().RetCat = LOG_CAT_L7VSD_PROTOCOL;
	retcat = logger_get_category_range_end( module );
	BOOST_CHECK_EQUAL( LOG_CAT_L7VSD_PROTOCOL, retcat );
}

//LOGGER_PUT_LOG_FATAL
void	wrapper_LOGGER_PUT_LOG_FATAL()
{
	//Loggetクラスのlogger_put_log_fatalメソッドが呼ばれることを確認する
	LOGGER_PUT_LOG_FATAL( LOG_CAT_L7VSD_START_STOP, 0, "testmessage %d", 100  );
}

//LOGGER_PUT_LOG_ERROR
void	wrapper_LOGGER_PUT_LOG_ERROR()
{
	//Loggetクラスのlogger_put_log_errorメソッドが呼ばれることを確認する
	LOGGER_PUT_LOG_ERROR( LOG_CAT_L7VSD_LOGGER, 0, "testmassage" );
}

//LOGGER_PUT_LOG_WARN
void	wrapper_LOGGER_PUT_LOG_WARN()
{
	//Loggetクラスのlogger_put_log_warnメソッドが呼ばれることを確認する
	LOGGER_PUT_LOG_WARN( LOG_CAT_L7VSADM_PARSE, 0, "testmessage %d", 100  );
}

//LOGGER_PUT_LOG_INFO
void	wrapper_LOGGER_PUT_LOG_INFO()
{
	//Loggetクラスのlogger_put_log_infoメソッドが呼ばれることを確認する
	LOGGER_PUT_LOG_INFO( LOG_CAT_SNMPAGENT_START_STOP, 0, "testmassage" );
}

//LOGGER_PUT_LOG_DEBUG
void	wrapper_LOGGER_PUT_LOG_DEBUG()
{
	//Loggetクラスのlogger_put_log_debugメソッドが呼ばれることを確認する
	LOGGER_PUT_LOG_DEBUG( LOG_CAT_SNMPAGENT_L7VSD_SEND, 0, "testmessage %d", 100  );
}

test_suite*
init_unit_test_suite(int argc, char* argv[])
{
	test_suite* test = BOOST_TEST_SUITE("parameter_test");
	test->add(BOOST_TEST_CASE( wrapper_logger_get_log_level ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_set_log_level ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_put_log_fatal ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_put_log_error ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_put_log_warn ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_put_log_info ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_put_log_debug ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_get_category_range_start ) );
	test->add(BOOST_TEST_CASE( wrapper_logger_get_category_range_end ) );
	test->add(BOOST_TEST_CASE( wrapper_LOGGER_PUT_LOG_FATAL ) );
	test->add(BOOST_TEST_CASE( wrapper_LOGGER_PUT_LOG_ERROR ) );
	test->add(BOOST_TEST_CASE( wrapper_LOGGER_PUT_LOG_WARN ) );
	test->add(BOOST_TEST_CASE( wrapper_LOGGER_PUT_LOG_INFO ) );
	test->add(BOOST_TEST_CASE( wrapper_LOGGER_PUT_LOG_DEBUG ) );

	return test;
}

//----------------------------logger class stub----------------------------
#include "logger.h"
#include "logger_impl.h"

l7vs::Logger::Logger()
{
	BOOST_MESSAGE( ">>>>l7vs::Logger constructor  called" );
	RetBool = true;
	RetCat	= LOG_CAT_L7VSD_PROTOCOL;
	RetLevel	= LOG_LV_DEBUG;
}

l7vs::Logger::~Logger()
{
	BOOST_MESSAGE( ">>>>l7vs::Logger destructor  called" );
}

LOG_LEVEL_TAG l7vs::Logger::getLogLevel(LOG_CATEGORY_TAG cat)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::getLogLevel  called" );
	return RetLevel;
}

bool l7vs::Logger::setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::setLogLevel  called" );
	return RetBool;
}

void l7vs::Logger::putLogFatal(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::putLogFatal  called" );
}

void l7vs::Logger::putLogError(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::putLogError  called" );
}

void l7vs::Logger::putLogWarn(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::putLogWarn  called" );
}

void l7vs::Logger::putLogInfo(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::putLogInfo  called" );
}

void l7vs::Logger::putLogDebug(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::putLogDebug  called" );
}

LOG_CATEGORY_TAG l7vs::Logger::getCategoryRangeStart(LOG_MODULE_TAG mod)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::getCategoryRangeStart  called" );
	return	RetCat;
}

LOG_CATEGORY_TAG l7vs::Logger::getCategoryRangeEnd(LOG_MODULE_TAG mod)
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::getCategoryRangeEnd  called" );
	return	RetCat;
}

bool l7vs::Logger::reloadConf()
{
	BOOST_MESSAGE( ">>>>l7vs::Logger::reloadConf  called" );
	return RetBool;
}

