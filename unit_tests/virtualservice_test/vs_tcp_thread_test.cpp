
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/thread.hpp>

#include "stub.h"
#include "virtualservice.h"
#include "vs_tcp.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

typedef	boost::asio::ip::tcp::endpoint	tcp_ep_type;
typedef	boost::asio::ip::udp::endpoint	udp_ep_type;


class	vs_access{
protected:
	boost::asio::io_service		dispatcher;

	l7vs::vs_tcp*				vs;
	l7vs::l7vsd					vsd;
	l7vs::replication*			rep;
	l7vs::error_code			vs_err;

	boost::condition			cond;
public:
	vs_access() : vs( NULL ), rep( NULL ) {}
	~vs_access(){}

	void	initialize( l7vs::virtualservice_element& in_elem ){
		rep	= new l7vs::replication( dispatcher );
		vs	= new l7vs::vs_tcp( vsd, *rep, in_elem );
	}

	void	finalize(){
		vs->finalize( vs_err );
		usleep(10);
		delete vs;
		vs = NULL;
		delete rep;
		rep = NULL;
	}

	void	start(){
		cond.notify_all();
	}

	void	step(){
		cond.notify_one();
	}

	l7vs::vs_tcp*	get_vs(){
		return vs;
	}

	void	rs_list_lock(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->rs_list_lock();
		std::cout << "" << std::endl;
	}
	void		rs_list_unlock(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->rs_list_unlock();

	}

	void		get_element(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_element();

	}

	void		get_qos_upstream(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_qos_upstream();

	}
	void		get_qos_downstream(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_qos_downstream();

	}
	void		get_throughput_upstream(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_throughput_upstream();

	}
	void		get_throughput_downstream(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_throughput_downstream();

	}
	void		get_up_recv_size(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_up_recv_size();

	}
	void		get_up_send_size(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_up_send_size();

	}
	void		get_down_recv_size(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_down_recv_size();

	}
	void		get_down_send_size(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_down_send_size();

	}

	void		update_up_recv_size( unsigned long long size ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->update_up_recv_size( size );

	}
	void		update_up_send_size( unsigned long long size ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->update_up_send_size( size );

	}
	void		update_down_recv_size( unsigned long long size ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->update_down_recv_size( size );

	}
	void		update_down_send_size( unsigned long long size ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->update_down_send_size( size );

	}

	void		get_protocol_module(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_protocol_module();

	}
	void		get_schedule_module(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->get_schedule_module();

	}

	void		vs_initialize(){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->initialize( vs_err );

	}
	void		vs_finalize(){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->finalize( vs_err );

		BOOST_MESSAGE( "finalize." << boost::this_thread::get_id() );
	}

	void		vs_eq( const l7vs::virtualservice_base* in_vs, bool* ret ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		*ret = vs->operator==(*in_vs);

	}
	void		vs_ne( const l7vs::virtualservice_base* in_vs, bool* ret ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		*ret = vs->operator!=(*in_vs);

	}

	void		set_virtualservice( const l7vs::virtualservice_element& elem ){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->set_virtualservice( elem, vs_err );

	}
	void		edit_virtualservice( const l7vs::virtualservice_element& elem ){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->edit_virtualservice( elem, vs_err );

	}

	void		add_realserver( const l7vs::virtualservice_element& elem ){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->add_realserver( elem, vs_err );

	}
	void		edit_realserver( const l7vs::virtualservice_element& elem ){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->edit_realserver( elem, vs_err );

	}
	void		del_realserver( const l7vs::virtualservice_element& elem ){
		l7vs::error_code	vs_err;
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->del_realserver( elem, vs_err );

	}

	void		run(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->run();

	}
	void		stop(){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->stop();

	}

	void		connection_active( const tcp_ep_type& ep ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->connection_active( ep );

	}
	void		connection_inactive( const tcp_ep_type& ep ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->connection_inactive( ep );

	}
	void		release_session( const boost::thread::id id ){
		boost::mutex	mtx;
		boost::mutex::scoped_lock	lk( mtx );
		cond.wait( lk );

		vs->release_session( id );

	}
};

