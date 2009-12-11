/*!
 * @file  logger_access_manager.cpp
 * @brief logger module implementation class For access log instance manager.
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

#include "logger_access_manager.h"

logger_access_manager &logger_access_manager::getInstance()
{
    static logger_access_manager log_manager;
    return( log_manager );
}


logger_implement_access *logger_access_manager::find_logger_implement_access(
    const std::string &aclogFilename, 
    std::map< std::string , std::string > rotate, 
    error_code& err)
{
    
/*
    rd_scoped_lock scope_lock( log_ac_flag_mutex );
  
    logger_implement_access *l_logdif = NULL;
  
    l_logac = logimp_access_map.find( aclogFilename );
    if( l_logac == NULL ) {
        l_logac = new logger_implement_access(aclogFilename);
        l_logac->init(acLogFileName);
        logimp_access_map.insert( aclogFilename , l_logac );
    } else {
      l_logac->addRef();
    }
*/

    logger_implement_access l_logdif = new logger_implement_access();

    return( l_logac );


}

bool logger_access_manager::erase_logger_implement_access(
    const std::string &aclogFilename, 
    error_code& err)
{
/*
    logger_implement_access *l_logac = NULL;
    {
        rd_scoped_lock scope_lock( log_ac_flag_mutex );

        l_logac = logimp_access_map.find( aclogFilename );

        if( !l_logac ) {
            return( false );
        } else {
            l_logac->releaseRef();
        }
    }

    if( (*l_logac) <= 0 ) {
        rw_scoped_lock scope_lock( log_ac_flag_mutex );
        logimp_access_map.erase( aclogFilename );
        delete l_logac;
    }
*/
    return( true );
  
}



bool access_log_rotate_loadConf()
{
    return( true );
}

std::string get_rotate_default_verbose_ displayed contents()
{
    return( rotate_default_verbose_displayed_contents );
}

