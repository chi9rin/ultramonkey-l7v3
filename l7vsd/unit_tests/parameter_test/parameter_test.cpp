#define    __STDC_LIMIT_MACROS
#include <stdint.h>
#include <fstream>
#include <unistd.h>
#include <boost/test/included/unit_test.hpp>
#include "../../src/parameter.cpp"
#include "../../src/parameter_impl.cpp"

// すべての試験関数を含んだ実行ファイルにて試験を実行すると
// 先頭試験関数でparameterインスタンスが作成されてしまうため、
// 試験対象関数毎に実行ファイルを作成すること

using namespace boost::unit_test;

class parameter_test : public l7vs::Parameter{

};

// Parameter::Parameter() test
void parameter_constructor_test(){

    int               ret = 99;
    l7vs::error_code  err;
    int               zero_value = 0;

    // unit_test [1] parameter constructor check init NG
    unlink( PARAMETER_FILE );
//    parameter_test    param_test1;
    // check log "Parameter initialize failure"

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "zero_value =" << zero_value << "\n";
    ofs.close();

    // unit_test [2] parameter constructor check
    parameter_test param_test2;
    ret = param_test2.get_int( l7vs::PARAM_COMP_L7VSD, "zero_value", err );
    BOOST_CHECK_EQUAL( ret, zero_value );

    // unit_test[0] parameter constructor re check
    parameter_test param_test0;
    // check log "It's initialized already."

    unlink( PARAMETER_FILE );

}

// Parameter::init() test
void parameter_init_test(){

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "zero_value = 0\n";
    ofs.close();

    parameter_test param_test1;
    parameter_test param_test2;

    // unit_test [3] parameter init() check
    BOOST_CHECK_EQUAL( param_test1.init( l7vs::PARAM_COMP_L7VSD,
                                         PARAMETER_FILE ), true );

    // uint_test [4] parameter init() false check
    BOOST_CHECK_EQUAL( param_test2.init( l7vs::PARAM_COMP_SSL,"./hoge.cf" ),
                       false );
    // check FATAL LOG

    unlink( PARAMETER_FILE );
}

// Parameter::read_file() test
void parameter_read_file_test(){

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "zero_value = 0\n";
    ofs.close();

    std::ofstream ofs2( "./add.cf" );
    ofs2 << "[ssl]\n";
    ofs2 << "zero_value = 0" << "\n";
    ofs2.close();

    parameter_test param_test;

    // unit_test [5] read_file(default file) check
    BOOST_CHECK_EQUAL( param_test.read_file( l7vs::PARAM_COMP_L7VSD ), true );

    // unit_test [6] read_file(filename) check
    BOOST_CHECK_EQUAL(
        param_test.read_file( l7vs::PARAM_COMP_SSL, "./add.cf" ),
        true );

    // unit_test [7] read_file ( no file ) check
    BOOST_CHECK_EQUAL(
        param_test.read_file( l7vs::PARAM_COMP_L7VSD, "./hoge.cf" ),
        false );

    unlink( PARAMETER_FILE );
    unlink( "./add.cf" );
}

// Parameter::get_int() test
void parameter_get_int_test(){

    l7vs::error_code  err;
    int               zero_value = 0;
    int               ret = 99;

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "zero_value = " << zero_value << "\n";
    ofs.close();

    parameter_test         param_test;

    // unit_test [8] get_int call check
    ret = param_test.get_int( l7vs::PARAM_COMP_L7VSD,"zero_value", err );
    BOOST_CHECK_EQUAL( ret, zero_value );

    unlink( PARAMETER_FILE );
}

// Parameter::get_string() test
void parameter_get_string_test(){

    l7vs::error_code  err;
    std::string       str_value = "\"strings\"";
    std::string       ret_str = "ZZZZ";

    std::ofstream     ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "string_value = " << str_value << "\n";
    ofs.close();

    parameter_test    param_test;

    // unit_test [9] get_string call check
    ret_str = param_test.get_string( l7vs::PARAM_COMP_L7VSD,
                                     "string_value", err );
    BOOST_CHECK_EQUAL( err, false );
    BOOST_CHECK_EQUAL( ret_str, "strings" );

    unlink( PARAMETER_FILE );
}

// Parameter::get_multistring() test
void parameter_get_multistring_test(){

    l7vs::error_code         err;
    std::string              str_value_1 = "\"strings_1\"";
    std::string              str_value_2 = "\"strings_2\"";
    std::vector<std::string> ret_vec;

    std::ofstream     ofs( PARAMETER_FILE );
    ofs << "[ssl]\n";
    ofs << "string_value = " << str_value_1 << "\n";
    ofs << "string_value = " << str_value_2 << "\n";
    ofs.close();

    parameter_test    param_test;

    // unit_test [10] get_multistring call check
    param_test.get_multistring( l7vs::PARAM_COMP_SSL,
                                "string_value", ret_vec, err );
    BOOST_CHECK_EQUAL( ret_vec.size(), (unsigned int)2 );
    BOOST_CHECK_EQUAL( ret_vec[0], "strings_1" );
    BOOST_CHECK_EQUAL( ret_vec[1], "strings_2" );

    unlink( PARAMETER_FILE );

}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // Logger initialize
    l7vs::Logger        logger_instance;
    logger_instance.loadConf();

    // create unit test suite
    // !!!! target test case is only one
    test_suite* ts = BOOST_TEST_SUITE( "parameter" );
    ts->add( BOOST_TEST_CASE( &parameter_constructor_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_init_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_read_file_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_get_int_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_get_string_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_get_multistring_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}

