#include <iostream>
#include <dlfcn.h>
#include <boost/test/included/unit_test.hpp>
#include <boost/function.hpp>
#include "schedule_module_base.h"
#include "logger.h"
#include "parameter.h"

#include "logger_stub.cpp"
#include "parameter_stub.cpp"

#include "schedule_module_control.h"
#include "../../module/schedule/schedule_module_lc.cpp"

using namespace boost::unit_test;

#define	CHECK_NAME	"schedule least connection"
#define	SM1			"schedule_module_lc"

//l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type& list );
//l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type& list );
l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type* list );
l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type* list );
l7vs::schedule_module_base::rslist_type::iterator	list_next( l7vs::schedule_module_base::rslist_type::iterator itr );

//test case1.
void	schedule_module_test(){
	l7vs::schedule_module_control& control = l7vs::schedule_module_control::getInstance();
	l7vs::schedule_module_base* schedule_module_lc = NULL;

	// unit_test[1]  インスタンス作成のテスト
	BOOST_MESSAGE( "unit_test[1]" );
	control.initialize( "." );
	try{
//		schedule_module_lc = create_module();
		schedule_module_lc = control.load_module( SM1 );
	}
	catch(...){
//		BOOST_ERROR( "exception : create_module" );
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( NULL != schedule_module_lc );

	// unit_test[2]  get_nameメソッドのテスト
	BOOST_MESSAGE( "unit_test[2]" );
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_lc->get_name() );

	l7vs::module_base::getloglevel_func_type	getloglevel = boost::bind( &( l7vs::Logger::getLogLevel ), l7vs::LOG_CAT_SCHEDULE );
	l7vs::module_base::logger_func_type			putLogFatal	= boost::bind( &( l7vs::Logger::putLogFatal ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogError	= boost::bind( &( l7vs::Logger::putLogError ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogWarn	= boost::bind( &( l7vs::Logger::putLogWarn ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogInfo	= boost::bind( &( l7vs::Logger::putLogInfo ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogDebug	= boost::bind( &( l7vs::Logger::putLogDebug ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );

	schedule_module_lc->init_logger_functions(	getloglevel,
													putLogFatal,
													putLogError,
													putLogWarn,
													putLogInfo,
													putLogDebug);

	// unit_test[3]  initializeメソッドのテスト
	BOOST_MESSAGE( "unit_test[3]" );
	schedule_module_lc->initialize();

	// unit_test[4]  is_tcpメソッドのテスト
	BOOST_MESSAGE( "unit_test[4]" );
	BOOST_CHECK_EQUAL( true, schedule_module_lc->is_tcp() );

	// unit_test[5]  is_udpメソッドのテスト(UDP非対応)
	BOOST_MESSAGE( "unit_test[5]" );
	BOOST_CHECK_EQUAL( false, schedule_module_lc->is_udp() );

	boost::thread::id							thread_id;
	l7vs::schedule_module_base::rslist_type		rs_list;
	l7vs::realserver	server1, server2, server3, server4;

	l7vs::schedule_module_base::rslist_iterator_begin_func_type		rslist_begin;
	l7vs::schedule_module_base::rslist_iterator_end_func_type		rslist_end;
	l7vs::schedule_module_base::rslist_iterator_next_func_type		rslist_next;

	// unit_test[6]  handle_schedule(tcp)メソッドのテスト(boost::functionのempty評価のため空のままイテレターメソッドを渡す　endpoint1は更新されない)
	BOOST_MESSAGE( "unit_test[6]" );
	boost::asio::ip::tcp::endpoint endpoint1, endpoint3 ;

	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[7]  handle_schedule(tcp)メソッドのテスト２(リストの内容が空　endpoint1は更新されない)
	BOOST_MESSAGE( "unit_test[7]" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );
	rslist_next = boost::bind( &list_next, _1 );

	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[8]  handle_schedule(tcp)メソッドのテスト３(リストの項目が全て振り分け無し　endpoint1は更新されない)
	BOOST_MESSAGE( "unit_test[8]" );
	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.87" ), 22  ) ;
	server1.weight = 0;
	rs_list.push_back( server1 );
	server2.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21  ) ;
	server2.weight = 0;
	rs_list.push_back( server2 );
	server3.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.85" ), 20  ) ;
	server3.weight = 0;
	rs_list.push_back( server3 );
	server4.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.84" ), 19  ) ;
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[9]  handle_schedule(tcp)メソッドのテスト４(重みが設定されているのでserver1が返る)
	BOOST_MESSAGE( "unit_test[9]" );
	rs_list.clear();
	server1.weight = 2;
	rs_list.push_back( server1 );
	server2.weight = 1;
	rs_list.push_back( server2 );
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[10]  handle_schedule(tcp)メソッドのテスト５(重みが設定されているので次に接続数の少ないserver2が返る)
	BOOST_MESSAGE( "unit_test[10]" );
	rs_list.clear();
	server1.increment_active();
	rs_list.push_back( server1 );
	rs_list.push_back( server2 );
	rs_list.push_back( server3 );
	rs_list.push_back( server4 );
	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );

	// unit_test[11]  handle_schedule(tcp)メソッドのテスト６(重みが設定されているので次に接続数の少ないserver3が返る)
	BOOST_MESSAGE( "unit_test[11]" );
	rs_list.clear();
	rs_list.push_back( server1 );
	server2.increment_active();
	rs_list.push_back( server2 );
	rs_list.push_back( server3 );
	rs_list.push_back( server4 );
	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[12]  handle_schedule(tcp)メソッドのテスト７(server4には重みがないので、一巡し重みが設定されているserver1が返る)
	BOOST_MESSAGE( "unit_test[12]" );
	rs_list.clear();
	rs_list.push_back( server1 );
	rs_list.push_back( server2 );
	server3.increment_active();
	rs_list.push_back( server3 );
	rs_list.push_back( server4 );
	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );


	// unit_test[13]  handle_schedule(udp)メソッドのテスト(UDPは非対応　endpoint2は更新されない)
	BOOST_MESSAGE( "unit_test[13]" );
	boost::asio::ip::udp::endpoint endpoint2, endpoint4 ;

	rs_list.clear();
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.87" ), 22  ) ;
	server1.weight = 2;
	rs_list.push_back( server1 );
	server2.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21  ) ;
	server2.weight = 1;
	rs_list.push_back( server2 );
	server3.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.85" ), 20  ) ;
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.84" ), 19  ) ;
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_lc->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );


	// unit_test[14]  replication_interruptメソッドのテスト
	BOOST_MESSAGE( "unit_test[14]" );
	schedule_module_lc->replication_interrupt();

//	destroy_module( schedule_module_lc );
	control.unload_module( schedule_module_lc );
	control.finalize();
}

//l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type& list ){
//	return(list.begin());
//}

//l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type& list ){
//	return(list.end());
//}

l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type* list ){
	return(list->begin());
}

l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type* list ){
	return(list->end());
}

l7vs::schedule_module_base::rslist_type::iterator	list_next( l7vs::schedule_module_base::rslist_type::iterator itr ){
	return(++itr);
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	l7vs::Logger	logger;
	l7vs::Parameter	parameter;

	logger.loadConf();

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "schedule_module_lc_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &schedule_module_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

