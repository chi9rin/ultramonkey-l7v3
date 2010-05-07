/*
 *    @file    schedule_module_wrr.cpp
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
#include "schedule_module_wrr.h"
#include <boost/format.hpp>

#include "utility.h"

namespace l7vs
{

//!    constractor
schedule_module_weighted_round_robin::schedule_module_weighted_round_robin() : schedule_module_base("wrr")
{
}

//! destractor
schedule_module_weighted_round_robin::~schedule_module_weighted_round_robin() {}

//!    initialize function
void    schedule_module_weighted_round_robin::initialize()
{
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(300000, "Function in : schedule_module_weighted_round_robin::initialize", __FILE__, __LINE__);
                        }
                }
        }

        boost::asio::ip::tcp::endpoint    tcp_local_endpoint ;
        boost::asio::ip::udp::endpoint    udp_local_endpoint ;

        tcp_endpoint = tcp_local_endpoint ;
        udp_endpoint = udp_local_endpoint ;

        vs_weights.currentWeight = 0;
        vs_weights.maxWeight = 0;
        vs_weights.gcd = 0;

        if (likely(!putLogInfo.empty())) {
                putLogInfo(300000, "Saved endpoint, weight and gcd were initialized.", __FILE__, __LINE__);
        }

        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(300001, "Function out : schedule_module_weighted_round_robin::initialize", __FILE__, __LINE__);
                        }
                }
        }
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool    schedule_module_weighted_round_robin::is_tcp()
{
        return true;
}

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool    schedule_module_weighted_round_robin::is_udp()
{
        return true;
}

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]    thread id
//! @param[in]    list iterator first function object
//!    @param[in]    list iterator last function object
//!    @param[in]    list iterator next function object
//! @param[out]    scheduled TCP/IP endpoint
void    schedule_module_weighted_round_robin::handle_schedule(
        boost::thread::id                    thread_id,
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next,
        boost::asio::ip::tcp::endpoint        &outendpoint)
{
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(300002, "Function in : schedule_module_weighted_round_robin::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }

        boost::asio::ip::tcp::endpoint    tcp_local_endpoint ;
        rslist_type::iterator            itr;
        std::string    buf;
        int            loop;

        //! set clear data as NULL
        outendpoint = tcp_local_endpoint;

        if (unlikely(inlist_begin.empty() || inlist_end.empty() || inlist_next.empty())) {
                //! invalid iterator function
                if (likely(!putLogFatal.empty())) {
                        putLogFatal(300000, "Iterator function is empty.", __FILE__, __LINE__);
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
                                        putLogDebug(300003, buf, __FILE__, __LINE__);
                                }
                        }
                }
        }
        //! Debug log END

        if (unlikely(-1 == sched_wrr_service_init(inlist_begin, inlist_end, inlist_next))) {
                //! init error( no data )
                if (likely(!putLogError.empty())) {
                        putLogError(300000, "There is no realserver on list.", __FILE__, __LINE__);
                }
                goto END;
        }

        //! first time
        if (tcp_local_endpoint == tcp_endpoint) {
                vs_weights.currentWeight = vs_weights.maxWeight;

                itr = inlist_begin();
        } else {
                if (vs_weights.currentWeight > vs_weights.maxWeight) {
                        vs_weights.currentWeight = vs_weights.maxWeight;
                }

                //! Debug log
                if (likely(!getloglevel.empty())) {
                        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                                if (likely(!putLogDebug.empty())) {
                                        buf = boost::io::str(boost::format("previous endpoint : %s:%d currentweight(%d)")
                                                             % tcp_endpoint.address()
                                                             % tcp_endpoint.port()
                                                             % vs_weights.currentWeight);
                                        putLogDebug(300004, buf, __FILE__, __LINE__);
                                }
                        }
                }
                //! Debug log END

                for (itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr)) {
                        if (itr->weight > 0) {
                                //! prev endpoint
                                if (tcp_endpoint == itr->tcp_endpoint) {
                                        itr = inlist_next(itr);
                                        if (itr == inlist_end()) {
                                                itr = inlist_begin();
                                                vs_weights.currentWeight -= vs_weights.gcd;
                                                if (vs_weights.currentWeight <= 0) {
                                                        vs_weights.currentWeight = vs_weights.maxWeight;
                                                }
                                        }
                                        break;
                                }
                        }
                }
                if (itr == inlist_end()) {
                        itr = inlist_begin();
                }
        }

        for (; ;) {
                for (; itr != inlist_end(); itr = inlist_next(itr)) {

                        //! Debug log
                        if (likely(!getloglevel.empty())) {
                                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                                        if (likely(!putLogDebug.empty())) {
                                                buf = boost::io::str(boost::format("itr : %s:%d weight(%d) currentweight(%d)")
                                                                     % itr->tcp_endpoint.address()
                                                                     % itr->tcp_endpoint.port()
                                                                     % itr->weight
                                                                     % vs_weights.currentWeight);
                                                putLogDebug(300005, buf, __FILE__, __LINE__);
                                        }
                                }
                        }
                        //! Debug log END

                        if (itr->weight >= vs_weights.currentWeight) {
                                //! set found data
                                tcp_endpoint = outendpoint = itr->tcp_endpoint;
                                goto END ;
                        }
                }
                if (itr == inlist_end()) {
                        itr = inlist_begin();
                        vs_weights.currentWeight -= vs_weights.gcd;
                        if (vs_weights.currentWeight <= 0) {
                                vs_weights.currentWeight = vs_weights.maxWeight;
                        }
                }
        }

END:
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(300006, "Function out : schedule_module_weighted_round_robin::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }
}

//! handle schedule calles then schedule function for UDP endpoint
//! @param[in]    thread id
//! @param[in]    list iterator first function object
//!    @param[in]    list iterator last function object
//!    @param[in]    list iterator next function object
//! @param[out]    scheduled UDP endpoint
void    schedule_module_weighted_round_robin::handle_schedule(
        boost::thread::id                    thread_id,
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next,
        boost::asio::ip::udp::endpoint        &outendpoint)
{
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(300007, "Function in : schedule_module_weighted_round_robin::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }

        boost::asio::ip::udp::endpoint    udp_local_endpoint ;
        rslist_type::iterator            itr;
        std::string    buf;
        int            loop;

        //! set clear data as NULL
        outendpoint = udp_local_endpoint;

        if (unlikely(inlist_begin.empty() || inlist_end.empty() || inlist_next.empty())) {
                //! invalid iterator function
                if (likely(!putLogFatal.empty())) {
                        putLogFatal(300001, "Iterator function is empty.", __FILE__, __LINE__);
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
                                        putLogDebug(300008, buf, __FILE__, __LINE__);
                                }
                        }
                }
        }

        //! Debug log END
        if (unlikely(-1 == sched_wrr_service_init(inlist_begin, inlist_end, inlist_next))) {
                //! init error( no data )
                if (likely(!putLogError.empty())) {
                        putLogError(300001, "There is no realserver on list.", __FILE__, __LINE__);
                }
                goto END;
        }

        //! first time
        if (udp_local_endpoint == udp_endpoint) {
                vs_weights.currentWeight = vs_weights.maxWeight;

                itr = inlist_begin();
        } else {
                if (vs_weights.currentWeight > vs_weights.maxWeight) {
                        vs_weights.currentWeight = vs_weights.maxWeight;
                }

                //! Debug log
                if (likely(!getloglevel.empty())) {
                        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                                if (likely(!putLogDebug.empty())) {
                                        buf = boost::io::str(boost::format("previous endpoint : %s:%d currentweight(%d)")
                                                             % udp_endpoint.address()
                                                             % udp_endpoint.port()
                                                             % vs_weights.currentWeight);
                                        putLogDebug(300009, buf, __FILE__, __LINE__);
                                }
                        }
                }
                //! Debug log END

                for (itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr)) {
                        if (itr->weight > 0) {
                                //! prev endpoint
                                if (udp_endpoint == itr->udp_endpoint) {
                                        itr = inlist_next(itr);
                                        if (itr == inlist_end()) {
                                                itr = inlist_begin();
                                                vs_weights.currentWeight -= vs_weights.gcd;
                                                if (vs_weights.currentWeight <= 0) {
                                                        vs_weights.currentWeight = vs_weights.maxWeight;
                                                }
                                        }
                                        break;
                                }
                        }
                }
                if (itr == inlist_end()) {
                        itr = inlist_begin();
                }
        }

        for (; ;) {
                for (; itr != inlist_end(); itr = inlist_next(itr)) {

                        //! Debug log
                        if (likely(!getloglevel.empty())) {
                                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                                        if (likely(!putLogDebug.empty())) {
                                                buf = boost::io::str(boost::format("itr : %s:%d weight(%d) currentweight(%d)")
                                                                     % itr->udp_endpoint.address()
                                                                     % itr->udp_endpoint.port()
                                                                     % itr->weight
                                                                     % vs_weights.currentWeight);
                                                putLogDebug(300010, buf, __FILE__, __LINE__);
                                        }
                                }
                        }
                        //! Debug log END

                        if (itr->weight >= vs_weights.currentWeight) {
                                //! set found data
                                udp_endpoint = outendpoint = itr->udp_endpoint;
                                goto END ;
                        }
                }
                if (itr == inlist_end()) {
                        itr = inlist_begin();
                        vs_weights.currentWeight -= vs_weights.gcd;
                        if (vs_weights.currentWeight <= 0) {
                                vs_weights.currentWeight = vs_weights.maxWeight;
                        }
                }
        }

END:
        if (likely(!getloglevel.empty())) {
                if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                        if (likely(!putLogDebug.empty())) {
                                putLogDebug(300011, "Function out : schedule_module_weighted_round_robin::handle_schedule", __FILE__, __LINE__);
                        }
                }
        }
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void    schedule_module_weighted_round_robin::replication_interrupt() {}


int        schedule_module_weighted_round_robin::sched_wrr_service_init(
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next)
{

        vs_weights.maxWeight = sched_wrr_getMaxWeight(inlist_begin, inlist_end, inlist_next);
        if (unlikely(vs_weights.maxWeight <= 0)) {
                return -1;
        }

        vs_weights.gcd = sched_wrr_getGCD(inlist_begin, inlist_end, inlist_next);
        if (unlikely(vs_weights.gcd <= 0)) {
                return -1;
        }
        return 0;
}


int        schedule_module_weighted_round_robin::sched_wrr_getMaxWeight(
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next)
{
        int                    weight = 0;
        rslist_type::iterator    itr;

        for (itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr)) {
                //! keep max weight of list
                if (itr->weight > weight) {
                        weight = itr->weight;
                }
        }

        return (weight);
}

int        schedule_module_weighted_round_robin::sched_wrr_gcd(int a, int b)
{
        if (a == 0 || b == 0) {
                return 0;
        }

        while (a != b) {
                if (a > b) {
                        a -= b;
                } else {
                        b -= a;
                }
        }

        return (a);
}

int        schedule_module_weighted_round_robin::sched_wrr_getGCD(
        rslist_iterator_begin_func_type        inlist_begin,
        rslist_iterator_end_func_type        inlist_end,
        rslist_iterator_next_func_type        inlist_next)
{
        int                    current_gcd = 1;
        rslist_type::iterator    itr;

        for (itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr)) {
                //! keep first data of list
                if (itr->weight > 0) {
                        current_gcd = itr->weight;
                        break;
                }
        }
        if (unlikely(itr == inlist_end())) {
                //! no data
                return -1;
        }

        for (; itr != inlist_end(); itr = inlist_next(itr)) {
                if (itr->weight > 0) {
                        current_gcd = sched_wrr_gcd(current_gcd, itr->weight);
                }
        }

        return (current_gcd);
}

}    //namespace l7vs


extern "C" l7vs::schedule_module_base*
create_module()
{
        return dynamic_cast<l7vs::schedule_module_base *>(new l7vs::schedule_module_weighted_round_robin());
}

extern "C" void
destroy_module(l7vs::schedule_module_base *in)
{
        delete in;
}
