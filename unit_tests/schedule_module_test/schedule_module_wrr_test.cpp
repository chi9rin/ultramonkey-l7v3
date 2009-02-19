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
#include "../../module/schedule/schedule_module_wrr.cpp"

using namespace boost::unit_test;

#define	CHECK_NAME	"schedule weighted round robin"
#define	SM1			"schedule_module_wrr"

namespace l7vs{

class schedule_module_weighted_round_robin_fake : public schedule_module_weighted_round_robin
{
public:
	//!	constractor
	schedule_module_weighted_round_robin_fake(){}
	//! destractor
	~schedule_module_weighted_round_robin_fake(){}

	boost::asio::ip::tcp::endpoint	get_tcp_endpoint(){ return tcp_endpoint ; }
	boost::asio::ip::udp::endpoint	get_udp_endpoint(){ return udp_endpoint ; }
	void	set_tcp_endpoint( const boost::asio::ip::tcp::endpoint& in_tcp ){ tcp_endpoint = in_tcp ; }
	void	set_udp_endpoint( const boost::asio::ip::udp::endpoint& in_udp ){ udp_endpoint = in_udp ; }

	int 	get_currentWeight(){ return vs_weights.currentWeight ; }
    int		get_maxWeight(){ return vs_weights.maxWeight ; }
    int		get_gcd(){ return vs_weights.gcd ; }
	void 	set_currentWeight( const int in_weight ){ vs_weights.currentWeight = in_weight ; }
    void	set_maxWeight( const int in_weight ){ vs_weights.maxWeight = in_weight ; }
    void	set_gcd( const int in_gcd ){ vs_weights.gcd = in_gcd ; }

	void	handle_init ( rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){
		sched_wrr_service_init ( inlist_begin, inlist_end, inlist_next );
	}

	int		sched_wrr_service_init_wrapper(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){
		return sched_wrr_service_init( inlist_begin, inlist_end, inlist_next );
	}
	int		sched_wrr_getMaxWeight_wrapper(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){
		return sched_wrr_getMaxWeight( inlist_begin, inlist_end, inlist_next );
	}
	int		sched_wrr_gcd_wrapper( int a, int b ){
		return sched_wrr_gcd( a, b );
	}
	int		sched_wrr_getGCD_wrapper(
							rslist_iterator_begin_func_type		inlist_begin,
							rslist_iterator_end_func_type		inlist_end,
							rslist_iterator_next_func_type		inlist_next ){
		return sched_wrr_getGCD( inlist_begin, inlist_end, inlist_next );
	}

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
	l7vs::schedule_module_base* schedule_module_wrr = NULL;

	// unit_test[1]  インスタンス作成のテスト
	BOOST_MESSAGE( "unit_test[1]" );
	control.initialize( "." );
	try{
//		schedule_module_wrr = create_module();
		schedule_module_wrr = control.load_module( SM1 );
	}
	catch(...){
//		BOOST_ERROR( "exception : create_module" );
		BOOST_ERROR( "exception : load_module" );
	}
	BOOST_CHECK( NULL != schedule_module_wrr );

	// unit_test[2]  get_nameメソッドのテスト
	BOOST_MESSAGE( "unit_test[2]" );
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_wrr->get_name() );

