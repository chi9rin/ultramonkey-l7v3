/*!
 * @file  logger_implement_access.h
 * @brief logger module implementation class For access log.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *      
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/
#ifndef	LOGGER_IMPLEMENT_ACCESS_H
#define	LOGGER_IMPLEMENT_ACCESS_H

#include <sstream>
#include <map>
#include <log4cxx/logger.h>
#include <log4cxx/level.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>
#include <boost/format.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/foreach.hpp>
#include "appender_property.h"

#define LOGGER_ACCESS_PROCESS_ID "ACCESS"


#define LOGGER_NULL "/dev/null"

#define LOGGER_LEVEL_NUM (6)

#define LOG_CAT_L7VSD_ACCESS_LOGGER "l7vsd_access_logger"

namespace log4cxx
{
    typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
}

namespace l7vs{

class logger_implement_access {
public:
    
    typedef std::map< std::string , std::string > accesslog_rotate_map_type;

    logger_implement_access(const std::string &aclogFilename);
    virtual ~logger_implement_access(){};

    //! initialze function
    virtual bool init(appender_property& access_log_default_property,const std::map<std::string,std::string>& rotate);

    /*!
     * output info log.
     *
     * @param   category that logging matter occured
     * @param   log message id
     * @param   log message
     * @param   current file
     * @param   current line
     * @retrun  void
     */
    virtual inline void putLog(const std::string& vsinfo,
								const std::string& cl_con_org,
								const std::string& rs_con_org,
								const std::string& rs_con_dest,
	                            const std::string& msg){
/*
		std::stringstream	buf;
		buf << boost::format( "%s%d%02d%05d %s %s" )
			% LOGGER_ACCESS_PROCESS_ID
			% 1
			% LOG_CAT_L7VSD_ACCESS_LOGGER
			% message_id
			% message.c_str()
			% hostname;
										  
		try {
			log4cxx::Logger::getLogger(LOG_CAT_L7VSD_ACCESS_LOGGER)->forcedLog(	log4cxx::Level::getInfo(),
																		buf.str(),
																		log4cxx::spi::LocationInfo(file, "", line));
		}
		catch (const std::exception& ex) {
			std::ostringstream oss;
			oss << "Logging Error (Info Log) : " << ex.what();
			errorConf( 6, oss.str(), __FILE__, __LINE__);
		}
*/
	}

    virtual void    addRef();

    virtual void    releaseRef();

    virtual bool    operator<=(const int access_num );

    virtual std::string    getAcLogFileName(){ return( this->acLogFileName ); }

    virtual bool    checkRotateParameterComp(accesslog_rotate_map_type &rotatedata);
    
    virtual bool    is_rotate_default_flag();
    
protected:

    
    int access_cnt;

    std::string acLogFileName;

  	//! initialized flag
	bool initialized;
  
	//! hostname
    appender_property access_log_property;

    accesslog_rotate_map_type aclog_args;
    
    bool rotate_default_flag;
    
    virtual bool setAcLoggerConf(appender_property& access_log_default_property,const accesslog_rotate_map_type& rotate);

};

};

#endif //    LOGGER_IMPLEMENT_ACCESS_H	

