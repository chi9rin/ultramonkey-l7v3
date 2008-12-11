//
//	@file	l7vs_realserver_element.h
//	@brief	realserver data prototype
//
//	copyright (c) sdy corporation. 2008
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	L7VS_REALSERVER_ELEMENT_H
#define	L7VS_REALSERVER_ELEMENT_H
#include <boost/asio.hpp>

namespace l7vsd{

class	realserver_element{
public:
	boost::asio::ip::tcp::endpoint	tcp_endpoint;
	boost::asio::ip::udp::endpoint	udp_endpoint;
	int								weight;
	realserver_element() : weight(-1){}
	realserver_element( const realserver_element& in ) : tcp_endpoint( in.tcp_endpoint ) ,
															 udp_endpoint( in.udp_endpoint ) ,
															 weight( in.weight ){}
	realserver_element& operator=( const realserver_element& elem ){
		tcp_endpoint = elem.tcp_endpoint;
		udp_endpoint = elem.udp_endpoint;
		weight = elem.weight;
		return *this;
	}
	friend	bool	operator==( const realserver_element& rselem1, const realserver_element& rselem2 ){
		return rselem1.tcp_endpoint == rselem2.tcp_endpoint &&
				rselem1.udp_endpoint == rselem2.udp_endpoint &&
				rselem1.weight == rselem2.weight;
	}
	
	friend bool	operator!=( const realserver_element& rselem1, const realserver_element& rselem2 ){
		return rselem1.tcp_endpoint != rselem2.tcp_endpoint ||
				rselem1.udp_endpoint != rselem2.udp_endpoint ||
				rselem1.weight != rselem2.weight;
	}
	
	friend bool	operator<( const realserver_element& rselem1, const realserver_element& rselem2 ){
		if( rselem1.tcp_endpoint < rselem2.tcp_endpoint ) return true;
		if( rselem1.tcp_endpoint != rselem2.tcp_endpoint ) return false;
		return rselem1.weight < rselem2.weight;
	}
};

};	//namespace l7vsd
#endif	//L7VS_REALSERVER_ELEMENT
