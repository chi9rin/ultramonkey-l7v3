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
#ifndef	REALSERVER_H
#define	REALSERVER_H

#include	<boost/thread.hpp>
#include	"realserver_element.h"

namespace l7vs{

class	realserver : public realserver_element{
protected:
	int				nactive;
	int				ninact;
	boost::mutex	active_mutex;
	boost::mutex	inact_mutex;
public:
	unsigned long long	send_byte;

	realserver() : nactive(0), ninact(0), send_byte(0LL){}
	realserver( const realserver& in ) : realserver_element( in ),
										 nactive( in.nactive ),
										 ninact( in.ninact ),
										 send_byte( in.send_byte ){}

	realserver& operator=( const realserver& rs ){
		realserver_element::operator= (rs);
		nactive = rs.nactive;
		ninact = rs.ninact;
		send_byte = rs.send_byte;
		return *this;
	}

	~realserver(){}

	friend	bool	operator==( const realserver& rs1, const realserver& rs2 ){
		realserver& rselem1 = const_cast<realserver&>( rs1 );
		realserver& rselem2 = const_cast<realserver&>( rs2 );
		return	rselem1.tcp_endpoint == rselem2.tcp_endpoint &&
				rselem1.udp_endpoint == rselem2.udp_endpoint &&
				rselem1.weight == rselem2.weight;
	}
	
	friend	bool	operator!=( const realserver& rs1, const realserver& rs2 ){
		realserver& rselem1 = const_cast<realserver&>( rs1 );
		realserver& rselem2 = const_cast<realserver&>( rs2 );
		return  rselem1.tcp_endpoint != rselem2.tcp_endpoint ||
				rselem1.udp_endpoint != rselem2.udp_endpoint ||
				rselem1.weight != rselem2.weight;
	}
	
	friend	bool	operator<( const realserver& rs1, const realserver& rs2 ){
		realserver& rselem1 = const_cast<realserver&>( rs1 );
		realserver& rselem2 = const_cast<realserver&>( rs2 );
		if( rselem1.tcp_endpoint < rselem2.tcp_endpoint ) return true;
		if( rselem1.tcp_endpoint != rselem2.tcp_endpoint ) return false;
		return rselem1.weight < rselem2.weight;
	}

	void	increment_active();
	void	decrement_active();
	void	increment_inact();
	int		get_active();
	int		get_inact();

};


}	//namespace l7vs
#endif	//REALSERVER_H
