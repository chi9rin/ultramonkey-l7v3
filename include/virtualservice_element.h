//
//	@file	l7vs_virtualservice_command_element
//	@brief	use l7vscommand virtualservice_data prototype
//
//	copyright (c) sdy corporation. 2008
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef	VIRTUALSERVICE_COMMAND_ELEMENT
#define	VIRTUALSERVICE_COMMAND_ELEMENT

#include <vector>
#include <boost/foreach.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include "realserver_element.h"
#include "endpoint.h"

namespace l7vs{

// virtual service element includes.
class	virtualservice_element{
public:
	bool					udpmode;
	boost::asio::ip::tcp::endpoint
							tcp_accept_endpoint;
	boost::asio::ip::udp::endpoint
							udp_recv_endpoint;
	std::vector<realserver_element>
							realserver_vector;
	std::string
							protocol_module_name;
	std::string
							schedule_module_name;
	std::vector<std::string>
							protocol_args;
	long long				sorry_maxconnection;
	boost::asio::ip::tcp::endpoint
							sorry_endpoint;
	int						sorry_flag;
	unsigned long long		qos_upstream;
	unsigned long long		qos_downstream;
	
	virtualservice_element() :	udpmode(false),
				sorry_maxconnection(0LL),
				sorry_flag(0),
				qos_upstream(0ULL),
				qos_downstream(0ULL){}
				
	virtualservice_element( const virtualservice_element& in )
			:	udpmode( in.udpmode ),
				tcp_accept_endpoint( in.tcp_accept_endpoint ),
				udp_recv_endpoint( in.udp_recv_endpoint ),
				protocol_module_name( in.protocol_module_name ),
				schedule_module_name( in.schedule_module_name ),
				sorry_maxconnection( in.sorry_maxconnection ),
				sorry_endpoint( in.sorry_endpoint ),
				sorry_flag( in.sorry_flag ),
				qos_upstream( in.qos_upstream ),
				qos_downstream( in.qos_downstream ){
		realserver_vector.clear();
		BOOST_FOREACH( std::string str, in.protocol_args ){
			protocol_args.push_back( str );
		}
		BOOST_FOREACH( realserver_element elem, in.realserver_vector ){
			realserver_vector.push_back( elem );
		}
	}
	
	virtualservice_element& operator=( const virtualservice_element& in ){
		udpmode = in.udpmode;
		tcp_accept_endpoint = in.tcp_accept_endpoint;
		udp_recv_endpoint = in.udp_recv_endpoint;
		protocol_module_name = in.protocol_module_name;
		schedule_module_name = in.schedule_module_name;
		sorry_maxconnection = in.sorry_maxconnection;
		sorry_endpoint = in.sorry_endpoint;
		sorry_flag = in.sorry_flag;
		qos_upstream = in.qos_upstream;
		qos_downstream = in.qos_downstream;
		
		realserver_vector.clear();
		BOOST_FOREACH( std::string str, in.protocol_args ){
			protocol_args.push_back( str );
		}
		BOOST_FOREACH( realserver_element elem, in.realserver_vector ){
			realserver_vector.push_back( elem );
		}
		return *this;
	}
	
	friend	bool operator==( const virtualservice_element& elem1, const virtualservice_element& elem2 ){
		if(	elem1.udpmode == elem2.udpmode &&
				elem1.tcp_accept_endpoint == elem2.tcp_accept_endpoint &&
				elem1.udp_recv_endpoint == elem2.udp_recv_endpoint &&
				elem1.protocol_module_name == elem2.protocol_module_name &&
				elem1.sorry_maxconnection == elem2.sorry_maxconnection &&
				elem1.sorry_flag == elem2.sorry_flag &&
				elem1.qos_upstream == elem2.qos_upstream &&
				elem1.qos_downstream == elem2.qos_downstream ){
				if( elem1.realserver_vector.size() != elem2.realserver_vector.size() ) return false;
				for( unsigned int i = 0; i < elem1.realserver_vector.size(); ++i ){
					if( elem1.realserver_vector[i] != elem2.realserver_vector[i] ) return false;	
				}
				return true;
		}
		return false;
	}
	
	friend bool operator!=( const virtualservice_element& elem1, const virtualservice_element& elem2 ){
		if( elem1.udpmode != elem2.udpmode ||
				elem1.tcp_accept_endpoint != elem2.tcp_accept_endpoint ||
				elem1.udp_recv_endpoint != elem2.udp_recv_endpoint ||
				elem1.protocol_module_name != elem2.protocol_module_name ||
				elem1.sorry_maxconnection != elem2.sorry_maxconnection ||
				elem1.sorry_flag != elem2.sorry_flag ||
				elem1.qos_upstream != elem2.qos_upstream ||
				elem1.qos_downstream != elem2.qos_downstream ){
				if( elem1.realserver_vector.size() == elem2.realserver_vector.size() ){
					for( unsigned int i = 0; i < elem1.realserver_vector.size(); ++i ){
						if( elem1.realserver_vector[i] == elem2.realserver_vector[i] ) return false;
					}
				}
				return true;
		}
		return false;
	}
	friend bool operator<( const virtualservice_element& elem1, const virtualservice_element& elem2 ){
		if( !elem1.udpmode && !elem2.udpmode ){
			return elem1.tcp_accept_endpoint < elem2.tcp_accept_endpoint;
		}
		else if( elem1.udpmode && elem2.udpmode ){
			return elem1.udp_recv_endpoint < elem2.udp_recv_endpoint;
		}
		return false;
	}

private:
	friend class	boost::serialization::access;		//! friend boost serializable class
	//! serializable
	//! @brief using boost serialiable. class serializable function.
	//! @param[in]	archive
	//! @param[in]	version
	template <class Archive > void serialize( Archive& ar, const unsigned int version ){
		ar & udpmode;
		ar & tcp_accept_endpoint;
		ar & udp_recv_endpoint;
		ar & realserver_vector;
		ar & protocol_module_name;
		ar & schedule_module_name;
		ar & protocol_args;
		ar & sorry_maxconnection;
		ar & sorry_endpoint;
		ar & sorry_flag;
		ar & qos_upstream;
		ar & qos_downstream;
	}
};



}			//namespace l7vsd
#endif		//L7VS_VIRTUALSERVICE_COMMAND_ELEMENT
