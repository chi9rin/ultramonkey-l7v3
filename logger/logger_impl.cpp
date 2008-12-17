/*
 * @file  logger_impl.cpp
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

#include "logger_impl.h"
#include "parameter.h"
#include "lexical_cast.h"
#include "strict_time_based_rolling_policy.h"
#include "time_and_size_based_rolling_policy.h"

#define LOGGER_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %c %m %t %F:%L%n"
#define LOGGER_DEFAULT_BUFFER_SIZE (8 * 1024)
#define LOGGER_SYSLOG_FACILITY "USER"
#define LOGGER_BACKUP_INDEX_LOWER_LIMIT (1)
#define LOGGER_BACKUP_INDEX_LIMIT (12)
#define LOGGER_FILESIZE_LOWER_LIMIT (65535)
#define LOGGER_FILE_PATTERN "%i"

#if defined(LOGGER_PROCESS_VSD)
#define LOGGER_LOG_FILENAME_KEY "l7vsd_log_filename"
#define LOGGER_ROTATION_KEY "l7vsd_rotation"
#define LOGGER_MAX_BACKUP_INDEX_KEY "l7vsd_max_backup_index"
#define LOGGER_MAX_FILE_SIZE_KEY "l7vsd_max_filesize"
#define LOGGER_ROTATION_TIMING_KEY "l7vsd_rotation_timing"
#define LOGGER_ROTATION_TIMING_VALUE_KEY "l7vsd_rotation_timing_value"

#elif defined(LOGGER_PROCESS_ADM)
#define LOGGER_LOG_FILENAME_KEY "l7vsadm_log_filename"
#define LOGGER_ROTATION_KEY "l7vsadm_rotation"
#define LOGGER_MAX_BACKUP_INDEX_KEY "l7vsadm_max_backup_index"
#define LOGGER_MAX_FILE_SIZE_KEY "l7vsadm_max_filesize"
#define LOGGER_ROTATION_TIMING_KEY "l7vsadm_rotation_timing"
#define LOGGER_ROTATION_TIMING_VALUE_KEY "l7vsadm_rotation_timing_value"
#else	//LOGGER_PROCESS_SNM
#define LOGGER_LOG_FILENAME_KEY "snmpagent_log_filename"
#define LOGGER_ROTATION_KEY "snmpagent_rotation"
#define LOGGER_MAX_BACKUP_INDEX_KEY "snmpagent_max_backup_index"
#define LOGGER_MAX_FILE_SIZE_KEY "snmpagent_max_filesize"
#define LOGGER_ROTATION_TIMING_KEY "snmpagent_rotation_timing"
#define LOGGER_ROTATION_TIMING_VALUE_KEY "snmpagent_rotation_timing_value"
#endif

//! for transration between log4cxx::LevelPtr and LOGER_LEVEL_TAG
char l7vs::LoggerImpl::categoryTable[][LOGGER_CATEGORY_NUM] = { 
	"none",
	"l7vsd_network",
	"l7vsd_network.bandwidth",
	"l7vsd_network.num_connection",
	"l7vsd_network.qos",
	"l7vsd_virtual_service",
	"l7vsd_real_server",
	"l7vsd_sorry_server",
	"l7vsd_real_server.balancing",
	"l7vsd_replication",
	"l7vsd_start_stop",
	"l7vsd_system",
	"l7vsd_system.memory",
	"l7vsd_system.socket",
	"l7vsd_system.signal",
	"l7vsd_environment",
	"l7vsd_environment.parameter",
	"l7vsd_logger",
	"l7vsd_parameter",
	"l7vsd_event",
	"l7vsd_schedule",
	"l7vsd_program",
	"l7vsd_protocol",
	"l7vsd_module",
	"l7vsadm_parse",
	"l7vsadm_operate",
	"l7vsadm_communicate",
	"l7vsadm_config_result",
	"l7vsadm_common",
	"l7vsadm_logger",
	"l7vsadm_parameter",
	"l7vsadm_protocol",
	"l7vsadm_module",
	"snmpagent_start_stop",
	"snmpagent_manager_receive",
	"snmpagent_manager_send",
	"snmpagent_l7vsd_receive",
	"snmpagent_l7vsd_send",
	"snmpagent_logger",
	"snmpagent_parameter"
	};

//! for transration between string and LOGGER_CATEGORY_TAG
log4cxx::LevelPtr l7vs::LoggerImpl::levelTable[LOGGER_LEVEL_NUM];

/*!
 * returns single instance.
 *
 * @param   void
 * @return  instance
 */
