/*
 * @file  ip_replication_data_processor.h
 * @brief ip replication data processor header file.
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

#ifndef IP_REPLICATION_DATA_PROCESSOR_H
#define IP_REPLICATION_DATA_PROCESSOR_H

#include <boost/asio.hpp>
#include <boost/thread/condition.hpp>
#include <queue>
#include "logger_enum.h"

#define IP_MAXLENGTH (40)
#define IP_SERVICE_NUMBER (128)
#define IP_REPLICATION_MAXLENGTH (256)
#define MAX_IP_SESSION_TEMPLIST_SIZE (20000)

namespace l7vs
{

struct ip_replication_data_header {
        char virtualserver_ip[IP_MAXLENGTH];
        unsigned short virtualserver_port;
        size_t offset;
        size_t size;
};

struct ip_replication_data {
        char realserver_ip[IP_MAXLENGTH];
        unsigned short realserver_port;
        time_t last_time;
        int valid;
};

struct ip_replication_temp_data {
        char op_code;
        int ip_hash;
        time_t last_time;
        boost::asio::ip::tcp::endpoint rs_endpoint;
};

class ip_replication_data_processor
{
public:
        //! loglevel get function object type
        typedef    boost::function< LOG_LEVEL_TAG(void) >
        getloglevel_func_type;
        //! log output function object type
        typedef    boost::function< void (const unsigned int, const std::string &, const char *, int) >
        logger_func_type;

public:
        ip_replication_data_processor(char *ip_replication_area_begin,
                                      int ip_replication_area_size,
                                      const boost::asio::ip::tcp::endpoint &virtual_service_endpoint,
                                      getloglevel_func_type ingetloglevel,
                                      logger_func_type inputLogFatal,
                                      logger_func_type inputLogError,
                                      logger_func_type inputLogWarn,
                                      logger_func_type inputLogInfo,
                                      logger_func_type inputLogDebug);

        virtual ~ip_replication_data_processor();

        void put_into_temp_list(const ip_replication_temp_data &data);

        void write_replication_area();

        ip_replication_data *get_replication_area();

        void register_replication_area_lock(boost::function<void(void)> intable_lock);

        void register_replication_area_unlock(boost::function<void(void)> intable_unlock);

protected:
        int get_from_temp_list(ip_replication_temp_data &data);

protected:
        std::deque<ip_replication_temp_data> temp_list;
        boost::mutex temp_list_mutex;
        boost::condition temp_list_condition;
        ip_replication_data *replication_area;
        boost::function<void(void)> replication_area_lock;
        boost::function<void(void)> replication_area_unlock;
        boost::asio::ip::tcp::endpoint virtual_service_endpoint;
        getloglevel_func_type getloglevel;        //! get loglevel function object
        logger_func_type putLogFatal;             //! fatal log output function object
        logger_func_type putLogError;             //! error log output function object
        logger_func_type putLogWarn;              //! warn log output function object
        logger_func_type putLogInfo;              //! info log output function object
        logger_func_type putLogDebug;             //! debug log output function object
};

}

#endif // IP_REPLICATION_DATA_PROCESSOR_H
