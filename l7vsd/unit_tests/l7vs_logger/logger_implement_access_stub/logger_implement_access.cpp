/*!
 * @file  logger_implement_access.cpp
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


bool l7vs::logger_implement_access::rtn_init_flag;

bool l7vs::logger_implement_access::rtn_checkRotateParameterComp_flag;

bool l7vs::logger_implement_access::rtn_setAcLoggerConf_flag;


l7vs::logger_implement_access::logger_implement_access(const std::string &access_log_file_name)
            :rotate_default_flag( false ),
            access_cnt(1)
{
    access_log_file_name_ = access_log_file_name;
    aclog_args.clear();
    
}


bool l7vs::logger_implement_access::init(const bool rotate_default_flag,const appender_property& access_log_default_property,accesslog_rotate_map_type& rotatedata)
{
    bool rtn = l7vs::logger_implement_access::rtn_init_flag;
    
    return (rtn);

}


bool l7vs::logger_implement_access::setAcLoggerConf(const appender_property& access_log_default_property,accesslog_rotate_map_type& rotatedata)
{
    bool rtn = l7vs::logger_implement_access::rtn_setAcLoggerConf_flag;

    return(rtn);

}


void l7vs::logger_implement_access::addRef()
{
    access_cnt++;
}


void l7vs::logger_implement_access::releaseRef()
{
    access_cnt--;
}


bool l7vs::logger_implement_access::operator<=(const int access_num )
{
    return( access_cnt <= access_num );
}

std::string l7vs::logger_implement_access::getAcLogFileName()
{
    return( access_log_file_name_ ); 
}


bool l7vs::logger_implement_access::checkRotateParameterComp(accesslog_rotate_map_type &rotatedata)
{
    bool rtn = l7vs::logger_implement_access::rtn_checkRotateParameterComp_flag;
    
    return(rtn);
}

bool l7vs::logger_implement_access::is_rotate_default_flag()
{
    bool rtn = rotate_default_flag;

    return(rtn);
}

// ログフォーマット 2008/12/07 20:08:31 [INFO] [[AccessLog] (CL)192.168.2.1 --> 192.168.2.2 --UM-- 192.168.1.101:37259 --> (RS-DST)192.168.1.106:80 ]
/*!
 * output access info log.
 *
 * @param   virtualservice endpoint info
 * @param   client endpoint info
 * @param   realserver connect origin info
 * @param   realserver connect destination info
 * @param   add msg
 * @retrun  void
 */
void l7vs::logger_implement_access::putLog(
                            const std::string& vsinfo,
                            const std::string& cl_con_org,
                            const std::string& rs_con_org,
                            const std::string& rs_con_dest,
                            const std::string& msg){

    std::stringstream    buf;
    buf << boost::format( "[ [AccessLog] (CL)%s --> %s --UM-- %s --> (RS-DST)%s %s]" )
        % LOGGER_ACCESS_PROCESS_ID
        % vsinfo
        % cl_con_org
        % rs_con_org
        % rs_con_dest
        % msg;

    try {
        log4cxx::Logger::getLogger( access_log_file_name_ )->forcedLog(    log4cxx::Level::getInfo(),
                                                                    buf.str(),
                                                                    log4cxx::spi::LocationInfo("", "", 0));
    }
    catch (const std::exception& ex) {
        std::ostringstream oss;
        oss << "Logging Error (Access Log) : " << ex.what();
        std::cout << oss.str() << "\n";
    }

}
