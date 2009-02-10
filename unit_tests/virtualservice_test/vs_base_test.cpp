
#include <iostream>
#include <sstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "stub.h"

#include "virtualservice.h"
#include "vs_test.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

using namespace boost::unit_test;

//test case1.create & destroy
void	virtualservice_base_test1(){
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	
	// unit_test[1]  object create
	BOOST_MESSAGE( "-------1" );
	l7vs::vs_test*	vs = new l7vs::vs_test( vsd, rep, element );

	BOOST_CHECK_EQUAL( &vsd, &(vs->get_vsd()) );
	BOOST_CHECK_EQUAL( &rep, &(vs->get_rep()) );
	BOOST_CHECK( element == vs->get_element() );

	// unit_test[2]  object destroy
	BOOST_MESSAGE( "-------2" );
	delete vs;
}

//test case2 method call1
void	virtualservice_base_test2(){
	l7vs::l7vsd					vsd;
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::virtualservice_element	element;
	//
	element.qos_upstream	= 1234ULL;
	element.qos_downstream	= 5678ULL;

	boost::shared_ptr<l7vs::vs_test>	vs( new l7vs::vs_test( vsd, rep, element ) );

	boost::system::error_code	test_err;
	l7vs::error_code			vs_err;

	// unit_test[3]  プロトコルモジュールをロードせずにreplication handlerを呼ぶ
	BOOST_MESSAGE( "-------3" );
	vs->call_handle_protomod_replication( test_err );
	// unit_test[4]  プロトコルモジュールをロードせずにget_protocol_moduleを呼ぶ
	BOOST_MESSAGE( "-------4" );
	BOOST_CHECK( NULL == vs->get_protocol_module() );

	// unit_test[5]  スケジュールモジュールをロードせずにreplication handlerを呼ぶ
	BOOST_MESSAGE( "-------5" );
	vs->call_handle_schedmod_replication( test_err );
	// unit_test[6]  プロトコルモジュールをロードせずにget_schedule_moduleを呼ぶ
	BOOST_MESSAGE( "-------6" );
	BOOST_CHECK( NULL == vs->get_schedule_module() );

	//派生先のinitialize()でモジュールがロードされる
	vs->initialize( vs_err );
	// unit_test[7]  プロトコルモジュールをロード後にreplication handlerを呼ぶ
	BOOST_MESSAGE( "-------7" );
	vs->call_handle_protomod_replication( test_err );
	// unit_test[8]  プロトコルモジュールをロード後にget_protocol_moduleを呼ぶ
	BOOST_MESSAGE( "-------8" );
	BOOST_CHECK( NULL != vs->get_protocol_module() );
	BOOST_CHECK( "PMtest1" == vs->get_protocol_module()->get_name() );

	// unit_test[9]  スケジュールモジュールをロード後にreplication handlerを呼ぶ
	BOOST_MESSAGE( "-------9" );
	vs->call_handle_schedmod_replication( test_err );
	// unit_test[10]  スケジュールモジュールをロード後にget_schedule_moduleを呼ぶ
	BOOST_MESSAGE( "-------10" );
	BOOST_CHECK( NULL != vs->get_schedule_module() );
	BOOST_CHECK( "SMtest1" == vs->get_schedule_module()->get_name() );

	// unit_test[11]  rs_list_lockメソッドを呼ぶと、参照カウントがカウントアップされることを確認
	BOOST_MESSAGE( "-------11" );
	vs->rs_list_lock();
	BOOST_CHECK( 1 == vs->get_refcount() );
	// unit_test[12]  rs_list_unlockメソッドを呼ぶと、参照カウントがカウントダウンされることを確認
	BOOST_MESSAGE( "-------12" );
	vs->rs_list_unlock();
	BOOST_CHECK( 0 == vs->get_refcount() );

	// unit_test[13]  rs_list_lockメソッド上限境界値(ULLONG_MAX + 1)
	BOOST_MESSAGE( "-------13" );
	vs->set_refcount( ULLONG_MAX );
	vs->rs_list_lock();
	std::cout << vs->get_refcount() << std::endl;
	BOOST_CHECK( 0ULL == vs->get_refcount() );

	// unit_test[14]  rs_list_lockメソッド下限境界値(ULLONG_MIN(=0) - 1)
	BOOST_MESSAGE( "-------14" );
	vs->set_refcount( 0 );
	vs->rs_list_unlock();
	std::cout << vs->get_refcount() << std::endl;
	BOOST_CHECK( ULLONG_MAX == vs->get_refcount() );

	// unit_test[15]  get_qos_upstreamメソッド(element.qos_upstreamと同じ値であることを確認)
	BOOST_MESSAGE( "-------15" );
	BOOST_CHECK( 1234ULL == vs->get_qos_upstream() );
	// unit_test[16]  get_qos_downstreamメソッド(element.qos_downstreamと同じ値であることを確認)
	BOOST_MESSAGE( "-------16" );
	BOOST_CHECK( 5678ULL == vs->get_qos_downstream() );

	// unit_test[17]  update_up_recv_sizeメソッド(Updateされることを確認)
	BOOST_MESSAGE( "-------17" );
	vs->update_up_recv_size( 12ULL );
	// unit_test[18]  get_up_recv_sizeメソッドで上り総受信サイズを取得
	BOOST_MESSAGE( "-------18" );
	BOOST_CHECK( 12ULL == vs->get_up_recv_size() );
	BOOST_MESSAGE( "up recv size : " << vs->get_up_recv_size() );
	// unit_test[19]  update_up_recv_sizeメソッド(以前の値に加算されることを確認)
	BOOST_MESSAGE( "-------18" );
	vs->update_up_recv_size( 28ULL );
	BOOST_CHECK( 40ULL == vs->get_up_recv_size() );
	BOOST_MESSAGE( "up recv size : " << vs->get_up_recv_size() );
	// unit_test[20]  update_up_recv_sizeメソッド上限境界値(加算するとULLONG_MAXを越える場合)
	BOOST_MESSAGE( "-------20" );
	vs->update_up_recv_size( ULLONG_MAX );
	BOOST_CHECK( 40ULL == vs->get_up_recv_size() );
	BOOST_MESSAGE( "up recv size : " << vs->get_up_recv_size() );

	// unit_test[21]  update_up_send_sizeメソッド(Updateされることを確認)
	BOOST_MESSAGE( "-------21" );
	vs->update_up_send_size( 1024ULL );
	// unit_test[22]  get_up_send_sizeメソッドで上り総送信サイズを取得
	BOOST_MESSAGE( "-------22" );
	BOOST_CHECK( 1024ULL == vs->get_up_send_size() );
	BOOST_MESSAGE( "up send size : " << vs->get_up_send_size() );
	// unit_test[23]  update_up_send_sizeメソッド(以前の値に加算されることを確認)
	BOOST_MESSAGE( "-------23" );
	vs->update_up_send_size( 2048ULL );
	BOOST_CHECK( 3072ULL == vs->get_up_send_size() );
	BOOST_MESSAGE( "up send size : " << vs->get_up_send_size() );
	// unit_test[24]  update_up_send_sizeメソッド上限境界値(加算するとULLONG_MAXを越える場合)
	BOOST_MESSAGE( "-------24" );
	vs->update_up_send_size( ULLONG_MAX );
	BOOST_CHECK( 3072ULL == vs->get_up_send_size() );
	BOOST_MESSAGE( "up send size : " << vs->get_up_send_size() );

	// unit_test[25]  update_down_recv_sizeメソッド(Updateされることを確認)
	BOOST_MESSAGE( "-------25" );
	vs->update_down_recv_size( 101010ULL );
	// unit_test[26]  get_down_recv_sizeメソッドで下り総受信サイズを取得
	BOOST_MESSAGE( "-------26" );
	BOOST_CHECK( 101010ULL == vs->get_down_recv_size() );
	BOOST_MESSAGE( "down recv size : " << vs->get_down_recv_size() );
	// unit_test[27]  update_down_recv_sizeメソッド(以前の値に加算されることを確認)
	BOOST_MESSAGE( "-------27" );
	vs->update_down_recv_size( 505050ULL );
	BOOST_CHECK( 606060ULL == vs->get_down_recv_size() );
	BOOST_MESSAGE( "down recv size : " << vs->get_down_recv_size() );
	// unit_test[28]  update_down_recv_sizeメソッド上限境界値(加算するとULLONG_MAXを越える場合)
	BOOST_MESSAGE( "-------28" );
	vs->update_down_recv_size( ULLONG_MAX );
	BOOST_CHECK( 606060ULL == vs->get_down_recv_size() );
	BOOST_MESSAGE( "down recv size : " << vs->get_down_recv_size() );

	// unit_test[29]  update_down_send_sizeメソッド(Updateされることを確認)
	BOOST_MESSAGE( "-------29" );
	vs->update_down_send_size( 123456789ULL );
	// unit_test[30]  get_down_send_sizeメソッドで下り総送信サイズを取得
	BOOST_MESSAGE( "-------30" );
	BOOST_CHECK( 123456789ULL == vs->get_down_send_size() );
	BOOST_MESSAGE( "down send size : " << vs->get_down_send_size() );
	// unit_test[31]  update_down_send_sizeメソッド(以前の値に加算されることを確認)
	BOOST_MESSAGE( "-------31" );
	vs->update_down_send_size( 987654321ULL );
	BOOST_CHECK( 1111111110ULL == vs->get_down_send_size() );
	BOOST_MESSAGE( "down send size : " << vs->get_down_send_size() );
	// unit_test[32]  update_down_send_sizeメソッド上限境界値(加算するとULLONG_MAXを越える場合)
	BOOST_MESSAGE( "-------32" );
	vs->update_down_send_size( ULLONG_MAX );
	BOOST_CHECK( 1111111110ULL == vs->get_down_send_size() );
	BOOST_MESSAGE( "down send size : " << vs->get_down_send_size() );

	// unit_test[33]  スループット計算のテスト
	BOOST_MESSAGE( "-------33" );
	//計算(curr_sizeクリアのため)
	vs->call_handle_throughput_update( test_err );
	//のぼり受信データサイズ更新
	vs->update_up_recv_size( 1000000000ULL );
	vs->update_down_recv_size( 2000000000ULL );
	//それから計算
	vs->call_handle_throughput_update( test_err );
	//コンソールに出して確認
	// unit_test[34]  get_throughput_upstreamメソッドで値を取得
	BOOST_MESSAGE( "-------34" );
	BOOST_MESSAGE( "bps up   : " << vs->get_throughput_upstream() );
	// unit_test[35]  get_throughput_downstreamメソッドで値を取得
	BOOST_MESSAGE( "-------35" );
	BOOST_MESSAGE( "bps down : " << vs->get_throughput_downstream() );

	// unit_test[36]  load_parameterでパラメータをロードする(パラメータが存在しないケース)
	BOOST_MESSAGE( "-------36" );
	debugg_flug_struct::getInstance().param_exist_flag() = false;
	vs->call_load_parameter();
	BOOST_CHECK( vs->get_param_data().session_pool_size == l7vs::virtualservice_base::SESSION_POOL_NUM_DEFAULT );

	// unit_test[37]  load_parameterでパラメータをロードする(パラメータが存在するケース)
	debugg_flug_struct::getInstance().param_exist_flag() = true;
	vs->call_load_parameter();
	BOOST_CHECK( vs->get_param_data().session_pool_size == SESSION_POOL_NUM_PARAM );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "virtualservice_base_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test1 ) );
	ts->add( BOOST_TEST_CASE( &virtualservice_base_test2 ) );

	framework::master_test_suite().add( ts );

	return 0;
}
