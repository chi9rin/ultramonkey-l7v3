#ifndef TCP_DATA_H
#define TCP_DATA_H


#include "data_buff_base.h"

namespace l7vs{

	class tcp_data : public data_buff_base{
		protected:
			boost::asio::ip::tcp::endpoint endpoint_info;
		public:
			tcp_data();
			~tcp_data();
			void initialize();
			void set_endpoint(const boost::asio::ip::tcp::endpoint set_endpoint);
			boost::asio::ip::tcp::endpoint get_endpoint();

	};// class tcp_data
}// namespace l7vs

#endif//TCP_DATA_H

