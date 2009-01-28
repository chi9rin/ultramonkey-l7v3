#ifndef VS_TEST
#define VS_TEST

#include "virtualservice.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

namespace l7vs{

class	vs_test : public virtualservice_base {
protected:
	void	handle_replication_interrupt( const boost::system::error_code& err ){
	}
	bool	read_replicationdata( vs_replication_data& in_rep_data ){
	}

public:
	vs_test(	const l7vsd& in_vsd,
				const replication& in_rep,
				const virtualservice_element& in_el ) : virtualservice_base( in_vsd, in_rep, in_el ){
	}
	~vs_test(){};

	void	initialize( error_code& err ){
		protomod.reset( protocol_module_control::getInstance().load_module( "PMtest1" ) );
		schedmod.reset( schedule_module_control::getInstance().load_module( "SMtest1" ) );
	}
	void	finalize( error_code& err ){
	}

	bool	operator==( const virtualservice_base& vs ){
	}
	bool	operator!=( const virtualservice_base& vs ){
	}

	void	set_virtualservice( const virtualservice_element& el, error_code& err ){
	}
	void	edit_virtualservice( const virtualservice_element& el, error_code& err ){
	}

	void	add_realserver( const virtualservice_element& el, error_code& err ){
	}
	void	edit_realserver( const virtualservice_element& el, error_code& err ){
	}
	void	del_realserver( const virtualservice_element& el, error_code& err ){
	}

	void	run(){
	}
	void	stop(){
	}

	void	connection_active( const boost::asio::ip::tcp::endpoint& ep ){
	}
	void	connection_inactive( const boost::asio::ip::tcp::endpoint& ep ){
	}
	void	release_session( const boost::thread::id in_id ){
	}

	//imprement for unit_test
	const l7vsd&		get_vsd(){ return vsd; }
	const replication&	get_rep(){ return rep; }

	void	call_handle_protomod_replication( const boost::system::error_code& err ){
		handle_protomod_replication( err );
	}
	void	call_handle_schedmod_replication( const boost::system::error_code& err ){
		handle_schedmod_replication( err );
	}

	unsigned long long	get_refcount(){
		return rs_list_ref_count;
	}
	void				set_refcount( unsigned long long in ){
		rs_list_ref_count = in;
	}
};

}

#endif
