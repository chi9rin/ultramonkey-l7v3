//
//!	@file	tcp_data.cpp
//!	@brief	tcp session processing data class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include "tcp_data.h"

namespace l7vs{
	//! construcor
	tcp_data::tcp_data(){
		endpoint_info = boost::asio::ip::tcp::endpoint();
	}
	//! destructor
	tcp_data::~tcp_data(){
	}
	//! initialize
	void tcp_data::initialize(){
		endpoint_info = boost::asio::ip::tcp::endpoint();
		data_buff_base::initialize();
	}
	//! get endpoint
	//! @return		endpoint_info
	boost::asio::ip::tcp::endpoint tcp_data::get_endpoint(){
		return endpoint_info;
	}
	//! set endpoint
	//! @param[in]	copy endpoint
	void tcp_data::set_endpoint(const boost::asio::ip::tcp::endpoint set_endpoint){
		endpoint_info = set_endpoint;
	}


}// namespace l7vsd

