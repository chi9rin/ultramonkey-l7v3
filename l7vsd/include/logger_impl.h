/*!
 * @file  logger_impl.h
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

#ifndef LOGGER_IMPL_H
#define LOGGER_IMPL_H

#include <sstream>
#include <map>
#include <log4cxx/logger.h>
#include <log4cxx/level.h>
#include <log4cxx/net/syslogappender.h>
#include <log4cxx/fileappender.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/patternlayout.h>
#include <boost/format.hpp>
#include <boost/tr1/unordered_map.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "logger_enum.h"
#include "logger_rotation_enum.h"
#include "appender_property.h"
#include "atomic.h"
#include "wrlock.h"

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

#define LOGGER_PROCESS_PROTOCOL_MODULE_ID "PRM"

#define LOGGER_PROCESS_SCHEDULE_MODULE_ID "SCM"

#define LOGGER_NULL "/dev/null"

#define LOGGER_LEVEL_NUM (6)

namespace log4cxx
{
typedef helpers::ObjectPtrT<RollingFileAppender> RollingFileAppenderPtr;
}

namespace l7vs
{

//! @class LoggerImpl
//! @brief Logger implement class.
//! @brief this class manage logger setting and logging by log4cxx.
class LoggerImpl
{
protected:
        //! typedef    category <-> level map
        typedef    std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
        category_level_map_type;
        //! typedef category <-> level map read only
        typedef std::tr1::unordered_map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG>
        category_level_read_map_type;
        //! typedef    categoryname <-> CATEGORY_TAG map
        typedef    std::map< std::string, LOG_CATEGORY_TAG>
        name_category_map_type;
        //! typedef CATEGORY_TAG <-> categoryname map
        typedef    std::map< LOG_CATEGORY_TAG, std::string>
        category_name_map_type;
public:
        //! returns current instance.
        static LoggerImpl &getInstance();

        //! initialize function
        virtual bool init();

        //! Configuration function
        virtual    void loadConf();

        /*!
         * retrieve category's log level.
         *
         * @param   category that want to know
         * @return  log level
         */
        virtual inline LOG_LEVEL_TAG getLogLevel(LOG_CATEGORY_TAG cat) {
                return category_level_read_map[cat];
        }

        /*!
         * retrieve all category's log level.
         *
         * @param[out]   category level list
         */
        virtual inline void getLogLevelAll(category_level_list_type &list) {
                category_level_read_map.clear();
                BOOST_FOREACH(category_level_map_type::value_type const & itr, category_level_map) {
                        category_level_read_map[itr.first] = itr.second;
                        list.push_back(std::make_pair(itr.first, itr.second));
                }
        }

        /*!
         * set category's log level.
         *
         * @param   category to set log level
         * @param   level
         * @retval  true  succeed
         * @retval  false failed
         */
        virtual inline bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level) {
                category_level_map_type::iterator lv_itr = category_level_map.find(cat);
                lv_itr->second = level;

                category_level_read_map.clear();
                BOOST_FOREACH(category_level_map_type::value_type const & itr, category_level_map) {
                        category_level_read_map[itr.first] = itr.second;
                }

                category_name_map_type::iterator categoryname_itr = category_name_map.find(cat);

                try {
                        log4cxx::Logger::getLogger(categoryname_itr->second)->setLevel(log4cxx::Level::toLevel(levelTable[level]));
                } catch (const std::exception &ex) {
                        return false;
                }
                return true;
        }

        /*!
         * set all category's log level.
         *
         * @param   category to set log level
         * @param   level
         * @retval  true  succeed
         * @retval  false failed
         */
        virtual inline bool setLogLevelAll(LOG_LEVEL_TAG level) {
                category_level_read_map.clear();
                BOOST_FOREACH(category_level_map_type::value_type & itr, category_level_map) {
                        itr.second = level;
                        category_level_read_map[itr.first] = itr.second;

                        category_name_map_type::iterator categoryname_itr = category_name_map.find(itr.first);
                        try {
                                log4cxx::Logger::getLogger(categoryname_itr->second)->setLevel(log4cxx::Level::toLevel(levelTable[level]));
                        } catch (const std::exception &ex) {
                                return false;
                        }
                }
                return true;
        }

        /*!
         * output fatal log.
         *
         * @param   category that logging matter occurred
         * @param   log message id
         * @param   log message
         * @param   current file
         * @param   current line
         * @retrun  void
         */
        virtual inline void putLogFatal(LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string &message,
                                        const char *file,
                                        int line) {
                std::stringstream   buf;


                switch (cat) {
                case    LOG_CAT_PROTOCOL:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_PROTOCOL_MODULE_ID
                            % LOG_LV_FATAL
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                case    LOG_CAT_SCHEDULE:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_SCHEDULE_MODULE_ID
                            % LOG_LV_FATAL
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                default:

                        buf << boost::format("%s%d%02d%05d %s %s")
                            % LOGGER_PROCESS_ID
                            % LOG_LV_FATAL
                            % cat
                            % message_id
                            % message.c_str()
                            % hostname;

                }

                try {
                        category_name_map_type::iterator categoryname_itr = category_name_map.find(cat);
                        log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(log4cxx::Level::getFatal(),
                                        buf.str(),
                                        log4cxx::spi::LocationInfo(file, "", line));
                        // send_trap
			if (logtrap_enabled.get() == 1) {
                                LOG_LEVEL_TAG tmp_level;
                                {
					rd_scoped_lock lock(logtrap_level_mutex);
                                        tmp_level = logtrap_level;
                                }
                                if ( (tmp_level <= LOG_LV_FATAL) &&
				      snmp_send_trap_func        &&
                                      (LOG_CAT_L7VSD_SNMPAGENT != cat)) {
                                        trapmessage trap_msg;
					trap_msg.type = trapmessage::FATAL_LOG;
                                        trap_msg.message = buf.str();
					error_code err;
                                        //send trap message
                                        snmp_send_trap_func(trap_msg, err);
                                }
                        }
                } catch (const std::exception &ex) {
                        std::ostringstream oss;
                        oss << "Logging Error (Fatal Log) : " << ex.what();
                        errorConf(3, oss.str(), __FILE__, __LINE__);
                }
        }
        /*!
         * output error log.
         *
         * @param   category that logging matter occurred
         * @param   log message id
         * @param   log message
         * @param   current file
         * @param   current line
         * @retrun  void
         */
        virtual inline void putLogError(LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string &message,
                                        const char *file,
                                        int line) {
                std::stringstream    buf;

                switch (cat) {
                case    LOG_CAT_PROTOCOL:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_PROTOCOL_MODULE_ID
                            % LOG_LV_ERROR
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                case    LOG_CAT_SCHEDULE:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_SCHEDULE_MODULE_ID
                            % LOG_LV_ERROR
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                default:

                        buf << boost::format("%s%d%02d%05d %s %s")
                            % LOGGER_PROCESS_ID
                            % LOG_LV_ERROR
                            % cat
                            % message_id
                            % message.c_str()
                            % hostname;

                }

                try {
                        category_name_map_type::iterator categoryname_itr = category_name_map.find(cat);
                        log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(log4cxx::Level::getError(),
                                        buf.str(),
                                        log4cxx::spi::LocationInfo(file, "", line));
                        // send_trap
                        if (logtrap_enabled.get() == 1) {
                                LOG_LEVEL_TAG tmp_level;
                                {
                                        rd_scoped_lock lock(logtrap_level_mutex);
                                        tmp_level = logtrap_level;
                                }
                                if ( (tmp_level <= LOG_LV_ERROR) &&
                                      snmp_send_trap_func        &&
                                      (LOG_CAT_L7VSD_SNMPAGENT != cat)) {
                                        trapmessage trap_msg;
                                        trap_msg.type = trapmessage::ERROR_LOG;
                                        trap_msg.message = buf.str();
					error_code err;
                                        //send trap message
                                        snmp_send_trap_func(trap_msg, err);
                                }
                        }
                } catch (const std::exception &ex) {
                        std::ostringstream oss;
                        oss << "Logging Error (Error Log) : " << ex.what();
                        errorConf(4, oss.str(), __FILE__, __LINE__);
                }
        }
        /*!
         * output warn log.
         *
         * @param   category that logging matter occurred
         * @param   log message id
         * @param   log message
         * @param   current file
         * @param   current line
         * @retrun  void
         */
        virtual inline void putLogWarn(LOG_CATEGORY_TAG cat,
                                       const unsigned int message_id,
                                       const std::string &message,
                                       const char *file,
                                       int line) {
                std::stringstream buf;

                switch (cat) {
                case    LOG_CAT_PROTOCOL:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_PROTOCOL_MODULE_ID
                            % LOG_LV_WARN
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                case    LOG_CAT_SCHEDULE:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_SCHEDULE_MODULE_ID
                            % LOG_LV_WARN
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                default:

                        buf << boost::format("%s%d%02d%05d %s %s")
                            % LOGGER_PROCESS_ID
                            % LOG_LV_WARN
                            % cat
                            % message_id
                            % message.c_str()
                            % hostname;

                }

                try {
                        category_name_map_type::iterator categoryname_itr = category_name_map.find(cat);
                        log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(log4cxx::Level::getWarn(),
                                        buf.str(),
                                        log4cxx::spi::LocationInfo(file, "", line));
                        // send_trap
                        if (logtrap_enabled.get() == 1) {
                                LOG_LEVEL_TAG tmp_level;
                                {
                                        rd_scoped_lock lock(logtrap_level_mutex);
                                        tmp_level = logtrap_level;
                                }
                                if ( (tmp_level <= LOG_LV_WARN) &&
                                      snmp_send_trap_func       &&
                                      (LOG_CAT_L7VSD_SNMPAGENT != cat)) {
                                        trapmessage trap_msg;
                                        trap_msg.type = trapmessage::WARN_LOG;
                                        trap_msg.message = buf.str();
					error_code err;
                                        //send trap message
                                        snmp_send_trap_func(trap_msg, err);
                                }
                        }
                } catch (const std::exception &ex) {
                        std::ostringstream oss;
                        oss << "Logging Error (Warn Log) : " << ex.what();
                        errorConf(5, oss.str(), __FILE__, __LINE__);
                }
        }
        /*!
         * output info log.
         *
         * @param   category that logging matter occurred
         * @param   log message id
         * @param   log message
         * @param   current file
         * @param   current line
         * @retrun  void
         */
        virtual inline void putLogInfo(LOG_CATEGORY_TAG cat,
                                       const unsigned int message_id,
                                       const std::string &message,
                                       const char *file,
                                       int line) {
                std::stringstream    buf;

                switch (cat) {
                case    LOG_CAT_PROTOCOL:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_PROTOCOL_MODULE_ID
                            % LOG_LV_INFO
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                case    LOG_CAT_SCHEDULE:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_SCHEDULE_MODULE_ID
                            % LOG_LV_INFO
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                default:

                        buf << boost::format("%s%d%02d%05d %s %s")
                            % LOGGER_PROCESS_ID
                            % LOG_LV_INFO
                            % cat
                            % message_id
                            % message.c_str()
                            % hostname;

                }

                try {
                        category_name_map_type::iterator categoryname_itr = category_name_map.find(cat);
                        log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(log4cxx::Level::getInfo(),
                                        buf.str(),
                                        log4cxx::spi::LocationInfo(file, "", line));

                        // send_trap
                        if (logtrap_enabled.get() == 1) {
                                LOG_LEVEL_TAG tmp_level;
                                {
                                        rd_scoped_lock lock(logtrap_level_mutex);
                                        tmp_level = logtrap_level;
                                }
                                if ( (tmp_level <= LOG_LV_INFO) &&
                                      snmp_send_trap_func       &&
                                      (LOG_CAT_L7VSD_SNMPAGENT != cat)) {
                                        trapmessage trap_msg;
                                        trap_msg.type = trapmessage::INFO_LOG;
                                        trap_msg.message = buf.str();
					error_code err;
                                        //send trap message
                                        snmp_send_trap_func(trap_msg, err);
                                }
                        }
                } catch (const std::exception &ex) {
                        std::ostringstream oss;
                        oss << "Logging Error (Info Log) : " << ex.what();
                        errorConf(6, oss.str(), __FILE__, __LINE__);
                }
        }
        /*!
         * output debug log.
         *
         * @param   category that logging matter occurred
         * @param   log message id
         * @param   log message
         * @param   current file
         * @param   current line
         * @retrun  void
         */
        virtual inline void putLogDebug(LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string &message,
                                        const char *file,
                                        int line) {
                std::stringstream    buf;
                std::string logger_process_id = LOGGER_PROCESS_ID;

                switch (cat) {
                case    LOG_CAT_PROTOCOL:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_PROTOCOL_MODULE_ID
                            % LOG_LV_DEBUG
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                case    LOG_CAT_SCHEDULE:

                        buf << boost::format("%s%d%07d %s %s")
                            % LOGGER_PROCESS_SCHEDULE_MODULE_ID
                            % LOG_LV_DEBUG
                            % message_id
                            % message.c_str()
                            % hostname;

                        break;
                default:

                        buf << boost::format("%s%d%02d%05d %s %s")
                            % LOGGER_PROCESS_ID
                            % LOG_LV_DEBUG
                            % cat
                            % message_id
                            % message.c_str()
                            % hostname;

                }


                try {
                        category_name_map_type::iterator categoryname_itr = category_name_map.find(cat);
                        log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(log4cxx::Level::getDebug(),
                                        buf.str(), log4cxx::spi::LocationInfo(file, "", line));
                        // send_trap
                        if (logtrap_enabled.get() == 1) {
                                LOG_LEVEL_TAG tmp_level;
                                {
                                        rd_scoped_lock lock(logtrap_level_mutex);
                                        tmp_level = logtrap_level;
                                }
                                if ( (tmp_level <= LOG_LV_DEBUG) &&
                                      snmp_send_trap_func        &&
                                      (LOG_CAT_L7VSD_SNMPAGENT != cat)) {
                                        trapmessage trap_msg;
                                        trap_msg.type = trapmessage::DEBUG_LOG;
                                        trap_msg.message = buf.str();

					error_code err;
                                        //send trap message
                                        snmp_send_trap_func(trap_msg, err);
                                }
                        }
                } catch (const std::exception &ex) {
                        std::ostringstream oss;
                        oss << "Logging Error (Debug Log) : " << ex.what();
                        errorConf(7, oss.str(), __FILE__, __LINE__);
                }
        }

        /*!
         * set snmp sendtrap function
         *
         * @param   snmp send trap function object
         * @retrun  void
         */
	 void    set_snmp_send_trap_func(const snmp_send_trap_func_type func) {
                snmp_send_trap_func = func;
        }

        /*!
         * set log trap enable
         * @retrun  void
         */
        void    logtrap_enable() {
                logtrap_enabled = true;
        }

        /*!
	 * set log trap disable 	
         * @retrun  void
         */
	void    logtrap_disable() {
                logtrap_enabled = false;
        }

        /*!
         * set log trap level
         * @param   log trap level
         * @retrun  void
         */
        void    set_logtrap_level(LOG_LEVEL_TAG in_logtrap_level) {
                rw_scoped_lock lock(logtrap_level_mutex);
                logtrap_level = in_logtrap_level;
        }


