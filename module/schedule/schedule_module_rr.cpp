/*
 *	@file	schedule_module_rr.cpp
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
#include "schedule_module_rr.h"
#include <boost/format.hpp>

namespace l7vs{

//!	constractor
schedule_module_round_robin::schedule_module_round_robin() : schedule_module_base( "rr" ){
}

//! destractor
schedule_module_round_robin::~schedule_module_round_robin(){}

//!	initialize function
void	schedule_module_round_robin::initialize(){
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 1, "Function in : schedule_module_round_robin::initialize", __FILE__, __LINE__);
			}
		}
	}

	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	boost::asio::ip::udp::endpoint	udp_local_endpoint ;

	tcp_endpoint = tcp_local_endpoint ;
	udp_endpoint = udp_local_endpoint ;

	if ( !putLogInfo.empty() )
	{
		putLogInfo( 1, "Saved endpoint was initialized.", __FILE__, __LINE__);
	}

	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 1, "Function out : schedule_module_round_robin::initialize", __FILE__, __LINE__);
			}
		}
	}
}

//! tcp protocol support check
//! @return tcp support is true
//! @return tcp not-support is false
bool	schedule_module_round_robin::is_tcp(){ return true; }

//! udp protocol support check
//! @return udp support is true
//! @return udp not-support is false
bool	schedule_module_round_robin::is_udp(){ return true; }

//! handle schedule called then schedule function for TCP/IP endpoint
//! @param[in]	thread id
//! @param[in]	list iterator first function object
//!	@param[in]	list iterator last function object
//!	@param[in]	list iterator next function object
//! @param[out]	scheduled TCP/IP endpoint
void	schedule_module_round_robin::handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint ){
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 1, "Function in : schedule_module_round_robin::handle_schedule", __FILE__, __LINE__);
			}
		}
	}

	boost::asio::ip::tcp::endpoint	tcp_local_endpoint ;
	rslist_type::iterator			itr;
	std::string	buf;
	int			loop;

	//! set clear data as NULL
	outendpoint = tcp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() || inlist_next.empty() ){
		//! invalid iterator function
		if ( !putLogFatal.empty() )
		{
			putLogFatal( 1, "Iterator function is empty.", __FILE__, __LINE__);
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
					putLogDebug( 1, buf, __FILE__, __LINE__);
				}
			}
		}
	}
	//! Debug log END

	for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ) ){
		//! keep first data of list
		if ( itr->weight > 0 ){
			outendpoint = itr->tcp_endpoint;
			break;
		}
	}
	if ( itr == inlist_end() ){
		//! no data
		if ( !putLogError.empty() )
		{
			putLogError( 1, "There is no realserver on list.", __FILE__, __LINE__);
		}
		goto END;
	}

	//! first time
	if ( tcp_local_endpoint == tcp_endpoint ){
		//! set first data
		tcp_endpoint = outendpoint;
		goto END;
	}

	//! Debug log
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				buf = boost::io::str( boost::format( "previous endpoint : %s:%d" ) 
													% tcp_endpoint.address()
													% tcp_endpoint.port() );
				putLogDebug( 1, buf, __FILE__, __LINE__);
			}
		}
	}
	//! Debug log END

	for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
		if ( itr->weight > 0 ){
			//! prev endpoint
			if ( tcp_endpoint == itr->tcp_endpoint ){
				itr = inlist_next( itr );
				break;
			}
		}
	}
	for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
		if ( itr->weight > 0 ){

			//! Debug log
			if ( !getloglevel.empty() ){
				if ( LOG_LV_DEBUG == getloglevel() ){
					if ( !putLogDebug.empty() ){
						buf = boost::io::str( boost::format( "itr : %s:%d weight(%d)" )
															% itr->tcp_endpoint.address()
															% itr->tcp_endpoint.port()
															% itr->weight );
						putLogDebug( 1, buf, __FILE__, __LINE__);
					}
				}
			}
			//! Debug log END

			//! set found data
			outendpoint = itr->tcp_endpoint;
			break ;
		}
	}

	//! set found or first data
	tcp_endpoint = outendpoint;

END:
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 1, "Function out : schedule_module_round_robin::handle_schedule", __FILE__, __LINE__);
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
void	schedule_module_round_robin::handle_schedule(
							boost::thread::id					thread_id,
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next,
							boost::asio::ip::udp::endpoint&		outendpoint ){
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 1, "Function in : schedule_module_round_robin::handle_schedule", __FILE__, __LINE__);
			}
		}
	}

	boost::asio::ip::udp::endpoint	udp_local_endpoint ;
	rslist_type::iterator			itr;
	std::string	buf;
	int			loop;

	//! set clear data as NULL
	outendpoint = udp_local_endpoint;

	if ( inlist_begin.empty() || inlist_end.empty() || inlist_next.empty() ){
		//! invalid iterator function
		if ( !putLogFatal.empty() )
		{
			putLogFatal( 1, "Iterator function is empty.", __FILE__, __LINE__);
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
														% itr->udp_endpoint.address()
														% itr->udp_endpoint.port()
														% itr->weight );
					putLogDebug( 1, buf, __FILE__, __LINE__);
				}
			}
		}
	}
	//! Debug log END

	for ( itr = inlist_begin(); itr != inlist_end(); itr = inlist_next( itr ) ){
		//! keep first data of list
		if ( itr->weight > 0 ){
			outendpoint = itr->udp_endpoint;
			break;
		}
	}
	if ( itr == inlist_end() ){
		//! no data
		if ( !putLogError.empty() )
		{
			putLogError( 1, "There is no realserver on list.", __FILE__, __LINE__);
		}
		goto END;
	}

	//! first time
	if ( udp_local_endpoint == udp_endpoint ){
		//! set first data
		udp_endpoint = outendpoint;
		goto END;
	}

	//! Debug log
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				buf = boost::io::str( boost::format( "previous endpoint : %s:%d" ) 
													% udp_endpoint.address()
													% udp_endpoint.port() );
				putLogDebug( 1, buf, __FILE__, __LINE__);
			}
		}
	}
	//! Debug log END

	for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
		if ( itr->weight > 0 ){
			//! prev endpoint
			if ( udp_endpoint == itr->udp_endpoint ){
				itr = inlist_next( itr );
				break;
			}
		}
	}
	for ( ; itr != inlist_end(); itr = inlist_next( itr ) ){
		if ( itr->weight > 0 ){

			//! Debug log
			if ( !getloglevel.empty() ){
				if ( LOG_LV_DEBUG == getloglevel() ){
					if ( !putLogDebug.empty() ){
						buf = boost::io::str( boost::format( "itr : %s:%d weight(%d)" )
															% itr->udp_endpoint.address()
															% itr->udp_endpoint.port()
															% itr->weight );
						putLogDebug( 1, buf, __FILE__, __LINE__);
					}
				}
			}
			//! Debug log END

			//! set found data
			outendpoint = itr->udp_endpoint;
			break ;
		}
	}

	//! set found or first data
	udp_endpoint = outendpoint;

END:
	if ( !getloglevel.empty() ){
		if ( LOG_LV_DEBUG == getloglevel() ){
			if ( !putLogDebug.empty() ){
				putLogDebug( 1, "Function out : schedule_module_round_robin::handle_schedule", __FILE__, __LINE__);
			}
		}
	}
}

//! replication interval interrrupt
//! timer thread call this function. from virtualservice.
void	schedule_module_round_robin::replication_interrupt(){}

}	//namespace l7vs

extern "C" l7vs::schedule_module_base*
create_module(){
	return dynamic_cast<l7vs::schedule_module_base*>(new l7vs::schedule_module_round_robin());
}

extern "C" void
destroy_module( l7vs::schedule_module_base* in ){
	delete in;
}
