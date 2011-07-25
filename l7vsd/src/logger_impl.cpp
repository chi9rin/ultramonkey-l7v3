/*!
 * @file  logger_impl.cpp
 * @brief logger module implementation class.
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

#include <iostream>
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
#include <boost/lexical_cast.hpp>

#include "error_code.h"
#include "logger_enum.h"
#include "logger_rotation_enum.h"
#include "logger_impl.h"
#include "parameter.h"
#include "strict_time_based_rolling_policy.h"
#include "time_and_size_based_rolling_policy.h"

#define LOGGER_LAYOUT "%d{%Y/%m/%d %H:%M:%S} [%p] %c %m %t %F:%L%n"
#define LOGGER_DEFAULT_BUFFER_SIZE (8 * 1024)
#define LOGGER_SYSLOG_FACILITY "USER"
#define LOGGER_BACKUP_INDEX_LOWER_LIMIT (1)
#define LOGGER_BACKUP_INDEX_LIMIT (12)
#define LOGGER_FILESIZE_LOWER_LIMIT (65535)
#define LOGGER_FILE_PATTERN "%i"

#if    defined(LOGGER_PROCESS_VSD)
#define    LOGGER_LOG_FILENAME_KEY              "l7vsd_log_filename"
#define LOGGER_ROTATION_KEY                     "l7vsd_rotation"
#define LOGGER_MAX_BACKUP_INDEX_KEY             "l7vsd_max_backup_index"
#define LOGGER_MAX_FILE_SIZE_KEY                "l7vsd_max_filesize"
#define LOGGER_ROTATION_TIMING_KEY              "l7vsd_rotation_timing"
#define LOGGER_ROTATION_TIMING_VALUE_KEY        "l7vsd_rotation_timing_value"

#elif defined(LOGGER_PROCESS_ADM)
#define    LOGGER_LOG_FILENAME_KEY              "l7vsadm_log_filename"
#define LOGGER_ROTATION_KEY                     "l7vsadm_rotation"
#define LOGGER_MAX_BACKUP_INDEX_KEY             "l7vsadm_max_backup_index"
#define LOGGER_MAX_FILE_SIZE_KEY                "l7vsadm_max_filesize"
#define LOGGER_ROTATION_TIMING_KEY              "l7vsadm_rotation_timing"
#define LOGGER_ROTATION_TIMING_VALUE_KEY        "l7vsadm_rotation_timing_value"

#endif

/*!
 * returns single instance.
 *
 * @param   void
 * @return  instance
 */
l7vs::LoggerImpl &l7vs::LoggerImpl::getInstance()
{
        static    LoggerImpl    instance;
        return instance;
}

