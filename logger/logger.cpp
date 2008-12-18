/*
 * @file  logger.cpp
 * @brief logger module creation class.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2008  NTT COMWARE Corporation.
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

#include <logger.h>
#include <logger_impl.h>

/*!
 * default constructor.
 * get instance of implement class, and initialize
 * @param   void
 * @return  void
 */
l7vs::Logger::Logger()
{
	if (!LoggerImpl::getInstance().init()) {
		exit(1);
	}
}

/*!
 * destructor.
 * @param   void
 * @return  void
 */
l7vs::Logger::~Logger()
{
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
 * output fatal log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id 
 * @param   log message 
 * @param   current file 
 * @param   current line
 * @retrun  void
 */
void l7vs::Logger::putLogFatal(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
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
void l7vs::Logger::putLogError(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
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
void l7vs::Logger::putLogWarn(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
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
void l7vs::Logger::putLogInfo(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
{
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
void l7vs::Logger::putLogDebug(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
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
