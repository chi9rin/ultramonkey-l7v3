#include <iostream>
#include <dlfcn.h>
#include <boost/test/included/unit_test.hpp>
#include <boost/function.hpp>
#include "schedule_module_base.h"

#include "../../module/schedule/schedule_module_wrr.cpp"

using namespace boost::unit_test;

#define	CHECK_NAME	"schedule weighted round robin"

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
    int	get_maxWeight(){ return vs_weights.maxWeight ; }
    int	get_gcd(){ return vs_weights.gcd ; }
	void 	set_currentWeight( const int in_weight ){ vs_weights.currentWeight = in_weight ; }
    void	set_maxWeight( const int in_weight ){ vs_weights.maxWeight = in_weight ; }
    void	set_gcd( const int in_gcd ){ vs_weights.gcd = in_gcd ; }

	void	handle_init ( rslist_iterator_func_type		inlist_begin,
							rslist_iterator_func_type		inlist_end,
							rslist_iterator_func_type		inlist_next ){
		sched_wrr_service_init ( inlist_begin, inlist_end, inlist_next );
	}
};

}	//namespace l7vs


//l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type& list );
//l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type& list );
l7vs::schedule_module_base::rslist_type::iterator	list_begin( l7vs::schedule_module_base::rslist_type* list );
l7vs::schedule_module_base::rslist_type::iterator	list_end( l7vs::schedule_module_base::rslist_type* list );

