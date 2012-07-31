/*le  logger.h
 * @brief logger module creation class.
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

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <logger_enum.h>
namespace l7vs
{

//! @class Logger
//! @brief Logger creation class.
//! @brief this class create Logger Inmlement class and mediate function.
class Logger
{
protected:
        const LOG_CATEGORY_TAG  scopedLogCategory;
        const unsigned int      scopedLogId;
        const std::string       scopedLogMessage;
        const char             *scopedLogFile;
        const int               scopedLogLine;

public:
        //! default constructor creates implement class.
        Logger();

        //! log output constractor
        //! scoped log output( Logger destractor output log)
        //! @param[in]  Log output category tag
        //! @param[in]  log id
        //! @param[in]  log message
        //! @param[in]  filename    (=__FILE__)
        //! @param[in]  lineno      (=__LINE__)
        Logger(LOG_CATEGORY_TAG, const unsigned int, const std::string &, const char *, int);

        //! destructor.(output log then use log output constractor)
        ~Logger();

        //! load Configuration.
        void loadConf();

        //! retrieve category's log level.
        //! @param[in]  logcategory
        //! @return     loglevel
        static LOG_LEVEL_TAG getLogLevel(LOG_CATEGORY_TAG);

        //! retrieve all category's log level.
        //! @param[in]  logcategory
        //! @return     loglevel
        static void getLogLevelAll(category_level_list_type &);

        //! set category's log level.
        //! @param[in]  logcategory
        //! @param[in]  loglevel
        //! @return true    success loglevel change
        //! @return false   failer loglevel change
        static bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level);

        //! set all category's log level.
        //! @param[in]  logcategory
        //! @param[in]  loglevel
        //! @return true    success loglevel change
        //! @return false   failer loglevel change
        static bool setLogLevelAll(LOG_LEVEL_TAG level);

        //! output fatal log.
        //! @param[in]  Log output category tag
        //! @param[in]  log id
        //! @param[in]  log message
        //! @param[in]  filename    (=__FILE__)
        //! @param[in]  lineno      (=__LINE__)
        static void putLogFatal(LOG_CATEGORY_TAG, const unsigned int, const std::string &, const char *, int);

        //! output error log.
        //! @param[in]  Log output category tag
        //! @param[in]  log id
        //! @param[in]  log message
        //! @param[in]  filename    (=__FILE__)
        //! @param[in]  lineno      (=__LINE__)
        static void putLogError(LOG_CATEGORY_TAG, const unsigned int, const std::string &, const char *, int);

        //! output warn log.
        //! @param[in]  Log output category tag
        //! @param[in]  log id
        //! @param[in]  log message
        //! @param[in]  filename    (=__FILE__)
        //! @param[in]  lineno      (=__LINE__)
        static void putLogWarn(LOG_CATEGORY_TAG, const unsigned int, const std::string &, const char *, int);

        //! output info log.
        //! @param[in]  Log output category tag
        //! @param[in]  log id
        //! @param[in]  log message
        //! @param[in]  filename    (=__FILE__)
        //! @param[in]  lineno      (=__LINE__)
        static void putLogInfo(LOG_CATEGORY_TAG, const unsigned int, const std::string &, const char *, int);

        //! output debug log.
        //! @param[in]  Log output category tag
        //! @param[in]  log id
        //! @param[in]  log message
        //! @param[in]  filename    (=__FILE__)
        //! @param[in]  lineno      (=__LINE__)
        static void putLogDebug(LOG_CATEGORY_TAG, const unsigned int, const std::string &, const char *, int);

        //! set snmp sendtrap function
        //! @param   snmp send trap function object
        //! @retrun  void
        static void setSnmpSendtrap(const snmpSendtrapFuncType func);
};
}   //namespace l7vs

#endif  //LOGGER_H