l7vs::LoggerImpl::LoggerImpl()
        :    initialized(false),
             snmp_send_trap_func(NULL),
             logtrap_level(LOG_LV_NONE)
{
        logtrap_enabled = 0;
        levelTable[LOG_LV_NONE]        = log4cxx::Level::DEBUG_INT;
        levelTable[LOG_LV_DEBUG]    = log4cxx::Level::DEBUG_INT;
        levelTable[LOG_LV_INFO]        = log4cxx::Level::INFO_INT;
        levelTable[LOG_LV_WARN]        = log4cxx::Level::WARN_INT;
        levelTable[LOG_LV_ERROR]    = log4cxx::Level::ERROR_INT;
        levelTable[LOG_LV_FATAL]    = log4cxx::Level::FATAL_INT;

#if    defined(LOGGER_PROCESS_VSD)
        // l7vsd network category initialize
        category_level_map[LOG_CAT_L7VSD_NETWORK] = LOG_LV_ERROR;
        name_category_map["l7vsd_network"] = LOG_CAT_L7VSD_NETWORK;
        category_name_map[LOG_CAT_L7VSD_NETWORK] = "l7vsd_network";

        // l7vsd network qos category initialize
        category_level_map[LOG_CAT_L7VSD_NETWORK_QOS] = LOG_LV_ERROR;
        name_category_map["l7vsd_network_qos"] = LOG_CAT_L7VSD_NETWORK_QOS;
        category_name_map[LOG_CAT_L7VSD_NETWORK_QOS] = "l7vsd_network_qos";

        // l7vsd network bandwidth category initialize
        category_level_map[LOG_CAT_L7VSD_NETWORK_BANDWIDTH] = LOG_LV_ERROR;
        name_category_map["l7vsd_network_bandwidth"] = LOG_CAT_L7VSD_NETWORK_BANDWIDTH;
        category_name_map[LOG_CAT_L7VSD_NETWORK_BANDWIDTH] = "l7vsd_network_bandwidth";

        // l7vsd network nu_connection category initialize
        category_level_map[LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION] = LOG_LV_ERROR;
        name_category_map["l7vsd_network_num_connection"] = LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION;
        category_name_map[LOG_CAT_L7VSD_NETWORK_NUM_CONNECTION] = "l7vsd_network_num_connection";

        // l7vsd network access log category initialize
        category_level_map[LOG_CAT_L7VSD_NETWORK_ACCESS] = LOG_LV_ERROR;
        name_category_map["l7vsd_network_access"] = LOG_CAT_L7VSD_NETWORK_ACCESS;
        category_name_map[LOG_CAT_L7VSD_NETWORK_ACCESS] = "l7vsd_network_access";

        // l7vsd mainthread category initialize
        category_level_map[LOG_CAT_L7VSD_MAINTHREAD] = LOG_LV_ERROR;
        name_category_map["l7vsd_mainthread"] = LOG_CAT_L7VSD_MAINTHREAD;
        category_name_map[LOG_CAT_L7VSD_MAINTHREAD] = "l7vsd_mainthread";

        //l7vsd virtualservice category initialize
        category_level_map[LOG_CAT_L7VSD_VIRTUALSERVICE] = LOG_LV_ERROR;
        name_category_map["l7vsd_virtualservice"] = LOG_CAT_L7VSD_VIRTUALSERVICE;
        category_name_map[LOG_CAT_L7VSD_VIRTUALSERVICE] = "l7vsd_virtualservice";

        //l7vsd virtualservice thread category initialize
        category_level_map[LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD] = LOG_LV_ERROR;
        name_category_map["l7vsd_virtualservice_thread"] = LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD;
        category_name_map[LOG_CAT_L7VSD_VIRTUALSERVICE_THREAD] = "l7vsd_virtualservice_thread";

        //l7vsd session category initialize
        category_level_map[LOG_CAT_L7VSD_SESSION] = LOG_LV_ERROR;
        name_category_map["l7vsd_session"] = LOG_CAT_L7VSD_SESSION;
        category_name_map[LOG_CAT_L7VSD_SESSION] = "l7vsd_session";

        //l7vsd session thread category initialize
        category_level_map[LOG_CAT_L7VSD_SESSION_THREAD] = LOG_LV_ERROR;
        name_category_map["l7vsd_session_thread"] = LOG_CAT_L7VSD_SESSION_THREAD;
        category_name_map[LOG_CAT_L7VSD_SESSION_THREAD] = "l7vsd_session_thread";

        //l7vsd realserver category initialize
        category_level_map[LOG_CAT_L7VSD_REALSERVER] = LOG_LV_ERROR;
        name_category_map["l7vsd_realserver"] = LOG_CAT_L7VSD_REALSERVER;
        category_name_map[LOG_CAT_L7VSD_REALSERVER] = "l7vsd_realserver";

        //l7vsd sorryserver category initialize
        category_level_map[LOG_CAT_L7VSD_SORRYSERVER] = LOG_LV_ERROR;
        name_category_map["l7vsd_sorryserver"] = LOG_CAT_L7VSD_SORRYSERVER;
        category_name_map[LOG_CAT_L7VSD_SORRYSERVER] = "l7vsd_sorryserver";

        //l7vsd module category initialize
        category_level_map[LOG_CAT_L7VSD_MODULE] = LOG_LV_ERROR;
        name_category_map["l7vsd_module"] = LOG_CAT_L7VSD_MODULE;
        category_name_map[LOG_CAT_L7VSD_MODULE] = "l7vsd_module";

        //l7vsd replication category initialize
        category_level_map[LOG_CAT_L7VSD_REPLICATION] = LOG_LV_ERROR;
        name_category_map["l7vsd_replication"] = LOG_CAT_L7VSD_REPLICATION;
        category_name_map[LOG_CAT_L7VSD_REPLICATION] = "l7vsd_replication";

        //l7vsd replication send thread category initialize
        category_level_map[LOG_CAT_L7VSD_REPLICATION_SENDTHREAD] = LOG_LV_ERROR;
        name_category_map["l7vsd_replication_sendthread"] = LOG_CAT_L7VSD_REPLICATION_SENDTHREAD;
        category_name_map[LOG_CAT_L7VSD_REPLICATION_SENDTHREAD] = "l7vsd_replication_sendthread";

        //l7vsd parameter category initialize
        category_level_map[LOG_CAT_L7VSD_PARAMETER] = LOG_LV_ERROR;
        name_category_map["l7vsd_parameter"] = LOG_CAT_L7VSD_PARAMETER;
        category_name_map[LOG_CAT_L7VSD_PARAMETER] = "l7vsd_parameter";

        //l7vsd logger category initialize
        category_level_map[LOG_CAT_L7VSD_LOGGER] = LOG_LV_ERROR;
        name_category_map["l7vsd_logger"] = LOG_CAT_L7VSD_LOGGER;
        category_name_map[LOG_CAT_L7VSD_LOGGER] = "l7vsd_logger";

        //l7vsd command category initialize
        category_level_map[LOG_CAT_L7VSD_COMMAND] = LOG_LV_ERROR;
        name_category_map["l7vsd_command"] = LOG_CAT_L7VSD_COMMAND;
        category_name_map[LOG_CAT_L7VSD_COMMAND] = "l7vsd_command";

        //l7vsd start stop category initialize
        category_level_map[LOG_CAT_L7VSD_START_STOP] = LOG_LV_ERROR;
        name_category_map["l7vsd_start_stop"] = LOG_CAT_L7VSD_START_STOP;
        category_name_map[LOG_CAT_L7VSD_START_STOP] = "l7vsd_start_stop";

        //l7vsd system category initialize
        category_level_map[LOG_CAT_L7VSD_SYSTEM] = LOG_LV_ERROR;
        name_category_map["l7vsd_system"] = LOG_CAT_L7VSD_SYSTEM;
        category_name_map[LOG_CAT_L7VSD_SYSTEM] = "l7vsd_system";

        //l7vsd system memory category initialize
        category_level_map[LOG_CAT_L7VSD_SYSTEM_MEMORY] = LOG_LV_ERROR;
        name_category_map["l7vsd_system_memory"] = LOG_CAT_L7VSD_SYSTEM_MEMORY;
        category_name_map[LOG_CAT_L7VSD_SYSTEM_MEMORY] = "l7vsd_system_memory";

        //l7vsd system endpoint category initialize
        category_level_map[LOG_CAT_L7VSD_SYSTEM_ENDPOINT] = LOG_LV_ERROR;
        name_category_map["l7vsd_system_endpoint"] = LOG_CAT_L7VSD_SYSTEM_ENDPOINT;
        category_name_map[LOG_CAT_L7VSD_SYSTEM_ENDPOINT] = "l7vsd_system_endpoint";

        //l7vsd signal category initialize
        category_level_map[LOG_CAT_L7VSD_SYSTEM_SIGNAL] = LOG_LV_ERROR;
        name_category_map["l7vsd_system_signal"] = LOG_CAT_L7VSD_SYSTEM_SIGNAL;
        category_name_map[LOG_CAT_L7VSD_SYSTEM_SIGNAL] = "l7vsd_system_signal";

        //l7vsd system environment category initialize
        category_level_map[LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT] = LOG_LV_ERROR;
        name_category_map["l7vsd_system_environment"] = LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT;
        category_name_map[LOG_CAT_L7VSD_SYSTEM_ENVIRONMENT] = "l7vsd_system_environment";

        //l7vsd snmpagent category initialize
        category_level_map[LOG_CAT_L7VSD_SNMPAGENT] = LOG_LV_ERROR;
        name_category_map["l7vsd_snmpagent"] = LOG_CAT_L7VSD_SNMPAGENT;
        category_name_map[LOG_CAT_L7VSD_SNMPAGENT] = "l7vsd_snmpagent";

        //l7vsd protocol module category initialize
        category_level_map[LOG_CAT_PROTOCOL] = LOG_LV_ERROR;
        name_category_map["l7vsd_protocol"] = LOG_CAT_PROTOCOL;
        category_name_map[LOG_CAT_PROTOCOL] = "l7vsd_protocol";

        //l7vsd schedule module category initialize
        category_level_map[LOG_CAT_SCHEDULE] = LOG_LV_ERROR;
        name_category_map["l7vsd_schedule"] = LOG_CAT_SCHEDULE;
        category_name_map[LOG_CAT_SCHEDULE] = "l7vsd_schedule";

#elif defined(LOGGER_PROCESS_ADM)
        //l7vsadm parse category initialize
        category_level_map[LOG_CAT_L7VSADM_PARSE] = LOG_LV_ERROR;
        name_category_map["l7vsadm_parse"] = LOG_CAT_L7VSADM_PARSE;
        category_name_map[LOG_CAT_L7VSADM_PARSE] = "l7vsadm_parse";

        //l7vsadm operate category initialize
        category_level_map[LOG_CAT_L7VSADM_OPERATE] = LOG_LV_ERROR;
        name_category_map["l7vsadm_operate"] = LOG_CAT_L7VSADM_OPERATE;
        category_name_map[LOG_CAT_L7VSADM_OPERATE] = "l7vsadm_operate";

        //l7vsadm config result category initialize
        category_level_map[LOG_CAT_L7VSADM_CONFIG_RESULT] = LOG_LV_ERROR;
        name_category_map["l7vsadm_config_result"] = LOG_CAT_L7VSADM_CONFIG_RESULT;
        category_name_map[LOG_CAT_L7VSADM_CONFIG_RESULT] = "l7vsadm_config_result";

        //l7vsadm common category initialize
        category_level_map[LOG_CAT_L7VSADM_COMMON] = LOG_LV_ERROR;
        name_category_map["l7vsadm_common"] = LOG_CAT_L7VSADM_COMMON;
        category_name_map[LOG_CAT_L7VSADM_COMMON] = "l7vsadm_common";

        //l7vsadm logger category initialize
        category_level_map[LOG_CAT_L7VSADM_LOGGER] = LOG_LV_ERROR;
        name_category_map["l7vsadm_logger"] = LOG_CAT_L7VSADM_LOGGER;
        category_name_map[LOG_CAT_L7VSADM_LOGGER] = "l7vsadm_logger";

        //l7vsadm parameter category initialize
        category_level_map[LOG_CAT_L7VSADM_PARAMETER] = LOG_LV_ERROR;
        name_category_map["l7vsadm_parameter"] = LOG_CAT_L7VSADM_PARAMETER;
        category_name_map[LOG_CAT_L7VSADM_PARAMETER] = "l7vsadm_parameter";

        //l7vsadm module category initialize
        category_level_map[LOG_CAT_L7VSADM_MODULE] = LOG_LV_ERROR;
        name_category_map["l7vsadm_module"] = LOG_CAT_L7VSADM_MODULE;
        category_name_map[LOG_CAT_L7VSADM_MODULE] = "l7vsadm_module";

        //l7vsadm protocol module category initialize
        category_level_map[LOG_CAT_PROTOCOL] = LOG_LV_ERROR;
        name_category_map["l7vsadm_protocol"] = LOG_CAT_PROTOCOL;
        category_name_map[LOG_CAT_PROTOCOL] = "l7vsadm_protocol";

        //l7vsadm schedule module category initialize
        category_level_map[LOG_CAT_SCHEDULE] = LOG_LV_ERROR;
        name_category_map["l7vsadm_schedule"] = LOG_CAT_SCHEDULE;
        category_name_map[LOG_CAT_SCHEDULE] = "l7vsadm_schedule";

#endif

        BOOST_FOREACH(category_level_map_type::value_type const & itr, category_level_map) {
                category_level_read_map[itr.first] = itr.second;
        }
}


