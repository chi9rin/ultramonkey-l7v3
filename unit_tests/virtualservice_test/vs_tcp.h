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

	bool	call_read_replicationdata( replication_data& in_rep ){
		return	read_replicationdata( in_rep );
	}

	void	call_handle_accept(	const session_thread_control_ptr,
								const boost::system::error_code& );

	session_map_type&	get_pool_sessions(){
		return	pool_sessions;
	}
	session_map_type&	get_active_sessions(){
		return	active_sessions;
	}
	std::list<realserver>&	get_rs_list(){
		return rs_list;
	}
};

}

#endif
