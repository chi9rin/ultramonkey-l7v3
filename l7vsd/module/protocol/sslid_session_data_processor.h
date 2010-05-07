/*
 * @file  sslid_session_data_processor.h
 * @brief sslid session data processor header file.
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

#ifndef SSLID_SESSION_DATA_PROCESSOR_H
#define SSLID_SESSION_DATA_PROCESSOR_H

#include "sslid_replication_data_processor.h"
#include "logger_enum.h"
#include <boost/asio.hpp>
#include <string>

namespace l7vs
{

class sslid_session_data_processor
{
public:
        //! loglevel get function object type
        typedef    boost::function< LOG_LEVEL_TAG(void) >
        getloglevel_func_type;
        //! log output function object type
        typedef    boost::function< void (const unsigned int, const std::string &, const char *, int) >
        logger_func_type;

public:
        sslid_session_data_processor(
                int maxlist,
                int timeout,
                sslid_replication_data_processor *replication_data_processor,
                getloglevel_func_type    ingetloglevel,
                logger_func_type inputLogFatal,
                logger_func_type inputLogError,
                logger_func_type inputLogWarn,
                logger_func_type inputLogInfo,
                logger_func_type inputLogDebug);

        virtual ~sslid_session_data_processor();

        int get_endpoint_from_session_data(
                const std::string &session_id,
                boost::asio::ip::tcp::endpoint &endpoint);

        int write_session_data(
                const std::string &session_id,
                const boost::asio::ip::tcp::endpoint &endpoint,
                time_t now_time);

        int read_session_data_from_replication_area(sslid_replication_data *replication_area);

protected:
        int clear_expired_session_data();

protected:
        int maxlist;
        int timeout;
        std::map<std::string, boost::asio::ip::tcp::endpoint> session_endpoint_map;
        std::map<std::string, time_t> session_lasttime_map;
        std::multimap<time_t, std::string> lasttime_session_map;
        boost::mutex session_map_mutex;
        sslid_replication_data_processor *replication_data_processor;
        getloglevel_func_type     getloglevel;        //!< get loglevel function object
        logger_func_type putLogFatal;                //!< fatal log output function object
        logger_func_type putLogError;                //!< error log output function object
        logger_func_type putLogWarn;                //!< warn log output function object
        logger_func_type putLogInfo;                //!< info log output function object
        logger_func_type putLogDebug;            //!< debug log output function object
private:
};

}
#endif // SSLID_SESSION_DATA_PROCESSOR_H
