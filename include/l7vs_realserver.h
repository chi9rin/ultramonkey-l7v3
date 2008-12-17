//
//	@file	l7vs_realserver.h
//	@brief	realserver data prototype
//
//	copyright (c) sdy corporation. 2008
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	L7VS_REALSERVER_H
#define	L7VS_REALSERVER_H
#include <boost/asio.hpp>
#include "l7vs_realserver_element.h"

namespace l7vsd{

class	realserver : public realserver_element{
public:
	int								nactive;
	int								ninact;
	int								send_byte;

	realserver() : nactive(0), ninact(0), send_byte(0){}
	realserver( const realserver& in ) : realserver_element( in ), 
													nactive( in.nactive ), 
													ninact( in.ninact ), 
													send_byte( in.send_byte ){}

	realserver& operator=( const realserver& rs ){
		realserver::operator= (rs);
		nactive = rs.nactive;
		ninact = rs.ninact;
		send_byte = rs.send_byte;
		return *this;
	}
/*
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
*/
};

};	//namespace l7vsd
#endif	//L7VS_REALSERVER
