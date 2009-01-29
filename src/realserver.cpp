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
#include	"parameter.h"
#include	"logger.h"

namespace l7vs{

void	realserver::increment_active(){
	Logger	logger( LOG_CAT_L7VSD_REALSERVER, 1, "realserver::increment_active", __FILE__, __LINE__ );
	boost::mutex::scoped_lock( active_mutex );

	nactive++;
	if ( nactive == INT_MAX ){
		nactive = 0;
		std::string msg("nactive was go round.");
		Logger::putLogInfo( LOG_CAT_L7VSD_REALSERVER, 1, msg, __FILE__, __LINE__);
	}
}

void	realserver::decrement_active(){
	Logger	logger( LOG_CAT_L7VSD_REALSERVER, 1, "realserver::decrement_active", __FILE__, __LINE__ );
	boost::mutex::scoped_lock( active_mutex );

	if ( nactive > 0 ){
		nactive--;
	}
}

void	realserver::increment_inact(){
	Logger	logger( LOG_CAT_L7VSD_REALSERVER, 1, "realserver::increment_inact", __FILE__, __LINE__ );
	boost::mutex::scoped_lock( inact_mutex );

	ninact++;
	if ( ninact == INT_MAX ){
		ninact = 0;
		std::string msg("ninact was go round.");
		Logger::putLogInfo( LOG_CAT_L7VSD_REALSERVER, 1, msg, __FILE__, __LINE__);
	}
}

int		realserver::get_active(){
	Logger	logger( LOG_CAT_L7VSD_REALSERVER, 1, "realserver::get_active", __FILE__, __LINE__ );
	boost::mutex::scoped_lock( active_mutex );

	return nactive;
}

int		realserver::get_inact(){
	Logger	logger( LOG_CAT_L7VSD_REALSERVER, 1, "realserver::get_inact", __FILE__, __LINE__ );
	boost::mutex::scoped_lock( inact_mutex );

	return ninact;
}

}	//namespace l7vs
