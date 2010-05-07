/*!
 *    @file    logger_rogrote_utility.h
 *    @brief   log rotation utility header.
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

#ifndef LOGGER_ROGROTATE_UTILITY
#define LOGGER_ROGROTATE_UTILITY

#include <sched.h>
#include <net/if.h>
#include <unistd.h>
#include <sched.h>
#include <error.h>
#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/level.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>
#include "logger_rotation_enum.h"
#include "appender_property.h"


#define ACCESS_LOG_ROTATE_TYPE              "--ac-rotate-type"
#define ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX  "--ac-rotate-max-backup-index"
#define ACCESS_LOG_ROTATE_MAX_FILESIZE      "--ac-rotate-max-filesize"
#define ACCESS_LOG_ROTATE_ROTATION_TIMING   "--ac-rotate-rotation-timing"
#define ACCESS_LOG_ROTATION_TIMING_VALUE    "--ac-rotate-rotation-timing-value"

#define LOGGER_ACCESS_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %m %t%n"

#define ACCESS_LOG_ROTATION_KEY                 "l7vsd_accesslog_rotate_type"
#define ACCESS_LOG_MAX_BACKUP_INDEX_KEY         "l7vsd_accesslog_rotate_max_backup_index"
#define ACCESS_LOG_MAX_FILE_SIZE_KEY            "l7vsd_accesslog_rotate_max_filesize"
#define ACCESS_LOG_ROTATION_TIMING_KEY          "l7vsd_accesslog_rotate_rotation_timing"
#define ACCESS_LOG_ROTATION_TIMING_VALUE_KEY    "l7vsd_accesslog_rotation_timing_value"



namespace l7vs
{

//! @class logger_logrotate_utility
//! @brief Logger logrotate utility class.
//! @brief this class logrotation common logic implementation class.
class logger_logrotate_utility
{


        typedef std::map< std::string , std::string >
        accesslog_rotate_map_type;

        typedef std::map< std::string , std::string >::iterator
        accesslog_rotate_map_type_iterator;

public:

        /*!
         * rotate type check.
         *
         * @param   rotaton string("size" or "date" or "datesize")
         * @return  LOG_ROTATION_TAG val
         */
        static LOG_ROTATION_TAG check_rotate_type(
                const std::string &rotation_type);

        /*!
         * log file max backup check.
         *
         * @param   file backup val string
         * @return  file backup val
         */
        static unsigned int check_max_backup_index(
                const std::string &max_backup_index);

        /*!
         * log file max file size check.
         *
         * @param   file max size string
         * @return  file max size val
         */
        static unsigned long long check_max_file_size(
                const std::string &max_file_size);

        /*!
         * log file rotate timing check.
         *
         * @param   rotate timing string
         * @return  LOG_ROTATION_TIMING_TAG val
         */
        static LOG_ROTATION_TIMING_TAG check_rotate_timing(
                const std::string &rotation_timing);

        /*!
         * rotate timing value check.
         *
         * @param   rotate timing val string
         * @param   LOG_ROTATION_TIMING_TAG val
         * @return rotate timing val
         */
        static std::string check_rotate_timing_value(
                const std::string rotation_timing_value_key,
                const LOG_ROTATION_TIMING_TAG rotation_timing);

        /*!
         * Logger set appender.
         *
         * @param   appender property
         * @param   logger layout
         * @param   log category
         * @return  void
         */
        static void set_appender(
                const appender_property &log_property,
                const char *logger_layout,
                const std::string &log_category);

        /*!
         * log rotate parameter check.
         *
         * @param   check object.
         * @param   appender property
         * @return  false faild
         */
        static bool acccess_log_LogrotateParamCheck(
                accesslog_rotate_map_type &rotatedata,
                appender_property &access_log_property);

        /*!
         * error log out put.
         *
         * @param   msg no.
         * @param   err msg
         * @param   file name
         * @param   line no
         * @return  void
         */
        static void loglotation_utility_logic_error(
                const unsigned int logno,
                const std::string &str,
                const char *file ,
                const unsigned int line);


};

};

#endif  // LOGGER_ROGROTATE_UTILITY
