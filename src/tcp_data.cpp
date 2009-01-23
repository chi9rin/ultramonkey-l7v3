#include "tcp_data.h"

namespace l7vs{

	tcp_data::tcp_data(){
		initialize();
	}

	tcp_data::~tcp_data(){
	}

	void tcp_data::initialize(){
		endpoint_info = boost::asio::ip::tcp::endpoint();
	}

	void tcp_data::set_endpoint(const boost::asio::ip::tcp::endpoint set_endpoint){
		endpoint_info = set_endpoint;
	}

	boost::asio::ip::tcp::endpoint tcp_data::get_endpoint(){
		return endpoint_info;
	}

}// namespace l7vsd

