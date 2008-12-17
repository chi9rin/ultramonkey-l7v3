/*
 * @file  logger_impl.h
 * @brief logger module implementation class.
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

#ifndef __LOGGER_IMPL_H__
#define __LOGGER_IMPL_H__

#include <string>
#include <map>
#include <log4cxx/logger.h>
#include <log4cxx/level.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>
#include "logger_enum.h"

#define BUF_LEN (4096)
#define LOGGER_LEVEL_NUM (6)
#define LOGGER_CATEGORY_NUM (40)

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#if defined(LOGGER_PROCESS_VSD)
#define LOGGER_PROCESS_ID "VSD"
#elif defined(LOGGER_PROCESS_ADM)
#define LOGGER_PROCESS_ID "ADM"
#else
#define LOGGER_PROCESS_ID "SNM"
#endif

#if defined(LOGGER_PROCESS_VSD)
#include "l7vs_snmpbridge.h"
#endif

#define LOGGER_NULL "/dev/null"

#define HOST_NAME_LEN (256)

namespace log4cxx
{
	typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
}

namespace l7vs
{
	/*!
	 *  Logger implement class.
	 *  operate log4cxx library. 
	 *  this is singleton class. 
	 */
	class LoggerImpl
	{
	public:
		//! returns current instance.
		static LoggerImpl& getInstance();
	protected:
		//! default constructor initialize member variables.
		LoggerImpl() : initialized(false), logFilename(""), rotation(LOG_ROT_SIZE), maxBackupIndex(0), maxFileSize(0), rotationTiming(LOG_TIM_YEAR), rotationTimingValue("")
		{
			levelTable[LOG_LV_NONE] = log4cxx::Level::getDebug();
			levelTable[LOG_LV_DEBUG] = log4cxx::Level::getDebug();
			levelTable[LOG_LV_INFO] = log4cxx::Level::getInfo();
			levelTable[LOG_LV_WARN] = log4cxx::Level::getWarn();
			levelTable[LOG_LV_ERROR] = log4cxx::Level::getError();
			levelTable[LOG_LV_FATAL] = log4cxx::Level::getFatal();
#if defined(LOGGER_PROCESS_VSD)
			loggerCategory = LOG_CAT_L7VSD_LOGGER;
			loggerProcess = LOG_MOD_L7VSD;
			//set default log level
			for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
				categoryLevel[cat] = log4cxx::Level::getError();
			}
#elif defined(LOGGER_PROCESS_ADM)
			loggerCategory = LOG_CAT_L7VSADM_LOGGER;
			loggerProcess = LOG_MOD_L7VSADM;
			//set default log level
			for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
				categoryLevel[cat] = log4cxx::Level::getError();
			}
#else	//LOGGER_PROCESS_SNM
			loggerCategory = LOG_CAT_SNMPAGENT_LOGGER;
			loggerProcess = LOG_MOD_SNMPAGENT;
			//set default log level
			for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
				categoryLevel[cat] = log4cxx::Level::getError();
			}
