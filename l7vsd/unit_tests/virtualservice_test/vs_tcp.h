#ifndef VS_TEST
#define VS_TEST

#include <boost/serialization/list.hpp>
#include "virtualservice.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

namespace l7vs{

class	vs_tcp : public virtualservice_tcp {
public:
	vs_tcp(	const l7vsd&					in_vsd,
			const replication&				in_rep,
			const virtualservice_element&	in_element ) : virtualservice_tcp( in_vsd, in_rep, in_element ){}

	//imprementation for UT
	const l7vsd&		get_vsd(){ return vsd; }
	const replication&	get_rep(){ return rep; }

	void	call_handle_replication_interrupt( const boost::system::error_code& in_err ){
		handle_replication_interrupt( in_err );
	}

	void	call_read_replicationdata(){
		read_replicationdata();
	}

//	void	call_handle_accept(	const session_thread_control_ptr,
//								const boost::system::error_code& );

	session_queue_type&	get_pool_sessions(){
		return	pool_sessions;
	}
	session_map_type&	get_active_sessions(){
		return	active_sessions;
	}
	session_map_type&	get_waiting_sessions(){
		return	waiting_sessions;
	}
	unsigned long long	get_sorry_sessions(){
		return	sorry_count.get();
	}
	std::list<realserver>&	get_rs_list(){
		return rs_list;
	}
	void	release_session( const tcp_session* session_ptr ){
		std::cout << "release_session called." << std::endl;
		virtualservice_tcp::release_session( session_ptr );
	}
	unsigned long long	get_ref_count(){
		return rs_list_ref_count.get();
// 		return rs_list_ref_count;
	}

	bool& get_defer_accept_opt(){
		return defer_accept_opt;
	};
	int& get_defer_accept_val(){
		return defer_accept_val;
	};
	l7vs::tcp_socket::tcp_socket_option_info& get_set_sock_opt(){
		return set_sock_opt;
	};
	boost::asio::ip::tcp::acceptor& get_acceptor(){
		return acceptor_;
	};
	l7vs::protocol_module_base::check_message_result parse_socket_option_test(std::vector<std::string>& args){
		return l7vs::virtualservice_tcp::parse_socket_option(args);
	};
};

}

#endif
