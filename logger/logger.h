/*
 * @file  logger.h
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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string>
#include <logger_enum.h>

#define BUF_LEN (4096)
#define DEBUG_STR_LEN (4096)

namespace l7vs
{
	/*!
	 *  Logger creation class.
	 *  this class create Logger Inmlement class and mediate function.
	 */
	class Logger
	{
	public:
		//! default constructor creates implement class.
		Logger();
		//! destructor.
		~Logger();
		/*!
		 * returns single instance.
		 *
		 * @param   void
		 * @return  instance
		 */		
		static Logger& getInstance() {
			return instance;
		}
		//! load Configuration.
		void loadConf();
		//! retrieve category's log level.
		LOG_LEVEL_TAG getLogLevel(LOG_CATEGORY_TAG cat);
		//! set category's log level.
		bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level);
		//! output fatal log.
		void putLogFatal(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line);
		//! output error log.
		void putLogError(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line);
		//! output warn log.
		void putLogWarn(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line);
		//! output info log.
		void putLogInfo(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line);
		//! output debug log.
		void putLogDebug(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line);
		//! return start category by using module.
		LOG_CATEGORY_TAG getCategoryRangeStart(LOG_MODULE_TAG mod);
		//! return end category by using module.
		LOG_CATEGORY_TAG getCategoryRangeEnd(LOG_MODULE_TAG mod);

	protected:
		/*!
		 * static instance initialize before main. 
		 * instance will defined in main program.
		 */
		static Logger instance;
	};
}	//namespace l7vs

#endif	//__LOGGER_H__
