/*!
 *	@file	virtualservice_element.h
 *	@brief	use l7vscommand virtualservice_data prototype
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#ifndef	VIRTUALSERVICE_COMMAND_ELEMENT
#define	VIRTUALSERVICE_COMMAND_ELEMENT

#include <vector>
#include <boost/foreach.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
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
	unsigned long long		throughput_upstream;
	unsigned long long		throughput_downstream;
	int				ssl_flag;

	virtualservice_element() :	udpmode(false),
				sorry_maxconnection(0LL),
				sorry_flag(0),
				qos_upstream(0ULL),
				qos_downstream(0ULL),
				throughput_upstream(0ULL),
				throughput_downstream(0ULL),
				ssl_flag(0) {}
				
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
				qos_downstream( in.qos_downstream ),
				throughput_upstream( in.throughput_upstream ),
				throughput_downstream( in.throughput_downstream ),
				ssl_flag( in.ssl_flag ){
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
		throughput_upstream = in.throughput_upstream;
		throughput_downstream = in.throughput_downstream;
		ssl_flag = in.ssl_flag;

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
				elem1.qos_downstream == elem2.qos_downstream &&
				elem1.throughput_upstream == elem2.throughput_upstream &&
				elem1.throughput_downstream == elem2.throughput_downstream &&
				elem1.ssl_flag == elem2.ssl_flag ){
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
				elem1.qos_downstream != elem2.qos_downstream ||
				elem1.throughput_upstream != elem2.throughput_upstream ||
				elem1.throughput_downstream != elem2.throughput_downstream ||
				elem1.ssl_flag != elem2.ssl_flag ){
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

	template <typename Elem, typename Traits>
	friend std::basic_ostream<Elem, Traits>& operator<<(
    	std::basic_ostream<Elem, Traits>& os,
    	const virtualservice_element& elem ){

		os << "virtualservice_element={";
		os << boost::format(	"udpmode=%s: "
								"tcp_accept_endpoint=%s: "
								"udp_recv_endpoint=%s: " )
								% elem.udpmode
								% elem.tcp_accept_endpoint
								% elem.udp_recv_endpoint;

		unsigned int i = 0;
		BOOST_FOREACH( realserver_element rs_elem, elem.realserver_vector ){
			os << boost::format( "realserver_vector[%d]=" ) % i;
			os << rs_elem;
			os << ": ";
			++i;
		}

		os << boost::format(	"protocol_module_name=%s: "
								"schedule_module_name=%s: " )
								% elem.protocol_module_name
								% elem.schedule_module_name;

		std::string	args = boost::algorithm::join( elem.protocol_args, " " );
		os << boost::format(	"protocol_args=%s: "
								"sorry_maxconnection=%d: "
								"sorry_endpoint=%s: "
								"sorry_flag=%d: "
								"qos_upstream=%d: "
								"qos_downstream=%d: "
								"throughput_upstream=%d: " 
								"throughput_downstream=%d: "
								"ssl_flag=%d}" )
								% args
								% elem.sorry_maxconnection
								% elem.sorry_endpoint
								% elem.sorry_flag
								% elem.qos_upstream
								% elem.qos_downstream
								% elem.throughput_upstream
								% elem.throughput_downstream
								% elem.ssl_flag;
		return os;
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
		ar & throughput_upstream;
		ar & throughput_downstream;
		ar & ssl_flag;
	}
};



}			//namespace l7vsd
#endif		//L7VS_VIRTUALSERVICE_COMMAND_ELEMENT