//test case1  create,initialize,run,stop,finalize,destroy(normal case)
void	virtualservice_tcp_test1(){
	boost::asio::io_service		dispatcher;

	l7vs::l7vsd					vsd;
	l7vs::replication			rep( dispatcher );

	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().param_exist_flag() = false;

	boost::system::error_code	test_err;

	std::stringstream	tmp_tcp_ep;
	std::stringstream	tmp_udp_ep;
	std::stringstream	tmp_sorry_ep;

	l7vs::virtualservice_element	elem1;

	//set element value
	elem1.udpmode					= false;
	elem1.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) );
	elem1.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem1.realserver_vector.clear();
	elem1.protocol_module_name		= "PMtest1";
	elem1.schedule_module_name		= "SMtest1";
	elem1.protocol_args.clear();
	elem1.sorry_maxconnection		= 1234LL;
	elem1.sorry_endpoint			= tcp_ep_type();
	elem1.sorry_flag				= false;
	elem1.qos_upstream				= 65535ULL;
	elem1.qos_downstream			= 32767ULL;

	//vs作成
	vs_access	vst;

	vst.initialize( elem1 );

	//同一メソッドの多重アクセス


	// unit_test[1]  operator==に２スレッドから同時アクセス
	//比較用のVSを2個作成
	l7vs::virtualservice_element	elem2;
	elem2.udpmode					= false;
	elem2.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.30" ), (40000) );
	elem2.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem2.realserver_vector.clear();
	elem2.protocol_module_name		= "PMtest1";
	elem2.schedule_module_name		= "SMtest1";
	elem2.protocol_args.clear();
	elem2.sorry_maxconnection		= 1234LL;
	elem2.sorry_endpoint			= tcp_ep_type();
	elem2.sorry_flag				= false;
	elem2.qos_upstream				= 65535ULL;
	elem2.qos_downstream			= 32767ULL;

	l7vs::vs_tcp*	vs1 = new l7vs::vs_tcp( vsd, rep, elem1 );
	l7vs::vs_tcp*	vs2 = new l7vs::vs_tcp( vsd, rep, elem2 );
	bool	retbool1 = true;
	bool	retbool2 = true;

	BOOST_CHECK( vst.get_vs()->get_element().udpmode == elem1.udpmode );
	BOOST_CHECK( vst.get_vs()->get_element().tcp_accept_endpoint == elem1.tcp_accept_endpoint );
	BOOST_CHECK( vst.get_vs()->get_element().udp_recv_endpoint == elem1.udp_recv_endpoint );
	BOOST_CHECK( vst.get_vs()->get_element().protocol_module_name == elem1.protocol_module_name );

	{
		boost::thread	eq1( &vs_access::vs_eq, &vst, vs1, &retbool1 );
		boost::thread	eq2( &vs_access::vs_eq, &vst, vs2, &retbool2 );
	
		usleep( 1000 );
		vst.start();
		usleep( 1000 );
		eq1.join();
		eq2.join();
	}

	BOOST_CHECK( retbool1 );
	BOOST_CHECK( !retbool2 );

	// unit_test[2]  operator!=に２スレッドから同時アクセス
	{
		boost::thread	ne1( &vs_access::vs_ne, &vst, vs1, &retbool1 );
		boost::thread	ne2( &vs_access::vs_ne, &vst, vs2, &retbool2 );
	
		usleep( 1000 );
		vst.start();
		usleep( 1000 );
		ne1.join();
		ne2.join();
	}

	BOOST_CHECK( !retbool1 );
	BOOST_CHECK( retbool2 );

	// unit_test[3]  rs_list_lockに２スレッドから同時アクセス
	{
		boost::thread	lk1( &vs_access::rs_list_lock, &vst );
		boost::thread	lk2( &vs_access::rs_list_lock, &vst );

		usleep( 1000 );
		vst.start();
		usleep( 1000 );
		lk1.join();
		lk2.join();
	}
	BOOST_CHECK( vst.get_vs()->get_ref_count() == 2ULL );

	// unit_test[4]  rs_list_unlockに２スレッドから同時アクセス
	{
		boost::thread	ulk1( &vs_access::rs_list_unlock, &vst );
		boost::thread	ulk2( &vs_access::rs_list_unlock, &vst );

		usleep( 1000 );
		vst.start();
		usleep( 1000 );
		ulk1.join();
		ulk2.join();
	}
	BOOST_CHECK( vst.get_vs()->get_ref_count() == 0ULL );

	//get_elementに２スレッドから同時アクセス
	{
		boost::thread	get_elem1( &vs_access::get_element, &vst );
		boost::thread	get_elem2( &vs_access::get_element, &vst );

		usleep( 1000 );
		vst.start();
		usleep( 1000 );
		get_elem1.join();
		get_elem2.join();
	}
