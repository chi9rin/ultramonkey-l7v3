/*!
 * @file  logger_rogrote_utility.cpp
 * @brief logger module implementation class For access log.
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
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <limits.h>
#include <errno.h>
#include <stdexcept>
#include <log4cxx/logmanager.h>
#include <log4cxx/helpers/loglog.h>
#include <log4cxx/rolling/rollingfileappender.h>
#include <log4cxx/rolling/fixedwindowrollingpolicy.h>
#include <log4cxx/rolling/sizebasedtriggeringpolicy.h>
#include <log4cxx/rolling/timebasedrollingpolicy.h>
#include <log4cxx/consoleappender.h>

#include "logger_enum.h"
#include "logger.h"

#include "logger_logrotate_utility.h"
#include "strict_time_based_rolling_policy.h"
#include "time_and_size_based_rolling_policy.h"


#define LOGGER_DEFAULT_BUFFER_SIZE  (8 * 1024)
#define LOGGER_SYSLOG_FACILITY  "USER"
#define LOGGER_BACKUP_INDEX_LOWER_LIMIT (1)
#define LOGGER_BACKUP_INDEX_LIMIT   (12)
#define LOGGER_FILESIZE_LOWER_LIMIT (65535)
#define LOGGER_FILE_PATTERN "%i"


using namespace log4cxx;
using namespace l7vs;

/*!
 * rotate type check.
 *
 * @param   rotaton string("size" or "date" or "datesize")
 * @return  LOG_ROTATION_TAG val
 */
LOG_ROTATION_TAG logger_logrotate_utility::check_rotate_type(const std::string &rotation_type)
{
    LOG_ROTATION_TAG rotation_value = LOG_ROT_SIZE;
    
    // get rotation
    if( rotation_type != "" ){
        if ("size" == rotation_type) rotation_value = LOG_ROT_SIZE;
        else if ("date" == rotation_type) rotation_value = LOG_ROT_DATE;
        else if ("datesize" == rotation_type) rotation_value = LOG_ROT_DATESIZE;
        else {
            std::stringstream   ss;
            ss << "Invalid Log Rotation Setting : " << rotation_type;
            logger_logrotate_utility::loglotation_utility_logic_error( 51, ss.str(), __FILE__, __LINE__ );
        }
    }
    else{
        logger_logrotate_utility::loglotation_utility_logic_error( 52, "Not Exist Log Rotation Setting.", __FILE__, __LINE__ );
    }
    
    return( rotation_value );
}

/*!
 * log file max backup check.
 *
 * @param   file backup val string
 * @return  file backup val
 */
unsigned int logger_logrotate_utility::check_max_backup_index(const std::string &max_backup_index)
{
    unsigned int max_backup_index_value = 0;
    
    if( max_backup_index != "" ) {
        try {
            max_backup_index_value = boost::lexical_cast<unsigned int>(max_backup_index);
        }
        catch (const boost::bad_lexical_cast& bc) {
            std::stringstream    ss;
            ss << "Invalid MaxBackupIndex Value : " << max_backup_index << ".";
            logger_logrotate_utility::loglotation_utility_logic_error( 53, ss.str(), __FILE__, __LINE__ );
        }
        if (LOGGER_BACKUP_INDEX_LOWER_LIMIT > max_backup_index_value) {
            std::stringstream ss;
            ss << "Max Backup Index must at least " << LOGGER_BACKUP_INDEX_LOWER_LIMIT << ".";
            logger_logrotate_utility::loglotation_utility_logic_error( 54, ss.str(), __FILE__, __LINE__ );
        }
        if (LOGGER_BACKUP_INDEX_LIMIT < max_backup_index_value) {
            std::stringstream ss;
            ss << "Max Backup Index must at most " << LOGGER_BACKUP_INDEX_LIMIT << ".";
            logger_logrotate_utility::loglotation_utility_logic_error( 55, ss.str(), __FILE__, __LINE__ );
        }
    }
    else {
        logger_logrotate_utility::loglotation_utility_logic_error( 56, "Not Exist Log MaxBackupIndex Setting.", __FILE__, __LINE__ );
    }

    return( max_backup_index_value );
}

/*!
 * log file max file size check.
 *
 * @param   file max size string
 * @return  file max size val
 */
