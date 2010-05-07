/*
 *    @file    l7vs_realserver.h
 *    @brief    realserver data prototype
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
#ifndef    REALSERVER_H
#define    REALSERVER_H

#include    <boost/thread.hpp>
#include    <boost/shared_ptr.hpp>
#include    "realserver_element.h"

namespace l7vs
{

class    realserver : public realserver_element
{
public:
        typedef    boost::shared_ptr<boost::mutex>        mutex_ptr;
protected:
        mutex_ptr            active_mutex_ptr;
        mutex_ptr            inact_mutex_ptr;
public:
        unsigned long long    send_byte;

        realserver() : send_byte(0LL) {
                active_mutex_ptr = mutex_ptr(new boost::mutex);
                inact_mutex_ptr = mutex_ptr(new boost::mutex);
        }
        realserver(const realserver &in) : realserver_element(in),
                send_byte(in.send_byte) {
                active_mutex_ptr = mutex_ptr(new boost::mutex);
                inact_mutex_ptr = mutex_ptr(new boost::mutex);
        }

        realserver &operator=(const realserver &rs) {
                realserver_element::operator= (rs);
                send_byte = rs.send_byte;
                return *this;
        }

        ~realserver() {}

        friend    bool    operator==(const realserver &rs1, const realserver &rs2) {
                realserver &rselem1 = const_cast<realserver &>(rs1);
                realserver &rselem2 = const_cast<realserver &>(rs2);
                return    rselem1.tcp_endpoint == rselem2.tcp_endpoint &&
                          rselem1.udp_endpoint == rselem2.udp_endpoint &&
                          rselem1.weight == rselem2.weight;
        }

        friend    bool    operator!=(const realserver &rs1, const realserver &rs2) {
                realserver &rselem1 = const_cast<realserver &>(rs1);
                realserver &rselem2 = const_cast<realserver &>(rs2);
                return  rselem1.tcp_endpoint != rselem2.tcp_endpoint ||
                        rselem1.udp_endpoint != rselem2.udp_endpoint ||
                        rselem1.weight != rselem2.weight;
        }

        friend    bool    operator<(const realserver &rs1, const realserver &rs2) {
                realserver &rselem1 = const_cast<realserver &>(rs1);
                realserver &rselem2 = const_cast<realserver &>(rs2);
                if (rselem1.tcp_endpoint < rselem2.tcp_endpoint) return true;
                if (rselem1.tcp_endpoint != rselem2.tcp_endpoint) return false;
                return rselem1.weight < rselem2.weight;
        }

        void    increment_active() {
                boost::mutex::scoped_lock lock(*active_mutex_ptr);

                nactive++;
                if (nactive == INT_MAX) {
                        nactive = 0;
                }
        }

        void    decrement_active() {
                boost::mutex::scoped_lock lock(*active_mutex_ptr);

                if (nactive > 0) {
                        nactive--;
                }
        }

        void    increment_inact() {
                boost::mutex::scoped_lock lock(*inact_mutex_ptr);

                ninact++;
                if (ninact == INT_MAX) {
                        ninact = 0;
                }
        }

protected:
        // nullify function, only effective on realserver_element
        void    set_active(const int in_active) {}
        void    set_inact(const int in_inact) {}
};


}    //namespace l7vs
#endif    //REALSERVER_H
