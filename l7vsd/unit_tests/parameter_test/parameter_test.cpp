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

class parameter_test : public l7vs::Parameter
{

};

// Parameter::Parameter() test
void parameter_constructor_test()
{

        l7vs::error_code  err;
        int               zero_value = 0;

        // unit_test [1] parameter constructor check init NG
        unlink(PARAMETER_FILE);
//    parameter_test    param_test1;
        // check log "Parameter initialize failure"

        std::ofstream ofs(PARAMETER_FILE);
        ofs << "[l7vsd]\n";
        ofs << "zero_value =" << zero_value << "\n";
        ofs.close();

        // unit_test [2] parameter constructor check
//    parameter_test param_test2;
//    int ret = param_test2.get_int( l7vs::PARAM_COMP_L7VSD, "zero_value", err );
//    BOOST_CHECK_EQUAL( ret, zero_value );

        // unit_test[0] parameter constructor re check
        parameter_test param_test0;
        // check log "It's initialized already."

        unlink(PARAMETER_FILE);

}

// Parameter::init() test
void parameter_init_test()
{

        std::ofstream ofs(PARAMETER_FILE);
        ofs << "[l7vsd]\n";
        ofs << "zero_value = 0\n";
        ofs.close();

        parameter_test param_test1;
        parameter_test param_test2;

        // unit_test [3] parameter init() check
        BOOST_CHECK_EQUAL(param_test1.init(l7vs::PARAM_COMP_L7VSD,
                                           PARAMETER_FILE), true);

        // uint_test [4] parameter init() false check
        BOOST_CHECK_EQUAL(param_test2.init(l7vs::PARAM_COMP_SSL, "./hoge.cf"),
                          false);
        // check FATAL LOG

        unlink(PARAMETER_FILE);
}

// Parameter::read_file() test
void parameter_read_file_test()
{

        std::ofstream ofs(PARAMETER_FILE);
        ofs << "[l7vsd]\n";
        ofs << "zero_value = 0\n";
        ofs.close();

        std::ofstream ofs2("./add.cf");
        ofs2 << "[ssl]\n";
        ofs2 << "zero_value = 0" << "\n";
        ofs2.close();

        parameter_test param_test;

        // unit_test [5] read_file(default file) check
        BOOST_CHECK_EQUAL(param_test.read_file(l7vs::PARAM_COMP_L7VSD), true);

        // unit_test [6] read_file(filename) check
        BOOST_CHECK_EQUAL(
                param_test.read_file(l7vs::PARAM_COMP_SSL, "./add.cf"),
                true);

        // unit_test [7] read_file ( no file ) check
        BOOST_CHECK_EQUAL(
                param_test.read_file(l7vs::PARAM_COMP_L7VSD, "./hoge.cf"),
                false);

        unlink(PARAMETER_FILE);
        unlink("./add.cf");
}

// Parameter::get_int() test
void parameter_get_int_test()
{

        l7vs::error_code  err;
        int               zero_value = 0;
        int               ret = 99;

        std::ofstream ofs(PARAMETER_FILE);
        ofs << "[l7vsd]\n";
        ofs << "zero_value = " << zero_value << "\n";
        ofs.close();

        parameter_test         param_test;

        // unit_test [8] get_int call check
        ret = param_test.get_int(l7vs::PARAM_COMP_L7VSD, "zero_value", err);
        BOOST_CHECK_EQUAL(ret, zero_value);

        unlink(PARAMETER_FILE);
}

// Parameter::get_string() test
void parameter_get_string_test()
{

        l7vs::error_code  err;
        std::string       str_value = "\"strings\"";
        std::string       ret_str = "ZZZZ";

        std::ofstream     ofs(PARAMETER_FILE);
        ofs << "[l7vsd]\n";
        ofs << "string_value = " << str_value << "\n";
        ofs.close();

        parameter_test    param_test;

        // unit_test [9] get_string call check
        ret_str = param_test.get_string(l7vs::PARAM_COMP_L7VSD,
                                        "string_value", err);
        BOOST_CHECK_EQUAL(err, false);
        BOOST_CHECK_EQUAL(ret_str, "strings");

        unlink(PARAMETER_FILE);
}

// Parameter::get_multistring() test
void parameter_get_multistring_test()
{

        l7vs::error_code         err;
        std::string              str_value_1 = "\"strings_1\"";
        std::string              str_value_2 = "\"strings_2\"";
        std::vector<std::string> ret_vec;

        std::ofstream     ofs(PARAMETER_FILE);
        ofs << "[ssl]\n";
        ofs << "string_value = " << str_value_1 << "\n";
        ofs << "string_value = " << str_value_2 << "\n";
        ofs.close();

        parameter_test    param_test;

        // unit_test [10] get_multistring call check
        param_test.get_multistring(l7vs::PARAM_COMP_SSL,
                                   "string_value", ret_vec, err);
        BOOST_CHECK_EQUAL(ret_vec.size(), (unsigned int)2);
        BOOST_CHECK_EQUAL(ret_vec[0], "strings_1");
        BOOST_CHECK_EQUAL(ret_vec[1], "strings_2");

        unlink(PARAMETER_FILE);

}


