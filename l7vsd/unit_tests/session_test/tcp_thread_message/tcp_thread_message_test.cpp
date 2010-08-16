
#define    TEST_CASE

#include <boost/test/included/unit_test.hpp>

#define TCP_SESSION_H
namespace l7vs{
    class tcp_session{
        public:
            enum TCP_PROCESS_TYPE_TAG{
                LOCAL_PROC = 0,
                MESSAGE_PROC
            };
            
            tcp_session();
            ~tcp_session();
        
    };
}

//#include "../../../src/tcp_thread_message.cpp"
#include "../../../include/tcp_thread_message.h"

using namespace boost::unit_test_framework;

//--test case--
void    construcor_test(){

    
    BOOST_MESSAGE( "----- construcor test start -----" );
    
    l7vs::tcp_thread_message test;

    BOOST_MESSAGE( "----- construcor test end -----" );
}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    test_suite* ts = BOOST_TEST_SUITE( "l7vs::tcp_thread_message class test" );

    ts->add( BOOST_TEST_CASE( &construcor_test ) );

    framework::master_test_suite().add( ts );

    return NULL;
}
