
#include "data_buff_base.h"

namespace l7vs{
	data_buff_base::data_buff_base(){
		
	}
	data_buff_base::~data_buff_base(){
	}
	
	void data_buff_base::initialize(){
	}
	std::size_t data_buff_base::get_size(){
		return data_size;
	}
	void data_buff_base::set_size(const std::size_t set_size){
	}
	boost::array< char , MAX_BUFFER_SIZE>& data_buff_base::get_data(){
		return data;
	}
	void data_buff_base::set_data(const boost::array< char , MAX_BUFFER_SIZE>& data){
	}
	void data_buff_base::set_send_size(const std::size_t set_size){
	}
	std::size_t data_buff_base::get_send_size(){
		return send_size;
	}
	
}// namespace l7vsd