void l7vs::LoggerImpl::logic_error(const unsigned int logno, const std::string &str, const char *file , const unsigned int line)
{
#if    defined(LOGGER_PROCESS_VSD)
        putLogError(l7vs::LOG_CAT_L7VSD_LOGGER, logno, str, file, line);
#elif defined(LOGGER_PROCESS_ADM)
        putLogError(l7vs::LOG_CAT_L7VSADM_LOGGER, logno, str, file, line);
#endif

        throw std::logic_error(str);

}

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
        using namespace log4cxx;
        if (initialized) return true;

        try {
                // set loot logger
                LoggerPtr root = Logger::getRootLogger();
                if (NULL == root)    return false;

                //create layout
                LayoutPtr layout = new PatternLayout(LOGGER_LAYOUT);

                //create Syslog appender from layout
                net::SyslogAppenderPtr syslogAppender =    new net::SyslogAppender(layout, net::SyslogAppender::getFacility(LOGGER_SYSLOG_FACILITY));
                root->addAppender(syslogAppender);

                // create console appender from layout
                WriterAppender *consoleAppender = new ConsoleAppender(layout, ConsoleAppender::getSystemErr());
                root->addAppender(consoleAppender);

                // set all log level to log4cxx
                category_level_read_map.clear();
                BOOST_FOREACH(category_level_map_type::value_type const & itr, category_level_map) {
                        category_level_read_map[itr.first] = itr.second;
                        category_name_map_type::iterator name_itr = category_name_map.find(itr.first);
                        Logger::getLogger(name_itr->second)->setLevel(log4cxx::Level::toLevel(levelTable[itr.second]));
                }

        } catch (const std::exception &e) {
                std::ostringstream oss;
                oss <<  "Logger Initialization Failed : " << e.what();
                errorConf(1, oss.str(), __FILE__, __LINE__);
                return false;
        }

        // get hostname
        int        ret = 0;
        char    buff[HOST_NAME_MAX];
        ret = gethostname(buff, HOST_NAME_MAX);
        hostname = buff;

        initialized = true;
        return true;
}

