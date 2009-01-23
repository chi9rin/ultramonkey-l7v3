#include "udp_data.h"

namespace l7vs{

	udp_data::udp_data(){
		initialize();
	}

	udp_data::~udp_data(){
	}

	void udp_data::initialize(){
		endpoint_info = boost::asio::ip::udp::endpoint();
	}

	void udp_data::set_endpoint(const boost::asio::ip::udp::endpoint set_endpoint){
		endpoint_info = set_endpoint;
	}

	boost::asio::ip::udp::endpoint udp_data::get_endpoint(){
		return endpoint_info;
	}

}// namespace l7vsd

