//
//!	@file	tcp_data.h
//!	@brief	tcp session processing data class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef TCP_DATA_H
#define TCP_DATA_H

#include "data_buff_base.h"

namespace l7vs{

//!	@class	tcp_data
//! @brief	this class is tcp session object use data for processing.
	class tcp_data : public data_buff_base{
		public:
			//! construcor
			tcp_data();
			//! destructor
			~tcp_data();
			//! initialize
			void initialize();
			//! get endpoint
			//! @return		endpoint_info
			boost::asio::ip::tcp::endpoint get_endpoint();
			//! set endpoint
			//! @param[in]	copy endpoint
			void set_endpoint(const boost::asio::ip::tcp::endpoint set_endpoint);
		protected:
			//! tcp endpoint
			boost::asio::ip::tcp::endpoint endpoint_info;
	};// class tcp_data
}// namespace l7vs

#endif//TCP_DATA_H