//test case1.
void	schedule_module_test(){
	l7vs::schedule_module_base* schedule_module_wrr = NULL;

	// unit_test[1]  インスタンス作成のテスト
	BOOST_MESSAGE( "unit_test[1]\n" );
	try{
		schedule_module_wrr = create_module();
	}
	catch(...){
		BOOST_ERROR( "exception : create_module" );
	}
	BOOST_CHECK( NULL != schedule_module_wrr );

	// unit_test[2]  get_nameメソッドのテスト
	BOOST_MESSAGE( "unit_test[2]\n" );
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_wrr->get_name() );

	// unit_test[3]  initializeメソッドのテスト
	BOOST_MESSAGE( "unit_test[3]\n" );
	schedule_module_wrr->initialize();

	// unit_test[4]  initializeメソッドのテスト２
	BOOST_MESSAGE( "unit_test[4]\n" );
	// 派生クラスにgetter/setterを追加してprotected変数の確認
	l7vs::schedule_module_weighted_round_robin_fake	schedule_module_wrr_fake ;
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_wrr_fake.get_name() );

	boost::asio::ip::tcp::endpoint endpoint1( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;
	boost::asio::ip::udp::endpoint endpoint2( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

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

	// unit_test[5]  is_tcpメソッドのテスト
	BOOST_MESSAGE( "unit_test[5]\n" );
	BOOST_CHECK_EQUAL( true, schedule_module_wrr->is_tcp() );

	// unit_test[6]  is_udpメソッドのテスト
	BOOST_MESSAGE( "unit_test[6]\n" );
	BOOST_CHECK_EQUAL( true, schedule_module_wrr->is_udp() );

	boost::thread::id								thread_id;
	l7vs::schedule_module_base::rslist_type		rs_list;
	l7vs::realserver	server1, server2, server3, server4;

	l7vs::schedule_module_base::rslist_iterator_func_type		rslist_begin;
	l7vs::schedule_module_base::rslist_iterator_func_type		rslist_end;
	l7vs::schedule_module_base::rslist_iterator_func_type		rslist_next;

	// unit_test[7]  handle_schedule(tcp)メソッドのテスト
	BOOST_MESSAGE( "unit_test[7]\n" );
	boost::asio::ip::tcp::endpoint endpoint3 ;

	endpoint1 = endpoint3 ;

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[8]  handle_schedule(tcp)メソッドのテスト２
	BOOST_MESSAGE( "unit_test[8]\n" );
	rslist_begin = boost::bind( &list_begin, &rs_list );
	rslist_end = boost::bind( &list_end, &rs_list );

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( endpoint3 == endpoint1 );

	// unit_test[9]  handle_schedule(tcp)メソッドのテスト３
	BOOST_MESSAGE( "unit_test[9]\n" );
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

	// unit_test[10]  handle_schedule(tcp)メソッドのテスト４
	BOOST_MESSAGE( "unit_test[10]\n" );
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

	// unit_test[11]  handle_schedule(tcp)メソッドのテスト５
	BOOST_MESSAGE( "unit_test[11]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[12]  handle_schedule(tcp)メソッドのテスト６
	BOOST_MESSAGE( "unit_test[12]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[13]  handle_schedule(tcp)メソッドのテスト７
	BOOST_MESSAGE( "unit_test[13]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server1.tcp_endpoint == endpoint1 );

	// unit_test[14]  handle_schedule(tcp)メソッドのテスト８
	BOOST_MESSAGE( "unit_test[14]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server2.tcp_endpoint == endpoint1 );

	// unit_test[15]  handle_schedule(tcp)メソッドのテスト９
	BOOST_MESSAGE( "unit_test[15]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );

	// unit_test[16]  handle_schedule(tcp)メソッドのテスト１０
	BOOST_MESSAGE( "unit_test[16]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint1 ) ;
	BOOST_CHECK( server3.tcp_endpoint == endpoint1 );


	// unit_test[17]  handle_schedule(udp)メソッドのテスト
	BOOST_MESSAGE( "unit_test[17]\n" );
	boost::asio::ip::udp::endpoint endpoint4 ;

	endpoint2 = endpoint4 ;

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[18]  handle_schedule(udp)メソッドのテスト２
	BOOST_MESSAGE( "unit_test[18]\n" );
//	rslist_begin = boost::bind( &list_begin, &rs_list );
//	rslist_end = boost::bind( &list_end, &rs_list );

	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( endpoint4 == endpoint2 );

	// unit_test[19]  handle_schedule(udp)メソッドのテスト３
	BOOST_MESSAGE( "unit_test[19]\n" );
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

	// unit_test[20]  handle_schedule(udp)メソッドのテスト４
	BOOST_MESSAGE( "unit_test[20]\n" );
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

	// unit_test[21]  handle_schedule(udp)メソッドのテスト５
	BOOST_MESSAGE( "unit_test[21]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server1.udp_endpoint == endpoint2 );

	// unit_test[22]  handle_schedule(udp)メソッドのテスト６
	BOOST_MESSAGE( "unit_test[22]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[23]  handle_schedule(udp)メソッドのテスト７
	BOOST_MESSAGE( "unit_test[23]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server1.udp_endpoint == endpoint2 );

	// unit_test[24]  handle_schedule(udp)メソッドのテスト８
	BOOST_MESSAGE( "unit_test[24]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server2.udp_endpoint == endpoint2 );

	// unit_test[25]  handle_schedule(udp)メソッドのテスト９
	BOOST_MESSAGE( "unit_test[25]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );

	// unit_test[26]  handle_schedule(udp)メソッドのテスト１０
	BOOST_MESSAGE( "unit_test[26]\n" );
	schedule_module_wrr->handle_schedule( thread_id, rslist_begin, rslist_end, rslist_next, endpoint2 ) ;
	BOOST_CHECK( server3.udp_endpoint == endpoint2 );


	// unit_test[27]  replication_interruptメソッドのテスト
	BOOST_MESSAGE( "unit_test[27]\n" );
	schedule_module_wrr->replication_interrupt();

	// unit_test[28]  sched_wrr_getMaxWeightメソッドのテスト
	BOOST_MESSAGE( "unit_test[28]\n" );
	schedule_module_wrr_fake.handle_init( rslist_begin, rslist_end, rslist_next );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_maxWeight(), 3 );

	// unit_test[29]  ssched_wrr_getGCDメソッドのテスト
	BOOST_MESSAGE( "unit_test[29]\n" );
	BOOST_CHECK_EQUAL( schedule_module_wrr_fake.get_gcd(), 1 );

	destroy_module( schedule_module_wrr );
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

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "schedule_module_wrr_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &schedule_module_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

