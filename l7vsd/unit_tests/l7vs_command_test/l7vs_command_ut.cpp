#include <boost/test/included/unit_test.hpp>
#include <boost/asio.hpp>
#include "../../include/l7vs_command.h"

using namespace boost::unit_test;
using namespace l7vs;

//
// using string to endpoint template function
//
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
    std::string::size_type pos = str.find( ":" );
    std::string    hostname = str.substr( 0, pos );
    std::string    portname = str.substr( pos+1, str.length() );
    boost::asio::io_service        io_service;
    typename InternetProtocol::resolver                resolver(io_service);
    typename InternetProtocol::resolver::query        query( hostname, portname );
    typename InternetProtocol::resolver::iterator    end;
    typename InternetProtocol::resolver::iterator    itr = resolver.resolve( query );
    if( itr == end ) return typename InternetProtocol::endpoint();
    return *itr;
}


// testcase 1.
void    realserver_element_test(){
    realserver_element    rs;
    boost::asio::ip::tcp::endpoint    tcp_default_endpoint;
    boost::asio::ip::udp::endpoint    udp_default_endpoint;
    // unit_test[1] default constractor tcp endpoint test
    BOOST_CHECK( rs.tcp_endpoint == tcp_default_endpoint );
    // unit_test[2] default constractor udp endpoint test
    BOOST_CHECK( rs.udp_endpoint == udp_default_endpoint );
    // unit_test[3] default constrafctor weight test
    BOOST_CHECK( rs.weight == -1 );
    rs.tcp_endpoint = string_to_endpoint<boost::asio::ip::tcp>( "10.10.10.10:6555" );
    rs.udp_endpoint = string_to_endpoint<boost::asio::ip::udp>( "20.20.20.20:4444" );
    rs.weight = 255;

    realserver_element    rs2 = rs;
    // unit_test[4] copy constractor tcp endpoint test
    BOOST_CHECK( rs.tcp_endpoint == rs2.tcp_endpoint );
    // unit_test[5] copy constractor udp endpoint test
    BOOST_CHECK( rs.udp_endpoint == rs2.udp_endpoint );
    
}


test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // create unit test suite
    test_suite* ts = BOOST_TEST_SUITE( "l7vs_command_ut" );
    ts->add( BOOST_TEST_CASE( &realserver_element_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}

