#include <iostream>
#include <fstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/asio.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "../../include/virtualservice_element.h"

using namespace boost::unit_test_framework;

//variable


//--tests--

void    equal_test(){
    l7vs::virtualservice_element    vs1;
    l7vs::virtualservice_element    vs2;


    BOOST_CHECK( vs1 == vs2 );
    BOOST_CHECK( !(vs1 != vs2) );

    vs1.udpmode = true;
    vs2.udpmode = false;

    BOOST_CHECK( vs1 != vs2 );
    BOOST_CHECK( !(vs1 == vs2) );

    vs1.udpmode = true;
    vs2.udpmode = true;

    vs1.access_log_rotate_arguments["key"] = "value";
    vs2.access_log_rotate_arguments["key"] = "val";

    std::stringstream s;
    s << "vs1=" << vs1;
    s << "vs2=" << vs2;
    BOOST_MESSAGE( s.str() );

    BOOST_CHECK( vs1 != vs2 );
    BOOST_CHECK( !(vs1 == vs2) );

}

void copy_test(){
    l7vs::virtualservice_element    vs1;
    l7vs::virtualservice_element    vs2;

    vs1.access_log_rotate_arguments["key"] = "value";

    BOOST_CHECK( vs1 != vs2 );
    BOOST_CHECK( !(vs1 == vs2) );

    vs2 = vs1;

    BOOST_CHECK( vs1 == vs2 );
    BOOST_CHECK( !(vs1 != vs2) );
    
}

void archive_test(){
    l7vs::virtualservice_element    vs1;
    l7vs::virtualservice_element    vs2;
    //save
    {
        vs1.access_log_rotate_arguments["key"] = "value";
        std::ofstream file( "save.dat" );
        boost::archive::text_oarchive oa( file );
        oa << (const l7vs::virtualservice_element)vs1;
    }
    //load
    {
        std::ifstream file("save.dat");
        boost::archive::text_iarchive ia( file );
        ia >> vs2;
    }
//    std::stringstream   buf;
//    buf << vs2;
    std::cout << vs2<< std::endl;
    //unit_test[1]
    BOOST_CHECK( vs1 == vs2 );
    BOOST_CHECK( !(vs1 != vs2) );

    
}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){
    test_suite* ts = BOOST_TEST_SUITE( "vselement class test" );

    ts->add( BOOST_TEST_CASE( &equal_test ) );
    ts->add( BOOST_TEST_CASE( &copy_test ) );
    ts->add( BOOST_TEST_CASE( &archive_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
