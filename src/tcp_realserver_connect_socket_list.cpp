#include "tcp_realserver_connect_socket_list.h"

namespace l7vs{

	tcp_realserver_connect_socket_list::tcp_realserver_connect_socket_list(){
	}
	tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list(){
	}
	
	void tcp_realserver_connect_socket_list::push_back(list_element realserver_socket){
		
	}
	
	tcp_realserver_connect_socket_list::list_element tcp_realserver_connect_socket_list::get_socket(){
		list_element& sock = *(connect_list.begin());
		return sock;
	}
	
	bool tcp_realserver_connect_socket_list::empty(){
		bool bres = connect_list.empty();
		return bres;
	}
	
}// namespace l7vs
 
