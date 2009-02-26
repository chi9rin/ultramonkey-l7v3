//
//!	@file	udp_data.h
//!	@brief	udp request processing data class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef UDP_DATA_H
#define UDP_DATA_H

#include "data_buff_base.h"

namespace l7vs{

//!	@class	udp_data
//! @brief	this class is udp session object use data for processing.
	typedef data_buff_base<boost::asio::ip::udp> udp_data;

}// namespace l7vs

#endif//UDP_DATA_H
