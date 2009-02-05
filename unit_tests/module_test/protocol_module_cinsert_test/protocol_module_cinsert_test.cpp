#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>

#include "protocol_module_cinsert.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

#define	THREAD_LOOP_NUM	(1)

//--stub functions--


//--test class--
class	protocol_module_cinsert_test : public protocol_module_cinsert {
public:
boost::mutex		sync_mutex;
boost::condition	sync_condition;
protocol_module_cinsert_test() : protocol_module_cinsert(){}
~protocol_module_cinsert_test(){}
};

//--test functions--
//void	check_http_method_test_thread( http_protocol_module_base_test* http_protocol_module_base_test_1, int thread_no, bool* ret )
//{
//}
void	check_http_method_test()
{
	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

//	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

//	BOOST_MESSAGE( "----- check_http_method test start -----" );
//	http_protocol_module_base_test_1.check_http_method_test();
//	BOOST_MESSAGE( "----- check_http_method test end -----" );

//	BOOST_MESSAGE( "----- check_http_method test multi thread start -----" );
//	boost::thread	t1(	boost::bind(	&check_http_method_test_thread,
//										&http_protocol_module_base_test_1, 1, &thread_1));
//	boost::thread	t2(	boost::bind(	&check_http_method_test_thread,
//										&http_protocol_module_base_test_1, 2, &thread_2));
//	boost::thread	t3(	boost::bind(	&check_http_method_test_thread,
//										&http_protocol_module_base_test_1, 3, &thread_3));
//	boost::thread	t4(	boost::bind(	&check_http_method_test_thread,
//										&http_protocol_module_base_test_1, 4, &thread_4));
//	boost::thread	t5(	boost::bind(	&check_http_method_test_thread,
//										&http_protocol_module_base_test_1, 5, &thread_5));

//	sleep(1);

//	http_protocol_module_base_test_1.sync_condition.notify_all();

//	t1.join();
//	t2.join();
//	t3.join();
//	t4.join();
//	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

//	BOOST_MESSAGE( "----- check_http_method test multi thread end -----" );
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "http_protocol_module_base class test" );

	ts->add( BOOST_TEST_CASE( &check_http_method_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