l7vs::LoggerImpl& l7vs::LoggerImpl::getInstance()
{
	if (!instance) {
		instance = new LoggerImpl;
	}
	return *instance;
}

//! static Logger instance pointer initialized by 0.
l7vs::LoggerImpl* l7vs::LoggerImpl::instance = 0;

/*!
 * initialize function.
 * logger initialized to use syslogappender and fileappender(/dev/console)
 *
 * @param   void
 * @retval  true succeed
 * @retval  false failed
 */
bool l7vs::LoggerImpl::init()
{
	int ret = 0;
	if (initialized) return false;

	try {
		log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
		if (0 == root) {
			return false;
		}
		log4cxx::LayoutPtr layout =
			new log4cxx::PatternLayout(LOGGER_LAYOUT);
		log4cxx::net::SyslogAppenderPtr syslogAppender =
			new log4cxx::net::SyslogAppender(layout, log4cxx::net::SyslogAppender::getFacility(LOGGER_SYSLOG_FACILITY));
		root->addAppender(syslogAppender);
		log4cxx::WriterAppender* consoleAppender =
			new log4cxx::ConsoleAppender( layout, log4cxx::ConsoleAppender::getSystemErr() );
		root->addAppender(consoleAppender);

		for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
			log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
		}
	}
	catch (const std::exception& e) {
		std::ostringstream oss;
		oss <<  "Logger Initialization Failed : " << e.what();
		errorConf(6, oss.str(), __FILE__, __LINE__);
		return false;
	}
	
	ret = gethostname(hostname, HOST_NAME_LEN);
	if (0 > ret) {
		if (LOG_LV_WARN >= this->getLogLevel(loggerCategory)) {
			this->putLogWarn(loggerCategory,1, "Fail to get Hostname", __FILE__, __LINE__);
		}
	}	

	if (LOG_LV_INFO >= this->getLogLevel(loggerCategory)) {
		this->putLogInfo(loggerCategory,1, "Logger Initialized.", __FILE__, __LINE__);
	}

	initialized = true;
	return true;
}

/*!
 * error handling function.
 * if error occured, switch appenders to syslogappender and fileappender(/dev/console)
 * message will output to syslog/fileappender appender
 * 
 * @param   log message id 
 * @param   log message 
 * @param   current file 
 * @param   current line
 * @return  void
 */
void l7vs::LoggerImpl::errorConf(unsigned int message_id, const std::string& errorMessage, const char* file, int line)
{
	try {
		log4cxx::LogManager::resetConfiguration();
		log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
		if (0 == root)
			return;
		log4cxx::LayoutPtr layout =
			new log4cxx::PatternLayout(LOGGER_LAYOUT);
		log4cxx::net::SyslogAppenderPtr syslogAppender =
			new log4cxx::net::SyslogAppender(layout, log4cxx::net::SyslogAppender::getFacility(LOGGER_SYSLOG_FACILITY));
		log4cxx::ConsoleAppender* consoleAppender = new log4cxx::ConsoleAppender(layout, log4cxx::ConsoleAppender::getSystemErr() );
                root->addAppender(consoleAppender);
		root->addAppender(syslogAppender);

		for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
			log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
		}

		char buf[BUF_LEN];
		snprintf(buf, BUF_LEN, "%s%d%03d%04d %s %s", LOGGER_PROCESS_ID, LOG_LV_FATAL, loggerCategory, message_id, errorMessage.c_str(), hostname);
		log4cxx::Logger::getLogger(categoryTable[loggerCategory])->forcedLog(log4cxx::Level::getFatal(), buf, log4cxx::spi::LocationInfo(file, "", line));
	}
	catch (const std::exception& e) {
		std::ostringstream oss;
		oss <<  "Logger Error Output Failed : " << e.what() << "\n";
		fputs(oss.str().c_str(), stderr);
	}
}

/*!
 * load the logger parameter.
 * get settings from parameter component, and configure log4cxx property
 *
 * @param   void
 * @return  void
 */