// read pattern test
void parameter_read_pattern0()
{

        l7vs::PARAMETER_COMPONENT_TAG comp_tag[10] = {
                l7vs::PARAM_COMP_L7VSD,          //!< parameter category l7vsd
                l7vs::PARAM_COMP_COMMAND,        //!< parameter category command
                l7vs::PARAM_COMP_SESSION,        //!< parameter category session
                l7vs::PARAM_COMP_VIRTUALSERVICE, //!< parameter category virtualservice
                l7vs::PARAM_COMP_MODULE,         //!< parameter category module control
                l7vs::PARAM_COMP_REPLICATION,    //!< parameter category replication
                l7vs::PARAM_COMP_LOGGER,         //!< parameter category logger
                l7vs::PARAM_COMP_L7VSADM,        //!< parameter category l7vsadm
                l7vs::PARAM_COMP_SNMPAGENT,      //!< parameter category snmpagent
                l7vs::PARAM_COMP_SSL
        };           //!< parameter category ssl
        l7vs::error_code err;
        // create init files.
        std::string sectionstrings[10];
        sectionstrings[0] = "l7vsd";
        sectionstrings[1] = "command";
        sectionstrings[2] = "session";
        sectionstrings[3] = "virtualservice";
        sectionstrings[4] = "module";
        sectionstrings[5] = "replication";
        sectionstrings[6] = "logger";
        sectionstrings[7] = "l7vsadm";
        sectionstrings[8] = "snmpagent";
        std::string keystr[2];
        keystr[0] = "key1";
        keystr[1] = "key2";
        std::string valuestr[2];
        valuestr[0] = "1";
        valuestr[1] = "\"strings\"";
        valuestr[2] = "2";
        valuestr[3] = "\"strings2\"";
        std::ofstream ofs(PARAMETER_FILE);
        for (int i = 0 ; i < 9; ++i) {
                ofs << "[" << sectionstrings[i] << "]\n";
                ofs << keystr[0] << "=" << valuestr[0] << "\n";
                ofs << keystr[1] << " = " << valuestr[1] << "\n";
        }
        ofs << "[" << sectionstrings[0] << "]\n";
        ofs << keystr[0] << "=" << valuestr[2] << std::endl;   // overwrite check
        ofs << keystr[1] << " = " << valuestr[3] << std::endl; // overwrite check
        ofs.close();

        // unit_test[a1] default file read
        parameter_test param;

        for (int i = 0; i < 9; ++i) {
                if (i == 0) {
                        BOOST_CHECK_EQUAL(param.get_int(comp_tag[i],
                                                        keystr[0], err, PARAMETER_FILE), 2);
                        BOOST_CHECK_EQUAL(param.get_string(comp_tag[i],
                                                           keystr[1], err, PARAMETER_FILE), "strings2");
                } else {
                        BOOST_CHECK_EQUAL(param.get_int(comp_tag[i],
                                                        keystr[0], err, PARAMETER_FILE), 1);
                        BOOST_CHECK_EQUAL(param.get_string(comp_tag[i],
                                                           keystr[1], err, PARAMETER_FILE), "strings");
                }
        }
        unlink(PARAMETER_FILE);
}

// read file pattern1
void parameter_read_pattern1()
{

        // ssl1.cf read
        // ssl1.cf update
        // ssl1.cf read

        // ssl file create
        l7vs::error_code         err;
        std::vector<std::string> strvec;

        {
                std::ofstream ofs("./ssl1.cf");
                ofs << "[ssl]\n";
                ofs << "string_value0 = \"ssl_10\"\n";
                ofs << "mstring_value = \"ssl_11\"\n";
                ofs << "mstring_value = \"ssl_12\"\n";
                ofs << "int_value = 10\n";
                ofs.close();
        }
        // default file read
        parameter_read_pattern0();

        parameter_test param;
        // read ssl file
        param.init(l7vs::PARAM_COMP_SSL, "./ssl1.cf");

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl1.cf"), 10);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl1.cf"), "ssl_10");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_11");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_12");

        unlink("./ssl1cf");

        {
                std::ofstream ofs("./ssl1.cf");
                ofs << "[ssl]\n";
                ofs << "string_value0 = \"ssl_20\"\n";
                ofs << "m2string_value = \"ssl_21\"\n";
                ofs << "m2string_value = \"ssl_22\"\n";
                ofs << "int_value = 20\n";
                ofs.close();
        }
        // re-read ssl file
        param.init(l7vs::PARAM_COMP_SSL, "./ssl1.cf");

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl1.cf"), 20);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl1.cf"), "ssl_20");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(err, true);
        param.get_multistring(l7vs::PARAM_COMP_SSL, "m2string_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_21");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_22");

        unlink("./ssl1cf");

}

