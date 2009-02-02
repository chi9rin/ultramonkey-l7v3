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
#include "../../module/schedule/schedule_module_rr.cpp"

using namespace boost::unit_test;

#define	CHECK_NAME	"schedule round robin"
#define	SM1			"schedule_module_rr"

namespace l7vs{

class schedule_module_round_robin_fake : public schedule_module_round_robin
{
public:
	//!	constractor
	schedule_module_round_robin_fake(){}
	//! destractor
	~schedule_module_round_robin_fake(){}

	boost::asio::ip::tcp::endpoint	get_tcp_endpoint(){ return tcp_endpoint ; }
	boost::asio::ip::udp::endpoint	get_udp_endpoint(){ return udp_endpoint ; }
	void	set_tcp_endpoint( const boost::asio::ip::tcp::endpoint& in_tcp ){ tcp_endpoint = in_tcp ; }
	void	set_udp_endpoint( const boost::asio::ip::udp::endpoint& in_udp ){ udp_endpoint = in_udp ; }
};

}	//namespace l7vs


//l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type& list );
//l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type& list );
l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type* list );
l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type* list );
l7vs::schedule_module_base::rslist_type::iterator	list_next( l7vs::schedule_module_base::rslist_type::iterator itr );

//test case1.
void	schedule_module_test(){
	l7vs::schedule_module_control& control = l7vs::schedule_module_control::getInstance();
	l7vs::schedule_module_base* schedule_module_rr = NULL;

	// unit_test[1]  インスタンス作成のテスト
	BOOST_MESSAGE( "unit_test[1]" );
	control.initialize( "." );
	try{
//		schedule_module_rr = create_module();
		schedule_module_rr = control.load_module( SM1 );
	}
	catch(...){
//		BOOST_ERROR( "exception : create_module" );
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( NULL != schedule_module_rr );

	// unit_test[2]  get_nameメソッドのテスト
	BOOST_MESSAGE( "unit_test[2]" );
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_rr->get_name() );

	l7vs::module_base::getloglevel_func_type	getloglevel = boost::bind( &( l7vs::Logger::getLogLevel ), l7vs::LOG_CAT_SCHEDULE );
	l7vs::module_base::logger_func_type			putLogFatal	= boost::bind( &( l7vs::Logger::putLogFatal ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogError	= boost::bind( &( l7vs::Logger::putLogError ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogWarn	= boost::bind( &( l7vs::Logger::putLogWarn ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogInfo	= boost::bind( &( l7vs::Logger::putLogInfo ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogDebug	= boost::bind( &( l7vs::Logger::putLogDebug ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );

	schedule_module_rr->init_logger_functions(	getloglevel,
													putLogFatal,
													putLogError,
													putLogWarn,
													putLogInfo,
													putLogDebug);

	// unit_test[3]  initializeメソッドのテスト
	BOOST_MESSAGE( "unit_test[3]" );
	schedule_module_rr->initialize();

	boost::asio::ip::tcp::endpoint endpoint1( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;
	boost::asio::ip::udp::endpoint endpoint2( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;
	// unit_test[4]  initializeメソッドのテスト２
	BOOST_MESSAGE( "unit_test[4]" );
	// 派生クラスにgetter/setterを追加してprotected変数の確認
	l7vs::schedule_module_round_robin_fake	schedule_module_rr_fake ;
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_rr_fake.get_name() );

	schedule_module_rr_fake.set_tcp_endpoint( endpoint1 );
	schedule_module_rr_fake.set_udp_endpoint( endpoint2 );

	BOOST_CHECK( schedule_module_rr_fake.get_tcp_endpoint() == endpoint1 );
	BOOST_CHECK( schedule_module_rr_fake.get_udp_endpoint() == endpoint2 );

	schedule_module_rr_fake.initialize();
	BOOST_CHECK( schedule_module_rr_fake.get_tcp_endpoint() != endpoint1 );
	BOOST_CHECK( schedule_module_rr_fake.get_udp_endpoint() != endpoint2 );

	// unit_test[5]  is_tcpメソッドのテスト
	BOOST_MESSAGE( "unit_test[5]" );
	BOOST_CHECK_EQUAL( true, schedule_module_rr->is_tcp() );

	// unit_test[6]  is_udpメソッドのテスト
	BOOST_MESSAGE( "unit_test[6]" );
	BOOST_CHECK_EQUAL( true, schedule_module_rr->is_udp() );

	boost::thread::id							thread_id;
	l7vs::schedule_module_base::rslist_type		rs_list;
	l7vs::realserver	server1, server2, server3, server4;

	l7vs::schedule_module_base::rslist_iterator_begin_func_type		rslist_begin;
	l7vs::schedule_module_base::rslist_iterator_end_func_type		rslist_end;
	l7vs::schedule_module_base::rslist_iterator_next_func_type		rslist_next;

	// unit_test[7]  handle_schedule(tcp)メソッドのテスト
	BOOST_MESSAGE( "unit_test[7]" );
	boost::asio::ip::tcp::endpoint endpoint3 ;

	endpoint1 = endpoint3 ;

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[8]  handle_schedule(tcp)メソッドのテスト２
	BOOST_MESSAGE( "unit_test[8]" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );
	rslist_next = boost::bind( &list_next, _1 );

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[9]  handle_schedule(tcp)メソッドのテスト３
	BOOST_MESSAGE( "unit_test[9]" );
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

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[10]  handle_schedule(tcp)メソッドのテスト４
	BOOST_MESSAGE( "unit_test[10]" );
	rs_list.clear();
	server1.weight = 2;
	rs_list.push_back( server1 );
	server2.weight = 1;
	rs_list.push_back( server2 );
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[11]  handle_schedule(tcp)メソッドのテスト５
	BOOST_MESSAGE( "unit_test[11]" );
	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );

	// unit_test[12]  handle_schedule(tcp)メソッドのテスト６
	BOOST_MESSAGE( "unit_test[12]" );
	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[13]  handle_schedule(tcp)メソッドのテスト７
	BOOST_MESSAGE( "unit_test[13]" );
	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );


	// unit_test[14]  handle_schedule(udp)メソッドのテスト
	BOOST_MESSAGE( "unit_test[14]" );
	boost::asio::ip::udp::endpoint endpoint4 ;

	rslist_begin.clear();
	rslist_end.clear();
	rslist_next.clear();

	rs_list.clear();

	endpoint2 = endpoint4 ;

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[15]  handle_schedule(udp)メソッドのテスト２
	BOOST_MESSAGE( "unit_test[15]" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );
	rslist_next = boost::bind( &list_next, _1 );

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[16]  handle_schedule(udp)メソッドのテスト３
	BOOST_MESSAGE( "unit_test[16]" );
	rs_list.clear();
	server1.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.87" ), 22  ) ;
	server1.weight = 0;
	rs_list.push_back( server1 );
	server2.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21  ) ;
	server2.weight = 0;
	rs_list.push_back( server2 );
	server3.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.85" ), 20  ) ;
	server3.weight = 0;
	rs_list.push_back( server3 );
	server4.udp_endpoint = boost::asio::ip::udp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.84" ), 19  ) ;
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[17]  handle_schedule(udp)メソッドのテスト４
	BOOST_MESSAGE( "unit_test[17]" );
	rs_list.clear();
	server1.weight = 2;
	rs_list.push_back( server1 );
	server2.weight = 1;
	rs_list.push_back( server2 );
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server1.udp_endpoint == endpoint2 );

	// unit_test[18]  handle_schedule(udp)メソッドのテスト５
	BOOST_MESSAGE( "unit_test[18]" );
	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server2.udp_endpoint == endpoint2 );

	// unit_test[19]  handle_schedule(udp)メソッドのテスト６
	BOOST_MESSAGE( "unit_test[19]" );
	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[20]  handle_schedule(udp)メソッドのテスト７
	BOOST_MESSAGE( "unit_test[20]" );
	schedule_module_rr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server1.udp_endpoint == endpoint2 );


	// unit_test[21]  replication_interruptメソッドのテスト
	BOOST_MESSAGE( "unit_test[21]" );
	schedule_module_rr->replication_interrupt();

//	destroy_module( schedule_module_rr );
	control.unload_module( schedule_module_rr );
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
	test_suite* ts = BOOST_TEST_SUITE( "schedule_module_rr_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &schedule_module_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

