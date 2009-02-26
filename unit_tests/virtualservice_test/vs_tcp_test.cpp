
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


//Acceptテスト用Client
void	client(){
	boost::system::error_code	b_err;

	boost::asio::io_service	dispatcher;
	boost::asio::ip::tcp::socket	sock( dispatcher );
	std::cout << "connect" << std::endl;
	sock.connect( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (60000) ) );

	sock.close( b_err );
}


//test case1  create,initialize,run,stop,finalize,destroy(normal case)
void	virtualservice_tcp_test1(){
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().param_exist_flag() = false;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep;
	l7vs::virtualservice_element	element;
	//set element value
	element.udpmode					= false;
	element.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
	element.udp_recv_endpoint		= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	element.realserver_vector.clear();
	element.protocol_module_name	= "PMtest1";
	element.schedule_module_name	= "SMtest1";
	element.protocol_args.clear();
	element.protocol_args.push_back( "testarg" );
	element.protocol_args.push_back( "testarg2" );
	element.sorry_maxconnection		= 1234LL;
	element.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
	element.sorry_flag				= false;
	element.qos_upstream			= 65535ULL;
	element.qos_downstream			= 32767ULL;
	
	// unit_test[1]  object create
	BOOST_MESSAGE( "-------1" );
	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, element );

	BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
	BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
	BOOST_CHECK( element == vs->get_element() );
	l7vs::error_code	vs_err;
	// unit_test[2]  initialize method call
	BOOST_MESSAGE( "-------2" );
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );

	vs->initialize( vs_err );
	BOOST_CHECK( vs_err == false );

	//protocol_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
	BOOST_CHECK( NULL != vs->get_protocol_module() );
	//schedule_module_controlのモジュールロードを呼んでいるか(モジュールロード正常終了)
	BOOST_CHECK( NULL != vs->get_schedule_module() );
	//session_poolを作成しているか(デフォルトサイズで作成)
	BOOST_CHECK( static_cast<unsigned int>(l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT)
				 == vs->get_pool_sessions().size() );

	// unit_test[3]  run method test
	BOOST_MESSAGE( "-------3" );
	boost::thread	vs_main( &l7vs::vs_tcp::run, vs );

	usleep( 1500000 );
	//1秒待って、ProtocolModule/ScheduleModuleのreplication_interruptが呼ばれることを確認
	//デフォルト設定は500msなので2回
	BOOST_MESSAGE( debugg_flug_struct::getInstance().get_pm_rep_count() );
	BOOST_MESSAGE( debugg_flug_struct::getInstance().get_sm_rep_count() );
	BOOST_CHECK( 2 == debugg_flug_struct::getInstance().get_pm_rep_count() );
	BOOST_CHECK( 2 == debugg_flug_struct::getInstance().get_sm_rep_count() );

	// unit_test[4]  stop method test
	BOOST_MESSAGE( "-------4" );
	vs->stop();
	usleep( 1000 );

	// unit_test[5]  release_session method test(stopすると、sessionスレッドからrelease_sessionが呼ばれる)
	BOOST_MESSAGE( "-------5" );
	BOOST_CHECK( vs->get_pool_sessions().size() == (l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-1) );
	BOOST_CHECK( vs->get_active_sessions().size() == 1 );
	vs->release_session( vs->get_active_sessions().begin()->second->get_upthread_id() );
	BOOST_CHECK( vs->get_pool_sessions().size() == static_cast<size_t>( l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT) );
	BOOST_CHECK( vs->get_active_sessions().size() == 0 );

	// unit_test[6]  stop method test(call twice)
	BOOST_MESSAGE( "-------6" );
	vs->stop();

	vs_main.join();

	// unit_test[7]  finalize method call
	BOOST_MESSAGE( "-------7" );
	vs->finalize( vs_err );
	BOOST_CHECK( vs_err == false );

	// unit_test[8]  finalizeを2回連続で呼んでみる
	BOOST_MESSAGE( "-------8" );
	vs->finalize( vs_err );
	BOOST_CHECK( vs_err == false );


	// unit_test[9]  object destroy
	BOOST_MESSAGE( "-------9" );
	delete vs;
}

