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
#include	<boost/shared_ptr.hpp>
#include	"realserver_element.h"

namespace l7vs{

class	realserver : public realserver_element{
public:
	typedef	boost::shared_ptr<boost::mutex>		mutex_ptr;
protected:
	int					nactive;
	int					ninact;
	mutex_ptr			active_mutex_ptr;
	mutex_ptr			inact_mutex_ptr;
public:
	unsigned long long	send_byte;

	realserver() : nactive(0), ninact(0), send_byte(0LL){
		active_mutex_ptr = mutex_ptr( new boost::mutex );
		inact_mutex_ptr = mutex_ptr( new boost::mutex );
	}
	realserver( const realserver& in ) : realserver_element( in ),
										 nactive( in.nactive ),
										 ninact( in.ninact ),
										 send_byte( in.send_byte ){
		active_mutex_ptr = mutex_ptr( new boost::mutex );
		inact_mutex_ptr = mutex_ptr( new boost::mutex );
	}

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

	void	increment_active(){
		boost::mutex::scoped_lock( active_mutex_ptr );

		nactive++;
		if ( nactive == INT_MAX ){
			nactive = 0;
		}
	}

	void	decrement_active(){
		boost::mutex::scoped_lock( active_mutex_ptr );

		if ( nactive > 0 ){
			nactive--;
		}
	}

	void	increment_inact(){
		boost::mutex::scoped_lock( inact_mutex_ptr );

		ninact++;
		if ( ninact == INT_MAX ){
			ninact = 0;
		}
	}

	int		get_active(){
		return nactive;
	}

	int		get_inact(){
		return ninact;
	}
};


}	//namespace l7vs
#endif	//REALSERVER_H