unsigned long long logger_logrotate_utility::check_max_file_size(const std::string &max_file_size)
{

    unsigned long long max_file_size_value = 0;
    
    if( max_file_size == "" ) {
        logger_logrotate_utility::loglotation_utility_logic_error( 57, "Not Exist Log MaxFileSize Setting.", __FILE__, __LINE__ );
    }

    std::string size_val;
    int maxFileSizeStr_length = max_file_size.length();
    if( maxFileSizeStr_length <= 0 ) {
        logger_logrotate_utility::loglotation_utility_logic_error( 58, "Invalid FileSize Value.", __FILE__, __LINE__ );
    }

    std::string last_str = max_file_size.substr(maxFileSizeStr_length - 1, 1);
    // when unit was specified
    if (("K" == last_str) || ("M" == last_str) || ("G" == last_str))
        size_val = max_file_size.substr(0, max_file_size.length() - 1);
    else
        size_val = max_file_size;

    try {
        max_file_size_value = boost::lexical_cast<size_t>(size_val);
    }
    catch (const boost::bad_lexical_cast& bc){
        logger_logrotate_utility::loglotation_utility_logic_error( 59, "Invalid FileSize Value.", __FILE__, __LINE__ );
    }

    if ("K" == last_str) {
        if ((ULLONG_MAX / 1024) < max_file_size_value){
            logger_logrotate_utility::loglotation_utility_logic_error( 60, "Invalid FileSize Value.", __FILE__, __LINE__);
        }
        max_file_size_value = max_file_size_value * 1024;
    }
    else if ("M" == last_str) {
        if ((ULLONG_MAX / 1024 / 1024) < max_file_size_value)
            logger_logrotate_utility::loglotation_utility_logic_error( 61, "Invalid FileSize Value.", __FILE__, __LINE__);
        max_file_size_value = max_file_size_value * 1024 * 1024;
    }
    else if ("G" == last_str) {
        if ((ULLONG_MAX / 1024 / 1024 / 1024) < max_file_size_value)
            logger_logrotate_utility::loglotation_utility_logic_error( 62, "Invalid FileSize Value.", __FILE__, __LINE__);
        max_file_size_value = max_file_size_value * 1024 * 1024 * 1024;
    }
    if (LOGGER_FILESIZE_LOWER_LIMIT > max_file_size_value){
        int limit = LOGGER_FILESIZE_LOWER_LIMIT;
        std::ostringstream oss;
        oss << "FileSize must at least " << limit << " bytes.";
        logger_logrotate_utility::loglotation_utility_logic_error( 63, oss.str(), __FILE__, __LINE__);
    }
    

    return( max_file_size_value );
}

/*!
 * log file rotate timing check.
 *
 * @param   rotate timing string
 * @return  LOG_ROTATION_TIMING_TAG val
 */
LOG_ROTATION_TIMING_TAG logger_logrotate_utility::check_rotate_timing( const std::string &rotation_timing )
{

    LOG_ROTATION_TIMING_TAG rotation_timing_value = LOG_TIM_YEAR;
    
    if( rotation_timing != "" ){
        if ("year" == rotation_timing) rotation_timing_value = LOG_TIM_YEAR;
        else if ("month" == rotation_timing) rotation_timing_value = LOG_TIM_MONTH;
        else if ("week" == rotation_timing) rotation_timing_value = LOG_TIM_WEEK;
        else if ("date" == rotation_timing) rotation_timing_value = LOG_TIM_DATE;
        else if ("hour" == rotation_timing) rotation_timing_value = LOG_TIM_HOUR;
        else logger_logrotate_utility::loglotation_utility_logic_error( 64, "Invalid Log RotationTiming Setting.", __FILE__, __LINE__);
    }
    else{    logger_logrotate_utility::loglotation_utility_logic_error( 65, "Not Exist Log RotaionTiming Setting.", __FILE__, __LINE__);}

    
    return( rotation_timing_value );
}

/*!
 * rotate timing value check.
 *
 * @param   rotate timing val string
 * @param   LOG_ROTATION_TIMING_TAG val
 * @return rotate timing val
 */
