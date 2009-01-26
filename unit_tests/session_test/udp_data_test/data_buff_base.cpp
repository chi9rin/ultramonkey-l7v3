// data_buff_base dummy

#include "data_buff_base.h"

namespace l7vs{

	//! construcor
	data_buff_base::data_buff_base() :
		b_construcor_call_chk(false),
		b_destructor_call_chk(false),
		b_initialize_call_chk(false),
		b_set_data_call_chk(false),
		b_get_data_call_chk(false),
		b_set_size_call_chk(false),
		b_get_size_call_chk(false),
		b_set_send_size_call_chk(false),
		b_get_send_size_call_chk(false){
			
		b_construcor_call_chk = true;
	}

	//! destructor
	data_buff_base::~data_buff_base(){
		b_destructor_call_chk = true;
	}

	//! initialize
	void data_buff_base::initialize(){
		b_initialize_call_chk = true;
	}

	//! set data
	//! @param[in]	reference to copy data
	void data_buff_base::set_data(const boost::array< char , MAX_BUFFER_SIZE>& set_data){
		data = set_data;
		b_set_data_call_chk = true;
	}

	//! get data
	//! @return		reference to data buffer object
	boost::array< char , MAX_BUFFER_SIZE>& data_buff_base::get_data(){
		b_get_data_call_chk = true;
		return data;
	}

	//! set data size
	//! @param[in]	copy size
	void data_buff_base::set_size(const std::size_t set_size){
		data_size = set_size;
		b_set_size_call_chk = true;
	}

	//! get data size
	//! @return		data size
	std::size_t data_buff_base::get_size(){
		b_get_size_call_chk = true;
		return data_size;
	}

	//! set send size
	//! @param[in]	copy size
	void data_buff_base::set_send_size(const std::size_t set_size){
		b_set_send_size_call_chk = true;
		send_size = set_size;
	}

	//! get send size
	//! @return		send size
	std::size_t data_buff_base::get_send_size(){
		b_get_send_size_call_chk = true;
		return send_size;
	}

}// namespace l7vsd

