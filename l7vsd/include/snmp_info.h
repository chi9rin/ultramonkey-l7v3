/*!
 * @file  snmp_info.h
 * @brief snmp function common header.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2010  NTT COMWARE Corporation.
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
 */
#ifndef __SNMP_INFO_H__
#define __SNMP_INFO_H__


#include<boost/asio.hpp>
#include <time.h>
#include "logger.h"

namespace l7vs
{

struct snmp_info {
        enum SNMP_OPTION_FLAG_TAG {
                SNMP_ENABLE_OPTION_FLAG             = 0x0001,
                SNMP_LOGTRAP_OPTION_FLAG            = 0x0002,
                SNMP_LOGTRAP_LEVEL_OPTION_FLAG      = 0x0004,
                SNMP_INTERVAL_OPTION_FLAG           = 0x0008,
                SNMP_REFRESH_OPTION_FLAG            = 0x0010,
                SNMP_REFRESH_ALL_OPTION_FLAG        = 0x0020,
                SNMP_TCP_SERVICE_OPTION_FLAG        = 0x0040,
                SNMP_PROTOCOL_MODULE_OPTION_FLAG    = 0x0080
        };
        unsigned int    enabled;
        unsigned int    logtrap_enabled;
        LOG_LEVEL_TAG   logtrap_level;
        unsigned int    interval;
        time_t          start_date;
        time_t          request_last_date;
        time_t          trap_last_date;
        unsigned long long    snmp_get_requests;
        unsigned long long    snmp_set_requests;
        unsigned long long    snmp_trap_count;
        boost::asio::ip::tcp::endpoint  vs_endpoint;
        std::string           protocol;
        int                   option_set_flag;

        snmp_info() : enabled(0), logtrap_enabled(0), logtrap_level(LOG_LV_NONE),
                interval(0), start_date(0),
                request_last_date(0), trap_last_date(0),
                snmp_get_requests(0ull), snmp_set_requests(0ull), snmp_trap_count(0ull), option_set_flag(0) {}
        snmp_info(const snmp_info &in)
                :   enabled(in.enabled),
                    logtrap_enabled(in.logtrap_enabled),
                    logtrap_level(in.logtrap_level),
                    interval(in.interval),
                    start_date(in.start_date),
                    request_last_date(in.request_last_date),
                    trap_last_date(in.trap_last_date),
                    snmp_get_requests(in.snmp_get_requests),
                    snmp_set_requests(in.snmp_set_requests),
                    snmp_trap_count(in.snmp_trap_count),
                    vs_endpoint(in.vs_endpoint),
                    protocol(in.protocol),
                    option_set_flag(in.option_set_flag) {}
        snmp_info &operator=(const snmp_info &in) {
                enabled = in.enabled;
                logtrap_enabled = in.logtrap_enabled;
                logtrap_level = in.logtrap_level;
                interval = in.interval;
                start_date = in.start_date;
                request_last_date = in.request_last_date;
                trap_last_date = in.trap_last_date;
                snmp_get_requests = in.snmp_get_requests;
                snmp_set_requests = in.snmp_set_requests;
                snmp_trap_count = in.snmp_trap_count;
                vs_endpoint = in.vs_endpoint;
                protocol = in.protocol;
                option_set_flag = in.option_set_flag;

                return *this;
        }
        template <typename Elem, typename Traits>
        friend std::basic_ostream<Elem, Traits>& operator<<(
                std::basic_ostream<Elem, Traits>& os,
                const snmp_info &info) {
                static const int MAX_TIME_LEN = 20;
                char start_date[MAX_TIME_LEN] = {0};
                char request_last_date[MAX_TIME_LEN] = {0};
                char trap_last_date[MAX_TIME_LEN] = {0};
                strftime(start_date, sizeof(start_date),
                         "%Y-%m-%d %H:%M:%S", gmtime(&(info.start_date)));
                strftime(request_last_date, sizeof(request_last_date),
                         "%Y-%m-%d %H:%M:%S", gmtime(&(info.request_last_date)));
                strftime(trap_last_date, sizeof(trap_last_date),
                         "%Y-%m-%d %H:%M:%S", gmtime(&(info.trap_last_date)));

                os << "{";
                os << boost::format("flag=%d; "
                                    "logtrap=%d; "
                                    "logtrap_level=%s; "
                                    "interval=%d; "
                                    "start_date=%s; "
                                    "request_last_date = %s; "
                                    "trap_last_date = %s ;"
                                    "snmp_get_requests = %d ;"
                                    "snmp_set_requests = %d ;"
                                    "snmp_trap_count= %d; "
                                    "vs_endpoint= %s:%d; "
                                    "protocol = %s; "
                                    "option_set_flag = %x }")
                   % info.enabled
                   % info.logtrap_enabled
                   % info.logtrap_level
                   % info.interval
                   % (info.start_date == 0 ? "none" : start_date)
                   % (info.request_last_date == 0 ? "none" : request_last_date)
                   % (info.trap_last_date == 0 ? "none" : trap_last_date)
                   % info.snmp_get_requests
                   % info.snmp_set_requests
                   % info.snmp_trap_count
                   % info.vs_endpoint.address().to_string()
                   % info.vs_endpoint.port()
                   % info.protocol
                   % info.option_set_flag;
                return os;

        }
private:
        friend class    boost::serialization::access;
        template <class Archive > void serialize(Archive &ar, const unsigned int version) {
                ar &enabled;
                ar &logtrap_enabled;
                ar &logtrap_level;
                ar &interval;
                ar &start_date;
                ar &request_last_date;
                ar &trap_last_date;
                ar &snmp_get_requests;
                ar &snmp_set_requests;
                ar &snmp_trap_count;
                ar &vs_endpoint;
                ar &protocol;
                ar &option_set_flag;

        }

};

struct rsdata {
        int    index;
        int    vs_index;
        int    address_type;
        char   address[128];
        int    port;
        int    forward;
        int    weigth;
        int    active_conn;
        int    inactive_conn;
};

}

#endif //__SNMP_INFO_H__