//test case2 method call
void	virtualservice_tcp_test2(){
	l7vs::error_code	vs_err;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep;
	l7vs::virtualservice_element	element;
	//set element value



	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[10]  initialize method call(poolsize from parameter file:value = 512)
	BOOST_MESSAGE( "-------10" );
	debugg_flug_struct::getInstance().param_exist_flag() = true;
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;
	vs->initialize( vs_err );
	BOOST_CHECK( SESSION_POOL_NUM_PARAM == vs->get_pool_sessions().size() );
	BOOST_CHECK( vs_err == false );
	vs->finalize( vs_err );
	BOOST_CHECK( vs_err == false );
	delete vs;
	vs = NULL;

	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[11]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------11" );
	debugg_flug_struct::getInstance().param_exist_flag() = false;
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= true;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	vs->initialize( vs_err );
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( PROTOMOD_LOAD_ERROR_MSG == vs_err.get_message() );

	vs->finalize( vs_err );
	delete vs;

	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[12]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------12" );
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= true;
	BOOST_CHECK( NULL == vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	vs->initialize( vs_err );
	BOOST_CHECK( NULL != vs->get_protocol_module() );
	BOOST_CHECK( NULL == vs->get_schedule_module() );
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( SCHEDMOD_LOAD_ERROR_MSG == vs_err.get_message() );

	vs->finalize( vs_err );
	delete vs;

	vs = new l7vs::vs_tcp( vsd, rep, element );
	vs->initialize( vs_err );
	l7vs::vs_tcp*	vs2 = new l7vs::vs_tcp( vsd, rep, element );
	vs2->initialize( vs_err );
	// unit_test[13]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------13" );
	BOOST_CHECK( *vs == *vs2 );
	vs->finalize( vs_err );
	delete vs;
	vs2->finalize( vs_err );
	delete vs2;

	// unit_test[14]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------14" );
	element.udpmode = false;
	element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
	vs = new l7vs::vs_tcp( vsd, rep, element );
	vs->initialize( vs_err );

	element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
	vs2 = new l7vs::vs_tcp( vsd, rep, element );
	vs2->initialize( vs_err );

	BOOST_CHECK( *vs != *vs2 );
	vs->finalize( vs_err );
	delete vs;
	vs2->finalize( vs_err );
	delete vs2;

	element.udpmode = false;
	element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
	vs = new l7vs::vs_tcp( vsd, rep, element );
	vs->initialize( vs_err );

	// unit_test[15]  add_realserver method call
	BOOST_MESSAGE( "-------15" );
	l7vs::virtualservice_element	add_element;
	add_element.udpmode = false;
	add_element.tcp_accept_endpoint = element.tcp_accept_endpoint;
	for( unsigned int i = 0; i < 10; ++i ){
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) );
		add_element.realserver_vector.push_back( rs_elem );
	}
	vs->add_realserver( add_element, vs_err );
	std::list<l7vs::realserver>&	rslist = vs->get_rs_list();
	//RSの数が10であることを確認
	BOOST_CHECK( 10 == rslist.size() );
	//vs_errがfalseであることを確認
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_MESSAGE( element.tcp_accept_endpoint );
	BOOST_MESSAGE( add_element.tcp_accept_endpoint );
	//各要素が一致するか確認
	for( std::list<l7vs::realserver>::iterator	itr = rslist.begin();
		itr != rslist.end(); ++itr ){
		for( int i = 0; i <= 10; ++i ){
			if( 10 <= i )BOOST_ERROR( "error add_realserver : endpoint unmatch" );
			if( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) ){
				BOOST_CHECK( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) );
				break;
			}
		}
	}

	// unit_test[16]  add_realserver method call(VSのEndpointが違う場合はエラーが返ること)
	BOOST_MESSAGE( "-------16" );
	l7vs::virtualservice_element	add_err_element;
	add_err_element.tcp_accept_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (80) );
	l7vs::realserver_element		rs_adderr_elem;
	rs_adderr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.100.10" ), (8080) );
	add_err_element.realserver_vector.push_back( rs_adderr_elem );
	vs->add_realserver( add_err_element, vs_err );
	//RSの数が10のままであることを確認
	BOOST_CHECK( 10 == rslist.size() );
	//vs_errがtrueでメッセージが存在することを確認
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );

	// unit_test[17]  add_realserver method call(既にあるRS(エンドポイントが同じ)追加はエラーが返ること)
	BOOST_MESSAGE( "-------17" );
	add_err_element.tcp_accept_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
	rs_adderr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
	add_err_element.realserver_vector.clear();
	add_err_element.realserver_vector.push_back( rs_adderr_elem );
	vs->add_realserver( add_err_element, vs_err );
	//RSの数が10のままであることを確認
	BOOST_CHECK( 10 == rslist.size() );
	//vs_errがtrueでメッセージが存在することを確認
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );

	// unit_test[18]  del_realserver method call(VSのエンドポイントが異なる場合はエラーが返ること)
	BOOST_MESSAGE( "-------18" );
	l7vs::virtualservice_element	del_err_element;
	l7vs::realserver_element		rs_delerr_elem;
	del_err_element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (80) );
	rs_delerr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
	del_err_element.realserver_vector.clear();
	del_err_element.realserver_vector.push_back( rs_delerr_elem );
	vs->del_realserver( del_err_element, vs_err );
	//RSの数が10のままであることを確認
	BOOST_CHECK( 10 == rslist.size() );
	//vs_errがtrueでメッセージが存在することを確認
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );

	// unit_test[19]  del_realserver method call(一致するRSが無い場合はエラーが返ること)
	BOOST_MESSAGE( "-------19" );
	del_err_element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
	rs_delerr_elem.tcp_endpoint = tcp_ep_type( boost::asio::ip::address::from_string( "100.90.80.70" ), (60000) );
	del_err_element.realserver_vector.clear();
	del_err_element.realserver_vector.push_back( rs_delerr_elem );
	vs->del_realserver( del_err_element, vs_err );
	//RSの数が10のままであることを確認
	BOOST_CHECK( 10 == rslist.size() );
	//vs_errがtrueでメッセージが存在することを確認
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );

	// unit_test[20]  del_realserver method call
	//addしたものと同じものを指定して、全削除されることを確認
	BOOST_MESSAGE( "-------20" );
	vs->del_realserver( add_element, vs_err );
	//RSの数が0であることを確認
	BOOST_CHECK( 0 == rslist.size() );
	//vs_errがfalseでメッセージが存在しないことを確認
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( vs_err.get_message() == "" );

	// unit_test[21]  add_realserver method call(再度追加)
	BOOST_MESSAGE( "-------21" );
	vs->add_realserver( add_element, vs_err );
	//RSの数が10であることを確認
	BOOST_CHECK( 10 == rslist.size() );
	//各要素が一致するか確認
	for( std::list<l7vs::realserver>::iterator	itr = rslist.begin();
		itr != rslist.end(); ++itr ){
		for( int i = 0; i <= 10; ++i ){
			if( 10 <= i )BOOST_ERROR( "error add_realserver : endpoint unmatch" );
			if( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) ){
				BOOST_CHECK( itr->tcp_endpoint == tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (i+8080) ) );
				break;
			}
		}
	}

	// unit_test[22]  connection_active method call
	BOOST_MESSAGE( "-------22" );
	tcp_ep_type	ep = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
	vs->connection_active( ep );
	for( std::list<l7vs::realserver>::iterator	itr = rslist.begin();
		 itr != rslist.end(); ++itr ){
		if( itr->tcp_endpoint == ep ){
			BOOST_CHECK( 1 == itr->get_active() );
			break;
		}
	}
	// unit_test[23]  connection_inactive method call
	BOOST_MESSAGE( "-------23" );
	vs->connection_inactive( ep );
	for( std::list<l7vs::realserver>::iterator	itr = rslist.begin();
		 itr != rslist.end(); ++itr ){
		if( itr->tcp_endpoint == ep ){
			BOOST_CHECK( 0 == itr->get_active() );
			BOOST_CHECK( 1 == itr->get_inact() );
			break;
		}
	}

	vs->finalize( vs_err );
	delete vs;
}