protected:
        //! default constructor initialize member variables.
        LoggerImpl();

        //! copy constructor disable
        LoggerImpl(const LoggerImpl &);

        //! operator= disable
        LoggerImpl &operator=(const LoggerImpl &);

        //! LOG_LEVEL_TAG to log4cxx::LevelPtr translator
        virtual inline const log4cxx::LevelPtr toLevel(LOG_LEVEL_TAG level)    {
                return log4cxx::Level::toLevel(levelTable[level]);
        }

        //! if error occurred, switch appenders to syslogappender and fileappender(/dev/console)
        virtual void errorConf(unsigned int messageId,
                               const std::string &errorMessage,
                               const char *file,
                               int line);

        virtual void logic_error(const unsigned int, const std::string &, const char *, const unsigned int);

        /*
            //! log4cxx::LevelPtr to LOG_LEVEL_TAG translator
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
        */

        //! destructor.
        virtual ~LoggerImpl() {}
        //! initialized flag
        bool initialized;
        //! hostname
        std::string hostname;

        //log4cxx::Level* levelTable[LOGGER_LEVEL_NUM];
        int    levelTable[LOGGER_LEVEL_NUM];

        //! category - loglevel hash map
        category_level_map_type    category_level_map;
        category_level_read_map_type category_level_read_map;
        //! category string -> logcategory hash map
        name_category_map_type    name_category_map;
        //! log_category -> category string hash map
        category_name_map_type    category_name_map;

        appender_property    normal_log_property;
        appender_property    access_log_property;

        //! snmp trap function
        snmp_send_trap_func_type    snmp_send_trap_func;

        //! snmp log trap enable flag
        atomic<unsigned int>    logtrap_enabled;

        //! snmp log trap level
        LOG_LEVEL_TAG    logtrap_level;

        //! snmp log trap level mutex
        wr_mutex    logtrap_level_mutex;

};

}    //namespace l7vs
#endif    //__LOGGER_IMPL_H__
