#include <iostream>
#include <dlfcn.h>
#include <boost/test/included/unit_test.hpp>
#include "schedule_module_base.h"

//class l7vs::module_base;
//class l7vs::schedule_module_base;

#include "../../module/schedule/schedule_module_rr.cpp"

using namespace boost::unit_test;

#define	CHECK_NAME	"schedule round robin"

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


//test case1.
void	schedule_module_test(){
	l7vs::schedule_module_base* schedule_module_rr = NULL ;

#if	0 // SO Load
	void*						p_so = dlopen( "./schedule_module_rr.so", RTLD_LAZY );
	l7vs::schedule_module_base*	( *p_create )( void ) ;
	void						( *p_destroy )( l7vs::schedule_module_base* ) ;

	if ( NULL != p_so )
	{
		dlerror();
		p_create = ( l7vs::schedule_module_base* (*)( void ) )dlsym( p_so, "create_module" ) ;
		p_destroy = ( void (*)( l7vs::schedule_module_base* ) )dlsym( p_so, "destroy_module" ) ;

		// unit_t_e_s_t[1]  インスタンス作成のテスト
		try{
			schedule_module_rr = p_create() ;
		}
		catch(...){
			BOOST_ERROR( "exception : create_module" );
		}
		BOOST_CHECK( NULL != schedule_module_rr );

		// unit_t_e_s_t[2]  get_nameのテスト
		BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_rr->get_name() );

		if ( schedule_module_rr )
		{
			p_destroy ( schedule_module_rr ) ;
		}
		dlclose ( p_so ) ;
	}
	else
	{
		BOOST_ERROR( "exception : dlopen" );
	}
#else
	// unit_test[1]  インスタンス作成のテスト
	try{
		schedule_module_rr = create_module() ;
	}
	catch(...){
		BOOST_ERROR( "exception : create_module" );
	}
	BOOST_CHECK( NULL != schedule_module_rr );

	// unit_test[2]  get_nameメソッドのテスト
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_rr->get_name() );

	// unit_test[3]  initializeメソッドのテスト
	schedule_module_rr->initialize();

	// 派生クラスにgetter/setterを追加してprotected変数の確認
	l7vs::schedule_module_round_robin_fake	schedule_module_rr_fake ;
	BOOST_CHECK_EQUAL( CHECK_NAME, schedule_module_rr_fake.get_name() );

	boost::asio::ip::tcp::endpoint endpoint1( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;
	boost::asio::ip::udp::endpoint endpoint2( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	schedule_module_rr_fake.set_tcp_endpoint( endpoint1 );
	schedule_module_rr_fake.set_udp_endpoint( endpoint2 );

	BOOST_CHECK( schedule_module_rr_fake.get_tcp_endpoint() == endpoint1 );
	BOOST_CHECK( schedule_module_rr_fake.get_udp_endpoint() == endpoint2 );

	schedule_module_rr_fake.initialize();
	BOOST_CHECK( schedule_module_rr_fake.get_tcp_endpoint() != endpoint1 );
	BOOST_CHECK( schedule_module_rr_fake.get_udp_endpoint() != endpoint2 );


	// unit_test[4]  is_tcpメソッドのテスト
	BOOST_CHECK_EQUAL( true, schedule_module_rr->is_tcp() );

	// unit_test[5]  is_udpメソッドのテスト
	BOOST_CHECK_EQUAL( true, schedule_module_rr->is_udp() );


	destroy_module( schedule_module_rr );
#endif
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "schedule_module_rr_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &schedule_module_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