void l7vs::LoggerImpl::loadConf()
{
	std::string ret;

	//get log filename
	if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY)) {
		logFilename = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_LOG_FILENAME_KEY);
	}
	else {
		if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
			this->putLogError(loggerCategory,1, "Not Exist Log Filename Setting.", __FILE__, __LINE__);
		}
		throw std::logic_error("Not Exist Log Filename Setting.");
	}
	
	//get rotation
	if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY)) {
		std::string rotationStr = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_KEY);
		if ("size" == rotationStr) rotation = LOG_ROT_SIZE;
		else if ("date" == rotationStr) rotation = LOG_ROT_DATE;
		else if ("datesize" == rotationStr) rotation = LOG_ROT_DATESIZE;
		else {
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,2, "Invalid Log Rotation Setting.", __FILE__, __LINE__);
			}
			throw std::logic_error("Invalid Log Rotation Setting.");
		}
	}
	else {
		if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
			this->putLogError(loggerCategory,3, "Not Exist Log Rotation Setting.", __FILE__, __LINE__);
		}
		throw std::logic_error("Not Exist Log Rotation Setting.");
	}

	//get max backup index
	if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY)) {
		std::string maxBackupIndexStr = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_BACKUP_INDEX_KEY);
		try {
			maxBackupIndex = lexical_cast<unsigned int>(maxBackupIndexStr);
		}
		catch (const l7vs::bad_lexical_cast& bc) {
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,4, "Invalid MaxBackupIndex Value.", __FILE__, __LINE__);
			}
			throw std::logic_error("Invalid MaxBackupIndex Value.");
		}
		if (LOGGER_BACKUP_INDEX_LOWER_LIMIT > maxBackupIndex) {
			std::ostringstream oss;
			oss << "Max Backup Index must at least " << LOGGER_BACKUP_INDEX_LOWER_LIMIT << ".";
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,5, oss.str(), __FILE__, __LINE__);
			}
			throw std::logic_error(oss.str());
		}		
		if (LOGGER_BACKUP_INDEX_LIMIT < maxBackupIndex) {
			std::ostringstream oss;
			oss << "Max Backup Index must at most " << LOGGER_BACKUP_INDEX_LIMIT << ".";
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,6, oss.str(), __FILE__, __LINE__);
			}
			throw std::logic_error(oss.str());
		}		
	}
	else {
		if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
			this->putLogError(loggerCategory,7, "Not Exist Log MaxBackupIndex Setting.", __FILE__, __LINE__);
		}
		throw std::logic_error("Not Exist Log MaxBackupIndex Setting.");
	}

	if (LOG_ROT_SIZE == rotation || LOG_ROT_DATESIZE == rotation) {
		// get max file size
		std::string maxFileSizeStr;
		if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY)) {
			maxFileSizeStr = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_MAX_FILE_SIZE_KEY);
		}
		else {
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,8, "Not Exist Log MaxFileSize Setting.", __FILE__, __LINE__);
			}
			throw std::logic_error("Not Exist Log MaxFileSize Setting.");
		}
		
		std::string size_val;
		std::string last_str = maxFileSizeStr.substr(maxFileSizeStr.length() - 1, 1);
		// when unit was specified
		if (("K" == last_str) || ("M" == last_str) || ("G" == last_str)) {
			size_val = maxFileSizeStr.substr(0, maxFileSizeStr.length() - 1);
		}
		else {
			size_val = maxFileSizeStr;
		}
			
		try {
			maxFileSize = lexical_cast<size_t>(size_val);
		}
		catch (const l7vs::bad_lexical_cast& bc) {
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,9, "Invalid FileSize Value.", __FILE__, __LINE__);
			}
			throw std::logic_error("Invalid FileSize Value.");
		}

		if ("K" == last_str) {
			if ((ULLONG_MAX / 1024) < maxFileSize) {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,10, "Invalid FileSize Value.", __FILE__, __LINE__);
				}
				throw std::logic_error("Invalid FileSize Value.");
			}
			maxFileSize = maxFileSize * 1024;
		}
		else if ("M" == last_str) {
			if ((ULLONG_MAX / 1024 / 1024) < maxFileSize) {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,11, "Invalid FileSize Value.", __FILE__, __LINE__);
				}
				throw std::logic_error("Invalid FileSize Value.");
			}
			maxFileSize = maxFileSize * 1024 * 1024;
		}
		else if ("G" == last_str) {
			if ((ULLONG_MAX / 1024 / 1024 / 1024) < maxFileSize) {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,12, "Invalid FileSize Value.", __FILE__, __LINE__);
				}
				throw std::logic_error("Invalid FileSize Value.");
			}
			maxFileSize = maxFileSize * 1024 * 1024 * 1024;
		}
		if (LOGGER_FILESIZE_LOWER_LIMIT > maxFileSize) {
			int limit = LOGGER_FILESIZE_LOWER_LIMIT;
			std::ostringstream oss;
			oss << "FileSize must at least " << limit << " bytes.";
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,13, oss.str(), __FILE__, __LINE__);
			}
			throw std::logic_error(oss.str());
		}
	}

	if (LOG_ROT_DATE == rotation || LOG_ROT_DATESIZE == rotation) {
		// get rotation timing
		if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY)) {
			std::string rotationTimingStr = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_KEY);
			if ("year" == rotationTimingStr) rotationTiming = LOG_TIM_YEAR;
			else if ("month" == rotationTimingStr) rotationTiming = LOG_TIM_MONTH;
			else if ("week" == rotationTimingStr) rotationTiming = LOG_TIM_WEEK;
			else if ("date" == rotationTimingStr) rotationTiming = LOG_TIM_DATE;
			else if ("hour" == rotationTimingStr) rotationTiming = LOG_TIM_HOUR;
			else {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,14, "Invalid Log RotationTiming Setting.", __FILE__, __LINE__);
				}
				throw std::logic_error("Invalid Log RotationTiming Setting.");
			}
		}
		else {
			if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
				this->putLogError(loggerCategory,15, "Not Exist Log RotaionTiming Setting.", __FILE__, __LINE__);
			}
			throw std::logic_error("Not Exist Log RotaionTiming Setting.");
		}

		if (LOG_TIM_YEAR == rotationTiming) {
			if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY)) {
				ret = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);

				std::string::size_type fpos = 0;
				std::string::size_type rpos = 0;
				int month = 0;
				int date = 0;
				int hour = 0;
				int minute = 0;
				// find month
				rpos = ret.find_first_of('/', fpos);
				if (std::string::npos != rpos) {
					std::string monthStr = ret.substr(fpos, rpos - fpos);
					try {
						month = lexical_cast<int>(monthStr);
					}
					catch (const l7vs::bad_lexical_cast& bc) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,16, "Parse Timing Year Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Year Error.");
					}
					if (1 > month || month > 12) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,17, "Parse Timing Year Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Year Error.");
					}
					fpos = rpos + 1;
					// find date
					rpos = ret.find_first_of(' ', fpos);
					if (std::string::npos != rpos) {
						std::string dateStr = ret.substr(fpos, rpos - fpos);
						try {
							date = lexical_cast<int>(dateStr);
						}
						catch (const l7vs::bad_lexical_cast& bc) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,18, "Parse Timing Year Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Year Error.");
						}
						if (1 > date || date > 31) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,19, "Parse Timing Year Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Year Error.");
						}
						int dates[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
						if (date > dates[month - 1]) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,20, "Parse Timing Year Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Year Error.");
						}
						fpos = rpos + 1;
						// find hour 
						rpos = ret.find_first_of(':', fpos);
						if (std::string::npos != rpos) {
							std::string hourStr = ret.substr(fpos, rpos - fpos);
							try {
								hour = lexical_cast<int>(hourStr);
							}
							catch (const l7vs::bad_lexical_cast& bc) {
								if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
									this->putLogError(loggerCategory,21, "Parse Timing Year Error.", __FILE__, __LINE__);
								}
								throw std::logic_error("Parse Timing Year Error.");
							}
							if (0 > hour || hour > 23) {
								if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
									this->putLogError(loggerCategory,22, "Parse Timing Year Error.", __FILE__, __LINE__);
								}
								throw std::logic_error("Parse Timing Year Error.");
							}
							// minute
							std::string minuteStr = ret.substr(rpos + 1);
							try {
								minute = lexical_cast<int>(minuteStr);
							}
							catch (const l7vs::bad_lexical_cast& bc) {
								if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
									this->putLogError(loggerCategory,23, "Parse Timing Year Error.", __FILE__, __LINE__);
								}
								throw std::logic_error("Parse Timing Year Error.");
							}
							if (0 > minute || minute > 59) {
								if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
									this->putLogError(loggerCategory,24, "Parse Timing Year Error.", __FILE__, __LINE__);
								}
								throw std::logic_error("Parse Timing Year Error.");
							}
						}
						else {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,25, "Parse Timing Year Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Year Error.");
						}
					}
					else {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {	
							this->putLogError(loggerCategory,26, "Parse Timing Year Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Year Error.");
					}
				}
				else {
					if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
						this->putLogError(loggerCategory,27, "Parse Timing Year Error.", __FILE__, __LINE__);
					}
					throw std::logic_error("Parse Timing Year Error.");
				}

				// format to internal rotation timing value expresson
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(2) << month
					<< std::setfill('0') << std::setw(2) << date
					<< std::setfill('0') << std::setw(2) << hour
					<< std::setfill('0') << std::setw(2) << minute;
				
				rotationTimingValue = oss.str();

			}
			else {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,28, "Not Exist Log RotaionTiming Year Setting.", __FILE__, __LINE__);
				}
				throw std::logic_error("Not Exist Log RotaionTiming Year Setting.");
			}
		}

		if (LOG_TIM_MONTH == rotationTiming) {
			if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY)) {
				ret = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);

				std::string::size_type fpos = 0;
				std::string::size_type rpos = 0;
				int date = 0;
				int hour = 0;
				int minute = 0;
				// find day
				rpos = ret.find_first_of(' ', fpos);
				if (std::string::npos != rpos) {
					std::string dateStr = ret.substr(fpos, rpos - fpos);
					try {
						date = lexical_cast<int>(dateStr);
					}
					catch (const l7vs::bad_lexical_cast& bc) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,29, "Parse Timing Month Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Month Error.");
					}
					if (1 > date || date > 31) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,30, "Parse Timing Month Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Month Error.");
					}
					fpos = rpos + 1;
					// find hour
					rpos = ret.find_first_of(':', fpos);
					if (std::string::npos != rpos) {
						std::string hourStr = ret.substr(fpos, rpos - fpos);
						try {
							hour = lexical_cast<int>(hourStr);
						}
						catch (const l7vs::bad_lexical_cast& bc) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,31, "Parse Timing Month Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Month Error.");
						}
						if (0 > hour || hour > 23) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,32, "Parse Timing Month Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Month Error.");
						}
						// minute
						std::string minuteStr = ret.substr(rpos + 1);
						try {
							minute = lexical_cast<int>(minuteStr);
						}
						catch (const l7vs::bad_lexical_cast& bc) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,33, "Parse Timing Month Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Month Error.");
						}
						if (0 > minute || minute > 59) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,34, "Parse Timing Month Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Month Error.");
						}
					}
					else {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,35, "Parse Timing Month Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Month Error.");
					}
				}
				else {
					if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
						this->putLogError(loggerCategory,36, "Parse Timing Month Error.", __FILE__, __LINE__);
					}
					throw std::logic_error("Parse Timing Month Error.");
				}

				// format to internal rotation timing value expresson
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(2) << date
					<< std::setfill('0') << std::setw(2) << hour
					<< std::setfill('0') << std::setw(2) << minute;
				
				rotationTimingValue = oss.str();

			}
			else {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,37, "Not Exist Log RotaionTiming Month Setting.", __FILE__, __LINE__);
				}
				throw std::logic_error("Not Exist Log RotaionTiming Month Setting.");
			}
		}

		if (LOG_TIM_WEEK == rotationTiming) {
			if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY)) {
				ret = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);

				std::string::size_type fpos = 0;
				std::string::size_type rpos = 0;
				int week = 0;
				int hour = 0;
				int minute = 0;
				rpos = ret.find_first_of(' ', fpos);
				//find week
				if (std::string::npos != rpos) {
					std::string weekStr = ret.substr(fpos, rpos - fpos);

					if ("sun" == weekStr) week = 0;
					else if ("mon" == weekStr) week = 1;
					else if ("tue" == weekStr) week = 2;
					else if ("wed" == weekStr) week = 3;
					else if ("thu" == weekStr) week = 4;
					else if ("fri" == weekStr) week = 5;
					else if ("sat" == weekStr) week = 6;
					else {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,38, "Parse Timing Week Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Week Error.");
					}
					fpos = rpos + 1;
					// find hour
					rpos = ret.find_first_of(':', fpos);
					if (std::string::npos != rpos) {
						std::string hourStr = ret.substr(fpos, rpos - fpos);
						try {
							hour = lexical_cast<int>(hourStr);
						}
						catch (const l7vs::bad_lexical_cast& bc) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,39, "Parse Timing Week Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Week Error.");
						}
						if (0 > hour || hour > 23) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,40, "Parse Timing Week Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Week Error.");
						}
						// minute
						std::string minuteStr = ret.substr(rpos + 1);
						try {
							minute = lexical_cast<int>(minuteStr);
						}
						catch (const l7vs::bad_lexical_cast& bc) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,41, "Parse Timing Week Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Week Error.");
						}
						if (0 > minute || minute > 59) {
							if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
								this->putLogError(loggerCategory,42, "Parse Timing Week Error.", __FILE__, __LINE__);
							}
							throw std::logic_error("Parse Timing Week Error.");
						}
					}
					else {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {	
							this->putLogError(loggerCategory,43, "Parse Timing Week Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Week Error.");
					}
				}
				else {
					if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
						this->putLogError(loggerCategory,44, "Parse Timing Week Error.", __FILE__, __LINE__);
					}
					throw std::logic_error("Parse Timing Week Error.");
				}

				// format to internal rotation timing value expresson
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(1) << week
					<< std::setfill('0') << std::setw(2) << hour
					<< std::setfill('0') << std::setw(2) << minute;
				
				rotationTimingValue = oss.str();

			}
			else {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,45, "Not Exist Log RotaionTiming Week Setting.", __FILE__, __LINE__);
				}
				throw std::logic_error("Not Exist Log RotaionTiming Week Setting.");
			}
		}

		if (LOG_TIM_DATE == rotationTiming) {
			if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY)) {
				ret = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);

				std::string::size_type fpos = 0;
				std::string::size_type rpos = 0;
				int hour = 0;
				int minute = 0;
				//find time
				rpos = ret.find_first_of(':', fpos);
				if (std::string::npos != rpos) {
					std::string hourStr = ret.substr(fpos, rpos - fpos);
					try {
						hour = lexical_cast<int>(hourStr);
					}
					catch (const l7vs::bad_lexical_cast& bc) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,46, "Parse Timing Date Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Date Error.");
					}
					if (0 > hour || hour > 23) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,47, "Parse Timing Date Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Date Error.");
					}
					// minute
					std::string minuteStr = ret.substr(rpos + 1);
					try {
						minute = lexical_cast<int>(minuteStr);
					}
					catch (const l7vs::bad_lexical_cast& bc) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,48, "Parse Timing Date Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Date Error.");
					}
					if (0 > minute || minute > 59) {
						if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
							this->putLogError(loggerCategory,49, "Parse Timing Date Error.", __FILE__, __LINE__);
						}
						throw std::logic_error("Parse Timing Date Error.");
					}
				}
				else {
					if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
						this->putLogError(loggerCategory,50, "Parse Timing Date Error.", __FILE__, __LINE__);
					}
					throw std::logic_error("Parse Timing Date Error.");
				}

				// format to internal rotation timing value expresson
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(2) << hour
					<< std::setfill('0') << std::setw(2) << minute;
				
				rotationTimingValue = oss.str();

			}
			else {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,51, "Not Exist Log RotaionTiming Date Setting.", __FILE__, __LINE__);
				}
				throw std::logic_error("Not Exist Log RotaionTiming Date Setting.");
			}
		}

		if (LOG_TIM_HOUR == rotationTiming) {
			if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY)) {
				ret = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, LOGGER_ROTATION_TIMING_VALUE_KEY);

				// minute
				int minute = 0;
				try {
					minute = lexical_cast<int>(ret);
				}
				catch (const l7vs::bad_lexical_cast& bc) {
					if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
						this->putLogError(loggerCategory,52, "Parse Timing Hour Error.", __FILE__, __LINE__);
					}
					throw std::logic_error("Parse Timing Hour Error.");
				}
				if (0 > minute || minute > 59) {
					if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
						this->putLogError(loggerCategory,53, "Parse Timing Hour Error.", __FILE__, __LINE__);
					}
					throw std::logic_error("Parse Timing Hour Error.");
				}

				// format to internal rotation timing value expresson
				std::ostringstream oss;
				oss << std::setfill('0') << std::setw(2) << minute;
				
				rotationTimingValue = oss.str();

			}
			else {
				if (LOG_LV_ERROR >= this->getLogLevel(loggerCategory)) {
					this->putLogError(loggerCategory,54, "Not Exist Log RotaionTiming Hour Setting.", __FILE__, __LINE__);
				}
				throw std::logic_error("Not Exist Log RotaionTiming Hour Setting.");
			}
		}
	}

	try {
		// reset current configuration
		log4cxx::helpers::Pool pool;
		log4cxx::LogManager::resetConfiguration();
		log4cxx::LoggerPtr root = log4cxx::Logger::getRootLogger();
		if (0 == root) {
			throw std::logic_error("getRootLogger Failed.");
		}
		log4cxx::LayoutPtr layout =
			new log4cxx::PatternLayout(LOGGER_LAYOUT);

		switch (rotation) {
		case LOG_ROT_SIZE:
			{
				// create FixedWindowRollingPolicy
				log4cxx::rolling::FixedWindowRollingPolicyPtr fixedRollingPolicy =
					new log4cxx::rolling::FixedWindowRollingPolicy();
	
				// setting minIndex
				fixedRollingPolicy->setMinIndex(1);
	
				// setting maxIndex
				fixedRollingPolicy->setMaxIndex(maxBackupIndex);

				// setting FileNamePattern
				std::ostringstream sizeFile;
				sizeFile << logFilename << "." << LOGGER_FILE_PATTERN;
				fixedRollingPolicy->setFileNamePattern(sizeFile.str());
	
				// create SizeBasedTriggeringPolicy
				log4cxx::rolling::SizeBasedTriggeringPolicyPtr sizeTriggeringPolicy =
					new log4cxx::rolling::SizeBasedTriggeringPolicy();

				// setting maxFileSize
				sizeTriggeringPolicy->setMaxFileSize(maxFileSize);
	
				// create RollingFileAppender
				log4cxx::rolling::RollingFileAppenderPtr sizeAppender =
					new log4cxx::rolling::RollingFileAppender();
	
				// set layout
				sizeAppender->setLayout(layout);
	
				// set RollingPolicy
				sizeAppender->setRollingPolicy(fixedRollingPolicy);
	
				// set TriggeringPolicy
				sizeAppender->setTriggeringPolicy(sizeTriggeringPolicy);

				// set Log Filename
				sizeAppender->setFile(logFilename, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
	
				// activate appender options
				sizeAppender->activateOptions(pool);
	
				// add size_base_appender to rootLogger
				root->addAppender(sizeAppender);
	
				break;
			}
		case LOG_ROT_DATE:
			{
				// create StrictTimeBasedRollingPolicy
				log4cxx::rolling::StrictTimeBasedRollingPolicyPtr strictRollingPolicy =
					new log4cxx::rolling::StrictTimeBasedRollingPolicy();
	
				// setting minIndex
				strictRollingPolicy->setMinIndex(1);
	
				// setting maxIndex
				strictRollingPolicy->setMaxIndex(maxBackupIndex);

				// setting FileNamePattern
				std::ostringstream dateFile;
				dateFile << logFilename << "." << LOGGER_FILE_PATTERN;
				strictRollingPolicy->setFileNamePattern(dateFile.str());

				// setting Rotation Timing
				strictRollingPolicy->setRotationTiming(rotationTiming);
	
				// setting Rotation Timing Value
				strictRollingPolicy->setRotationTimingValue(rotationTimingValue);
	
				//create RollingFileAppender
				log4cxx::rolling::RollingFileAppenderPtr dateAppender =
					new log4cxx::rolling::RollingFileAppender();
			
				// set layout
				dateAppender->setLayout(layout);

				// set RollingPolicy (TriggeringPolicy also included RollingPolicy)
				dateAppender->setRollingPolicy(strictRollingPolicy);
	
				// set Log Filename
				dateAppender->setFile(logFilename, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
	
				// activate appender options
				dateAppender->activateOptions(pool);
	
				// add date_based_appender to rootRogger
				root->addAppender(dateAppender);
	
				break;
			}
		default:	//LOG_ROT_DATESIZE:
			{
				// create TimeAndSizeBasedRollingPolicy
				log4cxx::rolling::TimeAndSizeBasedRollingPolicyPtr timeSizeRollingPolicy =
					new log4cxx::rolling::TimeAndSizeBasedRollingPolicy();
	
				// setting minIndex
				timeSizeRollingPolicy->setMinIndex(1);
	
				// setting maxIndex
				timeSizeRollingPolicy->setMaxIndex(maxBackupIndex);

				// setting FileNamePattern
				std::ostringstream dateSizeFile;
				dateSizeFile << logFilename << "." << LOGGER_FILE_PATTERN;
				timeSizeRollingPolicy->setFileNamePattern(dateSizeFile.str());
	
				// setting Rotation Timing
				timeSizeRollingPolicy->setRotationTiming(rotationTiming);
	
				// setting Rotation Timing Value
				timeSizeRollingPolicy->setRotationTimingValue(rotationTimingValue);
	
				// setting MaxFileSize
				timeSizeRollingPolicy->setMaxFileSize(maxFileSize);
	
				// create Rolling FileAppender
				log4cxx::rolling::RollingFileAppenderPtr dateSizeAppender =
					new log4cxx::rolling::RollingFileAppender();
	
				// set layout
				dateSizeAppender->setLayout(layout);
	
				// set RollingPolicy (TriggeringPolicy also included RollingPolicy)
				dateSizeAppender->setRollingPolicy(timeSizeRollingPolicy);
	
				// set Log Filename
				dateSizeAppender->setFile(logFilename, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
	
				// activate appender options
				dateSizeAppender->activateOptions(pool);
	
				// add time_and_size_based_appender to rootLogger
				root->addAppender(dateSizeAppender);
			}
		}
		//set default log level
		for (LOG_CATEGORY_TAG cat = LOG_CAT_NONE; cat <= getCategoryRangeEnd(LOG_MOD_SNMPAGENT); ++cat) {
			log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
		}
		
		//get category level
		for (LOG_CATEGORY_TAG cat = getCategoryRangeStart(loggerProcess); cat <= getCategoryRangeEnd(loggerProcess); ++cat) {
			if (l7vs::Parameter::getInstance().isStringExist(PARAM_COMP_LOGGER, categoryTable[cat])) {
				std::string levelStr = l7vs::Parameter::getInstance().getStringValue(PARAM_COMP_LOGGER, categoryTable[cat]);
				if ("debug" == levelStr) {
					categoryLevel[cat] = log4cxx::Level::getDebug();
					log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
				}
				else if ("info" == levelStr) {
					categoryLevel[cat] = log4cxx::Level::getInfo();
					log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
				}
				else if ("warn" == levelStr) {
					categoryLevel[cat] = log4cxx::Level::getWarn();
					log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
				}
				else if ("error" == levelStr) {
					categoryLevel[cat] = log4cxx::Level::getError();
					log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
				}
				else if ("fatal" == levelStr) {
					categoryLevel[cat] = log4cxx::Level::getFatal();
					log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
				}
				else {
					std::ostringstream oss;
					oss << "Invalid Log Category Setting : " << categoryTable[cat];
					if (LOG_LV_WARN >= this->getLogLevel(loggerCategory)) {
						this->putLogWarn(loggerCategory,2, oss.str(), __FILE__, __LINE__);
					}
					categoryLevel[cat] = log4cxx::Level::getInfo();
					log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
				}
			}
			else {
				std::ostringstream oss;
				oss << "Not Exist Log Category Setting : " << categoryTable[cat];
				if (LOG_LV_WARN >= this->getLogLevel(loggerCategory)) {
					this->putLogWarn(loggerCategory,3, oss.str(), __FILE__, __LINE__);
				}
				categoryLevel[cat] = log4cxx::Level::getInfo();
				log4cxx::Logger::getLogger(categoryTable[cat])->setLevel(categoryLevel[cat]);
			}
		}
	}
	catch (const std::exception& e) {
		std::ostringstream oss;
		oss <<  "Logger Reload Config Failed : " << e.what();
		errorConf(7, oss.str(), __FILE__, __LINE__);
		throw std::logic_error(oss.str());
	}

	if (LOG_LV_INFO >= this->getLogLevel(loggerCategory)) {
		this->putLogInfo(loggerCategory,2, "Logger Configuration Succeed.", __FILE__, __LINE__);
	}
	initLogLevelTable();
}

/*!
 * initialize LogLevel table.
 * initialize the LogLevel table.
 *
 * @param   void
 * @return  void
 */
void l7vs::LoggerImpl::initLogLevelTable(void)
{
  int nCounter = 0;
  
  for (;nCounter < LOGGER_CATEGORY_NUM; nCounter++) {
    loglevel[nCounter] = LOG_LV_NONE;
  }

  return;
}
