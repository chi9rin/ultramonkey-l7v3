/*
 * @file  ip_session_data_processor.h
 * @brief ip ip data processor header file.
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

#ifndef IP_SESSION_DATA_PROCESSOR_H
#define IP_SESSION_DATA_PROCESSOR_H

#include "ip_replication_data_processor.h"
#include "logger_enum.h"
#include <boost/asio.hpp>
#include <string>

#define HASH_TABLE_BITS 8
#define MAX_IP_SESSION_TBL_SIZE (1 << HASH_TABLE_BITS)

namespace l7vs
{

struct  ip_session_table_entry {
        time_t last_time;
        boost::asio::ip::tcp::endpoint rs_endpoint;
};


class ip_session_data_processor
{
public:
        //! loglevel get function object type
        typedef    boost::function< LOG_LEVEL_TAG(void) >
        getloglevel_func_type;
        //! log output function object type
        typedef    boost::function< void (const unsigned int, const std::string &, const char *, int) >
        logger_func_type;

public:
        ip_session_data_processor(
                int timeout,
                ip_replication_data_processor *replication_data_processor,
                getloglevel_func_type    ingetloglevel,
                logger_func_type inputLogFatal,
                logger_func_type inputLogError,
                logger_func_type inputLogWarn,
                logger_func_type inputLogInfo,
                logger_func_type inputLogDebug);

        virtual ~ip_session_data_processor();

        int get_endpoint_from_session_data(
                int ip_hash,
                boost::asio::ip::tcp::endpoint &endpoint);

        int write_session_data(
                int ip_hash,
                const boost::asio::ip::tcp::endpoint &endpoint,
                time_t now_time);

        int read_session_data_from_replication_area(ip_replication_data *replication_area);

protected:
        int timeout;
        boost::array<ip_session_table_entry, MAX_IP_SESSION_TBL_SIZE> session_table;
        ip_replication_data_processor *replication_data_processor;
        getloglevel_func_type getloglevel;          //!< get loglevel function object
        logger_func_type putLogFatal;               //!< fatal log output function object
        logger_func_type putLogError;               //!< error log output function object
        logger_func_type putLogWarn;                //!< warn log output function object
        logger_func_type putLogInfo;                //!< info log output function object
        logger_func_type putLogDebug;               //!< debug log output function object
private:
};

}
#endif // IP_SESSION_DATA_PROCESSOR_H
