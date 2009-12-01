/*
 * @file  logger_wapprer.h
 * @brief logger module c wrapper.
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

#ifndef __LOGGER_WRAPPER_H__
#define __LOGGER_WRAPPER_H__

#include "logger.h"

/*!
 * retrieve category's log level.
 * this is only wrapper to implement method.
 * @param   category that want to know
 * @return  log level
 */
inline	l7vs::LOG_LEVEL_TAG logger_get_log_level(const l7vs::LOG_CATEGORY_TAG cat){
	return l7vs::Logger::getLogLevel(cat);
}

/*!
 * set category's log level.
 * this is only wrapper to implement method.
 * @param   category to set log level
 * @param   level
 * @retval  0  succeed
 * @retval  -1 failed
 */
inline int	logger_set_log_level(const l7vs::LOG_CATEGORY_TAG cat, const l7vs::LOG_LEVEL_TAG level){
	if (l7vs::Logger::setLogLevel(cat, level)) {
		return 0;
	}
	return -1;
}

/*!
 * output fatal log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id 
 * @param   current file 
 * @param   current line
 * @param   log message 
 * @retrun  void
 */
inline	void	logger_put_log_fatal(const l7vs::LOG_CATEGORY_TAG cat, const unsigned int message_id, char* file, int line, const char* message){
	l7vs::Logger::putLogFatal(cat, message_id, message, file, line);
}

/*!
 * output error log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id 
 * @param   current file 
 * @param   current line
 * @param   log message 
 * @retrun  void
 */
inline	void	logger_put_log_error(const l7vs::LOG_CATEGORY_TAG cat, const unsigned int message_id, char* file, int line, const char* message){
	l7vs::Logger::putLogError(cat, message_id, message, file, line);
}

/*!
 * output warn log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id 
 * @param   current file 
 * @param   current line
 * @param   log message 
 * @retrun  void
 */
inline	void	logger_put_log_warn(const l7vs::LOG_CATEGORY_TAG cat, const unsigned int  message_id, char* file, int line, const char* message){
	l7vs::Logger::putLogWarn(cat, message_id, message, file, line);
}

/*!
 * output info log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id 
 * @param   current file 
 * @param   current line
 * @param   log message 
 * @retrun  void
 */
inline	void	logger_put_log_info(const l7vs::LOG_CATEGORY_TAG cat, const unsigned int message_id, char* file, int line, const char* message){
	l7vs::Logger::putLogInfo(cat, message_id, message, file, line);
}

/*!
 * output debug log.
 * this is only wrapper to implement method.
 * @param   category that logging matter occured
 * @param   log message id 
 * @param   current file 
 * @param   current line
 * @param   log message 
 * @retrun  void
 */
inline	void	logger_put_log_debug(const l7vs::LOG_CATEGORY_TAG cat, const unsigned int message_id, char* file, int line, const char* message){
	l7vs::Logger::putLogDebug(cat, message_id, message, file, line);
}

#define LOGGER_PUT_LOG_FATAL(cat, message_id, message, arg...) { \
	char buf[BUF_LEN]; \
	snprintf(buf, BUF_LEN, message, ##arg); \
	logger_put_log_fatal(cat, message_id, __FILE__, __LINE__, buf); }
	
#define LOGGER_PUT_LOG_ERROR(cat, message_id, message, arg...) { \
	char buf[BUF_LEN]; \
	snprintf(buf, BUF_LEN, message, ##arg); \
	logger_put_log_error(cat, message_id, __FILE__, __LINE__, buf); }

#define LOGGER_PUT_LOG_WARN(cat, message_id, message, arg...) { \
	char buf[BUF_LEN]; \
	snprintf(buf, BUF_LEN, message, ##arg); \
	logger_put_log_warn(cat, message_id, __FILE__, __LINE__, buf); }

#define LOGGER_PUT_LOG_INFO(cat, message_id, message, arg...) { \
	char buf[BUF_LEN]; \
	snprintf(buf, BUF_LEN, message, ##arg); \
	logger_put_log_info(cat, message_id, __FILE__, __LINE__, buf); }

#define LOGGER_PUT_LOG_DEBUG(cat, message_id, message, arg...) { \
	char buf[BUF_LEN]; \
	snprintf(buf, BUF_LEN, message, ##arg); \
	logger_put_log_debug(cat, message_id, __FILE__, __LINE__, buf); }

#endif	//__LOGGER_WRAPPER_H__