std::string logger_logrotate_utility::check_rotate_timing_value(const std::string rotation_timing_value_key,const LOG_ROTATION_TIMING_TAG rotation_timing)
{
    std::string rotation_timing_value_value = "";
    std::string ret = rotation_timing_value_key;
    
    if(LOG_TIM_YEAR == rotation_timing ){
        if( rotation_timing_value_key != "" ){
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
                }
                catch (const boost::bad_lexical_cast& bc) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 66, "Parse Timing Year Error.", __FILE__, __LINE__);
                }
                if (1 > month || month > 12) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 67, "Parse Timing Year Error.", __FILE__, __LINE__);
                }
                fpos = rpos + 1;
                // find date
                rpos = ret.find_first_of(' ', fpos);
                if (std::string::npos != rpos){
                    std::string dateStr = ret.substr(fpos, rpos - fpos);
                    try {
                        date = boost::lexical_cast<int>(dateStr);
                    }
                    catch (const boost::bad_lexical_cast& bc) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 68, "Parse Timing Year Error.", __FILE__, __LINE__);
                    }
                }
                if (1 > date || date > 31)  logger_logrotate_utility::loglotation_utility_logic_error( 69, "Parse Timing Year Error.", __FILE__, __LINE__);

                int dates[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                if (date > dates[month - 1]) logger_logrotate_utility::loglotation_utility_logic_error( 70, "Parse Timing Year Error.", __FILE__, __LINE__);

                fpos = rpos + 1;
                // find hour
                rpos = ret.find_first_of(':', fpos);
                if(std::string::npos != rpos) {
                    std::string hourStr = ret.substr(fpos, rpos - fpos);
                    try{
                        hour = boost::lexical_cast<int>(hourStr);
                    }
                    catch(const boost::bad_lexical_cast& bc){
                        logger_logrotate_utility::loglotation_utility_logic_error( 71, "Parse Timing Year Error.", __FILE__, __LINE__);
                    }
                    if(0 > hour || hour > 23) logger_logrotate_utility::loglotation_utility_logic_error( 72, "Parse Timing Year Error.", __FILE__, __LINE__);

                    // minute
                    std::string minuteStr = ret.substr(rpos + 1);
                    try {
                        minute = boost::lexical_cast<int>(minuteStr);
                    }
                    catch(const boost::bad_lexical_cast& bc){
                        logger_logrotate_utility::loglotation_utility_logic_error( 73, "Parse Timing Year Error.", __FILE__, __LINE__);
                    }
                    if (0 > minute || minute > 59) logger_logrotate_utility::loglotation_utility_logic_error( 74, "Parse Timing Year Error.", __FILE__, __LINE__);
                }
                else{
                    logger_logrotate_utility::loglotation_utility_logic_error( 75, "Parse Timing Year Error.", __FILE__, __LINE__);
                }
            }
            else{
                logger_logrotate_utility::loglotation_utility_logic_error( 76, "Parse Timing Year Error.", __FILE__, __LINE__);
            }

            // format to internal rotation timing value expresson
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << month
                << std::setfill('0') << std::setw(2) << date
                << std::setfill('0') << std::setw(2) << hour
                << std::setfill('0') << std::setw(2) << minute;

            rotation_timing_value_value = oss.str();

        }
        else {
            logger_logrotate_utility::loglotation_utility_logic_error( 77, "Not Exist Log RotaionTiming Year Setting.", __FILE__, __LINE__);
        }
    }


    if (LOG_TIM_MONTH == rotation_timing ) {
        if( rotation_timing_value_key != "" ){
            std::string::size_type fpos = 0;
            std::string::size_type rpos = 0;
            int date = 0;
            int hour = 0;
            int minute = 0;
            // find day
            rpos = ret.find_first_of(' ', fpos);
            if(std::string::npos != rpos) {
                std::string dateStr = ret.substr(fpos, rpos - fpos);
                try{
                    date = boost::lexical_cast<int>(dateStr);
                }
                catch(const boost::bad_lexical_cast& bc) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 78, "Parse Timing Month Error.", __FILE__, __LINE__);
                }
                if(1 > date || date > 31) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 79, "Parse Timing Month Error.", __FILE__, __LINE__);
                }
                fpos = rpos + 1;
                // find hour
                rpos = ret.find_first_of(':', fpos);
                if(std::string::npos != rpos) {
                    std::string hourStr = ret.substr(fpos, rpos - fpos);
                    try{
                        hour = boost::lexical_cast<int>(hourStr);
                    }
                    catch(const boost::bad_lexical_cast& bc) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 80, "Parse Timing Month Error.", __FILE__, __LINE__);
                    }
                    if(0 > hour || hour > 23) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 81, "Parse Timing Month Error.", __FILE__, __LINE__);
                    }
                    // minute
                    std::string minuteStr = ret.substr(rpos + 1);
                    try{
                        minute = boost::lexical_cast<int>(minuteStr);
                    }
                    catch(const boost::bad_lexical_cast& bc) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 82, "Parse Timing Month Error.", __FILE__, __LINE__);
                    }
                    if(0 > minute || minute > 59) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 83, "Parse Timing Month Error.", __FILE__, __LINE__);
                    }
                }
                else{
                    logger_logrotate_utility::loglotation_utility_logic_error( 84, "Parse Timing Month Error.", __FILE__, __LINE__);
                }
            }
            else{
                logger_logrotate_utility::loglotation_utility_logic_error( 85, "Parse Timing Month Error.", __FILE__, __LINE__);
            }

            // format to internal rotation timing value expresson
            std::stringstream oss;
            oss << std::setfill('0') << std::setw(2) << date
                << std::setfill('0') << std::setw(2) << hour
                << std::setfill('0') << std::setw(2) << minute;

            rotation_timing_value_value = oss.str();

        }
        else{
            logger_logrotate_utility::loglotation_utility_logic_error( 86, "Not Exist Log RotaionTiming Month Setting.", __FILE__, __LINE__);
        }
    }

    if(LOG_TIM_WEEK == rotation_timing ){
        if( rotation_timing_value_key != "" ){
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
                else if("mon" == weekStr) week = 1;
                else if("tue" == weekStr) week = 2;
                else if("wed" == weekStr) week = 3;
                else if("thu" == weekStr) week = 4;
                else if("fri" == weekStr) week = 5;
                else if("sat" == weekStr) week = 6;
                else{
                    logger_logrotate_utility::loglotation_utility_logic_error( 87, "Parse Timing Week Error.", __FILE__, __LINE__);
                }
                fpos = rpos + 1;
                // find hour
                rpos = ret.find_first_of(':', fpos);
                if(std::string::npos != rpos) {
                    std::string hourStr = ret.substr(fpos, rpos - fpos);
                    try{
                        hour = boost::lexical_cast<int>(hourStr);
                    }
                    catch (const boost::bad_lexical_cast& bc) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 88, "Parse Timing Week Error.", __FILE__, __LINE__);
                    }
                    if(0 > hour || hour > 23) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 89, "Parse Timing Week Error.", __FILE__, __LINE__);
                    }
                    // minute
                    std::string minuteStr = ret.substr(rpos + 1);
                    try{
                        minute = boost::lexical_cast<int>(minuteStr);
                    }
                    catch(const boost::bad_lexical_cast& bc) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 90, "Parse Timing Week Error.", __FILE__, __LINE__);
                    }
                    if(0 > minute || minute > 59) {
                        logger_logrotate_utility::loglotation_utility_logic_error( 91, "Parse Timing Week Error.", __FILE__, __LINE__);
                    }
                }
                else{
                    logger_logrotate_utility::loglotation_utility_logic_error( 92, "Parse Timing Week Error.", __FILE__, __LINE__);
                }
            }
            else{
                logger_logrotate_utility::loglotation_utility_logic_error( 93, "Parse Timing Week Error.", __FILE__, __LINE__);
            }

            // format to internal rotation timing value expresson
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(1) << week
                << std::setfill('0') << std::setw(2) << hour
                << std::setfill('0') << std::setw(2) << minute;

            rotation_timing_value_value = oss.str();
        }
        else{
            logger_logrotate_utility::loglotation_utility_logic_error( 94, "Not Exist Log RotaionTiming Week Setting.", __FILE__, __LINE__);
        }
    }

    if(LOG_TIM_DATE == rotation_timing){
        if( rotation_timing_value_key != "" ){
            std::string::size_type fpos = 0;
            std::string::size_type rpos = 0;
            int hour = 0;
            int minute = 0;
            //find time
            rpos = ret.find_first_of(':', fpos);
            if(std::string::npos != rpos) {
                std::string hourStr = ret.substr(fpos, rpos - fpos);
                try{
                    hour = boost::lexical_cast<int>(hourStr);
                }
                catch(const boost::bad_lexical_cast& bc) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 95, "Parse Timing Date Error.", __FILE__, __LINE__);
                }
                if(0 > hour || hour > 23) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 96, "Parse Timing Date Error.", __FILE__, __LINE__);
                }
                // minute
                std::string minuteStr = ret.substr(rpos + 1);
                try{
                    minute = boost::lexical_cast<int>(minuteStr);
                }
                catch(const boost::bad_lexical_cast& bc) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 97, "Parse Timing Date Error.", __FILE__, __LINE__);
                }
                if(0 > minute || minute > 59) {
                    logger_logrotate_utility::loglotation_utility_logic_error( 98, "Parse Timing Date Error.", __FILE__, __LINE__);
                }
            }
            else{
                logger_logrotate_utility::loglotation_utility_logic_error( 99, "Parse Timing Date Error.", __FILE__, __LINE__);
            }

            // format to internal rotation timing value expresson
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << hour
                << std::setfill('0') << std::setw(2) << minute;

            rotation_timing_value_value = oss.str();
        }
        else{
            logger_logrotate_utility::loglotation_utility_logic_error( 100, "Not Exist Log RotaionTiming Date Setting.", __FILE__, __LINE__);
        }
    }

    if(LOG_TIM_HOUR == rotation_timing) {
        if( rotation_timing_value_key != "" ){
            // minute
            int minute = 0;
            try {
                minute = boost::lexical_cast<int>(ret);
            }
            catch(const boost::bad_lexical_cast& bc){
                logger_logrotate_utility::loglotation_utility_logic_error( 101, "Parse Timing Hour Error.", __FILE__, __LINE__);
            }
            if (0 > minute || minute > 59) {
                logger_logrotate_utility::loglotation_utility_logic_error( 102, "Parse Timing Hour Error.", __FILE__, __LINE__);
            }

            // format to internal rotation timing value expresson
            std::ostringstream oss;
            oss << std::setfill('0') << std::setw(2) << minute;

            rotation_timing_value_value = oss.str();
        }
        else {
            logger_logrotate_utility::loglotation_utility_logic_error( 103, "Not Exist Log RotaionTiming Hour Setting.", __FILE__, __LINE__);
        }
    }

    return( rotation_timing_value_value );
}

