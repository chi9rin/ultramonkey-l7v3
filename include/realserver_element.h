//
//	@file	realserver_element.h
//	@brief	realserver data prototype
//
//	copyright (c) sdy corporation. 2008
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef	REALSERVER_ELEMENT_H
#define	REALSERVER_ELEMENT_H
#include <boost/asio.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include "endpoint.h"

namespace l7vs{

class	realserver_element{
protected:
	int								nactive;
	int								ninact;
public:
	boost::asio::ip::tcp::endpoint	tcp_endpoint;
	boost::asio::ip::udp::endpoint	udp_endpoint;
	int								weight;
	realserver_element() : nactive(0), ninact(0), weight(-1){}
	realserver_element( const realserver_element& in ) :	nactive( in.nactive ), ninact( in.ninact ),
															tcp_endpoint( in.tcp_endpoint ) ,
															udp_endpoint( in.udp_endpoint ) ,
															weight( in.weight ){}
	realserver_element& operator=( const realserver_element& elem ){
		nactive = elem.nactive;
		ninact = elem.ninact;
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

	int		get_active(){
		return nactive;
	}

	int		get_inact(){
		return ninact;
	}

private:
	friend class	boost::serialization::access;		//! friend boost serializable class
	//! serializable
	//! @brief using boost serialiable. class serializable function.
	//! @param[in]	archive
	//! @param[in]	version
	template <class Archive > void serialize( Archive& ar, const unsigned int version ){
		ar & nactive;
		ar & ninact;
		ar & tcp_endpoint;
		ar & udp_endpoint;
		ar & weight;
	}
};

}	//namespace l7vsd
#endif	//REALSERVER_ELEMENT
