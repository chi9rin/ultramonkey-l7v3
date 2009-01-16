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

#ifndef LOGGER_IMPL_H
#define LOGGER_IMPL_H

#include <sttream>
#include <map>
#include <log4cxx/logger.h>
#include <log4cxx/level.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>
#include <boost/format.hpp>
#include "logger_enum.h"
#include "logger_rotation_enum.h"

#define LOGGER_LEVEL_NUM (6)
#define LOGGER_NULL "/dev/null"


namespace log4cxx
{
	typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
}

namespace l7vs{

//! @class LoggerImpl
//! @brief Logger implement class.
//! @brief this class manage logger setting and logging by log4cxx.
class LoggerImpl{
protected:
	//! typedef	category <-> level map
	typedef	std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
		category_level_map_type;
	//! typedef	categoryname <-> CATEGORY_TAG map
	typedef	std::map< std::string, LOG_CATEGORY_TAG>
		name_category_map_type;
	//! typedef CATEGORY_TAG <-> categoryname map
	typedef	std::map< LOG_CATEGORY_TAG, std::string>
		category_name_map_type;
public:
	//! returns current instance.
	static LoggerImpl& getInstance();

	//! initialze function
 	virtual bool init();

	//! Configuration function
	virtual	void loadConf();

	/*!
	 * retrieve category's log level.
	 *
	 * @param   category that want to know
	 * @return  log level
	 */
	virtual inline LOG_LEVEL_TAG getLogLevel(LOG_CATEGORY_TAG cat){
		category_level_map_type::iterator itr = category_level_map.find( cat );
		return itr->second;
	}

	/*!
	 * set category's log level.
	 *
	 * @param   category to set log level
	 * @param   level
	 * @retval  true  succeed
	 * @retval  false failed
	 */
	virtual inline bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level){
		category_level_map_type::iterator lv_itr = category_level_map.find( cat );
		lv_itr->second = level;

		category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );

		try {
			log4cxx::Logger::getLogger(categoryname_itr->second)->setLevel(levelTable[level));
		}
		catch (const std::exception& ex) {
			return false;
		}
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
	virtual inline void putLogFatal(	LOG_CATEGORY_TAG cat,
										const unsigned int message_id,
										const std::string& message,
										const char *file,
										int line){
	std::stringstream buf;
	buf << boost::format( "%s%d%03d%04d %s %s" )
		% LOGGER_PROCESS_ID
		% LOG_LV_FATAL
		% cat
		% message_id
		% message.c_str()
		% hostname;
	try {
			log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(	log4cxx::Level::getFatal(),
																		buf,
																		log4cxx::spi::LocationInfo(file, "", line));
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
	virtual inline void putLogError(	LOG_CATEGORY_TAG cat,
										const unsigned int message_id,
										const std::string& message,
										const char *file,
										int line){
		std::stringstream	buf;
		buf << boost::format( "%s%d%03d%04d %s %s" )
			% LOGGER_PROCESS_ID
			% LOG_LV_ERROR
			% cat
			% message_id
			% message.c_str()
			% hostname;
		try {
			log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(	log4cxx::Level::getError(),
																		buf,
																		log4cxx::spi::LocationInfo(file, "", line));
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
	virtual inline void putLogWarn(		LOG_CATEGORY_TAG cat,
										const unsigned int message_id,
										const std::string& message,
										const char *file,
										int line){
		stringstream buf;
		buf << boost::format( "%s%d%03d%04d %s %s" )
			% LOGGER_PROCESS_ID
			% LOG_LV_WARN
			% cat
			% message_id
			% message.c_str()
			% hostname);
		try {
			log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(	log4cxx::Level::getWarn(),
																		buf,
																		og4cxx::spi::LocationInfo(file, "", line));
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
	virtual inline void putLogInfo(		LOG_CATEGORY_TAG cat,
										const unsigned int message_id,
										const std::string& message,
										const char *file,
										int line){
		stringstream	buf;
		buff << boost::format( "%s%d%03d%04d %s %s" )
			% LOGGER_PROCESS_ID
			% LOG_LV_INFO
			% cat
			% message_id
			% message.c_str()
			% hostname;
		try {
			log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(	log4cxx::Level::getInfo(),
																		buf,
																		log4cxx::spi::LocationInfo(file, "", line));
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
	virtual inline void putLogDebug(	LOG_CATEGORY_TAG cat,
										const unsigned int message_id,
										const std::string& message,
										const char *file,
										int line){
		std::stringstream	buf;
		buf << boost::format( "%s%d%03d%04d %s %s" )
			% LOGGER_PROCESS_ID
			% LOG_LV_DEBUG
			% cat
			% message_id
			% message
			% hostname;
		try {
			log4cxx::Logger::getLogger(categoryTable[cat])->forcedLog(	log4cxx::Level::getDebug(),
																		buf, log4cxx::spi::LocationInfo(file,"", line));
		}
		catch (const std::exception& ex) {
			std::ostringstream oss;
			oss << "Logging Error (Debug Log) : " << ex.what();
			errorConf(5, oss.str(), __FILE__, __LINE__);
		}
	}

protected:
	//! default constructor initialize member variables.
	LoggerImpl() : initialized(false){}

	//! cpoy constructor disable
	LoggerImpl( const LoggerImpl& ){}

	//! operator= disable
	LoggerImpl& operator=( const LoggerImpl& ){}

	//! LOG_LEVEL_TAG to log4cxx::LevelPtr transrator
	virtual inline const log4cxx::LevelPtr toLevel(LOG_LEVEL_TAG level)	{
		return levelTable[level];
	}

	//! if error occured, switch appenders to syslogappender and fileappender(/dev/console)
	virtual void errorConf	unsigned int messageId,
							const std::string& errorMessage,
							const char* file,
							int line);
	virtual void logic_error( std::string& );

	//! log4cxx::LevelPtr to LOG_LEVEL_TAG transrator
	virtual inline LOG_LEVEL_TAG toLevelTag(const log4cxx::LevelPtr level){
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
	//! destructor.
	virtual ~LoggerImpl() {}
	//! static Logger instance
	static LoggerImpl* instance;
	//! initialized flag
	bool initialized;
	//! hostname
	std::string hostname;

	//! category - loglevel hash map
	category_map_type		category_level_map;
	//! category string -> logcateogry hash map
	name_category_map_type	name_category_map_type;
	//! log_category -> category string hash map
	category_name_map_type	category_name_map_type;

	struct appender_property{
		std::string 	log_filename_key;			// parameter logfile key
		std::string		log_filename_value;			// logfile value
		std::string 	rotation_key;				// parameter rotation timing key
		LOG_ROTATION_TAG							// rotation timing
						rotation_value;
		std::string 	max_backup_index_key;		// parameter backup maximam number key
		unsigned int	max_backup_index_value;		// backup maximam number
		std::string 	max_file_size_key;			// parameter maximam backup size key
		unsigned long long
						 	max_file_size_value;	// maximam backup size
		std::string		rotation_timing_key;		// parameter rotation timing key
		LOG_ROTATION_TIMING_TAG
						rotation_timing_value;		// rotation timing
		logile_property() :							// constractor
			rotation(LOG_ROT_SIZE),
			maxBackupIndex(0),
			maxFileSize(0),
			rotationTiming(LOG_TIM_YEAR),
		{}
	};
	logfile_property	nomal_log_property;
	logfile_property	access_log_property;
};

}	//namespace l7vs
#endif	//__LOGGER_IMPL_H__
