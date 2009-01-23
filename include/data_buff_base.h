

#ifndef DATA_BUFF_BASE_H
#define DATA_BUFF_BASE_H

#include <cstdlib>
#include <boost/asio.hpp>

namespace l7vs{

	class data_buff_base : private boost::noncopyable{
		protected:
			std::size_t data_size;
			boost::array< char , MAX_BUFFER_SIZE> data;
			std::size_t send_size;
		public:
			data_buff_base();
			~data_buff_base();
			void initialize();
			std::size_t get_size();
			void set_size(const std::size_t set_size);
			boost::array< char , MAX_BUFFER_SIZE>& get_data();
			void set_data(const boost::array< char , MAX_BUFFER_SIZE>& data);
			void set_send_size(const std::size_t set_size);
			std::size_t get_send_size();
	};// class data_buff_base
}// namespace l7vs

#endif//DATA_BUFF_BASE_H
