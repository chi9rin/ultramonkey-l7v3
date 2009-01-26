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
	class udp_data : public data_buff_base{
		public:
			//! construcor
			udp_data();
			//! destructor
			~udp_data();
			//! initialize
			void initialize();
			//! get endpoint
			//! @return		endpoint_info
			boost::asio::ip::udp::endpoint get_endpoint();
			//! set endpoint
			//! @param[in]	copy endpoint
			void set_endpoint(const boost::asio::ip::udp::endpoint set_endpoint);
		protected:
			//! udp endpoint
			boost::asio::ip::udp::endpoint endpoint_info;

	};// class udp_data
}// namespace l7vs

#endif//UDP_DATA_H
