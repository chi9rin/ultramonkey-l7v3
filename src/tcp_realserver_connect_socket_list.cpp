//
//!	@file	tcp_realserver_connect_socket_list.h
//!	@brief	tcp session connect realserver socket list class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include "tcp_realserver_connect_socket_list.h"
#include "logger.h"

namespace l7vs{
	
	//! construcor
	tcp_realserver_connect_socket_list::tcp_realserver_connect_socket_list(){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_realserver_connect_socket_list::tcp_realserver_connect_socket_list", __FILE__, __LINE__ );
	}
	//! destructor
	tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list(){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list", __FILE__, __LINE__ );
	}
	
	//! push back list
	//! @param[in/out]	realserver_socket is realserver list push back element 
	void tcp_realserver_connect_socket_list::push_back(list_element realserver_socket){		
		boost::mutex::scoped_lock scope_lock(list_mutex);
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_realserver_connect_socket_list::push_back", __FILE__, __LINE__ );
		
		connect_list.push_back(realserver_socket);
	}
	
	//! get socket list
	//! @return			realserver list element
	tcp_realserver_connect_socket_list::list_element tcp_realserver_connect_socket_list::get_socket(){
		boost::mutex::scoped_lock scope_lock(list_mutex);
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_realserver_connect_socket_list::get_socket", __FILE__, __LINE__ );
		
		list_element& sock = *(connect_list.begin());
		connect_list.pop_front();
		return sock;
	}
	
	//! check empty list
	bool tcp_realserver_connect_socket_list::empty(){
		boost::mutex::scoped_lock scope_lock(list_mutex);
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_realserver_connect_socket_list::empty", __FILE__, __LINE__ );
		
		bool bres = connect_list.empty();
		return bres;
	}
	
}// namespace l7vs
 