// 	BOOST_CHECK( vst.get_vs()->get_ref_count() == 0ULL );

	//runに２スレッドから同時アクセス
	//stopに２スレッドから同時アクセス

	//connection_activeに２スレッドから同時アクセス
	//connection_inactiveに２スレッドから同時アクセス
	//release_sessionに２スレッドから同時アクセス

	//get_qos_upstreamに２スレッドから同時アクセス
	//get_qos_downstreamに２スレッドから同時アクセス
	//get_throughput_upstreamに２スレッドから同時アクセス
	//get_throughput_downstreamに２スレッドから同時アクセス
	//get_up_recv_sizeに２スレッドから同時アクセス
	//get_up_send_sizeに２スレッドから同時アクセス
	//get_down_recv_sizeに２スレッドから同時アクセス
	//get_down_send_sizeに２スレッドから同時アクセス

	//update_up_recv_sizeに２スレッドから同時アクセス
	//update_up_send_sizeに２スレッドから同時アクセス
	//update_down_recv_sizeに２スレッドから同時アクセス
	//update_down_send_sizeに２スレッドから同時アクセス

	//get_protocol_moduleに２スレッドから同時アクセス
	//get_schedule_moduleに２スレッドから同時アクセス


	vst.finalize();
}

//組み合わせアクセステスト
void	virtualservice_tcp_test2(){
	//sessionがActiveになるときに、release_sessionされるケース
	//sessionがActiveになるときにfinalize
	//release_sessionされるときにfinalize
}

void	rslist_access_test(){
	//rs追加
	//add_rsとrs_list_lock
	//add_rsとrs_list_unlock
	//add_rsとconnection_active
	//add_rsとconnection_inactive
	//rs変更
	//edit_rsとrs_list_lock
	//edit_rsとrs_list_unlock
	//edit_rsとconnection_active
	//edit_rsとconnection_inactive
	//rs削除
	//del_rsとrs_list_lock
	//del_rsとrs_list_unlock
	//del_rsとconnection_active
	//del_rsとconnection_inactive
	//rs_list_lockを複数スレッドから同時に呼ぶ
	//rs_list_unlockを複数スレッドから同時に呼ぶ
	//rs_list_lockとrs_list_unlockを複数スレッドから同時に呼ぶ
	//connection_activeを複数スレッドから同時に呼ぶ
	//connection_inactiveを複数スレッドから同時に呼ぶ
	//connection_activeとconnection_inactiveを複数スレッドから同時に呼ぶ
	//rs_list_lockとconnection_activeを複数スレッドから同時に呼ぶ
	//rs_list_unlockとrs_list_unlockを複数スレッドから同時に呼ぶ
	//rs_list_lockとconnection_inactiveを複数スレッドから同時に呼ぶ
	//rs_list_unlockとconnection_activeを複数スレッドから同時に呼ぶ
}

void	element_access_test(){
	//get_qos_upを複数スレッドから同時に呼ぶ
	//get_qos_downを複数スレッドから同時に呼ぶ
	//get_qos_upとget_qos_downを複数スレッドから同時に呼ぶ
}

void	datasize_access_test(){
	//update_up_recvを複数スレッドから同時に呼ぶ
	//update_up_sendを複数スレッドから同時に呼ぶ
	//update_down_recvを複数スレッドから同時に呼ぶ
	//update_down_sendを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_up_recvを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_up_sendを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_down_recvを複数スレッドから同時に呼ぶ
	//update_throughputとupdate_down_sendを複数スレッドから同時に呼ぶ
	//update_throughputとget_throughput_upを複数スレッドから同時に呼ぶ
	//update_throughputとget_throughput_downを複数スレッドから同時に呼ぶ
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test1 ) );

	framework::master_test_suite().add( ts );

	return 0;
}
