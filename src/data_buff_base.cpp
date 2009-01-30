//
//!	@file	data_buff_base.cpp
//!	@brief	session object receive and send buffer class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include "data_buff_base.h"
#include "logger.h"

namespace l7vs{

	//! construcor
	data_buff_base::data_buff_base(){
		initialize();
	}

	//! destructor
	data_buff_base::~data_buff_base(){
	}

	//! initialize
	void data_buff_base::initialize(){
		data_size = 0;
		send_size = 0;
		for(int i = 0;i < MAX_BUFFER_SIZE;i++)
			data[i] = '\0';
	}

	//! set data
	//! @param[in]	reference to copy data
	void data_buff_base::set_data(const boost::array< char , MAX_BUFFER_SIZE>& set_data){
		data = set_data;
	}

	//! get data
	//! @return		reference to data buffer object
	boost::array< char , MAX_BUFFER_SIZE>& data_buff_base::get_data(){
		return data;
	}

	//! set data size
	//! @param[in]	copy size
	void data_buff_base::set_size(const std::size_t set_size){
		data_size = set_size;
	}

	//! get data size
	//! @return		data size
	std::size_t data_buff_base::get_size(){
		return data_size;
	}

	//! set send size
	//! @param[in]	copy size
	void data_buff_base::set_send_size(const std::size_t set_size){
		send_size = set_size;
	}

	//! get send size
	//! @return		send size
	std::size_t data_buff_base::get_send_size(){
		return send_size;
	}

}// namespace l7vsd

