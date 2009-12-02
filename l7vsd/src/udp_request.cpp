/*!
 *	@file	udp_request.cpp
 *	@brief	udp request class
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

#include "udp_request.h"
#include "virtualservice.h"
#include "udp_session.h"

namespace l7vs{

	udp_request::udp_request(l7vs::virtual_service_udp& vs,l7vs::udp_session& session,boost::asio::io_service& request_io) : 
		io(request_io),
		parent_service(vs),
		parent_session(session),
		realserver_socket(request_io){

	}

	udp_request::~udp_request(){


	}

	void udp_request::initialize(boost::asio::ip::udp::endpoint client_endpoint , boost::array<char , MAX_BUFFER_SIZE>& data,std::size_t data_size){
	}

	void udp_request::run(void){
	}

	void udp_request::stop(void){
	}

	void udp_request::client_receive_event(void){
	}

	void udp_request::realserver_get_destination_event(void){
	}

	void udp_request::realserver_send(void){
	}

	void udp_request::realserver_receive(void){
	}

	void udp_request::client_get_destination_event(void){
	}

	void udp_request::client_send(void){
	}

	void udp_request::realserver_close(void){
	}

	void udp_request::realserver_close_event(void){
	}

	void udp_request::thread_exit(void){
	}

 }// namespace l7vs
