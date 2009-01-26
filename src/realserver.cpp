//
//	@file	l7vs_realserver.cpp
//	@brief	realserver data class
//
//	copyright (c) xxx corporation. 2009
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include	"realserver.h"

namespace l7vs{

void	realserver::increment_active(){
	boost::mutex::scoped_lock( active_mutex );

	nactive++;
	if ( nactive == INT_MAX ){
		nactive = 0;
	}
}

void	realserver::decrement_active(){
	boost::mutex::scoped_lock( active_mutex );

	if ( nactive > 0 ){
		nactive--;
	}
}

void	realserver::increment_inact(){
	boost::mutex::scoped_lock( inact_mutex );

	ninact++;
	if ( ninact == INT_MAX ){
		ninact = 0;
	}
}

int		realserver::get_active(){
	boost::mutex::scoped_lock( active_mutex );

	return nactive;
}

int		realserver::get_inact(){
	boost::mutex::scoped_lock( inact_mutex );

	return ninact;
}

}	//namespace l7vs
