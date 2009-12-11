#include <fstream>
#include <unistd.h>
#include <boost/test/included/unit_test.hpp>
#include "../../src/parameter_impl.cpp"

using namespace boost::unit_test;

void    create_inis(){
    std::ofstream    ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "sample_value = \"sample\"";
    ofs.close();
}


class    impl_test : public l7vs::ParameterImpl{
public:
    void*    get_intmap_address(){ return (void*) &intMap; }
    static impl_test&    get_instance(){
        static impl_test    instance;
        return instance;
    }

};

void*    thread1_intmap_address = NULL;
void*    thread2_intmap_address = NULL;

void    thread1(){
    impl_test&    impl = impl_test::get_instance();
    thread1_intmap_address = impl.get_intmap_address();
}

void    thread2(){
    impl_test&    impl = impl_test::get_instance();
    thread2_intmap_address = impl.get_intmap_address();
}

void    thread_test1(){
    // unit_test[1] thread initialize once test( one map access check )
    boost::thread    thd1( &thread1 );
    boost::thread    thd2( &thread2 );
    thd1.join();
    thd2.join();
    BOOST_CHECK_EQUAL( thread2_intmap_address, thread1_intmap_address );
}

void    thread3(){
    l7vs::ParameterImpl& impl = l7vs::ParameterImpl::get_instance();
    for( size_t i = 0 ; i < 100; ++i ){
        l7vs::error_code err;
        std::string    str = impl.get_string( l7vs::PARAM_COMP_L7VSD, "sample_value", err );
        BOOST_CHECK_EQUAL( err, false );
        BOOST_CHECK_EQUAL( str, "sample" );
    }
}

void    thread_test2(){
//     unit_test[2] multi thread access check
    boost::thread    thd1( &thread3 );
    boost::thread    thd2( &thread3 );
    boost::thread    thd3( &thread3 );
    thd1.join();
    thd2.join();
    thd3.join();
}


test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // Logger initialize
    l7vs::Logger        logger_instance;
    logger_instance.loadConf();

    // create unit test suite
    test_suite* ts = BOOST_TEST_SUITE( "parameter_impl_thread" );
    unlink( PARAMETER_FILE );
    create_inis();
    l7vs::ParameterImpl&    impl = l7vs::ParameterImpl::get_instance();
    impl.init();

    ts->add( BOOST_TEST_CASE( &thread_test1 ) );
    ts->add( BOOST_TEST_CASE( &thread_test2 ) );

    framework::master_test_suite().add( ts );

    return 0;
}