//test case3
void	virtualservice_tcp_test3(){
	l7vs::error_code			vs_err;
	boost::system::error_code	test_err;
	debugg_flug_struct::getInstance().param_exist_flag() = false;
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;

	std::stringstream	tmp_tcp_ep;
	std::stringstream	tmp_udp_ep;
	std::stringstream	tmp_sorry_ep;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep;
	l7vs::virtualservice_element	element;
	//set element value

	element.udpmode					= false;
	element.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
	element.udp_recv_endpoint		= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	element.realserver_vector.clear();
	element.protocol_module_name	= "PMtest1";
	element.schedule_module_name	= "SMtest1";
	element.protocol_args.clear();
	element.sorry_maxconnection		= 1234LL;
	element.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
	element.sorry_flag				= false;
	element.qos_upstream			= 65535ULL;
	element.qos_downstream			= 32767ULL;

	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, element );
	vs->initialize( vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );

	// unit_test[24]  replication dataが作成できるか確認(初回作成)
	BOOST_MESSAGE( "-------24" );
	//まず、stubのレプリケーションエリア作成をする
	debugg_flug_struct::getInstance().create_rep_area();

	//レプリケーションデータ作成
	vs->call_handle_replication_interrupt( test_err );

	l7vs::virtualservice_base::replication_header*	rep_head =
		reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
	//data_numが0になってることを確認
	BOOST_CHECK( rep_head->data_num == 1 );
	l7vs::virtualservice_base::replication_data*	rep_data =
		reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
	//udpmode
	BOOST_CHECK( rep_data->udpmode == false );
	//tcp_endpoint
	tmp_tcp_ep << element.tcp_accept_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
	//udp_endpoint
	tmp_udp_ep << element.udp_recv_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
	//sorry_maxconnection
	BOOST_CHECK( rep_data->sorry_maxconnection == 1234LL );
	//sorry_endpoint
	tmp_sorry_ep << element.sorry_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
	//sorry_flag
	BOOST_CHECK( rep_data->sorry_flag == false );
	//qos_up
	BOOST_CHECK( rep_data->qos_up == 65535ULL );
	//qos_down
	BOOST_CHECK( rep_data->qos_down == 32767ULL );

	// unit_test[25]  edit_virtualserviceのテスト
	BOOST_MESSAGE( "-------25" );
	//変更前の値確認
	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) ) ==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) ) ==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest1" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 1234LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) ) == vs->get_element().sorry_endpoint );
	BOOST_CHECK( false == vs->get_element().sorry_flag );
	BOOST_CHECK( 65535ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 32767ULL == vs->get_element().qos_downstream );

	//変更値の設定
	l7vs::virtualservice_element	elem2;
	elem2.udpmode					= false;
	elem2.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
	elem2.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem2.realserver_vector.clear();
	elem2.protocol_module_name		= "PMtest1";
	elem2.schedule_module_name		= "SMtest2";
	elem2.protocol_args.clear();
	elem2.sorry_maxconnection		= 5678LL;
	elem2.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) );
	elem2.sorry_flag				= true;
	elem2.qos_upstream				= 1024ULL;
	elem2.qos_downstream			= 4096ULL;

	debugg_flug_struct::getInstance().smcontrol_err_flag() = false;
	vs->edit_virtualservice( elem2, vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );
	//変更後の確認
	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( elem2.tcp_accept_endpoint	==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( element.udp_recv_endpoint	==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
	BOOST_CHECK( true == vs->get_element().sorry_flag );
	BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

	// unit_test[26]  edit_virtualserviceのテスト(udpmodeが一致しないものはエラーが返る＆値が反映されない)
	BOOST_MESSAGE( "-------26" );
	l7vs::virtualservice_element	elem3;
	elem3.udpmode					= true;
	elem3.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
	elem3.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem3.realserver_vector.clear();
	elem3.protocol_module_name		= "PMtest1";
	elem3.schedule_module_name		= "SMtest2";
	elem3.protocol_args.clear();
	elem3.sorry_maxconnection		= 65535LL;
	elem3.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) );
	elem3.sorry_flag				= false;
	elem3.qos_upstream				= 1024ULL;
	elem3.qos_downstream			= 4096ULL;

	vs->edit_virtualservice( elem3, vs_err );
	//vs_errがtrueなのをチェック
	BOOST_CHECK( vs_err == true );

	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( elem2.tcp_accept_endpoint	==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( element.udp_recv_endpoint	==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
	BOOST_CHECK( true == vs->get_element().sorry_flag );
	BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

	// unit_test[27]  edit_virtualserviceのテスト(tcp_accept_endpointが一致しないものはエラーが返る＆値が反映されない)
	BOOST_MESSAGE( "-------27" );
	elem3.udpmode					= false;
	elem3.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (8080) );
	elem3.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem3.protocol_module_name		= "PMtest1";

	vs->edit_virtualservice( elem3, vs_err );
	//vs_errがtrueなのをチェック
	BOOST_CHECK( vs_err == true );

	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( elem2.tcp_accept_endpoint	==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( element.udp_recv_endpoint	==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
	BOOST_CHECK( true == vs->get_element().sorry_flag );
	BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

	// unit_test[28]  edit_virtualserviceのテスト(protocol_module_nameが一致しないものはエラーが返る＆値が反映されない)
	BOOST_MESSAGE( "-------28" );
	elem3.udpmode					= false;
	elem3.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) );
	elem3.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem3.protocol_module_name		= "PMtest2";

	vs->edit_virtualservice( elem3, vs_err );
	//vs_errがtrueなのをチェック
	BOOST_CHECK( vs_err == true );

	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( elem2.tcp_accept_endpoint	==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( element.udp_recv_endpoint	==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( elem2.sorry_endpoint == vs->get_element().sorry_endpoint );
	BOOST_CHECK( true == vs->get_element().sorry_flag );
	BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

	// unit_test[29]  edit_virtualserviceのテスト(sorry_endpointの変更無し)
	BOOST_MESSAGE( "-------29" );
	elem2.udpmode					= false;
	elem2.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
	elem2.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem2.realserver_vector.clear();
	elem2.protocol_module_name		= "PMtest1";
	elem2.schedule_module_name		= "SMtest2";
	elem2.protocol_args.clear();
	elem2.sorry_maxconnection		= 5678LL;
	elem2.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "0.0.0.0" ), (0) );
	elem2.sorry_flag				= true;
	elem2.qos_upstream				= 1024ULL;
	elem2.qos_downstream			= 4096ULL;

	vs->edit_virtualservice( elem2, vs_err );
	//vs_errがtrueなのをチェック
	BOOST_CHECK( vs_err == false );

	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( elem2.tcp_accept_endpoint	==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( element.udp_recv_endpoint	==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 5678LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) ) == vs->get_element().sorry_endpoint );
	BOOST_CHECK( true == vs->get_element().sorry_flag );
	BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

	// unit_test[30]  edit_virtualserviceのテスト(sorry_endpointクリア)
	BOOST_MESSAGE( "-------30" );
	elem2.udpmode					= false;
	elem2.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (60000) );
	elem2.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem2.realserver_vector.clear();
	elem2.protocol_module_name		= "PMtest1";
	elem2.schedule_module_name		= "SMtest2";
	elem2.protocol_args.clear();
	elem2.sorry_maxconnection		= 5678LL;
	elem2.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "255.255.255.255" ), (0) );
	elem2.sorry_flag				= true;
	elem2.qos_upstream				= 1024ULL;
	elem2.qos_downstream			= 4096ULL;

	vs->edit_virtualservice( elem2, vs_err );
	//vs_errがtrueなのをチェック
	BOOST_CHECK( vs_err == false );

	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( elem2.tcp_accept_endpoint	==  vs->get_element().tcp_accept_endpoint );
	BOOST_CHECK( element.udp_recv_endpoint	==  vs->get_element().udp_recv_endpoint );
	BOOST_CHECK( "PMtest1" == vs->get_element().protocol_module_name );
	BOOST_CHECK( "SMtest2" == vs->get_element().schedule_module_name );
	BOOST_CHECK( 0LL == vs->get_element().sorry_maxconnection );
	BOOST_CHECK( tcp_ep_type() == vs->get_element().sorry_endpoint );
	BOOST_CHECK( false == vs->get_element().sorry_flag );
	BOOST_CHECK( 1024ULL == vs->get_element().qos_upstream );
	BOOST_CHECK( 4096ULL == vs->get_element().qos_downstream );

	// unit_test[31]  replication dataが作成できるか確認(既にあるデータを上書き)
	BOOST_MESSAGE( "-------31" );
	vs->call_handle_replication_interrupt( test_err );
	rep_head =	reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
	//data_numが1になってることを確認
	BOOST_CHECK( rep_head->data_num == 1 );
	rep_data =	reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
	//udpmode
	BOOST_CHECK( rep_data->udpmode == false );
	//tcp_endpoint
	tmp_tcp_ep.str( "" );
	tmp_tcp_ep << elem2.tcp_accept_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
	//udp_endpoint
	tmp_udp_ep.str( "" );
	tmp_udp_ep << element.udp_recv_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
	//sorry_maxconnection
	BOOST_CHECK( rep_data->sorry_maxconnection == 0LL );
	//sorry_endpoint
	tmp_sorry_ep.str( "" );
	tmp_sorry_ep << tcp_ep_type();
	BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
	//sorry_flag
	BOOST_CHECK( rep_data->sorry_flag == false );
	//qos_up
	BOOST_CHECK( rep_data->qos_up == 1024ULL );
	//qos_down
	BOOST_CHECK( rep_data->qos_down == 4096ULL );

	// unit_test[32]  vsをもう一つ作って、そこからもレプリケーションデータを作成する
	BOOST_MESSAGE( "-------32" );
	elem3.udpmode					= false;
	elem3.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address_v4::loopback(), (65000) );
	elem3.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (40000) );
	elem3.realserver_vector.clear();
	elem3.protocol_module_name		= "PMtest1";
	elem3.schedule_module_name		= "SMtest1";
	elem3.protocol_args.clear();
	elem3.sorry_maxconnection		= 7890LL;
	elem3.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.86" ), (80) );
	elem3.sorry_flag				= false;
	elem3.qos_upstream				= 14142ULL;
	elem3.qos_downstream			= 22362ULL;


	l7vs::vs_tcp*	vs2 = new l7vs::vs_tcp( vsd, rep, elem3 );

	vs2->initialize( vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );

	//data_numが2になってることを確認
	vs2->call_handle_replication_interrupt( test_err );
	rep_head =	reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
	//data_numが2になってることを確認
	BOOST_CHECK( rep_head->data_num == 2 );
	rep_data =	reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
	++rep_data;
	//udpmode
	BOOST_CHECK( rep_data->udpmode == false );
	//tcp_endpoint
	tmp_tcp_ep.str( "" );
	tmp_tcp_ep << elem3.tcp_accept_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
	//udp_endpoint
	tmp_udp_ep.str( "" );
	tmp_udp_ep << elem3.udp_recv_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
	//sorry_maxconnection
	BOOST_CHECK( rep_data->sorry_maxconnection == 7890LL );
	//sorry_endpoint
	tmp_sorry_ep.str( "" );
	tmp_sorry_ep << elem3.sorry_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
	//sorry_flag
	BOOST_CHECK( rep_data->sorry_flag == false );
	//qos_up
	BOOST_CHECK( rep_data->qos_up == 14142ULL );
	//qos_down
	BOOST_CHECK( rep_data->qos_down == 22362ULL );

	//一端削除
	vs->finalize( vs_err );
	delete vs;

	// unit_test[33]  VSを削除したら、レプリケーションデータが読み出せないことを確認
	BOOST_MESSAGE( "-------33" );
	vs = new l7vs::vs_tcp( vsd, rep, element );
	vs->initialize( vs_err );

	//replication dataの確認
	vs->call_handle_replication_interrupt( test_err );
	vs2->call_handle_replication_interrupt( test_err );
	rep_head =	reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
	//data_numが2になってることを確認
	BOOST_CHECK( rep_head->data_num == 2 );
	rep_data =	reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
	//udpmode
	BOOST_CHECK( rep_data->udpmode == element.udpmode );
	//tcp_endpoint
	tmp_tcp_ep.str( "" );
	tmp_tcp_ep << element.tcp_accept_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
	//udp_endpoint
	tmp_udp_ep.str( "" );
	tmp_udp_ep << element.udp_recv_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
	//sorry_maxconnection
	BOOST_CHECK( rep_data->sorry_maxconnection == 1234LL );
	BOOST_MESSAGE( rep_data->sorry_maxconnection );
	//sorry_endpoint
	tmp_sorry_ep.str( "" );
	tmp_sorry_ep << tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) );
	BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
	//sorry_flag
	BOOST_CHECK( rep_data->sorry_flag == false );
	//qos_up
	BOOST_CHECK( rep_data->qos_up == 65535ULL );
	//qos_down
	BOOST_CHECK( rep_data->qos_down == 32767ULL );

	//古いデータの確認
	BOOST_MESSAGE( "sorry max_conn : " << vs->get_element().sorry_maxconnection );
	BOOST_CHECK( 1234LL == vs->get_element().sorry_maxconnection );
	BOOST_MESSAGE( "sorry endpoint : " << vs->get_element().sorry_endpoint );
	BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (8080) ) 
					== vs->get_element().sorry_endpoint );
	BOOST_MESSAGE( "sorry flag     : " << vs->get_element().sorry_flag );
	BOOST_CHECK( false == vs->get_element().sorry_flag );
	BOOST_MESSAGE( "QoS upstream   : " << vs->get_element().qos_upstream );
	BOOST_CHECK( 65535ULL == vs->get_element().qos_upstream );
	BOOST_MESSAGE( "QoS downstream : " << vs->get_element().qos_downstream );
	BOOST_CHECK( 32767ULL == vs->get_element().qos_downstream );


	//テストが終わったらStubのレプリケーションエリアを削除
	debugg_flug_struct::getInstance().create_rep_area();

	// unit_test[34]  edit_virtualserviceのテスト(schedule_moduleのロードに失敗するケース：致命的エラー)
	BOOST_MESSAGE( "-------34" );
	debugg_flug_struct::getInstance().smcontrol_err_flag() = true;
	elem2.schedule_module_name		= "SMtest3";
	vs->edit_virtualservice( elem2, vs_err );
	//vs_errがtrueなのをチェック
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs_err.get_message() );

	vs2->finalize( vs_err );
	delete vs2;
	vs->finalize( vs_err );
	delete vs;
}

