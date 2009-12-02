/*!
 *	@file	udp_request_thread_control.cpp
 *	@brief	request used 1 threads. thread pooling unit is 1 thread control.
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

#include "udp_request_thread_control.h"

namespace l7vs{

	udp_request_thread_control::udp_request_thread_control(request_ptr request) : 
		my_request(request){
	}

	udp_request_thread_control::~udp_request_thread_control(void){
	}

	void udp_request_thread_control::start(void){
	}

	void udp_request_thread_control::stop(void){
	}

	void udp_request_thread_control::join(void){
	}

	boost::thread::id udp_request_thread_control::get_thread_id(){
		return request_thread->get_id();
	}

	boost::shared_ptr< l7vs::udp_request > udp_request_thread_control::get_request(){
		return my_request;
	}

	void udp_request_thread_control::run(void){
	}

}// namespace l7vs
