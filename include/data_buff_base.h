//
//!	@file	data_buff_base.h
//!	@brief	receive and send buffer class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef DATA_BUFF_BASE_H
#define DATA_BUFF_BASE_H

#include <cstdlib>
#include <boost/asio.hpp>

namespace l7vs{

//!	@class	data_buff_base
//! @brief	this class is base class of tcp_data and udp_data classes.
	class data_buff_base : private boost::noncopyable{
		public:
			//! construcor
			data_buff_base();
			//! destructor
			~data_buff_base();
			//! initialize
			void initialize();
			//! set data
			//! @param[in]	reference to copy data
			void set_data(const boost::array< char , MAX_BUFFER_SIZE>& set_data);
			//! get data
			//! @return		reference to data buffer object
			boost::array< char , MAX_BUFFER_SIZE>& get_data();
			//! set data size
			//! @param[in]	copy size
			void set_size(const std::size_t set_size);
			//! get data size
			//! @return		data size
			std::size_t get_size();
			//! set send size
			//! @param[in]	copy size
			void set_send_size(const std::size_t set_size);
			//! get send size
			//! @return		send size
			std::size_t get_send_size();
		protected:
			//! data buffer object
			boost::array< char , MAX_BUFFER_SIZE> data;
			//! data size
			std::size_t data_size;
			//! send data size
			std::size_t send_size;
	};// class data_buff_base
}// namespace l7vs

#endif//DATA_BUFF_BASE_H