//IPv6関係のテスト
void	virtualservice_tcp_test4(){
	debugg_flug_struct::getInstance().param_exist_flag() = false;
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;

	l7vs::error_code			vs_err;
	boost::system::error_code	test_err;

	std::stringstream	tmp_tcp_ep;
	std::stringstream	tmp_udp_ep;
	std::stringstream	tmp_sorry_ep;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep;
	l7vs::virtualservice_element	elem1;

	//set element value
	elem1.udpmode					= false;
	elem1.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
	elem1.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem1.realserver_vector.clear();
	elem1.protocol_module_name		= "PMtest1";
	elem1.schedule_module_name		= "SMtest1";
	elem1.protocol_args.clear();
	elem1.sorry_maxconnection		= 1234LL;
	elem1.sorry_endpoint			= 
			tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
	elem1.sorry_flag				= false;
	elem1.qos_upstream				= 65535ULL;
	elem1.qos_downstream			= 32767ULL;

	// unit_test[35]  IPv6アドレスでVS作成
	BOOST_MESSAGE( "-------35" );
	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, elem1 );
	BOOST_CHECK( vs_err == false );
	vs->initialize( vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );

	// unit_test[36]  IPv6アドレスでレプリケーションデータ作成
	//まず、stubのレプリケーションエリア作成をする
	debugg_flug_struct::getInstance().create_rep_area();
	BOOST_MESSAGE( "-------36" );
	vs->call_handle_replication_interrupt( test_err );
	l7vs::virtualservice_base::replication_header*	rep_head =	
		reinterpret_cast<l7vs::virtualservice_base::replication_header*>( debugg_flug_struct::getInstance().get_rep_area() );
	//data_numが0になってることを確認
	BOOST_CHECK( rep_head->data_num == 1 );
	l7vs::virtualservice_base::replication_data*	rep_data =	
		reinterpret_cast<l7vs::virtualservice_base::replication_data*>( ++rep_head );
	//udpmode
	BOOST_CHECK( rep_data->udpmode == false );
	//tcp_endpoint
	tmp_tcp_ep.str( "" );
	tmp_tcp_ep << elem1.tcp_accept_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->tcp_endpoint, tmp_tcp_ep.str().c_str(), 47 ) );
	//udp_endpoint
	tmp_udp_ep.str( "" );
	tmp_udp_ep << elem1.udp_recv_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->udp_endpoint, tmp_udp_ep.str().c_str(), 47 ) );
	//sorry_maxconnection
	BOOST_CHECK( rep_data->sorry_maxconnection == elem1.sorry_maxconnection );
	//sorry_endpoint
	tmp_sorry_ep.str( "" );
	tmp_sorry_ep << elem1.sorry_endpoint;
	BOOST_CHECK( 0 == strncmp( rep_data->sorry_endpoint, tmp_sorry_ep.str().c_str(), 47 ) );
	//sorry_flag
	BOOST_CHECK( rep_data->sorry_flag == elem1.sorry_flag );
	//qos_up
	BOOST_CHECK( rep_data->qos_up == elem1.qos_upstream );
	//qos_down
	BOOST_CHECK( rep_data->qos_down == elem1.qos_downstream );

	//比較用VS作成
	l7vs::vs_tcp*	vs2 = new l7vs::vs_tcp( vsd, rep, elem1 );
	bool	chkflg;
	// unit_test[37]  IPv6アドレスでoperator==のテスト(VSが一致するケース)
	BOOST_MESSAGE( "-------37" );
	chkflg = ( *vs == *vs2 );
	BOOST_CHECK( chkflg );
	// unit_test[38]  IPv6アドレスでoperator!=のテスト(VSが一致するケース)
	BOOST_MESSAGE( "-------38" );
	chkflg = ( *vs != *vs2 );
	BOOST_CHECK( !chkflg );
	delete vs2;
	vs2 = NULL;

	//比較用VS作成
	//set element-2
	l7vs::virtualservice_element	elem2;
	elem2.udpmode					= false;
	elem2.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (55000) );
	elem2.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem2.realserver_vector.clear();
	elem2.protocol_module_name		= "PMtest1";
	elem2.schedule_module_name		= "SMtest1";
	elem2.protocol_args.clear();
	elem2.sorry_maxconnection		= 1234LL;
	elem2.sorry_endpoint			= 
			tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
	elem2.sorry_flag				= false;
	elem2.qos_upstream				= 65535ULL;
	elem2.qos_downstream			= 32767ULL;
	vs2 = new l7vs::vs_tcp( vsd, rep, elem2 );
	// unit_test[39]  IPv6アドレスでoperator==のテスト(VSが一致しないケース)
	BOOST_MESSAGE( "-------39" );
	chkflg = ( *vs == *vs2 );
	BOOST_CHECK( !chkflg );
	// unit_test[40]  IPv6アドレスでoperator!=のテスト(VSが一致しないケース)
	BOOST_MESSAGE( "-------40" );
	chkflg = ( *vs != *vs2 );
	BOOST_CHECK( chkflg );
	delete vs2;
	vs2 = NULL;

	// unit_test[41]  IPv6アドレスでVS編集
	BOOST_MESSAGE( "-------41" );
	l7vs::virtualservice_element	elem3;
	elem3.udpmode					= false;
	elem3.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
	elem3.udp_recv_endpoint			= 
			udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem3.realserver_vector.clear();
	elem3.protocol_module_name		= "PMtest1";
	elem3.schedule_module_name		= "SMtest1";
	elem3.protocol_args.clear();
	elem3.sorry_maxconnection		= 3333LL;
	elem3.sorry_endpoint			= tcp_ep_type( boost::asio::ip::address::from_string( "255.255.255.255" ), (0) );
	elem3.sorry_flag				= true;
	elem3.qos_upstream				= 10ULL;
	elem3.qos_downstream			= 20ULL;

	vs->edit_virtualservice( elem3, vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( vs->get_element().sorry_endpoint == tcp_ep_type() );
	BOOST_CHECK( vs->get_element().sorry_maxconnection == 0LL );
	BOOST_CHECK( vs->get_element().sorry_flag == false );
	BOOST_CHECK( vs->get_element().qos_upstream == elem3.qos_upstream );
	BOOST_CHECK( vs->get_element().qos_downstream == elem3.qos_downstream );

	// unit_test[42]  IPv6アドレスでレプリケーションデータの読み出し：削除したら作りなおしたデータになること
	BOOST_MESSAGE( "-------42" );
	//一旦VSを削除
	vs->finalize( vs_err );
	delete vs;
	//elem3でVS作成
	vs = new l7vs::vs_tcp( vsd, rep, elem3 );
	BOOST_CHECK( vs_err == false );
	vs->call_read_replicationdata();
	BOOST_CHECK( vs->get_element().udpmode == elem3.udpmode );
	BOOST_CHECK( vs->get_element().tcp_accept_endpoint == elem3.tcp_accept_endpoint );
	BOOST_CHECK( vs->get_element().udp_recv_endpoint == elem3.udp_recv_endpoint );
	BOOST_CHECK( vs->get_element().protocol_module_name == elem3.protocol_module_name );
	BOOST_CHECK( vs->get_element().schedule_module_name == elem3.schedule_module_name );
	BOOST_CHECK( vs->get_element().sorry_maxconnection == elem3.sorry_maxconnection );
	BOOST_CHECK( vs->get_element().sorry_endpoint == elem3.sorry_endpoint );
	BOOST_CHECK( vs->get_element().sorry_flag == elem3.sorry_flag );
	BOOST_CHECK( vs->get_element().qos_upstream == elem3.qos_upstream );
	BOOST_CHECK( vs->get_element().qos_downstream == elem3.qos_downstream );

	// unit_test[43]  IPv6アドレスでVS変更(TCP endpoint不一致による失敗ケース)
	BOOST_MESSAGE( "-------43" );
	l7vs::virtualservice_element	elem4;
	elem4.udpmode					= false;
	elem4.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:BA00" ), (60000) );
	elem4.udp_recv_endpoint			= 
			udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem4.realserver_vector.clear();
	elem4.protocol_module_name		= "PMtest1";
	elem4.schedule_module_name		= "SMtest1";
	elem4.protocol_args.clear();
	elem4.sorry_maxconnection		= 3333LL;
	elem4.sorry_endpoint			= tcp_ep_type();
	elem4.sorry_flag				= true;
	elem4.qos_upstream				= 65535ULL;
	elem4.qos_downstream			= 32767ULL;

	vs->edit_virtualservice( elem4, vs_err );
	BOOST_CHECK( vs_err == true );
	BOOST_MESSAGE( vs->get_element().sorry_endpoint );
	BOOST_CHECK( vs->get_element().sorry_endpoint == elem3.sorry_endpoint );
	BOOST_MESSAGE( vs->get_element().sorry_maxconnection );
	BOOST_CHECK( vs->get_element().sorry_maxconnection == elem3.sorry_maxconnection );
	BOOST_MESSAGE( vs->get_element().sorry_flag );
	BOOST_CHECK( vs->get_element().sorry_flag == elem3.sorry_flag );
	BOOST_MESSAGE( vs->get_element().qos_upstream );
	BOOST_CHECK( vs->get_element().qos_upstream == elem3.qos_upstream );
	BOOST_MESSAGE( vs->get_element().qos_downstream );
	BOOST_CHECK( vs->get_element().qos_downstream == elem3.qos_downstream );

	// unit_test[44]  IPv6アドレスでRS追加
	BOOST_MESSAGE( "-------44" );
	//RSパラメータ設定
	l7vs::virtualservice_element	elem_add_rs1;
	elem_add_rs1.udpmode					= false;
	elem_add_rs1.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
	elem_add_rs1.udp_recv_endpoint			= 
			udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem_add_rs1.realserver_vector.clear();
	elem_add_rs1.protocol_module_name		= "PMtest1";
	elem_add_rs1.schedule_module_name		= "SMtest1";
	elem_add_rs1.protocol_args.clear();
	elem_add_rs1.sorry_maxconnection		= 1234LL;
	elem_add_rs1.sorry_endpoint				= tcp_ep_type();
	elem_add_rs1.sorry_flag					= false;
	elem_add_rs1.qos_upstream				= 65535ULL;
	elem_add_rs1.qos_downstream				= 32767ULL;
	for( size_t i = 0; i < 10; ++i ){
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
			tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (i+8080) );
		elem_add_rs1.realserver_vector.push_back( rs_elem );
		rs_elem.weight	= i+1;
	}
	vs->add_realserver( elem_add_rs1, vs_err );
	BOOST_CHECK( vs_err == false );

	// unit_test[45]  IPv6アドレスでRS追加(既に追加されているendpointを追加しようとして失敗ケース)
	BOOST_MESSAGE( "-------45" );
	//RSパラメータ設定
	l7vs::virtualservice_element	elem_add_rs2;
	{
		elem_add_rs2.udpmode					= false;
		elem_add_rs2.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
		elem_add_rs2.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_add_rs2.realserver_vector.clear();
		elem_add_rs2.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
		rs_elem.weight	= 10;
		elem_add_rs2.realserver_vector.push_back( rs_elem );
	}
	vs->add_realserver( elem_add_rs2, vs_err );
	BOOST_CHECK( vs_err == true );

	// unit_test[46]  IPv6アドレスでRS追加(VSが不一致で失敗ケース)
	BOOST_MESSAGE( "-------46" );
	l7vs::virtualservice_element	elem_add_rs3;
	{
		elem_add_rs3.udpmode					= false;
		elem_add_rs3.tcp_accept_endpoint		= 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:0009" ), (60000) );
		elem_add_rs3.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_add_rs3.realserver_vector.clear();
		elem_add_rs3.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (6006) );
		rs_elem.weight	= 10;
		elem_add_rs3.realserver_vector.push_back( rs_elem );
	}
	vs->add_realserver( elem_add_rs2, vs_err );
	BOOST_CHECK( vs_err == true );

	// unit_test[47]  IPv6アドレスでRS編集
	BOOST_MESSAGE( "-------47" );
	l7vs::virtualservice_element	elem_edit_rs1;
	{
		elem_edit_rs1.udpmode					= false;
		elem_edit_rs1.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
		elem_edit_rs1.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_edit_rs1.realserver_vector.clear();
		elem_edit_rs1.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
		rs_elem.weight	= 10;
		elem_edit_rs1.realserver_vector.push_back( rs_elem );
	}
	vs->edit_realserver( elem_edit_rs1, vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_CHECK( vs->get_rs_list().begin()->weight == 10 );

	// unit_test[48]  IPv6アドレスでRS編集(リストに一致するRSが存在しないので失敗する)
	BOOST_MESSAGE( "-------48" );
	l7vs::virtualservice_element	elem_edit_rs2;
	{
		elem_edit_rs2.udpmode					= false;
		elem_edit_rs2.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
		elem_edit_rs2.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_edit_rs2.realserver_vector.clear();
		elem_edit_rs2.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:3891" ), (8080) );
		rs_elem.weight	= 10;
		elem_edit_rs2.realserver_vector.push_back( rs_elem );
	}
	vs->edit_realserver( elem_edit_rs2, vs_err );
	BOOST_CHECK( vs_err == true );

	// unit_test[49]  IPv6アドレスでRS編集(VSが不一致で失敗する)
	BOOST_MESSAGE( "-------49" );
	l7vs::virtualservice_element	elem_edit_rs3;
	{
		elem_edit_rs3.udpmode					= false;
		elem_edit_rs3.tcp_accept_endpoint		= 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:0009" ), (60000) );
		elem_edit_rs3.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_edit_rs3.realserver_vector.clear();
		elem_edit_rs3.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
		rs_elem.weight	= 10;
		elem_edit_rs3.realserver_vector.push_back( rs_elem );
	}
	vs->edit_realserver( elem_edit_rs3, vs_err );
	BOOST_CHECK( vs_err == true );

	// unit_test[50]  IPv6アドレスでRS削除(VSが一致しないので失敗する)
	BOOST_MESSAGE( "-------50" );
	l7vs::virtualservice_element	elem_del_rs1;
	{
		elem_del_rs1.udpmode					= false;
		elem_del_rs1.tcp_accept_endpoint		= 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678:90AB:CDEF:0000:0000:FEDC:0009" ), (60000) );
		elem_del_rs1.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_del_rs1.realserver_vector.clear();
		elem_del_rs1.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
		rs_elem.weight	= 10;
		elem_del_rs1.realserver_vector.push_back( rs_elem );
	}
	vs->del_realserver( elem_del_rs1, vs_err );
	BOOST_CHECK( vs_err == true );

	// unit_test[51]  IPv6アドレスでRS削除(リストに一致するRSが存在しないので失敗する)
	BOOST_MESSAGE( "-------51" );
	l7vs::virtualservice_element	elem_del_rs2;
	{
		elem_del_rs2.udpmode					= false;
		elem_del_rs2.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
		elem_del_rs2.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_del_rs2.realserver_vector.clear();
		elem_del_rs2.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:1111:CDEF" ), (8080) );
		rs_elem.weight	= 10;
		elem_del_rs2.realserver_vector.push_back( rs_elem );
	}
	vs->del_realserver( elem_del_rs2, vs_err );
	BOOST_CHECK( vs_err == true );

	// unit_test[52]  IPv6アドレスでRS削除
	BOOST_MESSAGE( "-------52" );
	l7vs::virtualservice_element	elem_del_rs3;
	{
		elem_del_rs3.udpmode					= false;
		elem_del_rs3.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address_v6::loopback(), (50000) );
		elem_del_rs3.udp_recv_endpoint			= 
				udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
		elem_del_rs3.realserver_vector.clear();
		elem_del_rs3.protocol_module_name		= "PMtest1";
		l7vs::realserver_element	rs_elem;
		rs_elem.tcp_endpoint = 
				tcp_ep_type( boost::asio::ip::address::from_string( "1234:5678::0000:0000:90AB:CDEF" ), (8080) );
		rs_elem.weight	= 10;
		elem_del_rs3.realserver_vector.push_back( rs_elem );
	}
	vs->del_realserver( elem_del_rs3, vs_err );
	BOOST_CHECK( vs_err == false );
	BOOST_CHECK( vs->get_rs_list().size() == 9 );

	//テストが終わったらStubのレプリケーションエリアを削除
	debugg_flug_struct::getInstance().create_rep_area();

	vs->finalize( vs_err );
	delete vs;
}

