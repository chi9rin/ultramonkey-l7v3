
#define    TEST_CASE

#include <boost/test/included/unit_test.hpp>

#include "session_result_message.h"

using namespace boost::unit_test_framework;

//--test case--
void    operator_equal_test(){


    BOOST_MESSAGE( "----- operator equal test start -----" );

    l7vs::session_result_message test_obj;
    l7vs::session_result_message chk_obj;
    bool bres;
    test_obj.flag = true;
    test_obj.message = "Test message 1";
    
    // ## operator equal test [1] equal case 1
    std::cout << "operator equal test [1] equal case 1" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = test_obj.message;
    bres = test_obj == chk_obj;
    BOOST_CHECK( bres );

    // ## operator equal test [2] inequal case 1
    std::cout << "operator equal test [2] inequal case 1" << std::endl;
    chk_obj.flag = !test_obj.flag;
    chk_obj.message = test_obj.message;
    bres = test_obj == chk_obj;
    BOOST_CHECK( !bres );

    // ## operator equal test [3] inequal case 2
    std::cout << "operator equal test [3] inequal case 2" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = "Test message 2";
    bres = test_obj == chk_obj;
    BOOST_CHECK( !bres );

    // ## operator equal test [4] inequal case 3
    std::cout << "operator equal test [4] inequal case 2" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = "Test message ";
    bres = test_obj == chk_obj;
    BOOST_CHECK( !bres );

    // ## operator equal test [5] inequal case 4
    std::cout << "operator equal test [5] inequal case 2" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = "Test message 1 ";
    bres = test_obj == chk_obj;
    BOOST_CHECK( !bres );

    BOOST_MESSAGE( "----- operator equal test end -----" );
}

void    operator_inequal_test(){
    BOOST_MESSAGE( "----- operator inequal test start -----" );

    l7vs::session_result_message test_obj;
    l7vs::session_result_message chk_obj;
    bool bres;
    test_obj.flag = true;
    test_obj.message = "Test message 1";
    
    // ## operator inequal test [1] equal case 1
    std::cout << "operator inequal test [1] equal case 1" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = test_obj.message;
    bres = test_obj != chk_obj;
    BOOST_CHECK( !bres );

    // ## operator inequal test [2] inequal case 1
    std::cout << "operator inequal test [2] inequal case 1" << std::endl;
    chk_obj.flag = !test_obj.flag;
    chk_obj.message = test_obj.message;
    bres = test_obj != chk_obj;
    BOOST_CHECK( bres );

    // ## operator inequal test [3] inequal case 2
    std::cout << "operator inequal test [3] inequal case 2" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = "Test message 2";
    bres = test_obj != chk_obj;
    BOOST_CHECK( bres );

    // ## operator inequal test [4] inequal case 3
    std::cout << "operator inequal test [4] inequal case 2" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = "Test message ";
    bres = test_obj != chk_obj;
    BOOST_CHECK( bres );

    // ## operator inequal test [5] inequal case 4
    std::cout << "operator inequal test [5] inequal case 2" << std::endl;
    chk_obj.flag = test_obj.flag;
    chk_obj.message = "Test message 1 ";
    bres = test_obj != chk_obj;
    BOOST_CHECK( bres );

    BOOST_MESSAGE( "----- operator inequal test end -----" );
}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    test_suite* ts = BOOST_TEST_SUITE( "l7vs::session_result_message test" );

    ts->add( BOOST_TEST_CASE( &operator_equal_test ) );
    ts->add( BOOST_TEST_CASE( &operator_inequal_test ) );

    framework::master_test_suite().add( ts );

    return NULL;
}
