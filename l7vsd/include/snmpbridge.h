/*!
 * @file  snmpbridge.h
 * @brief snmpagent connection module.
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
#ifndef __SNMPBRIDGE_H__
#define __SNMPBRIDGE_H__

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/array.hpp>
#include "l7vsd.h"
#include "l7ag_parameter.h"
#include "message.h"

#define NIC_DEFAULT            "eth0"
#define ADDR_DEFAULT        "127.0.0.1"
#define PORT_DEFAULT        60162
#define INTERVAL_DEFAULT    1000
#define READBUF_SIZE        2048


namespace l7vs
{

class    l7vsd;

//! @class snmpbridge
//! @brief
class    snmpbridge
{

protected:
        struct l7ag_parameter                            snmp_param;
        char                                            *send_buffer;
        size_t                                            send_buffer_size;
        boost::mutex                                    send_buffer_mutex;
        boost::array<char, READBUF_SIZE>                recv_buffer;
        boost::asio::io_service                        &snmp_io_service;
        boost::asio::ip::tcp::acceptor                    snmp_acceptor;
        boost::asio::ip::tcp::socket                    snmp_socket;
        bool                                            connection_state;

        typedef    std::map<LOG_CATEGORY_TAG, std::string>    snmp_logcategory_map_type;
        snmp_logcategory_map_type                        snmp_loglevel_map;

        typedef    std::map<std::string, LOG_LEVEL_TAG>    string_loglevel_map_type;
        string_loglevel_map_type                        levelstring_map;

        l7vsd                                            &vsd;

public:
        //! constractor
        //! @param[in]
        //! @param[in]
        snmpbridge(l7vsd &l7vsd_in, boost::asio::io_service &io_service_in) :
                send_buffer(NULL),
                send_buffer_size(0),
                snmp_io_service(io_service_in),
                snmp_acceptor(io_service_in),
                snmp_socket(io_service_in),
                connection_state(false),
                vsd(l7vsd_in) {
                snmp_loglevel_map.clear();
                levelstring_map.clear();
        }
        //! destractor
        ~snmpbridge() {}

        //! initilalize function
        //! @return
        int                initialize();

        //! finalize function
        void            finalize();

        //! trap send function
        //! @param[in]
        //! @return
        int                send_trap(const std::string &message);

        //! param reload function
        void            reload_config();

        //! change log level function
        //! @param[in]
        //! @param[in]
        //! @return
        int                change_loglevel(const LOG_CATEGORY_TAG snmp_log_category, const LOG_LEVEL_TAG loglevel);

        //! view all log level function
        //! @param[in]
        //! @return
        int                change_loglevel_allcategory(const LOG_LEVEL_TAG loglevel);

        //! send mibcollection
        //! @param[in]
        //! @return
        int                send_mibcollection(struct l7ag_mibrequest_message *payload);

        //! connection status function
        //! @return
        bool            get_connectionstate();

        //! loglevel getting function
        //! @param[in]
        //! @return
        LOG_LEVEL_TAG    get_loglevel(const LOG_CATEGORY_TAG snmp_log_category);

        //! loglevel getting function
        //! @param[out]
        //! @return
        void    get_loglevel_allcategory(std::map<LOG_CATEGORY_TAG, LOG_LEVEL_TAG>&    loglevelmap);

        //! accept callback function
        //! @param[in]
        //! @return
        void    handle_accept(const boost::system::error_code &error);

        //! receive callback function
        //! @param[in]
        //! @return
        void    handle_receive(const boost::system::error_code &error, size_t bytes_transferred);

        //! send callback function
        //! @param[in]
        //! @return
        void    handle_send(const boost::system::error_code &error, size_t bytes_transferred);

protected:
        int                send_message();
        int                load_config();
        void            load_loglevel();
};

}    //namespace l7vs

#endif //__SNMPBRIDGE_H__
