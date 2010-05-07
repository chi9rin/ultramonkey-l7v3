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
#include "logger.h"
#include "logger_logrotate_utility.h"

#define LOGGER_ACCESS_PROCESS_ID "AccessLog"


#define LOGGER_NULL "/dev/null"

#define LOGGER_LEVEL_NUM (6)

#define LOG_CAT_L7VSD_ACCESS_LOGGER "l7vsd_access_logger"


namespace log4cxx
{
typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
}

namespace l7vs
{

//! @class logger_implement_access
//! @brief access logger implement class.
//! @brief this class manage logger for access setting and access logging by log4cxx.
class logger_implement_access
{
public:

        typedef std::map< std::string , std::string >
        accesslog_rotate_map_type;

        typedef std::map< std::string , std::string >::iterator
        accesslog_rotate_map_type_iterator;

        /*!
         * constructor initialize member variables.
         *
         * @param   access log file name
         */
        logger_implement_access(const std::string &access_log_file_name);

        /*!
         * destructor.
         */
        virtual ~logger_implement_access() {};

        /*!
         * initialze function.
         *
         * @param   default logrotation info use flag
         * @param   default logrotation info
         * @param   logrotation designation contents
         * @retrun  false failed
         */
        virtual bool init(
                const bool rotate_default_flag,
                const appender_property &access_log_default_property,
                accesslog_rotate_map_type &rotatedata);

        /*!
         * output access info log.
         *
         * @param   virtualservice endpoint info
         * @param   client ip info
         * @param   realserver connection origin info
         * @param   realserver connection destination info
         * @param   add msg
         * @retrun  void
         */
        virtual inline void putLog(
                const std::string &vsinfo,
                const std::string &cl_con_org,
                const std::string &rs_con_org,
                const std::string &rs_con_dest,
                const std::string &msg = "") {
                std::stringstream    buf;
                buf << boost::format("[ [AccessLog] (CL)%s --> %s --UM-- %s --> (RS-DST)%s %s]")
                    % cl_con_org
                    % vsinfo
                    % rs_con_org
                    % rs_con_dest
                    % msg;

                try {
                        log4cxx::Logger::getLogger(access_log_file_name_)
                        ->forcedLog(
                                log4cxx::Level::getInfo(),
                                buf.str(),
                                log4cxx::spi::LocationInfo("", "", 0));
                } catch (const std::exception &ex) {
                        std::ostringstream oss;
                        oss << "Logging Error (Access Log) : " << ex.what();
                        Logger::putLogError(
                                LOG_CAT_L7VSD_SESSION,
                                3,
                                oss.str(),
                                __FILE__,
                                __LINE__);

                }
        }

        /*!
         * increase reffernce count.
         *
         * @retrun void
         */
        virtual void    addRef();

        /*!
         * decrease reffernce count.
         *
         * @retrun void
         */
        virtual void    releaseRef();

        /*!
         * decrease reffernce count.
         *
         * @param inequal check object
         * @return check result
         */
        virtual bool    operator<=(const int access_num);

        /*!
         * member variable data getter.
         *
         * @retrun member variable data
         */
        virtual std::string getAcLogFileName();

        /*!
         * logrotate data compare.
         *
         * @param check object
         * @retrun true logrotate data equal
         */
        virtual bool    checkRotateParameterComp(
                accesslog_rotate_map_type &rotatedata);

        /*!
         * member variable data getter.
         *
         * @retrun member variable data
         */
        virtual bool    is_rotate_default_flag();

protected:


        int access_cnt;

        std::string access_log_file_name_;

        //! initialized flag
        bool initialized;

        appender_property access_log_property;

        accesslog_rotate_map_type aclog_args;

        bool rotate_default_flag;

        /*!
         * Logger setting function.
         *
         * @param   default logrotation info use flag
         * @param   logrotation designation contents
         * @retrun  false failed
         */
        virtual bool setAcLoggerConf(
                const appender_property &access_log_default_property,
                accesslog_rotate_map_type &rotatedata);

};

};

#endif //    LOGGER_IMPLEMENT_ACCESS_H    

