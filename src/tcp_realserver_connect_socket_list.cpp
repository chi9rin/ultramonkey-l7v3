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
	}
	//! destructor
	tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list(){
	}
	
	//! push back list
	//! @param[in/out]	realserver_socket is realserver list push back element 
	void tcp_realserver_connect_socket_list::push_back(list_element realserver_socket){		
		rw_scoped_lock scope_lock(list_mutex);
		
		connect_list.push_back(realserver_socket);
	}
	
	//! get socket list
	//! @return			realserver list element
	tcp_realserver_connect_socket_list::list_element tcp_realserver_connect_socket_list::get_socket(){
        rw_scoped_lock scope_lock(list_mutex);
		
		list_element sock;
		if(!connect_list.empty()){
			sock = *(connect_list.begin());
			connect_list.pop_front();
		}
		return sock;
	}
	
	//! check empty list
	bool tcp_realserver_connect_socket_list::empty(){
        rd_scoped_lock scope_lock(list_mutex);
		
		bool bres = connect_list.empty();
		return bres;
	}
	
	//! clear list
	void tcp_realserver_connect_socket_list::clear(){
        rw_scoped_lock scope_lock(list_mutex);
		connect_list.clear();
	}
	
}// namespace l7vs
 
