/*!
 * @file  logger.cpp
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

#include "logger.h"
#include "logger_impl.h"
#include "utility.h"

/*!
 * default constructor.
 * get instance of implement class, and initialize
 * @param   void
 * @return  void
 */
l7vs::Logger::Logger() :
        scopedLogCategory(LOG_CAT_L7VSD_LOGGER),
        scopedLogId(0),
        scopedLogMessage("Logger Constructor"),
        scopedLogFile(__FILE__),
        scopedLogLine(__LINE__)
{
        if (unlikely(!LoggerImpl::getInstance().init())) {
                exit(1);
        }
}

//! log output constractor
//! scoped log output( Logger destractor output log)
//! @param[in]    Log output category tag
//! @param[in]    log id
//! @param[in]    log message
//!    @param[in]    filename    (=__FILE__)
//! @param[in]    lineno        (=__LINE__)
l7vs::Logger::Logger(LOG_CATEGORY_TAG cat, const unsigned int id, const std::string &msg, const char *file, int line) :
        scopedLogCategory(cat),
        scopedLogId(id),
        scopedLogMessage(msg),
        scopedLogFile(file),
        scopedLogLine(line)
{
        if (unlikely(LOG_LV_DEBUG == LoggerImpl::getInstance().getLogLevel(scopedLogCategory))) {
                LoggerImpl::getInstance().putLogDebug(
                        scopedLogCategory,
                        scopedLogId,
                        "Function in : " + scopedLogMessage,
                        scopedLogFile,
                        scopedLogLine);
        }
}

/*!
 * destructor.
 * @param   void
 * @return  void
 */
l7vs::Logger::~Logger()
{
        if (unlikely(LOG_LV_DEBUG == LoggerImpl::getInstance().getLogLevel(scopedLogCategory))) {
                LoggerImpl::getInstance().putLogDebug(
                        scopedLogCategory,
                        scopedLogId,
                        "Function out : " + scopedLogMessage,
                        scopedLogFile,
                        scopedLogLine);
        }
}

/*!
 * retrieve category's log level.
 * this is only wrapper to implement method.
 * @param   category that want to know
 * @return  log level
 */
l7vs::LOG_LEVEL_TAG l7vs::Logger::getLogLevel(LOG_CATEGORY_TAG cat)
{
        return LoggerImpl::getInstance().getLogLevel(cat);
}

/*!
 * retrieve all category's log level.
 * this is only wrapper to implement method.
 * @param[out]   category level list
 */
void l7vs::Logger::getLogLevelAll(category_level_list_type &list)
{
        return LoggerImpl::getInstance().getLogLevelAll(list);
}

/*!
 * set category's log level.
 * this is only wrapper to implement method.
 * @param   category to set log level
 * @param   level
 * @retval  true  succeed
 * @retval  false failed
 */
bool l7vs::Logger::setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level)
{
        return LoggerImpl::getInstance().setLogLevel(cat, level);
}

/*!
 * set all category's log level.
 * this is only wrapper to implement method.
 * @param   category to set log level
 * @param   level
 * @retval  true  succeed
 * @retval  false failed
 */
bool l7vs::Logger::setLogLevelAll(LOG_LEVEL_TAG level)
{
        return LoggerImpl::getInstance().setLogLevelAll(level);
}

/*!
 * output fatal log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id
 * @param   log message
 * @param   current file
 * @param   current line
 * @retrun  void
 */
void l7vs::Logger::putLogFatal(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string &message, const char *file, int line)
{
        if (LOG_LV_FATAL >= getLogLevel(cat))
                LoggerImpl::getInstance().putLogFatal(cat, message_id, message, file, line);
}

/*!
 * output errorl log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id
 * @param   log message
 * @param   current file
 * @param   current line
 * @retrun  void
 */
void l7vs::Logger::putLogError(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string &message, const char *file, int line)
{
        if (LOG_LV_ERROR >= getLogLevel(cat))
                LoggerImpl::getInstance().putLogError(cat, message_id, message, file, line);
}

/*!
 * output warn log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id
 * @param   log message
 * @param   current file
 * @param   current line
 * @retrun  void
 */
void l7vs::Logger::putLogWarn(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string &message, const char *file, int line)
{
        if (LOG_LV_WARN >= getLogLevel(cat))
                LoggerImpl::getInstance().putLogWarn(cat, message_id, message, file, line);
}

/*!
 * output info log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id
 * @param   log message
 * @param   current file
 * @param   current line
 * @retrun  void
 */
void l7vs::Logger::putLogInfo(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string &message, const char *file, int line)
{
        if (LOG_LV_INFO >= getLogLevel(cat))
                LoggerImpl::getInstance().putLogInfo(cat, message_id, message, file, line);
}

/*!
 * output debug log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id
 * @param   log message
 * @param   current file
 * @param   current line
 * @retrun  void
 */
void l7vs::Logger::putLogDebug(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string &message, const char *file, int line)
{
        LoggerImpl::getInstance().putLogDebug(cat, message_id, message, file, line);
}

/*!
 * reload configuration.
 * this is only wrapper to implement method.
 * @param   void
 * @return  void
 */
void l7vs::Logger::loadConf()
{
        LoggerImpl::getInstance().loadConf();
}

/*!
 * set snmp sendtrap function.
 * @param   snmp send trap function object
 * @retrun  void
 */
void    l7vs::Logger::setSnmpSendtrap(const snmpSendtrapFuncType func)
{
        LoggerImpl::getInstance().setSnmpSendtrap(func);
}

/*!
 * set log trap enable flag.
 * @param   log trap enable flag
 * @retrun  void
 */
void    l7vs::Logger::set_logtrap(int in_logtrap) {
        LoggerImpl::getInstance().set_logtrap(in_logtrap);
}

/*!
 * set log trap level.
 * @param   log trap level
 * @retrun  void
 */
void    l7vs::Logger::set_logtrap_level(LOG_LEVEL_TAG in_logtrap_level) {
        LoggerImpl::getInstance().set_logtrap_level(in_logtrap_level);
}
