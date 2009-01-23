#ifndef UDP_DATA_H
#define UDP_DATA_H


#include "data_buff_base.h"

namespace l7vs{

	class udp_data : public data_buff_base{
		protected:
			boost::asio::ip::udp::endpoint endpoint_info;
		public:
			udp_data();
			~udp_data();
			void initialize();
			void set_endpoint(const boost::asio::ip::udp::endpoint set_endpoint);
			boost::asio::ip::udp::endpoint get_endpoint();

	};// class udp_data
}// namespace l7vs

#endif//UDP_DATA_H