#endif
		}
		//! cpoy constructor disable
		LoggerImpl( const LoggerImpl& );
		//! operator= disable
		LoggerImpl& operator=( const LoggerImpl& );
		//! destructor.
		virtual ~LoggerImpl() {}
		//! static Logger instance
		static LoggerImpl* instance;
		//! initialized flag
		bool initialized;
		//! logger category
		LOG_CATEGORY_TAG loggerCategory;
		//! logger process
		LOG_MODULE_TAG loggerProcess;
		//! hostname
		char hostname[HOST_NAME_LEN];

		//! if error occured, switch appenders to syslogappender and fileappender(/dev/console)
		virtual void errorConf(unsigned int messageId, const std::string& errorMessage, const char* file, int line);

		//! base logFileanme
		std::string logFilename;
		//! rotation way (size base, date base, or both size and date base)
		LOG_ROTATION_TAG rotation;
		//! number of backup log file
		unsigned int maxBackupIndex;
		//! max size of log file
		unsigned long long maxFileSize;
		/*!
		 *  rotation timing
		 *  "year"   = yearly
		 *  "monthr" = monthly
		 *  "week"   = weekly
		 *  "date"   = daily
		 *  "hour"   = hourly
		 */
		LOG_ROTATION_TIMING_TAG rotationTiming;
		/*!
		 *  rotation timing value 
		 *
		 *  rotation timing	value
		 *  -------------------------------------------------
		 *  year		"03051500"	(3/5 15:00)
		 *  month		"051100"	(5 11:00)
		 *  week		"12000"		(mon 20:00) sun = 0, sat = 6
		 *  date		"1500"		(15:00)
		 *  hour		"45"		(45)
		 */
		std::string rotationTimingValue;

		//! for transration between log4cxx::LevelPtr and LOGER_LEVEL_TAG
		static log4cxx::LevelPtr levelTable[LOGGER_LEVEL_NUM];
		//! for transration between string and LOGGER_CATEGORY_TAG
		static char categoryTable[][LOGGER_CATEGORY_NUM];
		//! holds category-loglevel
		log4cxx::LevelPtr categoryLevel[LOGGER_CATEGORY_NUM];
	
		//! LOG_LEVEL_TAG to log4cxx::LevelPtr transrator
		virtual inline const log4cxx::LevelPtr toLevel(LOG_LEVEL_TAG level)
		{
			return levelTable[level];
		}
		//! log4cxx::LevelPtr to LOG_LEVEL_TAG transrator
		virtual inline LOG_LEVEL_TAG toLevelTag(const log4cxx::LevelPtr level)
		{
			int levelInt = level->toInt();
			switch (levelInt) {
			case log4cxx::Level::DEBUG_INT:
				return LOG_LV_DEBUG;
			case log4cxx::Level::INFO_INT:
				return LOG_LV_INFO;
			case log4cxx::Level::WARN_INT:
				return LOG_LV_WARN;
			case log4cxx::Level::ERROR_INT:
				return LOG_LV_ERROR;
			case log4cxx::Level::FATAL_INT:
				return LOG_LV_FATAL;
			default: 
				return LOG_LV_DEBUG;
			}
		}
	private:
		//! loglevel tagle
		LOG_LEVEL_TAG loglevel[LOGGER_CATEGORY_NUM];

	public:
		//! initialze function
	 	virtual bool init();
		//! Configuration function
		virtual	void loadConf();
		//1 initialize loglevel table
		void initLogLevelTable();

		/*!
		 * retrieve category's log level.
		 *
		 * @param   category that want to know
		 * @return  log level
		 */
		virtual inline LOG_LEVEL_TAG getLogLevel(LOG_CATEGORY_TAG cat)
		{
#if defined(LOGGER_PROCESS_VSD)
			if (cat >= getCategoryRangeStart(LOG_MOD_SNMPAGENT) && cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT)) {
				return l7vs_snmpbridge_get_loglevel(cat);
			}
