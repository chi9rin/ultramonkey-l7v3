
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "stub.h"
#include "virtualservice.h"
#include "vs_tcp.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

typedef	boost::asio::ip::tcp::endpoint	tcp_ep_type;
typedef	boost::asio::ip::udp::endpoint	udp_ep_type;

//test case1  create,initialize,run,stop,finalize,destroy(normal case)
void	virtualservice_tcp_test1(){
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().param_exist_flag() = false;

	l7vs::error_code			vs_err;
	boost::system::error_code	test_err;

	std::stringstream	tmp_tcp_ep;
	std::stringstream	tmp_udp_ep;
	std::stringstream	tmp_sorry_ep;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
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
	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, elem1 );
	vs->initialize( vs_err );


	vs->finalize( vs_err );
	delete vs;
}

void	session_pool_access_test(){
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
