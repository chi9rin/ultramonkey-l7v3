/*
 *    @file    schedule_module_base.h
 *    @brief    shared object schedule module abstract class
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
#ifndef    SCHEDULE_MODULE_BASE_H
#define    SCHEDULE_MODULE_BASE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include "logger_enum.h"
#include "module_base.h"
#include "realserver.h"

namespace l7vs
{

//
//! @class schedule_module_base
//!    @brief    schedule module abstract base class.
//! @brief    all schedule module is extend this class.
class schedule_module_base : public module_base
{
public:
        //! realserver list iterator type
        typedef    std::list<l7vs::realserver>
        rslist_type;
        typedef    boost::function< rslist_type::iterator(void)>
        rslist_iterator_begin_func_type;
        typedef    boost::function< rslist_type::iterator(void)>
        rslist_iterator_end_func_type;
        typedef    boost::function< rslist_type::iterator(rslist_type::iterator)>
        rslist_iterator_next_func_type;
public:
        //!    constructor
        schedule_module_base(std::string in_modulename) : module_base(in_modulename) {};
        //! destructor
        virtual    ~schedule_module_base() {};
        //!    initialize function
        virtual    void    initialize() = 0;
        //! handle schedule called then schedule function for TCP/IP endpoint
        //! @param[in]    thread id
        //! @param[in]    list iterator first function object
        //!    @param[in]    list iterator last function object
        //!    @param[in]    list iterator next function object
        //! @param[out]    scheduled TCP/IP endpoint
        virtual    void    handle_schedule(
                boost::thread::id,
                rslist_iterator_begin_func_type,
                rslist_iterator_end_func_type,
                rslist_iterator_next_func_type,
                boost::asio::ip::tcp::endpoint &) = 0;

        //! handle schedule calls then schedule function for UDP endpoint
        //! @param[in]    thread id
        //! @param[in]    list iterator first function object
        //!    @param[in]    list iterator last function object
        //!    @param[in]    list iterator next function object
        //! @param[out]    scheduled UDP endpoint
        virtual    void    handle_schedule(
                boost::thread::id,
                rslist_iterator_begin_func_type,
                rslist_iterator_end_func_type,
                rslist_iterator_next_func_type,
                boost::asio::ip::udp::endpoint &) = 0;

};

}    //namespace l7vs

#endif //SCHEDULE_MODULE_BASE_H
