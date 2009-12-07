#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits.h>
#include <log4cxx/logmanager.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/rolling/rollingfileappender.h>
#include <log4cxx/rolling/fixedwindowrollingpolicy.h>
#include <log4cxx/rolling/sizebasedtriggeringpolicy.h>
#include <log4cxx/rolling/timebasedrollingpolicy.h>
#include <log4cxx/consoleappender.h>
#include <errno.h>
#include <stdexcept>
#include <boost/lexical_cast.hpp>



#include "logger_implement_access.h"


#define LOGGER_ACCESS_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %c %m %t %F:%L%n"
#define LOGGER_ACCESS_DEFAULT_BUFFER_SIZE (8 * 1024)
#define LOGGER_ACCESS_SYSLOG_FACILITY "USER"
#define LOGGER_ACCESS_BACKUP_INDEX_LOWER_LIMIT (1)
#define LOGGER_ACCESS_BACKUP_INDEX_LIMIT (12)
#define LOGGER_ACCESS_FILESIZE_LOWER_LIMIT (65535)
#define LOGGER_ACCESS_FILE_PATTERN "%i"


l7vs::logger_implement_access::logger_implement_access(const std::string &acLogFileName):access_cnt(1),initialized( false ),rotate_default_flag( false )
{
    this->acLogFileName = acLogFileName;
    aclog_args.clear();
}


bool l7vs::logger_implement_access::init(appender_property& access_log_default_property,const accesslog_rotate_map_type& rotate)
{
/*
	using namespace log4cxx;
	if (initialized) return true;

  bool rtnFlg = true;
  
	try {
		// set loot logger
		LoggerPtr root = Logger::getRootLogger();
		if ( NULL == root)	return false;

    rtnFlg = this->setAcLoggerConf();

	}
	catch (const std::exception& e) {
		std::ostringstream oss;
		oss <<  "Logger Initialization Failed : " << e.what();
		return false;
	}

  if( rtnFlg == true ) {
  	// get hostname
  	int		ret = 0;
  	char	buff[HOST_NAME_MAX];
  	ret = gethostname(buff, HOST_NAME_MAX);
  	hostname = buff;
    
  	initialized = true;
  }
*/
  return (true);
}


bool l7vs::logger_implement_access::setAcLoggerConf(appender_property& access_log_default_property,const accesslog_rotate_map_type& rotate)
{
    
/*
	using namespace log4cxx;
	using namespace l7vs;

	// appender setting
	try {
		log4cxx::helpers::Pool pool;

		log4cxx::LayoutPtr layout =
			new log4cxx::PatternLayout(LOGGER_ACCESS_LAYOUT);

		//create RollingFileAppender
		log4cxx::FileAppenderPtr	accessLogAppender =
			new log4cxx::FileAppender();
	  
		// set layout
		accessLogAppender->setLayout(layout);

		// set Log Filename
		try{
			accessLogAppender->setFile(this->acLogFileName, true, false, LOGGER_ACCESS_DEFAULT_BUFFER_SIZE, pool);
		}
		catch( const std::exception& e ){
			std::stringstream	buf;
			buf << "File Create Failed:" << this->acLogFileName;
			throw std::logic_error( buf.str() );
		}

		// activate appender options
		accessLogAppender->activateOptions(pool);

		log4cxx::LoggerPtr cat_logger = log4cxx::Logger::getLogger( LOG_CAT_L7VSD_ACCESS_LOGGER );
		if (0 == cat_logger) {
			throw std::logic_error("getLogger Failed.");
		}

		cat_logger->addAppender(accessLogAppender);

		//default log level settting
		cat_logger->setLevel( log4cxx::Level::getInfo() );

	}
	catch (const std::exception& e) {
		std::ostringstream oss;
		oss <<  "Logger Reload Config Failed : " << e.what();
	  return(false);
	}
*/
    
  return(true);
  
}


void	l7vs::logger_implement_access::addRef()
{
  this->access_cnt++;
}


void	l7vs::logger_implement_access::releaseRef()
{
  this->access_cnt--;
}


bool	l7vs::logger_implement_access::operator<=(const int access_num )
{
  return( this->access_cnt <= access_num );
}

bool l7vs::logger_implement_access::checkRotateParameterComp(accesslog_rotate_map_type &rotatedata)
{
    return(true);
}


bool l7vs::logger_implement_access::is_rotate_default_flag()
{
    return(true);
}



