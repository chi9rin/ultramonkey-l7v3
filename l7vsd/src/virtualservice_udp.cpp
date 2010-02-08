/*!
 *    @file    virtualservice_udp.cpp
 *    @brief    VirtualService class implementations for UDP
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

#include <boost/date_time/posix_time/posix_time.hpp>

#include "virtualservice.h"

// imprementation for virtualservice_udp
/*!
 * virtualservice_udp class constructor.
 */
l7vs::virtualservice_udp::virtualservice_udp(    const l7vs::l7vsd& invsd,
                                                const l7vs::replication& inrep,
                                                const l7vs::virtualservice_element& inelement) : 
                                                    l7vs::virtualservice_base( invsd, inrep, inelement ){}
l7vs::virtualservice_udp::~virtualservice_udp(){}

void    l7vs::virtualservice_udp::handle_replication_interrupt( const boost::system::error_code& in ){
}
void    l7vs::virtualservice_udp::read_replicationdata(){
}

void    l7vs::virtualservice_udp::initialize( l7vs::error_code& err ){
    err.setter( true, "" );
}
void    l7vs::virtualservice_udp::finalize( l7vs::error_code& err ){
    err.setter( true, "" );
}

bool    l7vs::virtualservice_udp::operator==( const l7vs::virtualservice_base& in ){
    return true;
}
bool    l7vs::virtualservice_udp::operator!=( const l7vs::virtualservice_base& in ){
    return true;
}

void    l7vs::virtualservice_udp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    err.setter( true, "" );
}
void    l7vs::virtualservice_udp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    err.setter( true, "" );
}

void    l7vs::virtualservice_udp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    err.setter( true, "" );
}
void    l7vs::virtualservice_udp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    err.setter( true, "" );
}
void    l7vs::virtualservice_udp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
    err.setter( true, "" );
}

void    l7vs::virtualservice_udp::run(){}
void    l7vs::virtualservice_udp::stop(){}

void    l7vs::virtualservice_udp::release_session( const tcp_session* session_ptr ){}
