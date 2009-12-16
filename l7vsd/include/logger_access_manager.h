/*!
 * @file  logger_access_manager.h
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

#ifndef LOGGER_ACCESS_MANAGER_H
#define LOGGER_ACCESS_MANAGER_H

#include <sched.h>
#include <net/if.h>
#include <unistd.h>
#include <sched.h>
#include <error.h>
#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "appender_property.h"
#include "error_code.h"
#include "logger_implement_access.h"
#include "wrlock.h"

namespace l7vs{

class logger_access_manager {

public:
    typedef std::map< std::string , logger_implement_access* > 
                    logimp_access_map_type;

    typedef std::map< std::string , std::string > 
                    accesslog_rotate_default_map_type;

    typedef std::map< std::string , std::string >::iterator 
                    accesslog_rotate_default_map_type_iterator;

    static logger_access_manager &getInstance();

    logger_implement_access *find_logger_implement_access(
                                const std::string &access_log_filename, 
                                std::map< std::string , std::string > rotatedata,
                                l7vs::error_code& err);

    void erase_logger_implement_access(
                                const std::string &access_log_filename, 
                                l7vs::error_code& err);

    bool access_log_logrotate_parameter_check(
                    const std::map<std::string,std::string>& rotatedata);

    void access_log_rotate_loadConf();

    std::string get_rotate_default_verbose_displayed_contents();

protected:


    wr_mutex log_ac_flag_mutex;

    logimp_access_map_type logimp_access_map;

    appender_property access_log_default_property;

    accesslog_rotate_default_map_type access_log_default_data;

    bool rotate_default_load_flag;

    std::string rotate_default_verbose_displayed_contents;

    logger_access_manager();

    virtual ~logger_access_manager();


};

};

#endif // LOGGER_ACCESS_MANAGER_H
