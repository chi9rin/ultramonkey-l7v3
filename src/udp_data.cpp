//
//!	@file	udp_data.cpp
//!	@brief	udp request processing data class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include "udp_data.h"

namespace l7vs{
	//! construcor
	udp_data::udp_data(){
		endpoint_info = boost::asio::ip::udp::endpoint();
	}
	//! destructor
	udp_data::~udp_data(){
	}
	//! initialize
	void udp_data::initialize(){
		endpoint_info = boost::asio::ip::udp::endpoint();
		data_buff_base::initialize();
	}
	//! get endpoint
	boost::asio::ip::udp::endpoint udp_data::get_endpoint(){
		return endpoint_info;
	}
	//! set endpoint
	void udp_data::set_endpoint(const boost::asio::ip::udp::endpoint set_endpoint){
		endpoint_info = set_endpoint;
	}


}// namespace l7vsd

