/*
 *	@file	schedule_module_lc.cpp
 *	@brief	shared object schedule module class
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
#include "schedule_module_lc.h"
#include <boost/format.hpp>

namespace l7vs{

//!	constractor
schedule_module_least_connection::schedule_module_least_connection() : schedule_module_base( "lc" ){
}

//! destractor
schedule_module_least_connection::~schedule_module_least_connection(){}

//!	initialize function
void	schedule_module_least_connection::initialize(){
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 25, "Function in : schedule_module_least_connection::initialize", __FILE__, __LINE__);
			}
		}
	}

	if ( !putLogInfo.empty() )
	{
		putLogInfo( 3, "Function was initialized.", __FILE__, __LINE__);
	}

	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 26, "Function out : schedule_module_least_connection::initialize", __FILE__, __LINE__);
			}
		}
	}
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool	schedule_module_least_connection::is_tcp(){ return true; }

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool	schedule_module_least_connection::is_udp(){ return false; }

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled TCP/IP endpoint
void	schedule_module_least_connection::handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint ){
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 27, "Function in : schedule_module_least_connection::handle_schedule", __FILE__, __LINE__);
			}
		}
	}

	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	int								active = INT_MAX;
	rslist_type::iterator			itr;
	std::string	buf;
	int			loop;

	//! set clear data as NULL
	outendpoint = tcp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() || inlist_next.empty() ){
		//! invalid iterator function
		if ( !putLogFatal.empty() )
		{
			putLogFatal( 5, "Iterator function is empty.", __FILE__, __LINE__);
		}
		goto END;
	}

	//! Debug log
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				for ( loop = 1, itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ), loop++ ){
					buf = boost::io::str( boost::format( "realserver[%d] : %s:%d weight(%d)" )
														% loop
														% itr->tcp_endpoint.address()
														% itr->tcp_endpoint.port()
														% itr->weight );
					putLogDebug( 28, buf, __FILE__, __LINE__);
				}
			}
		}
	}
	//! Debug log END

	for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next(itr) ){
		if ( itr->weight > 0 && active > itr->get_active() ){
			//! set found data
			outendpoint = itr->tcp_endpoint;
			active = itr->get_active();
		}
	}

	if ( active == INT_MAX ){
		//! no data
		if ( !putLogError.empty() )
		{
			putLogError( 5, "There is no realserver on list.", __FILE__, __LINE__);
		}
		goto END;
	}

END:
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 29, "Function out : schedule_module_least_connection::handle_schedule", __FILE__, __LINE__);
			}
		}
	}
}

//! handle schedule calles then schedule function for UDP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled UDP endpoint
void	schedule_module_least_connection::handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&		outendpoint ){
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 30, "Function in : schedule_module_least_connection::handle_schedule", __FILE__, __LINE__);
			}
		}
	}

	if ( !putLogWarn.empty() )
	{
		putLogWarn( 1, "UDP function was not supported.", __FILE__, __LINE__);
	}

	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 31, "Function out : schedule_module_least_connection::handle_schedule", __FILE__, __LINE__);
			}
		}
	}
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void	schedule_module_least_connection::replication_interrupt(){}

}	//namespace l7vs

extern "C" l7vs::schedule_module_base*
create_module(){
	return dynamic_cast<l7vs::schedule_module_base*>(new l7vs::schedule_module_least_connection());
}

extern "C" void
destroy_module( l7vs::schedule_module_base* in ){
	delete in;
}