//実際にAcceptさせるテスト
void	virtualservice_tcp_test5(){
	// unit_test[53]  ClientからConnectさせてAcceptすることを確認する
	BOOST_MESSAGE( "-------53" );
	debugg_flug_struct::getInstance().param_exist_flag() = false;
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;

	l7vs::error_code			vs_err;
	boost::system::error_code	test_err;

	std::stringstream	tmp_tcp_ep;
	std::stringstream	tmp_udp_ep;
	std::stringstream	tmp_sorry_ep;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep;
	l7vs::virtualservice_element	elem1;

	//set element value
	elem1.udpmode					= false;
	elem1.tcp_accept_endpoint		= 
			tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.87" ), (60000) );
	elem1.udp_recv_endpoint			= udp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (50000) );
	elem1.realserver_vector.clear();
	elem1.protocol_module_name		= "PMtest1";
	elem1.schedule_module_name		= "SMtest1";
	elem1.protocol_args.clear();
	elem1.sorry_maxconnection		= 1234LL;
	elem1.sorry_endpoint			= 
			tcp_ep_type( boost::asio::ip::address::from_string( "AFAF:0E08::8FDE" ), (8080) );
	elem1.sorry_flag				= false;
	elem1.qos_upstream				= 65535ULL;
	elem1.qos_downstream			= 32767ULL;

	//vs作成
	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, elem1 );
	vs->initialize( vs_err );

	boost::thread	vs_main( &l7vs::vs_tcp::run, vs );
	boost::thread	cl_thread( &client );

	//2秒待ってsessionプールのサイズをチェック
	usleep( 2000000 );
	BOOST_CHECK( vs->get_pool_sessions().size() == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT-2 );

	debugg_flug_struct::getInstance().stub_loglevel() = l7vs::LOG_LV_DEBUG;
	//1秒待ってmainをSTOP
	usleep( 1000000 );
	vs->stop();
std::cout << "!" << std::endl;

	usleep( 1000 );
	cl_thread.join();
	vs_main.join();
std::cout << "!" << std::endl;

	vs->finalize( vs_err );
std::cout << "!" << std::endl;
	delete vs;
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
/*	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test1 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test2 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test3 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test4 ) );*/
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test5 ) );

	framework::master_test_suite().add( ts );

	return 0;
}