/*!
 * Logger set appender.
 *
 * @param   appender property
 * @param   logger layout
 * @param   log category
 * @return  void
 */
void logger_logrotate_utility::set_appender(const appender_property& log_property,const char *logger_layout,const std::string &log_category)
{


    // appender setting
    try {
        log4cxx::helpers::Pool pool;

        log4cxx::LayoutPtr layout =
            new log4cxx::PatternLayout(logger_layout);

        log4cxx::rolling::RollingFileAppenderPtr    normalAppender;

        switch (log_property.rotation_value) {
        case LOG_ROT_SIZE:
            {
                // create FixedWindcd owRollingPolicy
                log4cxx::rolling::FixedWindowRollingPolicyPtr fixedRollingPolicy =
                    new log4cxx::rolling::FixedWindowRollingPolicy();

                // setting minIndex
                fixedRollingPolicy->setMinIndex(1);

                // setting maxIndex
                fixedRollingPolicy->setMaxIndex(log_property.max_backup_index_value);

                // setting FileNamePattern
                std::ostringstream sizeFile;
                sizeFile << log_property.log_filename_value << "." << LOGGER_FILE_PATTERN;
                fixedRollingPolicy->setFileNamePattern(sizeFile.str());

                // create SizeBasedTriggeringPolicy
                log4cxx::rolling::SizeBasedTriggeringPolicyPtr sizeTriggeringPolicy =
                    new log4cxx::rolling::SizeBasedTriggeringPolicy();

                // setting maxFileSize
                sizeTriggeringPolicy->setMaxFileSize(log_property.max_file_size_value);

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
                try{
                    sizeAppender->setFile(log_property.log_filename_value, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
                }
                catch( const std::exception& e ){
                    std::stringstream    buf;
                    buf << "File Create Failed:" << log_property.log_filename_value;
                    logger_logrotate_utility::loglotation_utility_logic_error( 104, buf.str(), __FILE__, __LINE__);
                }

                // activate appender options
                sizeAppender->activateOptions(pool);

                // add size_base_appender to CategoryLogger
                //cat_logger->addAppender(sizeAppender);
                normalAppender = sizeAppender;

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
                strictRollingPolicy->setMaxIndex(log_property.max_backup_index_value);

                // setting FileNamePattern
                std::ostringstream dateFile;
                dateFile << log_property.log_filename_value << "." << LOGGER_FILE_PATTERN;
                strictRollingPolicy->setFileNamePattern(dateFile.str());

                // setting Rotation Timing
                strictRollingPolicy->setRotationTiming(log_property.rotation_timing_value);

                // setting Rotation Timing Value
                strictRollingPolicy->setRotationTimingValue(log_property.rotation_timing_value_value);

                //create RollingFileAppender
                log4cxx::rolling::RollingFileAppenderPtr dateAppender =
                    new log4cxx::rolling::RollingFileAppender();

                // set layout
                dateAppender->setLayout(layout);

                // set RollingPolicy (TriggeringPolicy also included RollingPolicy)
                dateAppender->setRollingPolicy(strictRollingPolicy);

                // set Log Filename
                try{
                    dateAppender->setFile(log_property.log_filename_value, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
                }
                catch( const std::exception& e ){
                    std::stringstream    buf;
                    buf << "File Create Failed:" << log_property.log_filename_value;
                    logger_logrotate_utility::loglotation_utility_logic_error( 105, buf.str(), __FILE__, __LINE__);
                }

                // activate appender options
                dateAppender->activateOptions(pool);

                // add date_based_appender to CategoryLogger
                //cat_logger->addAppender(dateAppender);
                normalAppender = dateAppender;

                break;
            }
        default:    //LOG_ROT_DATESIZE:
            {
                // create TimeAndSizeBasedRollingPolicy
                log4cxx::rolling::TimeAndSizeBasedRollingPolicyPtr timeSizeRollingPolicy =
                    new log4cxx::rolling::TimeAndSizeBasedRollingPolicy();

                // setting minIndex
                timeSizeRollingPolicy->setMinIndex(1);

                // setting maxIndex
                timeSizeRollingPolicy->setMaxIndex(log_property.max_backup_index_value);

                // setting FileNamePattern
                std::ostringstream dateSizeFile;
                dateSizeFile << log_property.log_filename_value << "." << LOGGER_FILE_PATTERN;
                timeSizeRollingPolicy->setFileNamePattern(dateSizeFile.str());

                // setting Rotation Timing
                timeSizeRollingPolicy->setRotationTiming(log_property.rotation_timing_value);

                // setting Rotation Timing Value
                timeSizeRollingPolicy->setRotationTimingValue(log_property.rotation_timing_value_value);

                // setting MaxFileSize
                timeSizeRollingPolicy->setMaxFileSize(log_property.max_file_size_value);

                // create Rolling FileAppender
                log4cxx::rolling::RollingFileAppenderPtr dateSizeAppender =
                    new log4cxx::rolling::RollingFileAppender();

                // set layout
                dateSizeAppender->setLayout(layout);

                // set RollingPolicy (TriggeringPolicy also included RollingPolicy)
                dateSizeAppender->setRollingPolicy(timeSizeRollingPolicy);

                // set Log Filename
                try{
                    dateSizeAppender->setFile(log_property.log_filename_value, true, false, LOGGER_DEFAULT_BUFFER_SIZE, pool);
                }
                catch( const std::exception& e ){
                    std::stringstream    buf;
                    buf << "File Create Failed:" << log_property.log_filename_value;
                    logger_logrotate_utility::loglotation_utility_logic_error( 106, buf.str(), __FILE__, __LINE__);
                }

                // activate appender options
                dateSizeAppender->activateOptions(pool);

                // add time_and_size_based_appender to CategoryLogger
                //cat_logger->addAppender(dateSizeAppender);
                normalAppender = dateSizeAppender;

            }
        }    //switch

        log4cxx::LoggerPtr cat_logger = log4cxx::Logger::getLogger( log_category );
        if (0 == cat_logger) {
            logger_logrotate_utility::loglotation_utility_logic_error( 107, "getLogger Failed.", __FILE__, __LINE__);
        }

        AppenderPtr appender_pointer = cat_logger->getAppender( log_category );
        if( appender_pointer != NULL ) {
            cat_logger->removeAppender( appender_pointer );
        }
        normalAppender->setName( log_category );

        cat_logger->addAppender(normalAppender);

        //log level settting
        cat_logger->setLevel( log4cxx::Level::getInfo() );

    }
    catch (const std::exception& e) {
        std::ostringstream oss;
        oss <<  "Logger Reload Config Failed : " << e.what();
        logger_logrotate_utility::loglotation_utility_logic_error( 108, oss.str(), __FILE__, __LINE__);

    }

}

/*!
 * log rotate parameter check.
 *
 * @param   check object.
 * @param   appender property
 * @return  false faild
 */
bool logger_logrotate_utility::acccess_log_LogrotateParamCheck(
    accesslog_rotate_map_type& rotatedata,
    appender_property &access_log_property)
{
    std::string rotation_type = "";
    std::string max_backup_index = "";
    std::string max_file_size = "";
    std::string rotation_timing = "";
    std::string rotation_timing_value_key = "";
    
    try {

        for( accesslog_rotate_map_type_iterator itr = rotatedata.begin(); itr != rotatedata.end(); itr++ ) {
            if( itr->first == ACCESS_LOG_ROTATE_TYPE ) {
                rotation_type = itr->second;
            } else if ( itr->first == ACCESS_LOG_ROTATE_MAX_BACKUP_INDEX ) {
                max_backup_index = itr->second;
            } else if ( itr->first == ACCESS_LOG_ROTATE_MAX_FILESIZE ) {
                max_file_size = itr->second;
            } else if ( itr->first == ACCESS_LOG_ROTATE_ROTATION_TIMING ) {
                rotation_timing = itr->second;
            } else if ( itr->first == ACCESS_LOG_ROTATION_TIMING_VALUE ) {
                rotation_timing_value_key = itr->second;
            } else {
                logger_logrotate_utility::loglotation_utility_logic_error( 109, "access log rotation invalid argument.", __FILE__, __LINE__);
            }
        }

        access_log_property.rotation_value = logger_logrotate_utility::check_rotate_type(rotation_type);

        access_log_property.max_backup_index_value = logger_logrotate_utility::check_max_backup_index(max_backup_index);
        
        if(  access_log_property.rotation_value == LOG_ROT_SIZE ) {
            access_log_property.max_file_size_value = logger_logrotate_utility::check_max_file_size(max_file_size);
        } else if(  access_log_property.rotation_value == LOG_ROT_DATE ) {
            access_log_property.rotation_timing_value = logger_logrotate_utility::check_rotate_timing(rotation_timing );
            access_log_property.rotation_timing_value_value = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key,access_log_property.rotation_timing_value);
        } else {
            access_log_property.max_file_size_value = logger_logrotate_utility::check_max_file_size(max_file_size);
            access_log_property.rotation_timing_value = logger_logrotate_utility::check_rotate_timing(rotation_timing );
            access_log_property.rotation_timing_value_value = logger_logrotate_utility::check_rotate_timing_value(rotation_timing_value_key,access_log_property.rotation_timing_value);
        }

    } 
    catch(...) {
        return( false );
    }

    return( true );
}

/*!
 * error log out put.
 *
 * @param   msg no.
 * @param   err msg
 * @param   file name
 * @param   line no
 * @return  void
 */
void logger_logrotate_utility::loglotation_utility_logic_error( const unsigned int logno, const std::string& str, const char* file , const unsigned int line){
#if    defined(LOGGER_PROCESS_VSD)
    Logger::putLogError( l7vs::LOG_CAT_L7VSD_LOGGER, logno, str, file, line );
#elif defined(LOGGER_PROCESS_ADM)
    Logger::putLogError( l7vs::LOG_CAT_L7VSADM_LOGGER,logno, str, file, line );
#elif defined(LOGGER_PROCESS_SNM)
    Logger::putLogError( l7vs::LOG_CAT_SNMPAGENT_LOGGER, logno, str, file, line );
#else
    Logger::putLogError( l7vs::LOG_CAT_L7VSD_LOGGER, logno, str, file, line );
#endif

    throw std::logic_error( str );

}