	l7vs::module_base::getloglevel_func_type	getloglevel = boost::bind( &( l7vs::Logger::getLogLevel ), l7vs::LOG_CAT_SCHEDULE );
	l7vs::module_base::logger_func_type			putLogFatal	= boost::bind( &( l7vs::Logger::putLogFatal ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogError	= boost::bind( &( l7vs::Logger::putLogError ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogWarn	= boost::bind( &( l7vs::Logger::putLogWarn ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogInfo	= boost::bind( &( l7vs::Logger::putLogInfo ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );
	l7vs::module_base::logger_func_type			putLogDebug	= boost::bind( &( l7vs::Logger::putLogDebug ), l7vs::LOG_CAT_SCHEDULE, _1, _2, _3, _4 );

	// unit_test[3]  init_logger_functionsメソッドのテスト
	BOOST_MESSAGE( "unit_test[3]" );
	schedule_module_wrr->init_logger_functions(	getloglevel,
													putLogFatal,
													putLogError,
													putLogWarn,
													putLogInfo,
													putLogDebug);

	// unit_test[4]  initializeメソッドのテスト
	BOOST_MESSAGE( "unit_test[4]" );
	schedule_module_wrr->initialize();

	boost::asio::ip::tcp::endpoint endpoint1( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;
	boost::asio::ip::udp::endpoint endpoint2( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	// unit_test[5]  initializeメソッドのテスト２(派生クラスにgetter/setterを追加してprotected変数の確認)
	BOOST_MESSAGE( "unit_test[5]" );
	l7vs::schedule_module_weighted_round_robin_fake	schedule_module_wrr_fake ;
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_wrr_fake.get_name() );

	schedule_module_wrr_fake.set_tcp_endpoint( endpoint1 );
	schedule_module_wrr_fake.set_udp_endpoint( endpoint2 );

	schedule_module_wrr_fake.set_currentWeight( 5 );
	schedule_module_wrr_fake.set_maxWeight( 10 );
	schedule_module_wrr_fake.set_gcd( 1 );

	BOOST_CHECK( schedule_module_wrr_fake.get_tcp_endpoint() == endpoint1 );
	BOOST_CHECK( schedule_module_wrr_fake.get_udp_endpoint() == endpoint2 );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_currentWeight(), 5 );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_maxWeight(), 10 );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_gcd(), 1 );

	schedule_module_wrr_fake.initialize();
	BOOST_CHECK( schedule_module_wrr_fake.get_tcp_endpoint() != endpoint1 );
	BOOST_CHECK( schedule_module_wrr_fake.get_udp_endpoint() != endpoint2 );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_currentWeight(), 0 );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_maxWeight(), 0 );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_gcd(), 0 );

	// unit_test[6]  is_tcpメソッドのテスト
	BOOST_MESSAGE( "unit_test[6]" );
	BOOST_CHECK_EQUAL( true, schedule_module_wrr->is_tcp() );

	// unit_test[7]  is_udpメソッドのテスト
	BOOST_MESSAGE( "unit_test[7]" );
	BOOST_CHECK_EQUAL( true, schedule_module_wrr->is_udp() );

	boost::thread::id							thread_id;
	l7vs::schedule_module_base::rslist_type		rs_list;
	l7vs::realserver	server1, server2, server3, server4;

	l7vs::schedule_module_base::rslist_iterator_begin_func_type		rslist_begin;
	l7vs::schedule_module_base::rslist_iterator_end_func_type		rslist_end;
	l7vs::schedule_module_base::rslist_iterator_next_func_type		rslist_next;

	// unit_test[8]  handle_schedule(tcp)メソッドのテスト(boost::functionのempty評価のため空のままイテレターメソッドを渡す　endpoint1は更新されない)
	BOOST_MESSAGE( "unit_test[8]" );
	boost::asio::ip::tcp::endpoint endpoint3 ;

	endpoint1 = endpoint3 ;

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[9]  handle_schedule(tcp)メソッドのテスト２(リストの内容が空　endpoint1は更新されない)
	BOOST_MESSAGE( "unit_test[9]" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );
	rslist_next = boost::bind( &list_next, _1 );

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[10]  handle_schedule(tcp)メソッドのテスト３(リストの項目が全て振り分け無し　endpoint1は更新されない)
	BOOST_MESSAGE( "unit_test[10]" );
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

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[11]  handle_schedule(tcp)メソッドのテスト４(重みが設定されているので最大値のserver3が返る)
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

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[12]  handle_schedule(tcp)メソッドのテスト５(一巡し重みが設定されているのでserver1が返る)
	BOOST_MESSAGE( "unit_test[12]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[13]  handle_schedule(tcp)メソッドのテスト６(重みが設定されているので次のserver3が返る)
	BOOST_MESSAGE( "unit_test[13]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[14]  handle_schedule(tcp)メソッドのテスト７(一巡し重みが設定されているのでserver1が返る)
	BOOST_MESSAGE( "unit_test[14]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[15]  handle_schedule(tcp)メソッドのテスト８(重みが設定されているので次のserver2が返る)
	BOOST_MESSAGE( "unit_test[15]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );

	// unit_test[16]  handle_schedule(tcp)メソッドのテスト９(重みが設定されているので次のserver3が返る)
	BOOST_MESSAGE( "unit_test[16]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[17]  handle_schedule(tcp)メソッドのテスト１０(一巡し重みが設定されているので最大値のserver3が返る)
	BOOST_MESSAGE( "unit_test[17]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );


	// unit_test[18]  handle_schedule(udp)メソッドのテスト(boost::functionのempty評価のため空のままイテレターメソッドを渡す　endpoint2は更新されない)
	BOOST_MESSAGE( "unit_test[18]" );
	boost::asio::ip::udp::endpoint endpoint4 ;

	rslist_begin.clear();
	rslist_end.clear();
	rslist_next.clear();

	rs_list.clear();

	endpoint2 = endpoint4 ;

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[19]  handle_schedule(udp)メソッドのテスト２(リストの内容が空　endpoint2は更新されない)
	BOOST_MESSAGE( "unit_test[19]" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );
	rslist_next = boost::bind( &list_next, _1 );

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[20]  handle_schedule(udp)メソッドのテスト３(リストの項目が全て振り分け無し　endpoint2は更新されない)
	BOOST_MESSAGE( "unit_test[20]" );
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

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[21]  handle_schedule(udp)メソッドのテスト４(重みが設定されているので最大値のserver3が返る)
	BOOST_MESSAGE( "unit_test[21]" );
	rs_list.clear();
	server1.weight = 2;
	rs_list.push_back( server1 );
	server2.weight = 1;
	rs_list.push_back( server2 );
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.weight = 0;
	rs_list.push_back( server4 );

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[22]  handle_schedule(udp)メソッドのテスト５(一巡し重みが設定されているのでserver1が返る)
	BOOST_MESSAGE( "unit_test[22]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server1.udp_endpoint == endpoint2 );

	// unit_test[23]  handle_schedule(udp)メソッドのテスト６(重みが設定されているので次のserver3が返る)
	BOOST_MESSAGE( "unit_test[23]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[24]  handle_schedule(udp)メソッドのテスト７(一巡し重みが設定されているのでserver1が返る)
	BOOST_MESSAGE( "unit_test[24]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server1.udp_endpoint == endpoint2 );

	// unit_test[25]  handle_schedule(udp)メソッドのテスト８(重みが設定されているので次のserver2が返る)
	BOOST_MESSAGE( "unit_test[25]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server2.udp_endpoint == endpoint2 );

	// unit_test[26]  handle_schedule(udp)メソッドのテスト９(重みが設定されているので次のserver3が返る)
	BOOST_MESSAGE( "unit_test[26]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[27]  handle_schedule(udp)メソッドのテスト１０(一巡し重みが設定されているので最大値のserver3が返る)
	BOOST_MESSAGE( "unit_test[27]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[28]  replication_interruptメソッドのテスト
	BOOST_MESSAGE( "unit_test[28]" );
	schedule_module_wrr->replication_interrupt();

	// unit_test[29]  sched_wrr_getMaxWeightメソッドのテスト(最大値3)
	BOOST_MESSAGE( "unit_test[29]" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_getMaxWeight_wrapper( rslist_begin, rslist_end, rslist_next ), 3 );

	// unit_test[30]  sched_wrr_getGCDメソッドのテスト(最大公約数１)
	BOOST_MESSAGE( "unit_test[30]" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_getGCD_wrapper( rslist_begin, rslist_end, rslist_next), 1);

	// unit_test[31]  sched_wrr_getMaxWeightメソッドのテスト２(内部領域に格納されているか確認)
	BOOST_MESSAGE( "unit_test[31]" );
	schedule_module_wrr_fake.handle_init( rslist_begin, rslist_end, rslist_next );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_maxWeight(), 3 );

	// unit_test[32]  sched_wrr_getGCDメソッドのテスト２(内部領域に格納されているか確認)
	BOOST_MESSAGE( "unit_test[32]" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_gcd(), 1 );

	rslist_begin.clear();
	rslist_end.clear();
	rslist_next.clear();

	rs_list.clear();

	// unit_test[33]  sched_wrr_service_initメソッドのテスト(リストの内容が空)
	BOOST_MESSAGE( "unit_test[33]" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );
	rslist_next = boost::bind( &list_next, _1 );

	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_service_init_wrapper( rslist_begin, rslist_end, rslist_next ), -1 );
	
	// unit_test[34] sched_wrr_service_initメソッドのテスト２(リストの項目が全て振り分け無し)
	BOOST_MESSAGE( "unit_test[34]" );
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

	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_service_init_wrapper( rslist_begin, rslist_end, rslist_next ), -1 );

	// unit_test[35]  sched_wrr_service_initメソッドのテスト３(重みが設定されている)
	BOOST_MESSAGE( "unit_test[35]" );
	rs_list.clear();
	server1.weight = 2;
	rs_list.push_back( server1 );
	server2.weight = 1;
	rs_list.push_back( server2 );
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.weight = 0;
	rs_list.push_back( server4 );

	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_service_init_wrapper( rslist_begin, rslist_end, rslist_next ), 0 );

	// unit_test[36]  sched_wrr_gcdメソッドのテスト（最大公約数２）
	BOOST_MESSAGE( "unit_test[36]" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_gcd_wrapper( 2, 4 ), 2 );

	// unit_test[37]  sched_wrr_gcdメソッドのテスト２（最大公約数１）
	BOOST_MESSAGE( "unit_test[37]" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_gcd_wrapper( 1, 3 ), 1 );

	// unit_test[38]  sched_wrr_gcdメソッドのテスト３（最大公約数３）
	BOOST_MESSAGE( "unit_test[38]" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.sched_wrr_gcd_wrapper( 6, 3 ), 3 );


	// unit_test[x1]  handle_schedule(tcp)メソッドのテスト(重みが設定されているので最大値のserver2が返る)
	BOOST_MESSAGE( "unit_test[x1]" );
	rs_list.clear();
	server1.weight = 1;
	rs_list.push_back( server1 );
	server2.weight = 2;
	rs_list.push_back( server2 );

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );

	// unit_test[x2]  handle_schedule(tcp)メソッドのテスト2(一巡し重みが設定されているのでserver1が返る)
	BOOST_MESSAGE( "unit_test[x2]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[x3]  handle_schedule(tcp)メソッドのテスト3(重みが設定されているので次のserver2が返る)
	BOOST_MESSAGE( "unit_test[x3]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );

	// unit_test[x4]  handle_schedule(tcp)メソッドのテスト4(一巡し重みが設定されているので最大値のserver2が返る)
	BOOST_MESSAGE( "unit_test[x4]" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );





//	destroy_module( schedule_module_wrr );
	control.unload_module( schedule_module_wrr );
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
	test_suite* ts = BOOST_TEST_SUITE( "schedule_module_wrr_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &schedule_module_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

