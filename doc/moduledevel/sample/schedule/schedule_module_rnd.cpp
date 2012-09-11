/*
 *    @file    schedule_module_rnd.cpp
 *    @brief    shared object schedule module class
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
#include "schedule_module_rnd.h"
#include <boost/format.hpp>

#include "utility.h"

namespace l7vs
{

//!    constructor
schedule_module_random::schedule_module_random() : schedule_module_base("rnd")
{
}

//! destructor
schedule_module_random::~schedule_module_random() {}

//!    initialize function
void    schedule_module_random::initialize()
{
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(200000, "Function in : schedule_module_random::initialize", __FILE__, __LINE__);
                        }
                }
        }

        boost::asio::ip::tcp::endpoint    tcp_local_endpoint ;
        boost::asio::ip::udp::endpoint    udp_local_endpoint ;

        tcp_endpoint = tcp_local_endpoint ;
        udp_endpoint = udp_local_endpoint ;

        if (likely(!putLogInfo.empty())) {
                putLogInfo(200000, "Saved endpoint was initialized.", __FILE__, __LINE__);
        }

        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(200001, "Function out : schedule_module_random::initialize", __FILE__, __LINE__);
                        }
                }
        }
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool    schedule_module_random::is_tcp()
{
        return true;
}

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool    schedule_module_random::is_udp()
{
        return true;
}

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]    thread id
//! @param[in]    list iterator first function object
//!    @param[in]    list iterator last function object
//!    @param[in]    list iterator next function object
//! @param[out]    scheduled TCP/IP endpoint
void    schedule_module_random::handle_schedule(
        boost::thread::id                    thread_id,
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next,
        boost::asio::ip::tcp::endpoint        &outendpoint)
{
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(200002, "Function in : schedule_module_random::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }

        boost::asio::ip::tcp::endpoint    tcp_local_endpoint ;
        rslist_type::iterator            itr;
        std::string    buf;
        int            loop;
        std::vector<boost::asio::ip::tcp::endpoint> tmp_list;

        //! set clear data as NULL
        outendpoint = tcp_local_endpoint;

        if (unlikely(inlist_begin.empty() || inlist_end.empty() || inlist_next.empty())) {
                //! invalid iterator function
                if (likely(!putLogFatal.empty())) {
                        putLogFatal(200000, "Iterator function is empty.", __FILE__, __LINE__);
                }
                goto END;
        }

        //! Debug log
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                for (loop = 1, itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr), loop++) {
                                        buf = boost::io::str(boost::format("realserver[%d] : %s:%d weight(%d)")
                                                             % loop
                                                             % itr->tcp_endpoint.address()
                                                             % itr->tcp_endpoint.port()
                                                             % itr->weight);
                                        putLogDebug(200003, buf, __FILE__, __LINE__);
                                }
                        }
                }
        }
        //! Debug log END

        for (itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr)) {
                // pure random
                /*
                if (itr->weight > 0) {
                        tmp_list.push_back(itr->tcp_endpoint);
                }
                */
                // rs1 weight=1 ,rs2 weight=2
                // hit % => rs1:33% rs2:66%
                for(int i=0; i < itr->weight; i++){
                        tmp_list.push_back(itr->tcp_endpoint);
                }
        }

        if (unlikely(tmp_list.empty())) {
                //! no data
                if (likely(!putLogError.empty())) {
                        putLogError(200000, "There is no realserver on list.", __FILE__, __LINE__);
                }
                goto END;
        }
        //! set endpoind
        outendpoint = tmp_list.at(rand() % tmp_list.size());

END:
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(200006, "Function out : schedule_module_random::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }
}

//! handle schedule calls then schedule function for UDP endpoint
//! @param[in]    thread id
//! @param[in]    list iterator first function object
//!    @param[in]    list iterator last function object
//!    @param[in]    list iterator next function object
//! @param[out]    scheduled UDP endpoint
void    schedule_module_random::handle_schedule(
        boost::thread::id                    thread_id,
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next,
        boost::asio::ip::udp::endpoint        &outendpoint)
{
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(200007, "Function in : schedule_module_random::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }

        boost::asio::ip::udp::endpoint    udp_local_endpoint ;
        rslist_type::iterator            itr;
        std::string    buf;
        int            loop;
        std::vector<boost::asio::ip::udp::endpoint> tmp_list;

        //! set clear data as NULL
        outendpoint = udp_local_endpoint;

        if (unlikely(inlist_begin.empty() || inlist_end.empty() || inlist_next.empty())) {
                //! invalid iterator function
                if (likely(!putLogFatal.empty())) {
                        putLogFatal(200001, "Iterator function is empty.", __FILE__, __LINE__);
                }
                goto END;
        }

        //! Debug log
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                for (loop = 1, itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr), loop++) {
                                        buf = boost::io::str(boost::format("realserver[%d] : %s:%d weight(%d)")
                                                             % loop
                                                             % itr->udp_endpoint.address()
                                                             % itr->udp_endpoint.port()
                                                             % itr->weight);
                                        putLogDebug(200008, buf, __FILE__, __LINE__);
                                }
                        }
                }
        }
        //! Debug log END

        for (itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr)) {
                // pure random
                /*
                if (itr->weight > 0) {
                        tmp_list.push_back(itr->udp_endpoint);
                }
                */
                // rs1 weight=1 ,rs2 weight=2
                // hit % => rs1:33% rs2:66%
                for(int i=0; i < itr->weight; i++){
                        tmp_list.push_back(itr->udp_endpoint);
                }
        }
        if (unlikely(tmp_list.empty())) {
                //! no data
                if (likely(!putLogError.empty())) {
                        putLogError(200000, "There is no realserver on list.", __FILE__, __LINE__);
                }
                goto END;
        }

        //! set endpoind
        outendpoint = tmp_list.at(rand() % tmp_list.size());

END:
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(200011, "Function out : schedule_module_random::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }
}

//! replication interval interrupt
//! timer thread call this function. from virtualservice.
void    schedule_module_random::replication_interrupt() {}

}    //namespace l7vs

extern "C" l7vs::schedule_module_base*
create_module()
{
        return dynamic_cast<l7vs::schedule_module_base *>(new l7vs::schedule_module_random());
}

extern "C" void
destroy_module(l7vs::schedule_module_base *in)
{
        delete in;
}
