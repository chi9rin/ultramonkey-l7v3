
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

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//set element value
	element.protocol_module_name	= "pmtest1";
	element.schedule_module_name	= "smtest1";
	
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

	// unit_test[5]  stop method test(call twice)
	BOOST_MESSAGE( "-------5" );
	vs->stop();

	vs_main.join();

	// unit_test[6]  finalize method call
	BOOST_MESSAGE( "-------" );
	vs->finalize( vs_err );

	// unit_test[7]  object destroy
	BOOST_MESSAGE( "-------" );
	delete vs;
}

//test case2 method call
void	virtualservice_tcp_test2(){
	l7vs::error_code	vs_err;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//set element value



	l7vs::vs_tcp*	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[8]  initialize method call(poolsize from parameter file:value = 512)
	BOOST_MESSAGE( "-------" );
	debugg_flug_struct::getInstance().param_exist_flag() = true;
	debugg_flug_struct::getInstance().pmcontrol_err_flag()	= false;
	debugg_flug_struct::getInstance().smcontrol_err_flag()	= false;
	vs->initialize( vs_err );
	BOOST_CHECK( SESSION_POOL_NUM_PARAM == vs->get_pool_sessions().size() );
	BOOST_CHECK( vs_err == false );
	delete vs;


	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[9]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------" );
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

	delete vs;

	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[10]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------" );
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

	delete vs;

	vs = new l7vs::vs_tcp( vsd, rep, element );
	l7vs::vs_tcp*	vs2 = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[11]  initialize method call(procotol_module_control::module_load error case)
	BOOST_MESSAGE( "-------" );
	BOOST_CHECK( *vs == *vs2 );
	delete vs;
	delete vs2;
	// unit_test[12]  initialize method call(procotol_module_control::module_load error case)
	element.udpmode = false;
	element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
	vs = new l7vs::vs_tcp( vsd, rep, element );
	element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
	vs2 = new l7vs::vs_tcp( vsd, rep, element );
	BOOST_MESSAGE( "-------" );
	BOOST_CHECK( *vs != *vs2 );
	delete vs;
	delete vs2;

	element.udpmode = false;
	element.tcp_accept_endpoint	= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (80) );
	vs = new l7vs::vs_tcp( vsd, rep, element );
	// unit_test[13]  add_realserver method call
	BOOST_MESSAGE( "-------" );
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
	// unit_test[14]  add_realserver method call(VSのEndpointが違う場合はエラーが返ること)
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

	// unit_test[15]  add_realserver method call(既にあるRS(エンドポイントが同じ)追加はエラーが返ること)
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

	// unit_test[16]  del_realserver method call(VSのエンドポイントが異なる場合はエラーが返ること)
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

	// unit_test[17]  del_realserver method call(一致するRSが無い場合はエラーが返ること)
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

	// unit_test[18]  del_realserver method call
	//addしたものと同じものを指定して、全削除されることを確認
	BOOST_MESSAGE( "-------" );
	vs->del_realserver( add_element, vs_err );
	//RSの数が0であることを確認
	BOOST_CHECK( 0 == rslist.size() );
	//vs_errがfalseでメッセージが存在しないことを確認
	BOOST_CHECK( vs_err == false );
	BOOST_MESSAGE( vs_err.get_message() );
	BOOST_CHECK( vs_err.get_message() == "" );

	// unit_test[19]  add_realserver method call(再度追加)
	BOOST_MESSAGE( "-------" );
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

	// unit_test[20]  connection_active method call
	tcp_ep_type	ep = tcp_ep_type( boost::asio::ip::address::from_string( "192.168.10.10" ), (8080) );
	vs->connection_active( ep );
	for( std::list<l7vs::realserver>::iterator	itr = rslist.begin();
		 itr != rslist.end(); ++itr ){
		if( itr->tcp_endpoint == ep ){
			BOOST_CHECK( 1 == itr->get_active() );
			break;
		}
	}
	// unit_test[21]  connection_inactive method call
	vs->connection_inactive( ep );
	for( std::list<l7vs::realserver>::iterator	itr = rslist.begin();
		 itr != rslist.end(); ++itr ){
		if( itr->tcp_endpoint == ep ){
			BOOST_CHECK( 0 == itr->get_active() );
			BOOST_CHECK( 1 == itr->get_inact() );
			break;
		}
	}

	delete vs;
}

//test case3
void	virtualservice_tcp_test3(){
	l7vs::error_code	vs_err;

	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//set element value

	element.udpmode					= false;
	element.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) );
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

	// unit_test[22]  replication dataが作成できるか確認(初回作成)
	//まず、stubのレプリケーションエリア作成をする
	debugg_flug_struct::getInstance().create_rep_area();
	//data_numが1になってることを確認
	
	//udpmode
	//tcp_endpoint
	//udp_endpoint
	//sorry_maxconnection
	//sorry_endpoint
	//sorry_flag
	//qos_up
	//qos_down

	// unit_test[23]  edit_virtualserviceのテスト
	//変更前の値確認
	BOOST_CHECK( false == vs->get_element().udpmode );
	BOOST_CHECK( tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) ) ==  vs->get_element().tcp_accept_endpoint );
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
	elem2.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) );
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

	// unit_test[24]  edit_virtualserviceのテスト(udpmodeが一致しないものはエラーが返る＆値が反映されない)
	l7vs::virtualservice_element	elem3;
	elem3.udpmode					= true;
	elem3.tcp_accept_endpoint		= tcp_ep_type( boost::asio::ip::address::from_string( "10.144.169.20" ), (60000) );
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

	// unit_test[25]  edit_virtualserviceのテスト(tcp_accept_endpointが一致しないものはエラーが返る＆値が反映されない)
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

	// unit_test[26]  edit_virtualserviceのテスト(protocol_module_nameが一致しないものはエラーが返る＆値が反映されない)
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

	// unit_test[]  replication dataが作成できるか確認(既にあるデータを上書き)
	//data_numが1になってることを確認
	//udpmode
	//tcp_endpoint
	//udp_endpoint
	//sorry_maxconnection
	//sorry_endpoint
	//sorry_flag
	//qos_up
	//qos_down

	// unit_test[]  vsをもう一つ作って、そこからもレプリケーションデータを作成する
	l7vs::vs_tcp*	vs2 = new l7vs::vs_tcp( vsd, rep, element );

	//data_numが2になってることを確認
	//udpmode
	//tcp_endpoint
	//udp_endpoint
	//sorry_maxconnection
	//sorry_endpoint
	//sorry_flag
	//qos_up
	//qos_down


	//テストが終わったらStubのレプリケーションエリアを削除
	debugg_flug_struct::getInstance().create_rep_area();

	// unit_test[]  edit_virtualserviceのテスト(schedule_moduleのロードに失敗するケース：致命的エラー)
	debugg_flug_struct::getInstance().smcontrol_err_flag() = false;

	delete vs2;
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
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test2 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_tcp_test3 ) );

	framework::master_test_suite().add( ts );

	return 0;
}




