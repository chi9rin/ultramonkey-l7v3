/*!
 * @file  logger_implement_access.cpp
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

#include "logger_implement_access.h"
#include "logger_logrotate_utility.h"


/*!
 * constructor initialize member variables.
 *
 * @param   access log file name
 */
l7vs::logger_implement_access::logger_implement_access(
                            const std::string &access_log_file_name)
                                                 :access_cnt         ( 1     ),
                                                  initialized        ( false ),
                                                  rotate_default_flag( false )
{
    access_log_file_name_ = access_log_file_name;
    aclog_args.clear();
}

/*!
 * initialze function.
 *
 * @param   default logrotation info use flag
 * @param   default logrotation info
 * @param   logrotation designation contents
 * @retrun  false failed
 */
bool l7vs::logger_implement_access::init(
              const bool                       rotate_default_flag,
              const appender_property&         access_log_default_property,
                    accesslog_rotate_map_type& rotatedata)
{

    using namespace log4cxx;
    if (initialized) return true;

    bool rtnFlg = true;

    try {

        this->rotate_default_flag = rotate_default_flag;
        rtnFlg = this->setAcLoggerConf(access_log_default_property,rotatedata);

    }
    catch (const std::exception& e) {
        return false;
    }

    return (rtnFlg);

}

/*!
 * Logger setting function.
 *
 * @param   default logrotation info use flag
 * @param   logrotation designation contents
 * @retrun  false failed
 */
bool l7vs::logger_implement_access::setAcLoggerConf(
        const appender_property&         access_log_default_property,
              accesslog_rotate_map_type& rotatedata)
{
    bool lotate_check_flag = true;
    aclog_args = rotatedata;

    access_log_property.log_filename_value = access_log_file_name_;
    
    if( this->rotate_default_flag == false ) {
        lotate_check_flag =
          logger_logrotate_utility::acccess_log_LogrotateParamCheck(
                                                        rotatedata,
                                                        access_log_property );
    } else {
        
        access_log_property.rotation_key                    
            = access_log_default_property.rotation_key;
        access_log_property.rotation_value                  
            = access_log_default_property.rotation_value;
        access_log_property.max_backup_index_key            
            = access_log_default_property.max_backup_index_key;
        access_log_property.max_backup_index_value          
            = access_log_default_property.max_backup_index_value;
        access_log_property.max_file_size_key               
            = access_log_default_property.max_file_size_key;
        access_log_property.max_file_size_value             
            = access_log_default_property.max_file_size_value;
        access_log_property.rotation_timing_key             
            = access_log_default_property.rotation_timing_key;
        access_log_property.rotation_timing_value           
            = access_log_default_property.rotation_timing_value;
        access_log_property.rotation_timing_value_key       
            = access_log_default_property.rotation_timing_value_key;
        access_log_property.rotation_timing_value_value     
            = access_log_default_property.rotation_timing_value_value;
        
    }

    if ( lotate_check_flag == true ) {
        logger_logrotate_utility::set_appender(
                                 access_log_property,
                                 LOGGER_ACCESS_LAYOUT,
                                 access_log_property.log_filename_value);
    }

    return(lotate_check_flag);

}

/*!
 * increase reffernce count.
 *
 * @retrun void
 */
void l7vs::logger_implement_access::addRef()
{
    access_cnt++;
}

/*!
 * decrease reffernce count.
 *
 * @retrun void
 */
void l7vs::logger_implement_access::releaseRef()
{
    access_cnt--;
}

/*!
 * decrease reffernce count.
 *
 * @param inequal check object
 * @return check result
 */
bool l7vs::logger_implement_access::operator<=(const int access_num )
{
    return( access_cnt <= access_num );
}

/*!
 * member variable data getter.
 *
 * @retrun member variable data
 */
std::string l7vs::logger_implement_access::getAcLogFileName()
{
    return( access_log_file_name_ ); 
}

/*!
 * logrotate data compare.
 *
 * @param check object
 * @retrun true logrotate data equal
 */
bool l7vs::logger_implement_access::checkRotateParameterComp(
                                       accesslog_rotate_map_type &rotatedata)
{
    bool comp_flg = true;
    accesslog_rotate_map_type_iterator itr_comp_in;
    accesslog_rotate_map_type_iterator itr_find;
    
    for( itr_comp_in  = rotatedata.begin();
         itr_comp_in != rotatedata.end()  ;
         itr_comp_in++ ){
        
        itr_find = aclog_args.find( itr_comp_in->first );

        if ( itr_find != aclog_args.end() ) {
        
            if( itr_comp_in->second != itr_find->second ) {
                comp_flg = false;
            }
            
        } else {
            comp_flg = false;
        }
        
        if( comp_flg == false ) {
            break;
        }
        
    }
    
    return(comp_flg);
}

/*!
 * member variable data getter.
 *
 * @retrun member variable data
 */
bool l7vs::logger_implement_access::is_rotate_default_flag()
{
    return(rotate_default_flag);
}

