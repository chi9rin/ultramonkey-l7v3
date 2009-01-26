// data_buff_base dummy

#ifndef DATA_BUFF_BASE_H
#define DATA_BUFF_BASE_H

#include <cstdlib>
#include <boost/asio.hpp>

namespace l7vs{

	class data_buff_base : private boost::noncopyable{
		public:
			//! construcor
			data_buff_base();
			bool b_construcor_call_chk;
			//! destructor
			~data_buff_base();
			bool b_destructor_call_chk;
			//! initialize
			void initialize();
			bool b_initialize_call_chk;
			//! set data
			//! @param[in]	reference to copy data
			void set_data(const boost::array< char , MAX_BUFFER_SIZE>& set_data);
			bool b_set_data_call_chk;
			//! get data
			//! @return		reference to data buffer object
			boost::array< char , MAX_BUFFER_SIZE>& get_data();
			bool b_get_data_call_chk;
			//! set data size
			//! @param[in]	copy size
			void set_size(const std::size_t set_size);
			bool b_set_size_call_chk;
			//! get data size
			//! @return		data size
			std::size_t get_size();
			bool b_get_size_call_chk;
			//! set send size
			//! @param[in]	copy size
			void set_send_size(const std::size_t set_size);
			bool b_set_send_size_call_chk;
			//! get send size
			//! @return		send size
			std::size_t get_send_size();
			bool b_get_send_size_call_chk;
			
			//! data buffer object
			boost::array< char , MAX_BUFFER_SIZE> data;
			//! data size
			std::size_t data_size;
			//! send data size
			std::size_t send_size;
	};// class data_buff_base
}// namespace l7vs

#endif//DATA_BUFF_BASE_H