// read file pattern2
void parameter_read_pattern2()
{

        // ssl1.cf read
        // ssl1.cf read

        // ssl file create
        l7vs::error_code         err;
        std::vector<std::string> strvec;

        {
                std::ofstream ofs("./ssl1.cf");
                ofs << "[ssl]\n";
                ofs << "string_value0 = \"ssl_10\"\n";
                ofs << "mstring_value = \"ssl_11\"\n";
                ofs << "mstring_value = \"ssl_12\"\n";
                ofs << "int_value = 10\n";
                ofs.close();
        }
        // default file read
        parameter_read_pattern0();

        parameter_test param;
        // read ssl file
        param.init(l7vs::PARAM_COMP_SSL, "./ssl1.cf");

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl1.cf"), 10);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl1.cf"), "ssl_10");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_11");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_12");

        // read ssl file
        param.init(l7vs::PARAM_COMP_SSL, "./ssl1.cf");

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl1.cf"), 10);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl1.cf"), "ssl_10");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_11");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_12");

        unlink("./ssl1cf");
}

// read file pattern3
void parameter_read_pattern3()
{

        // ssl1.cf read
        // ssl2.cf read

        // ssl file create
        l7vs::error_code         err;
        std::vector<std::string> strvec;

        {
                std::ofstream ofs("./ssl1.cf");
                ofs << "[ssl]\n";
                ofs << "string_value0 = \"ssl_10\"\n";
                ofs << "mstring_value = \"ssl_11\"\n";
                ofs << "mstring_value = \"ssl_12\"\n";
                ofs << "int_value = 10\n";
                ofs.close();
        }
        // default file read
        parameter_read_pattern0();

        parameter_test param;
        // read ssl file
        param.init(l7vs::PARAM_COMP_SSL, "./ssl1.cf");

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl1.cf"), 10);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl1.cf"), "ssl_10");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_11");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_12");


        {
                std::ofstream ofs("./ssl2.cf");
                ofs << "[ssl]\n";
                ofs << "string_value0 = \"ssl_20\"\n";
                ofs << "m2string_value = \"ssl_21\"\n";
                ofs << "m2string_value = \"ssl_22\"\n";
                ofs << "int_value = 20\n";
                ofs.close();
        }
        // re-read ssl file
        param.init(l7vs::PARAM_COMP_SSL, "./ssl2.cf");

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl1.cf"), 10);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl1.cf"), "ssl_10");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_11");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_12");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "m2string_value",
                              strvec, err, "./ssl1.cf");
        BOOST_CHECK_EQUAL(err, true);

        BOOST_CHECK_EQUAL(param.get_int(l7vs::PARAM_COMP_SSL,
                                        "int_value", err, "./ssl2.cf"), 20);
        BOOST_CHECK_EQUAL(param.get_string(l7vs::PARAM_COMP_SSL,
                                           "string_value0", err, "./ssl2.cf"), "ssl_20");
        param.get_multistring(l7vs::PARAM_COMP_SSL, "mstring_value",
                              strvec, err, "./ssl2.cf");
        BOOST_CHECK_EQUAL(err, true);
        param.get_multistring(l7vs::PARAM_COMP_SSL, "m2string_value",
                              strvec, err, "./ssl2.cf");
        BOOST_CHECK_EQUAL(strvec[0], "ssl_21");
        BOOST_CHECK_EQUAL(strvec[1], "ssl_22");

        unlink("./ssl1.cf");
        unlink("./ssl2.cf");

}


test_suite    *init_unit_test_suite(int argc, char *argv[])
{

        // Logger initialize
        l7vs::Logger        logger_instance;
        logger_instance.loadConf();

        // create unit test suite
        // !!!! target test case is only one
        test_suite *ts = BOOST_TEST_SUITE("parameter");
//    ts->add( BOOST_TEST_CASE( &parameter_constructor_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_init_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_read_file_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_get_int_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_get_string_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_get_multistring_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_read_pattern1 ) );
//    ts->add( BOOST_TEST_CASE( &parameter_read_pattern2 ) );
        ts->add(BOOST_TEST_CASE(&parameter_read_pattern3));

        framework::master_test_suite().add(ts);

        return 0;
}

