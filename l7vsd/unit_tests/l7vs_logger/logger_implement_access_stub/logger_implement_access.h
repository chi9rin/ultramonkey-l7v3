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
#ifndef LOGGER_IMPLEMENT_ACCESS_H
#define LOGGER_IMPLEMENT_ACCESS_H

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

#define LOGGER_ACCESS_PROCESS_ID "AccessLog"


#define LOGGER_NULL "/dev/null"

#define LOGGER_LEVEL_NUM (6)

#define LOG_CAT_L7VSD_ACCESS_LOGGER "l7vsd_access_logger"


namespace log4cxx
{
    typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
}

namespace l7vs{

//! @class logger_implement_access
//! @brief access logger implement class.
//! @brief this class manage logger for access setting and access logging by log4cxx.
class logger_implement_access {
public:
    
    typedef std::map< std::string , std::string > accesslog_rotate_map_type;

    typedef std::map< std::string , std::string >::iterator accesslog_rotate_map_type_iterator;

    logger_implement_access(const std::string &access_log_file_name);
    virtual ~logger_implement_access(){};

    //! initialze function
    virtual bool init(
        const bool rotate_default_flag,
        const appender_property& access_log_default_property,
        accesslog_rotate_map_type& rotatedata);

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
    virtual inline void putLog(
                                const std::string& vsinfo,
                                const std::string& cl_con_org,
                                const std::string& rs_con_org,
                                const std::string& rs_con_dest,
                                const std::string& msg = "");

    virtual void    addRef();

    virtual void    releaseRef();

    virtual bool    operator<=(const int access_num );

    virtual std::string getAcLogFileName();

    virtual bool    checkRotateParameterComp(
                        accesslog_rotate_map_type &rotatedata);
    
    virtual bool    is_rotate_default_flag();
    
    static bool rtn_init_flag;
    
    static bool rtn_checkRotateParameterComp_flag;
    
    bool rotate_default_flag;
    
    static bool rtn_setAcLoggerConf_flag;
    
    
    int access_cnt;

    std::string access_log_file_name_;

    //! hostname
    appender_property access_log_property;

    accesslog_rotate_map_type aclog_args;
    
    
    virtual bool setAcLoggerConf(
                    const appender_property& access_log_default_property,
                    accesslog_rotate_map_type& rotatedata);

};

};

#endif //    LOGGER_IMPLEMENT_ACCESS_H    

