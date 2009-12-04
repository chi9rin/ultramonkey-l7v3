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
#include "logger_enum.h"
#include "logger_rotation_enum.h"

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

#define LOGGER_NULL "/dev/null"

#define LOGGER_LEVEL_NUM (6)

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
    static LoggerImpl& getInstance();

    //! initialze function
     virtual bool init();

    //! Configuration function
    virtual    void loadConf();

    /*!
     * retrieve category's log level.
     *
     * @param   category that want to know
     * @return  log level
     */
    virtual inline LOG_LEVEL_TAG getLogLevel(LOG_CATEGORY_TAG cat){
        return category_level_read_map[cat];
    }

    /*!
     * retrieve all category's log level.
     *
     * @param[out]   category level list
     */
    virtual inline void getLogLevelAll( category_level_list_type& list ){
        category_level_read_map.clear();
        BOOST_FOREACH( category_level_map_type::value_type const& itr, category_level_map ){
            category_level_read_map[itr.first] = itr.second;
            list.push_back( std::make_pair( itr.first, itr.second ) );
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
    virtual inline bool setLogLevel(LOG_CATEGORY_TAG cat, LOG_LEVEL_TAG level){
        category_level_map_type::iterator lv_itr = category_level_map.find( cat );
        lv_itr->second = level;

        category_level_read_map.clear();
        BOOST_FOREACH( category_level_map_type::value_type const& itr, category_level_map ){
            category_level_read_map[itr.first] = itr.second;
        }

        category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );

        try {
            log4cxx::Logger::getLogger(categoryname_itr->second)->setLevel( log4cxx::Level::toLevel( levelTable[level] ) );
        }
        catch (const std::exception& ex) {
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
    virtual inline bool setLogLevelAll( LOG_LEVEL_TAG level ){
        category_level_read_map.clear();
        BOOST_FOREACH( category_level_map_type::value_type& itr, category_level_map ){
            itr.second = level;
            category_level_read_map[itr.first] = itr.second;

            category_name_map_type::iterator categoryname_itr = category_name_map.find( itr.first );
            try {
                log4cxx::Logger::getLogger(categoryname_itr->second)->setLevel( log4cxx::Level::toLevel( levelTable[level] ) );
            }
            catch (const std::exception& ex) {
                return false;
            }
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
    virtual inline void putLogFatal(    LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string& message,
                                        const char *file,
                                        int line){
        std::stringstream buf;
        switch( cat ){
        case    LOG_CAT_PROTOCOL:
            buf << boost::format( "%s%d%06d %s %s" )
                % "PM"
                % LOG_LV_FATAL
                % message_id
                % message.c_str()
                % hostname;
            break;
        case    LOG_CAT_SCHEDULE:
            buf << boost::format( "%s%d%06d %s %s" )
                % "SM"
                % LOG_LV_FATAL
                % message_id
                % message.c_str()
                % hostname;
            break;
        default:
            buf << boost::format( "%s%d%02d%05d %s %s" )
                % LOGGER_PROCESS_ID
                % LOG_LV_FATAL
                % cat
                % message_id
                % message.c_str()
                % hostname;
        }
        try {
            category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );
            log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(    log4cxx::Level::getFatal(),
                                                                        buf.str(),
                                                                        log4cxx::spi::LocationInfo(file, "", line));
            // send_trap
            if( snmpSendtrap && ( LOG_CAT_L7VSD_SNMPBRIDGE != cat ) ){
                snmpSendtrap( buf.str() );
            }
        }
        catch (const std::exception& ex) {
            std::ostringstream oss;
            oss << "Logging Error (Fatal Log) : " << ex.what();
            errorConf( 3, oss.str(), __FILE__, __LINE__);
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
    virtual inline void putLogError(    LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string& message,
                                        const char *file,
                                        int line){
        std::stringstream    buf;
        switch( cat ){
        case    LOG_CAT_PROTOCOL:
            buf << boost::format( "%s%d%06d %s %s" )
                % "PM"
                % LOG_LV_ERROR
                % message_id
                % message.c_str()
                % hostname;
            break;
        case    LOG_CAT_SCHEDULE:
            buf << boost::format( "%s%d%06d %s %s" )
                % "SM"
                % LOG_LV_ERROR
                % message_id
                % message.c_str()
                % hostname;
            break;
        default:
            buf << boost::format( "%s%d%02d%05d %s %s" )
                % LOGGER_PROCESS_ID
                % LOG_LV_ERROR
                % cat
                % message_id
                % message.c_str()
                % hostname;
        }
        try {
            category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );
            log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(    log4cxx::Level::getError(),
                                                                        buf.str(),
                                                                        log4cxx::spi::LocationInfo(file, "", line));
            // send_trap
            if( snmpSendtrap && ( LOG_CAT_L7VSD_SNMPBRIDGE != cat ) ){
                snmpSendtrap( buf.str() );
            }
        }
        catch (const std::exception& ex) {
            std::ostringstream oss;
            oss << "Logging Error (Error Log) : " << ex.what();
            errorConf( 4, oss.str(), __FILE__, __LINE__);
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
    virtual inline void putLogWarn(        LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string& message,
                                        const char *file,
                                        int line){
        std::stringstream buf;
        switch( cat ){
        case    LOG_CAT_PROTOCOL:
            buf << boost::format( "%s%d%06d %s %s" )
                % "PM"
                % LOG_LV_WARN
                % message_id
                % message.c_str()
                % hostname;
            break;
        case    LOG_CAT_SCHEDULE:
            buf << boost::format( "%s%d%06d %s %s" )
                % "SM"
                % LOG_LV_WARN
                % message_id
                % message.c_str()
                % hostname;
            break;
        default:
            buf << boost::format( "%s%d%02d%05d %s %s" )
                % LOGGER_PROCESS_ID
                % LOG_LV_WARN
                % cat
                % message_id
                % message.c_str()
                % hostname;
        }
        try {
            category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );
            log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(    log4cxx::Level::getWarn(),
                                                                        buf.str(),
                                                                        log4cxx::spi::LocationInfo(file, "", line));
        }
        catch (const std::exception& ex) {
            std::ostringstream oss;
            oss << "Logging Error (Warn Log) : " << ex.what();
            errorConf( 5, oss.str(), __FILE__, __LINE__);
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
    virtual inline void putLogInfo(        LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string& message,
                                        const char *file,
                                        int line){
        std::stringstream    buf;
        switch( cat ){
        case    LOG_CAT_PROTOCOL:
            buf << boost::format( "%s%d%06d %s %s" )
                % "PM"
                % LOG_LV_INFO
                % message_id
                % message.c_str()
                % hostname;
            break;
        case    LOG_CAT_SCHEDULE:
            buf << boost::format( "%s%d%06d %s %s" )
                % "SM"
                % LOG_LV_INFO
                % message_id
                % message.c_str()
                % hostname;
            break;
        default:
            buf << boost::format( "%s%d%02d%05d %s %s" )
                % LOGGER_PROCESS_ID
                % LOG_LV_INFO
                % cat
                % message_id
                % message.c_str()
                % hostname;
        }
        try {
            category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );
            log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(    log4cxx::Level::getInfo(),
                                                                        buf.str(),
                                                                        log4cxx::spi::LocationInfo(file, "", line));
        }
        catch (const std::exception& ex) {
            std::ostringstream oss;
            oss << "Logging Error (Info Log) : " << ex.what();
            errorConf( 6, oss.str(), __FILE__, __LINE__);
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
    virtual inline void putLogDebug(    LOG_CATEGORY_TAG cat,
                                        const unsigned int message_id,
                                        const std::string& message,
                                        const char *file,
                                        int line){
        std::stringstream    buf;
        switch( cat ){
        case    LOG_CAT_PROTOCOL:
            buf << boost::format( "%s%d%06d %s %s" )
                % "PM"
                % LOG_LV_DEBUG
                % message_id
                % message.c_str()
                % hostname;
            break;
        case    LOG_CAT_SCHEDULE:
            buf << boost::format( "%s%d%06d %s %s" )
                % "SM"
                % LOG_LV_DEBUG
                % message_id
                % message.c_str()
                % hostname;
            break;
        default:
            buf << boost::format( "%s%d%02d%05d %s %s" )
                % LOGGER_PROCESS_ID
                % LOG_LV_DEBUG
                % cat
                % message_id
                % message.c_str()
                % hostname;
        }
        try {
            category_name_map_type::iterator categoryname_itr = category_name_map.find( cat );
            log4cxx::Logger::getLogger(categoryname_itr->second)->forcedLog(    log4cxx::Level::getDebug(),
                                                                        buf.str(), log4cxx::spi::LocationInfo(file,"", line));
        }
        catch (const std::exception& ex) {
            std::ostringstream oss;
            oss << "Logging Error (Debug Log) : " << ex.what();
            errorConf( 7, oss.str(), __FILE__, __LINE__);
        }
    }

    /*!
     * set snmp sendtrap function
     *
     * @param   snmp send trap function object
     * @retrun  void
     */
    void    setSnmpSendtrap( const snmpSendtrapFuncType func ){
        snmpSendtrap = func;
    }

protected:
    //! default constructor initialize member variables.
    LoggerImpl();

    //! cpoy constructor disable
    LoggerImpl( const LoggerImpl& );

    //! operator= disable
    LoggerImpl& operator=( const LoggerImpl& );

    //! LOG_LEVEL_TAG to log4cxx::LevelPtr transrator
    virtual inline const log4cxx::LevelPtr toLevel(LOG_LEVEL_TAG level)    {
        return log4cxx::Level::toLevel( levelTable[level] );
    }

    //! if error occured, switch appenders to syslogappender and fileappender(/dev/console)
    virtual void errorConf(    unsigned int messageId,
                            const std::string& errorMessage,
                            const char* file,
                            int line);

    virtual void logic_error( const unsigned int, const std::string&, const char*, const unsigned int);

/*
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
    //! category string -> logcateogry hash map
    name_category_map_type    name_category_map;
    //! log_category -> category string hash map
    category_name_map_type    category_name_map;

    struct appender_property{
        std::string     log_filename_key;            //!< parameter logfile key
        std::string        log_filename_value;            //!< parameter logfile value
        std::string     rotation_key;                //!< parameter rotation kind key
        LOG_ROTATION_TAG
                        rotation_value;                //!< parameter rotation kind value
        std::string     max_backup_index_key;        //!< parameter backup maximam number key
        unsigned int    max_backup_index_value;        //!< parameter backup maximam number value
        std::string     max_file_size_key;            //!< parameter maximam backup size key
        unsigned long long
                             max_file_size_value;    //!< parameter maximam backup size value
        std::string        rotation_timing_key;        //!< parameter rotation timing key
        LOG_ROTATION_TIMING_TAG
                        rotation_timing_value;        //!< parameter rotation timing value

        std::string        rotation_timing_value_key;    //!< parameter rotation timing value key
        std::string        rotation_timing_value_value;    //!< parameter rotation timing value value
        appender_property() :                            //!< constractor
            rotation_value(LOG_ROT_SIZE),
            max_backup_index_value(0),
            max_file_size_value(0),
            rotation_timing_value(LOG_TIM_YEAR)
        {}
    };
    appender_property    normal_log_property;
    appender_property    access_log_property;

    //! snmp trap function
    snmpSendtrapFuncType    snmpSendtrap;

};

}    //namespace l7vs
#endif    //__LOGGER_IMPL_H__