/*!
 * error handling function.
 * if error occurred, switch appenders to syslogappender and fileappender(/dev/console)
 * message will output to syslog/fileappender appender
 *
 * @param   log message id
 * @param   log message
 * @param   current file
 * @param   current line
 * @return  void
 */
void l7vs::LoggerImpl::errorConf(unsigned int message_id,
                                 const std::string &errorMessage,
                                 const char *file,
                                 int line)
{
        using namespace log4cxx;
        using namespace l7vs;

        try {
                LogManager::resetConfiguration();

                LoggerPtr root = log4cxx::Logger::getRootLogger();

                if (0 == root) {
                        std::cerr << "Oops! don't get root logger" << std::endl;
                        return;
                }
                LayoutPtr layout = new PatternLayout(LOGGER_LAYOUT);
                net::SyslogAppenderPtr syslogAppender =
                        new net::SyslogAppender(layout, net::SyslogAppender::getFacility(LOGGER_SYSLOG_FACILITY));
                ConsoleAppender *consoleAppender =
                        new log4cxx::ConsoleAppender(layout, ConsoleAppender::getSystemErr());

                root->addAppender(consoleAppender);
                root->addAppender(syslogAppender);

                BOOST_FOREACH(category_level_map_type::value_type & itr, category_level_map) {
                        itr.second = LOG_LV_INFO;        //set default level
                        category_level_read_map[itr.first] = itr.second;
                        category_name_map_type::iterator name_itr = category_name_map.find(itr.first);
                        Logger::getLogger(name_itr->second)-> setLevel(log4cxx::Level::getInfo());            //set default level
                }

                std::stringstream    buf;
                LOG_CATEGORY_TAG    log_category;

#if    defined(LOGGER_PROCESS_VSD)
                log_category = LOG_CAT_L7VSD_LOGGER;
#elif defined(LOGGER_PROCESS_ADM)
                log_category = LOG_CAT_L7VSADM_LOGGER;
#else
                log_category = LOG_CAT_L7VSD_LOGGER;
#endif

                buf << boost::format("%s%d%03d%04d %s %s")
                    % LOGGER_PROCESS_ID
                    % LOG_LV_FATAL
                    % log_category
                    % message_id
                    % errorMessage
                    % hostname;
                Logger::getRootLogger()->forcedLog(Level::getFatal(),
                                                   buf.str(),
                                                   spi::LocationInfo(file, "", line));
        } catch (const std::exception &e) {
                std::cerr <<  "Logger Error Output Failed : " << e.what() << std::endl;
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
        using namespace log4cxx;
        using namespace l7vs;

        Parameter param;

        // Connection category Logger setting.
        normal_log_property.log_filename_key                = LOGGER_LOG_FILENAME_KEY;
        normal_log_property.rotation_key                    = LOGGER_ROTATION_KEY;
        normal_log_property.max_backup_index_key            = LOGGER_MAX_BACKUP_INDEX_KEY;
        normal_log_property.max_file_size_key                = LOGGER_MAX_FILE_SIZE_KEY;
        normal_log_property.rotation_timing_key                = LOGGER_ROTATION_TIMING_KEY;
        normal_log_property.rotation_timing_value_key        = LOGGER_ROTATION_TIMING_VALUE_KEY;

        //get normal log filename
        appender_property *property = &normal_log_property;

        // setting keyname check
        if ("" == property->log_filename_key)    return;     // no conn_log setting.

        // filename check
        l7vs::error_code ec;
        property->log_filename_value = param.get_string(PARAM_COMP_LOGGER, property->log_filename_key, ec);
        if (ec) {
                std::stringstream    ss;
                ss << "Not exist logfilename_key : " << property->log_filename_key;
                logic_error(1, ss.str(), __FILE__, __LINE__);
        }

        // get rotation
        std::string rotationStr = param.get_string(PARAM_COMP_LOGGER, property->rotation_key, ec);
        if (!ec) {
                if ("size" == rotationStr) property->rotation_value = LOG_ROT_SIZE;
                else if ("date" == rotationStr) property->rotation_value = LOG_ROT_DATE;
                else if ("datesize" == rotationStr) property->rotation_value = LOG_ROT_DATESIZE;
                else {
                        std::stringstream    ss;
                        ss << "Invalid Log Rotation Setting : " << rotationStr;
                        logic_error(118, ss.str(), __FILE__, __LINE__);
                }
        } else {
                logic_error(2, "Not Exist Log Rotation Setting.", __FILE__, __LINE__);
        }

        // get max backup index
        std::string maxBackupIndexStr = param.get_string(PARAM_COMP_LOGGER, property->max_backup_index_key, ec);
        if (!ec) {
                try {
                        property->max_backup_index_value = boost::lexical_cast<unsigned int>(maxBackupIndexStr);
                } catch (const boost::bad_lexical_cast &bc) {
                        std::stringstream    ss;
                        ss << "Invalid MaxBackupIndex Value : " << maxBackupIndexStr << ".";
                        logic_error(119, ss.str(), __FILE__, __LINE__);
                }
                if (LOGGER_BACKUP_INDEX_LOWER_LIMIT > property->max_backup_index_value) {
                        std::stringstream ss;
                        ss << "Max Backup Index must at least " << LOGGER_BACKUP_INDEX_LOWER_LIMIT << ".";
                        logic_error(120, ss.str(), __FILE__, __LINE__);
                }
                if (LOGGER_BACKUP_INDEX_LIMIT < property->max_backup_index_value) {
                        std::stringstream ss;
                        ss << "Max Backup Index must at most " << LOGGER_BACKUP_INDEX_LIMIT << ".";
                        logic_error(121, ss.str(), __FILE__, __LINE__);
                }
        } else {
                logic_error(3, "Not Exist Log MaxBackupIndex Setting.", __FILE__, __LINE__);
        }

        // get max filesize check
        if (LOG_ROT_SIZE == property->rotation_value || LOG_ROT_DATESIZE == property->rotation_value) {
                // get max file size
                std::string maxFileSizeStr;
                maxFileSizeStr = param.get_string(PARAM_COMP_LOGGER, property->max_file_size_key, ec);
                if (ec)    logic_error(4, "Not Exist Log MaxFileSize Setting.", __FILE__, __LINE__);

                std::string size_val;
                int maxFileSizeStr_length = maxFileSizeStr.length();
                if (maxFileSizeStr_length <= 0) {
                        logic_error(5, "Invalid FileSize Value.", __FILE__, __LINE__);
                }

                std::string last_str = maxFileSizeStr.substr(maxFileSizeStr_length - 1, 1);
                // when unit was specified
                if (("K" == last_str) || ("M" == last_str) || ("G" == last_str))
                        size_val = maxFileSizeStr.substr(0, maxFileSizeStr.length() - 1);
                else
                        size_val = maxFileSizeStr;

                try {
                        property->max_file_size_value = boost::lexical_cast<size_t>(size_val);
                } catch (const boost::bad_lexical_cast &bc) {
                        logic_error(5, "Invalid FileSize Value.", __FILE__, __LINE__);
                }

                if ("K" == last_str) {
                        if ((ULLONG_MAX / 1024) < property->max_file_size_value) {
                                logic_error(6, "Invalid FileSize Value.", __FILE__, __LINE__);
                        }
                        property->max_file_size_value = property->max_file_size_value * 1024;
                } else if ("M" == last_str) {
                        if ((ULLONG_MAX / 1024 / 1024) < property->max_file_size_value)
                                logic_error(7, "Invalid FileSize Value.", __FILE__, __LINE__);
                        property->max_file_size_value = property->max_file_size_value * 1024 * 1024;
                } else if ("G" == last_str) {
                        if ((ULLONG_MAX / 1024 / 1024 / 1024) < property->max_file_size_value)
                                logic_error(8, "Invalid FileSize Value.", __FILE__, __LINE__);
                        property->max_file_size_value = property->max_file_size_value * 1024 * 1024 * 1024;
                }
                if (LOGGER_FILESIZE_LOWER_LIMIT > property->max_file_size_value) {
                        int limit = LOGGER_FILESIZE_LOWER_LIMIT;
                        std::ostringstream oss;
                        oss << "FileSize must at least " << limit << " bytes.";
                        logic_error(9, oss.str(), __FILE__, __LINE__);
                }
        }

        // get rotation timing
        if (LOG_ROT_DATE == property->rotation_value || LOG_ROT_DATESIZE == property->rotation_value) {
                // get rotation timing
                std::string rotationTimingStr = param.get_string(PARAM_COMP_LOGGER, property->rotation_timing_key, ec);
                if (!ec) {
                        if ("year" == rotationTimingStr) property->rotation_timing_value = LOG_TIM_YEAR;
                        else if ("month" == rotationTimingStr) property->rotation_timing_value = LOG_TIM_MONTH;
                        else if ("week" == rotationTimingStr) property->rotation_timing_value = LOG_TIM_WEEK;
                        else if ("date" == rotationTimingStr) property->rotation_timing_value = LOG_TIM_DATE;
                        else if ("hour" == rotationTimingStr) property->rotation_timing_value = LOG_TIM_HOUR;
                        else logic_error(10, "Invalid Log RotationTiming Setting.", __FILE__, __LINE__);
                } else {
                        logic_error(11, "Not Exist Log RotationTiming Setting.", __FILE__, __LINE__);
                }

                if (LOG_TIM_YEAR == property->rotation_timing_value) {
                        std::string ret = param.get_string(PARAM_COMP_LOGGER, property->rotation_timing_value_key, ec);
                        if (!ec) {
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
                                                month = boost::lexical_cast<int>(monthStr);
                                        } catch (const boost::bad_lexical_cast &bc) {
                                                logic_error(12, "Parse Timing Year Error.", __FILE__, __LINE__);
                                        }
                                        if (1 > month || month > 12) {
                                                logic_error(13, "Parse Timing Year Error.", __FILE__, __LINE__);
                                        }
                                        fpos = rpos + 1;
                                        // find date
                                        rpos = ret.find_first_of(' ', fpos);
                                        if (std::string::npos != rpos) {
                                                std::string dateStr = ret.substr(fpos, rpos - fpos);
                                                try {
                                                        date = boost::lexical_cast<int>(dateStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(14, "Parse Timing Year Error.", __FILE__, __LINE__);
                                                }
                                        }
                                        if (1 > date || date > 31)  logic_error(15, "Parse Timing Year Error.", __FILE__, __LINE__);

                                        int dates[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                                        if (date > dates[month - 1]) logic_error(16, "Parse Timing Year Error.", __FILE__, __LINE__);

                                        fpos = rpos + 1;
                                        // find hour
                                        rpos = ret.find_first_of(':', fpos);
                                        if (std::string::npos != rpos) {
                                                std::string hourStr = ret.substr(fpos, rpos - fpos);
                                                try {
                                                        hour = boost::lexical_cast<int>(hourStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(17, "Parse Timing Year Error.", __FILE__, __LINE__);
                                                }
                                                if (0 > hour || hour > 23) logic_error(18, "Parse Timing Year Error.", __FILE__, __LINE__);

                                                // minute
                                                std::string minuteStr = ret.substr(rpos + 1);
                                                try {
                                                        minute = boost::lexical_cast<int>(minuteStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(19, "Parse Timing Year Error.", __FILE__, __LINE__);
                                                }
                                                if (0 > minute || minute > 59) logic_error(20, "Parse Timing Year Error.", __FILE__, __LINE__);
                                        } else {
                                                logic_error(21, "Parse Timing Year Error.", __FILE__, __LINE__);
                                        }
                                } else {
                                        logic_error(22, "Parse Timing Year Error.", __FILE__, __LINE__);
                                }

                                // format to internal rotation timing value expression
                                std::ostringstream oss;
                                oss << std::setfill('0') << std::setw(2) << month
                                    << std::setfill('0') << std::setw(2) << date
                                    << std::setfill('0') << std::setw(2) << hour
                                    << std::setfill('0') << std::setw(2) << minute;

                                property->rotation_timing_value_value = oss.str();

                        } else {
                                logic_error(23, "Not Exist Log RotationTiming Year Setting.", __FILE__, __LINE__);
                        }
                }


                if (LOG_TIM_MONTH == property->rotation_timing_value) {
                        std::string ret = param.get_string(PARAM_COMP_LOGGER, property->rotation_timing_value_key, ec);
                        if (!ec) {
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
                                                date = boost::lexical_cast<int>(dateStr);
                                        } catch (const boost::bad_lexical_cast &bc) {
                                                logic_error(24, "Parse Timing Month Error.", __FILE__, __LINE__);
                                        }
                                        if (1 > date || date > 31) {
                                                logic_error(25, "Parse Timing Month Error.", __FILE__, __LINE__);
                                        }
                                        fpos = rpos + 1;
                                        // find hour
                                        rpos = ret.find_first_of(':', fpos);
                                        if (std::string::npos != rpos) {
                                                std::string hourStr = ret.substr(fpos, rpos - fpos);
                                                try {
                                                        hour = boost::lexical_cast<int>(hourStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(26, "Parse Timing Month Error.", __FILE__, __LINE__);
                                                }
                                                if (0 > hour || hour > 23) {
                                                        logic_error(27, "Parse Timing Month Error.", __FILE__, __LINE__);
                                                }
                                                // minute
                                                std::string minuteStr = ret.substr(rpos + 1);
                                                try {
                                                        minute = boost::lexical_cast<int>(minuteStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(28, "Parse Timing Month Error.", __FILE__, __LINE__);
                                                }
                                                if (0 > minute || minute > 59) {
                                                        logic_error(29, "Parse Timing Month Error.", __FILE__, __LINE__);
                                                }
                                        } else {
                                                logic_error(30, "Parse Timing Month Error.", __FILE__, __LINE__);
                                        }
                                } else {
                                        logic_error(31, "Parse Timing Month Error.", __FILE__, __LINE__);
                                }

                                // format to internal rotation timing value expression
                                std::stringstream oss;
                                oss << std::setfill('0') << std::setw(2) << date
                                    << std::setfill('0') << std::setw(2) << hour
                                    << std::setfill('0') << std::setw(2) << minute;

                                property->rotation_timing_value_value = oss.str();

                        } else {
                                logic_error(32, "Not Exist Log RotationTiming Month Setting.", __FILE__, __LINE__);
                        }
                }

                if (LOG_TIM_WEEK == property->rotation_timing_value) {
                        std::string ret = param.get_string(PARAM_COMP_LOGGER, property->rotation_timing_value_key, ec);
                        if (!ec) {
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
                                                logic_error(33, "Parse Timing Week Error.", __FILE__, __LINE__);
                                        }
                                        fpos = rpos + 1;
                                        // find hour
                                        rpos = ret.find_first_of(':', fpos);
                                        if (std::string::npos != rpos) {
                                                std::string hourStr = ret.substr(fpos, rpos - fpos);
                                                try {
                                                        hour = boost::lexical_cast<int>(hourStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(34, "Parse Timing Week Error.", __FILE__, __LINE__);
                                                }
                                                if (0 > hour || hour > 23) {
                                                        logic_error(35, "Parse Timing Week Error.", __FILE__, __LINE__);
                                                }
                                                // minute
                                                std::string minuteStr = ret.substr(rpos + 1);
                                                try {
                                                        minute = boost::lexical_cast<int>(minuteStr);
                                                } catch (const boost::bad_lexical_cast &bc) {
                                                        logic_error(36, "Parse Timing Week Error.", __FILE__, __LINE__);
                                                }
                                                if (0 > minute || minute > 59) {
                                                        logic_error(37, "Parse Timing Week Error.", __FILE__, __LINE__);
                                                }
                                        } else {
                                                logic_error(38, "Parse Timing Week Error.", __FILE__, __LINE__);
                                        }
                                } else {
                                        logic_error(39, "Parse Timing Week Error.", __FILE__, __LINE__);
                                }

                                // format to internal rotation timing value expression
                                std::ostringstream oss;
                                oss << std::setfill('0') << std::setw(1) << week
                                    << std::setfill('0') << std::setw(2) << hour
                                    << std::setfill('0') << std::setw(2) << minute;

                                property->rotation_timing_value_value = oss.str();
                        } else {
                                logic_error(40, "Not Exist Log RotationTiming Week Setting.", __FILE__, __LINE__);
                        }
                }

                if (LOG_TIM_DATE == property->rotation_timing_value) {
                        std::string ret = param.get_string(PARAM_COMP_LOGGER, property->rotation_timing_value_key, ec);
                        if (!ec) {
                                std::string::size_type fpos = 0;
                                std::string::size_type rpos = 0;
                                int hour = 0;
                                int minute = 0;
                                //find time
                                rpos = ret.find_first_of(':', fpos);
                                if (std::string::npos != rpos) {
                                        std::string hourStr = ret.substr(fpos, rpos - fpos);
                                        try {
                                                hour = boost::lexical_cast<int>(hourStr);
                                        } catch (const boost::bad_lexical_cast &bc) {
                                                logic_error(41, "Parse Timing Date Error.", __FILE__, __LINE__);
                                        }
                                        if (0 > hour || hour > 23) {
                                                logic_error(42, "Parse Timing Date Error.", __FILE__, __LINE__);
                                        }
                                        // minute
                                        std::string minuteStr = ret.substr(rpos + 1);
                                        try {
                                                minute = boost::lexical_cast<int>(minuteStr);
                                        } catch (const boost::bad_lexical_cast &bc) {
                                                logic_error(43, "Parse Timing Date Error.", __FILE__, __LINE__);
                                        }
                                        if (0 > minute || minute > 59) {
                                                logic_error(44, "Parse Timing Date Error.", __FILE__, __LINE__);
                                        }
                                } else {
                                        logic_error(45, "Parse Timing Date Error.", __FILE__, __LINE__);
                                }

                                // format to internal rotation timing value expression
                                std::ostringstream oss;
                                oss << std::setfill('0') << std::setw(2) << hour
                                    << std::setfill('0') << std::setw(2) << minute;

                                property->rotation_timing_value_value = oss.str();
                        } else {
                                logic_error(46, "Not Exist Log RotationTiming Date Setting.", __FILE__, __LINE__);
                        }
                }

                if (LOG_TIM_HOUR == property->rotation_timing_value) {
                        std::string ret = param.get_string(PARAM_COMP_LOGGER, property->rotation_timing_value_key, ec);
                        if (!ec) {
                                // minute
                                int minute = 0;
                                try {
                                        minute = boost::lexical_cast<int>(ret);
                                } catch (const boost::bad_lexical_cast &bc) {
                                        logic_error(47, "Parse Timing Hour Error.", __FILE__, __LINE__);
                                }
                                if (0 > minute || minute > 59) {
                                        logic_error(48, "Parse Timing Hour Error.", __FILE__, __LINE__);
                                }

                                // format to internal rotation timing value expression
                                std::ostringstream oss;
                                oss << std::setfill('0') << std::setw(2) << minute;

                                property->rotation_timing_value_value = oss.str();
                        } else {
                                logic_error(49, "Not Exist Log RotationTiming Hour Setting.", __FILE__, __LINE__);
                        }
                }
        }

        // appender setting
        try {
                log4cxx::helpers::Pool pool;
                // reset current configuration
                log4cxx::LogManager::resetConfiguration();

                log4cxx::LayoutPtr layout =
                        new log4cxx::PatternLayout(LOGGER_LAYOUT);

                log4cxx::rolling::RollingFileAppenderPtr    normalAppender;
                log4cxx::rolling::RollingFileAppenderPtr    accessAppender;

                property = &normal_log_property;

                if ("" == property->log_filename_key)    return;     // no conn_log setting.

                switch (property->rotation_value) {
                case LOG_ROT_SIZE: {
                        // create FixedWindowRollingPolicy
                        log4cxx::rolling::FixedWindowRollingPolicyPtr fixedRollingPolicy =
                                new log4cxx::rolling::FixedWindowRollingPolicy();

                        // setting minIndex
                        fixedRollingPolicy->setMinIndex(1);

                        // setting maxIndex
                        fixedRollingPolicy->setMaxIndex(property->max_backup_index_value);

                        // setting FileNamePattern
                        std::ostringstream sizeFile;
                        sizeFile << property->log_filename_value << "." << LOGGER_FILE_PATTERN;
                        fixedRollingPolicy->setFileNamePattern(sizeFile.str());

                        // create SizeBasedTriggeringPolicy
                        log4cxx::rolling::SizeBasedTriggeringPolicyPtr sizeTriggeringPolicy =
                                new log4cxx::rolling::SizeBasedTriggeringPolicy();

                        // setting maxFileSize
                        sizeTriggeringPolicy->setMaxFileSize(property->max_file_size_value);

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
                        try {
                                sizeAppender->setFile(property->log_filename_value, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
                        } catch (const std::exception &e) {
                                std::stringstream    buf;
                                buf << "File Create Failed:" << property->log_filename_value;
                                throw std::logic_error(buf.str());
                        }

                        // activate appender options
                        sizeAppender->activateOptions(pool);

                        // add size_base_appender to CategoryLogger
                        //cat_logger->addAppender(sizeAppender);
                        normalAppender = sizeAppender;

                        break;
                }
                case LOG_ROT_DATE: {
                        // create StrictTimeBasedRollingPolicy
                        log4cxx::rolling::StrictTimeBasedRollingPolicyPtr strictRollingPolicy =
                                new log4cxx::rolling::StrictTimeBasedRollingPolicy();

                        // setting minIndex
                        strictRollingPolicy->setMinIndex(1);

                        // setting maxIndex
                        strictRollingPolicy->setMaxIndex(property->max_backup_index_value);

                        // setting FileNamePattern
                        std::ostringstream dateFile;
                        dateFile << property->log_filename_value << "." << LOGGER_FILE_PATTERN;
                        strictRollingPolicy->setFileNamePattern(dateFile.str());

                        // setting Rotation Timing
                        strictRollingPolicy->setRotationTiming(property->rotation_timing_value);

                        // setting Rotation Timing Value
                        strictRollingPolicy->setRotationTimingValue(property->rotation_timing_value_value);

                        //create RollingFileAppender
                        log4cxx::rolling::RollingFileAppenderPtr dateAppender =
                                new log4cxx::rolling::RollingFileAppender();

                        // set layout
                        dateAppender->setLayout(layout);

                        // set RollingPolicy (TriggeringPolicy also included RollingPolicy)
                        dateAppender->setRollingPolicy(strictRollingPolicy);

                        // set Log Filename
                        try {
                                dateAppender->setFile(property->log_filename_value, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
                        } catch (const std::exception &e) {
                                std::stringstream    buf;
                                buf << "File Create Failed:" << property->log_filename_value;
                                throw std::logic_error(buf.str());
                        }

                        // activate appender options
                        dateAppender->activateOptions(pool);

                        // add date_based_appender to CategoryLogger
                        //cat_logger->addAppender(dateAppender);
                        normalAppender = dateAppender;

                        break;
                }
                default: {  //LOG_ROT_DATESIZE:
                        // create TimeAndSizeBasedRollingPolicy
                        log4cxx::rolling::TimeAndSizeBasedRollingPolicyPtr timeSizeRollingPolicy =
                                new log4cxx::rolling::TimeAndSizeBasedRollingPolicy();

                        // setting minIndex
                        timeSizeRollingPolicy->setMinIndex(1);

                        // setting maxIndex
                        timeSizeRollingPolicy->setMaxIndex(property->max_backup_index_value);

                        // setting FileNamePattern
                        std::ostringstream dateSizeFile;
                        dateSizeFile << property->log_filename_value << "." << LOGGER_FILE_PATTERN;
                        timeSizeRollingPolicy->setFileNamePattern(dateSizeFile.str());

                        // setting Rotation Timing
                        timeSizeRollingPolicy->setRotationTiming(property->rotation_timing_value);

                        // setting Rotation Timing Value
                        timeSizeRollingPolicy->setRotationTimingValue(property->rotation_timing_value_value);

                        // setting MaxFileSize
                        timeSizeRollingPolicy->setMaxFileSize(property->max_file_size_value);

                        // create Rolling FileAppender
                        log4cxx::rolling::RollingFileAppenderPtr dateSizeAppender =
                                new log4cxx::rolling::RollingFileAppender();

                        // set layout
                        dateSizeAppender->setLayout(layout);

                        // set RollingPolicy (TriggeringPolicy also included RollingPolicy)
                        dateSizeAppender->setRollingPolicy(timeSizeRollingPolicy);

                        // set Log Filename
                        try {
                                dateSizeAppender->setFile(property->log_filename_value, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
                        } catch (const std::exception &e) {
                                std::stringstream    buf;
                                buf << "File Create Failed:" << property->log_filename_value;
                                throw std::logic_error(buf.str());
                        }

                        // activate appender options
                        dateSizeAppender->activateOptions(pool);

                        // add time_and_size_based_appender to CategoryLogger
                        //cat_logger->addAppender(dateSizeAppender);
                        normalAppender = dateSizeAppender;

                }
                }    //switch

                for (category_level_map_type::iterator cat_itr = category_level_map.begin();
                     cat_itr != category_level_map.end();
                     ++cat_itr) {

                        category_name_map_type::iterator name_itr = category_name_map.find(cat_itr->first);
                        log4cxx::LoggerPtr cat_logger = log4cxx::Logger::getLogger(name_itr->second);
                        if (0 == cat_logger) {
                                throw std::logic_error("getLogger Failed.");
                        }

#if    defined(LOGGER_PROCESS_VSD)
                        if (cat_itr->first == LOG_CAT_L7VSD_NETWORK_ACCESS) {
                                cat_logger->addAppender(accessAppender);
                        } else {
                                cat_logger->addAppender(normalAppender);
                        }
#elif defined(LOGGER_PROCESS_ADM)
                        cat_logger->addAppender(normalAppender);
#else
                        cat_logger->addAppender(normalAppender);
#endif


                        //default log level settting
                        cat_itr->second = LOG_LV_INFO;
                        cat_logger->setLevel(log4cxx::Level::getInfo());

                }    //for (category logger setteing)

                //category level setting
                category_level_read_map.clear();
                BOOST_FOREACH(category_level_map_type::value_type & cat_itr, category_level_map) {

                        category_name_map_type::iterator name_itr = category_name_map.find(cat_itr.first);
                        log4cxx::LoggerPtr cat_logger = log4cxx::Logger::getLogger(name_itr->second);
                        if (0 == cat_logger) {
                                throw std::logic_error("getLogger Failed.");
                        }

                        l7vs::error_code ec;

                        LOG_CATEGORY_TAG    log_category;

#if    defined(LOGGER_PROCESS_VSD)
                        log_category = LOG_CAT_L7VSD_LOGGER;
#elif defined(LOGGER_PROCESS_ADM)
                        log_category = LOG_CAT_L7VSADM_LOGGER;
#else
                        log_category = LOG_CAT_L7VSD_LOGGER;
#endif

                        std::string levelStr = param.get_string(PARAM_COMP_LOGGER, name_itr->second, ec);
                        if (!ec) {
                                if ("debug" == levelStr) {
                                        cat_itr.second = LOG_LV_DEBUG;
                                } else if ("info" == levelStr) {
                                        cat_itr.second = LOG_LV_INFO;
                                } else if ("warn" == levelStr) {
                                        cat_itr.second = LOG_LV_WARN;
                                } else if ("error" == levelStr) {
                                        cat_itr.second = LOG_LV_ERROR;
                                } else if ("fatal" == levelStr) {
                                        cat_itr.second = LOG_LV_FATAL;
                                } else {
                                        std::ostringstream oss;
                                        oss << "Invalid Log Category Setting : " << name_itr->second;

                                        if (LOG_LV_WARN >= this->getLogLevel(log_category)) {
                                                this->putLogWarn(log_category, 1, oss.str(), __FILE__, __LINE__);
                                        }
                                        cat_itr.second = LOG_LV_INFO;
                                }
                                cat_logger->setLevel(log4cxx::Level::toLevel(levelTable[cat_itr.second]));
                        } else {
                                std::ostringstream oss;
                                oss << "Not Exist Log Category Setting : " << name_itr->second;
                                if (LOG_LV_WARN >= this->getLogLevel(log_category)) {
                                        this->putLogWarn(log_category, 2, oss.str(), __FILE__, __LINE__);
                                }
                                cat_itr.second = LOG_LV_INFO;
                                cat_logger->setLevel(log4cxx::Level::getInfo());
                        }
                        category_level_read_map[cat_itr.first] = cat_itr.second;
                }    //for (category level setting)

        } catch (const std::exception &e) {
                std::ostringstream oss;
                oss <<  "Logger Reload Config Failed : " << e.what();
                errorConf(2, oss.str(), __FILE__, __LINE__);
        }

}
