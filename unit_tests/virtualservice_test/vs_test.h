#ifndef VS_TEST
#define VS_TEST

#include "virtualservice.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"

namespace l7vs{

class	vs_test : public virtualservice_base {
protected:
	boost::xtime	diffelencial_time;
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

	unsigned long long	get_up_recv_size(){ return recvsize_up; }
	unsigned long long	get_up_send_size(){ return sendsize_up; }
	unsigned long long	get_down_recv_size(){ return recvsize_down; }
	unsigned long long	get_down_send_size(){ return sendsize_down; }

	unsigned long long	get_curr_up_recv_size(){
		return current_up_recvsize;
	}
	unsigned long long	get_curr_down_recv_size(){
		return current_down_recvsize;
	}
	void	call_handle_throughput_update( const boost::system::error_code& err ){
		boost::xtime_get( &diffelencial_time, boost::TIME_UTC );
		handle_throughput_update( err );
		unsigned long long dif = diffelencial_time.sec;
		if( (ULLONG_MAX / 1000000000ULL) > diffelencial_time.sec ){
			dif = (dif * 1000000000ULL) + diffelencial_time.nsec;
			if( 0 < dif ) dif = dif / 1000ULL;
		}else dif = ULLONG_MAX;
		std::cout << dif << "[usec]" << std::endl;
	}
	void	test_replication(){
		//serialize出力
		virtualservice_base::vs_replication_data	a1;
		//set header data
		a1.header.udpflag		= true;
		a1.header.tcp_endpoint	= boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "192.168.10.10" ), 8080 );
		a1.header.udp_endpoint	= boost::asio::ip::udp::endpoint( boost::asio::ip::address::from_string( "192.168.10.20" ), 8080 );
		a1.header.bodytype		= 1U;
		a1.header.datasize		= ULLONG_MAX;
		//set body data
		a1.body.sorry_maxconnection	= LLONG_MAX;
		a1.body.sorry_endpoint		= boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "192.168.10.1" ), 80 );
		a1.body.sorry_flag			= true;
		a1.body.qos_up				= 0ULL;
		a1.body.qos_down			= 30ULL;
	
	
	
		std::stringstream	ss;
		boost::archive::text_oarchive	oa( ss );
		oa << (const l7vs::virtualservice_base::vs_replication_data&)a1;
	
		std::cout << ss.str() << std::endl;
		std::cout << ss.str().size() << std::endl;
	
	//serialize入力
		boost::archive::text_iarchive	ia( ss );
		l7vs::virtualservice_base::vs_replication_data	a2;
		ia >> a2;
		//check header data
		BOOST_CHECK( a2.header.udpflag == a1.header.udpflag );
		BOOST_CHECK( a2.header.tcp_endpoint == a1.header.tcp_endpoint );
		BOOST_CHECK( a2.header.udp_endpoint == a1.header.udp_endpoint );
		BOOST_CHECK( a2.header.bodytype == a1.header.bodytype );
		BOOST_CHECK( a2.header.datasize == a1.header.datasize );
		//check body data
		BOOST_CHECK( a2.body.sorry_maxconnection == a1.body.sorry_maxconnection );
		BOOST_CHECK( a2.body.sorry_endpoint == a1.body.sorry_endpoint );
		BOOST_CHECK( a2.body.sorry_flag == a1.body.sorry_flag );
		BOOST_CHECK( a2.body.qos_up == a1.body.qos_up );
		BOOST_CHECK( a2.body.qos_down == a1.body.qos_down );
	}
};

}

#endif