#endif
			if (LOG_LV_NONE == loglevel[cat]) {
				this->loglevel[cat] = toLevelTag(log4cxx::Logger::getLogger(categoryTable[cat])->getLevel());
			}

			return loglevel[cat];
			//return toLevelTag(log4cxx::Logger::getLogger(categoryTable[cat])->getLevel());
		}

		/*!
		 * set category's log level.
		 *
		 * @param   category to set log level
		 * @param   level
		 * @retval  true  succeed
		 * @retval  false failed
		 */
		virtual inline bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level)
		{
			try {
#if defined(LOGGER_PROCESS_VSD)
				if (cat >= getCategoryRangeStart(LOG_MOD_SNMPAGENT) && cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT)) {
					l7vs_snmpbridge_change_loglevel(cat, level);
				}
#endif
				log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(toLevel(level));
			}
			catch (const std::exception& ex) {
				return false;	
			}

			loglevel[cat] = level;
			return true;
		}

		/*!
		 * output fatal log.
		 *
		 * @param   category that logging matter occured
		 * @param   log message id 
		 * @param   log message 
		 * @param   current file 
		 * @param   current line
		 * @retrun  void
		 */
		virtual inline void putLogFatal(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
		{
			char buf[BUF_LEN];
			snprintf(buf, BUF_LEN, "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_FATAL, cat, message_id, message.c_str(), hostname);
			try {
				log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(log4cxx::Level::getFatal(), buf, log4cxx::spi::LocationInfo(file, "", line));
			}
			catch (const std::exception& ex) {
				std::ostringstream oss;
				oss << "Logging Error (Fatal Log) : " << ex.what();
				errorConf(1, oss.str(), __FILE__, __LINE__);
			}
		}
		/*!
		 * output error log.
		 *
		 * @param   category that logging matter occured
		 * @param   log message id 
		 * @param   log message 
		 * @param   current file 
		 * @param   current line
		 * @retrun  void
		 */
		//! output fatal log.
		virtual inline void putLogError(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
		{
			char buf[BUF_LEN];
			snprintf(buf, BUF_LEN, "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_ERROR, cat, message_id, message.c_str(), hostname);
			try {
				log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(log4cxx::Level::getError(), buf, log4cxx::spi::LocationInfo(file, "", line));
			}
			catch (const std::exception& ex) {
				std::ostringstream oss;
				oss << "Logging Error (Error Log) : " << ex.what();
				errorConf(2, oss.str(), __FILE__, __LINE__);
			}
		}
		/*!
		 * output warn log.
		 *
		 * @param   category that logging matter occured
		 * @param   log message id 
		 * @param   log message 
		 * @param   current file 
		 * @param   current line
		 * @retrun  void
		 */
		//! output fatal log.
		virtual inline void putLogWarn(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
		{
			char buf[BUF_LEN];
			snprintf(buf, BUF_LEN, "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_WARN, cat, message_id, message.c_str(), hostname);
			try {
				log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(log4cxx::Level::getWarn(), buf, log4cxx::spi::LocationInfo(file, "", line));
			}
			catch (const std::exception& ex) {
				std::ostringstream oss;
				oss << "Logging Error (Warn Log) : " << ex.what();
				errorConf(3, oss.str(), __FILE__, __LINE__);
			}
		}
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
		//! output fatal log.
		virtual inline void putLogInfo(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
		{
			char buf[BUF_LEN];
			snprintf(buf, BUF_LEN, "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_INFO, cat, message_id, message.c_str(), hostname);
			try {
				log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(log4cxx::Level::getInfo(), buf, log4cxx::spi::LocationInfo(file, "", line));
			}
			catch (const std::exception& ex) {
				std::ostringstream oss;
				oss << "Logging Error (Info Log) : " << ex.what();
				errorConf(4, oss.str(), __FILE__, __LINE__);
			}
		}
		/*!
		 * output debug log.
		 *
		 * @param   category that logging matter occured
		 * @param   log message id 
		 * @param   log message 
		 * @param   current file 
		 * @param   current line
		 * @retrun  void
		 */
		virtual inline void putLogDebug(LOG_CATEGORY_TAG cat, const unsigned int message_id, const std::string& message, const char *file, int line)
		{
			char buf[BUF_LEN];
			snprintf(buf, BUF_LEN, "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_DEBUG, cat, message_id, message.c_str(), hostname);
			try {
				log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(log4cxx::Level::getDebug(), buf, log4cxx::spi::LocationInfo(file, "", line));
			}
			catch (const std::exception& ex) {
				std::ostringstream oss;
				oss << "Logging Error (Debug Log) : " << ex.what();
				errorConf(5, oss.str(), __FILE__, __LINE__);
			}
		}

		/*!
		 * return start category by using module.
		 *
		 * @param   module
		 * @retrun  start category
		 */
		virtual inline LOG_CATEGORY_TAG getCategoryRangeStart(LOG_MODULE_TAG mod)
		{
			switch (mod) {
				case LOG_MOD_L7VSADM: return LOG_CAT_L7VSADM_PARSE;
				case LOG_MOD_SNMPAGENT: return LOG_CAT_SNMPAGENT_START_STOP;
				default: return LOG_CAT_L7VSD_NETWORK;
			}
		}
		/*!
		 * return end category by using module.
		 *
		 * @param   module
		 * @retrun  end category
		 */
		virtual inline LOG_CATEGORY_TAG getCategoryRangeEnd(LOG_MODULE_TAG mod)
		{
			switch (mod) {
				case LOG_MOD_L7VSADM: return LOG_CAT_L7VSADM_MODULE;
				case LOG_MOD_SNMPAGENT: return LOG_CAT_SNMPAGENT_PARAMETER;
				default: return LOG_CAT_L7VSD_MODULE;
			}
		}
	};
}	//namespace l7vs
#endif	//__LOGGER_IMPL_H__
