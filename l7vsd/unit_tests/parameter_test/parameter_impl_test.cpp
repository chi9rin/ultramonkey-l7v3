#define    __STDC_LIMIT_MACROS
#include <stdint.h>
#include <fstream>
#include <unistd.h>
#include <boost/test/included/unit_test.hpp>
#include "../../src/parameter_impl.cpp"

// すべての試験関数を含んだ実行ファイルにて試験を実行すると
// 先頭試験関数でparameterインスタンスが作成されてしまうため、
// 試験対象関数毎に実行ファイルを作成すること

using namespace boost::unit_test;

class parameter_impl_test : public l7vs::ParameterImpl{
public:
    std::multimap<std::string, std::string>&
            getMultiStringMap(){ return multistringMap; }
    std::map<std::string, int>&
            getIntMap(){ return intMap; }
    std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string>&
            get_section_map(){ return tag_section_table_map;}

    static    parameter_impl_test& get_instance(){
        boost::mutex::scoped_lock( param_mutex );
        static parameter_impl_test instance;
        return instance;
    }
};

// ParameterImpl::init() test & read_file()test
void parameter_impl_init_test(){
    parameter_impl_test& impl = parameter_impl_test::get_instance();

    std::multimap<std::string, std::string>& strmap = impl.getMultiStringMap();
    std::map<std::string, int>& intmap = impl.getIntMap();
    std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string >&
                                sectionmap = impl.get_section_map();

    // create dummy data
    strmap.insert( std::pair< std::string, std::string >( "abc", "def" ) );
    intmap.insert( std::pair< std::string, int >( "abc", 65000 ) );
    sectionmap.insert( std::pair< l7vs::PARAMETER_COMPONENT_TAG, std::string >( l7vs::PARAM_COMP_NOCAT, "sample" ) );
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
    sectionstrings[9] = "ssl";
    std::string keystr[2];
    keystr[0] = "key1";
    keystr[1] = "key2";
    std::string valuestr[2];
    valuestr[0] = "1";
    valuestr[1] = "\"strings\"";

    // unit_test[0] no create,init no file
    BOOST_CHECK_EQUAL( impl.init(), false );

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "# comment1" << std::endl;
    ofs << " # spase comment2" << std::endl;
    ofs << "	# tab comment3" << std::endl;
    for( int i = 0 ; i < 10; ++i ){
        ofs << "[" << sectionstrings[i] << "]\n";
            ofs << keystr[0] << "=" << valuestr[0] << "# comment4\n";
            ofs << keystr[1] << " = " << valuestr[1] << "	#comment5\n";
            ofs << "	#comment 6 \n";
    }
    ofs << "[" << sectionstrings[0] << "]\n";
    ofs << keystr[0] << "=2" << std::endl;  // overwrite check
    ofs << keystr[1] << " =\"strings2\"" << std::endl;  // overwrite check
    ofs << "	#comment 7 \n";

    ofs.close();

    // unit_test[1] no create,init(filename)
    BOOST_CHECK_EQUAL( impl.init( l7vs::PARAM_COMP_SSL, "./ssl.cf" ), false ); 

    // unit_test[2] file load check(default filename)
    // unit_test[3] comment line erase check
    // unit_test[4] first space comment line check
    // unit_test[5] first tab comment line check
    // unit_test[6] section - section comment line check
    // unit_test[7] last comment line check
    BOOST_CHECK_EQUAL( impl.init(), true );

    // unit_test[8] created map, init call
    BOOST_CHECK_EQUAL( impl.init(), true );

    // unit_test[9] stringmap size check(loaded files.)
    // unit_test[10] int last comment erase check(loaded files)
    // unit_test[11] sama key overwrite(init)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 10 );

    // unit_test[12] integermap size check(loaded files)
    // unit_test[13] sama key overwrite(string)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 10 );
    // unit_test[14] sectionmap size check(loaded files)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 10);

    for( unsigned int i = 0 ; i < 10; ++i ){
        std::string key = sectionstrings[i] + "." + keystr[0];
        // unit test[15] int value check(loaded file)
        if( i == 0 )
            BOOST_CHECK_EQUAL( intmap.find( key )->second , 2 );
        else
            BOOST_CHECK_EQUAL( intmap.find( key )->second , 1 );
        key = sectionstrings[i] + "." + keystr[1];
        // unit_test[16] string value check(loaded file)
        // unit_test[17] double cautation erase check(loaded file)
        if( i == 0 )
            BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings2" );
        else
            BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings" );
    }

    // unit_test[18] created init(filename) no file check
    BOOST_CHECK_EQUAL( impl.init( l7vs::PARAM_COMP_SSL, "./add.cf" ), false); 

    std::ofstream ofs2( "./add.cf" );
    ofs2 << "# comment1" << std::endl;
    ofs2 << " # spase comment2" << std::endl;
    ofs2 << "	# tab comment3" << std::endl;
    ofs2 << "[" << sectionstrings[0] << "]\n";
    ofs2 << "key1" << "=" << valuestr[0] << std::endl; //non target tag
    ofs2 << "key2" << "=" << valuestr[1] << std::endl; //non target tag
    ofs2 << "key3" << "=" << valuestr[0] << std::endl; //non target tag
    ofs2 << "key4" << "=" << valuestr[1] << std::endl; //non target tag
    ofs2 << "[" << sectionstrings[9] << "]\n";
    ofs2 << "key1" << "=" << valuestr[0] << std::endl; //overwrite int
    ofs2 << "key2" << "=" << valuestr[1] << std::endl; //add string
    ofs2 << "key3" << "=" << valuestr[0] << std::endl; //add int
    ofs2 << "key4" << "=" << valuestr[1] << std::endl; //add string
    ofs2.close();

    // unit_test[19] init(filename)
    BOOST_CHECK_EQUAL( impl.init( l7vs::PARAM_COMP_SSL, "./add.cf" ), true ); 

    // unit_test[20] stringmap size check(+loaded files)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 12 );
    // unit_test[21] integermap size check(+loaded files)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 11 );
    // unit_test[22] sectionmap size check(+loaded files)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 10 );

    for( unsigned int i = 0 ; i < 10; ++i ){
        std::string key = sectionstrings[i] + "." + keystr[0];
        // unit test[23] int value check(loaded file)
        if( i == 0 )
            BOOST_CHECK_EQUAL( intmap.find( key )->second , 2 );
        else
            BOOST_CHECK_EQUAL( intmap.find( key )->second , 1 );
        key = sectionstrings[i] + "." + keystr[1];
        // unit_test[24] string value check(loaded file)
        // unit_test[25] double cautation erase check(loaded file)
        if( i == 0 )
            BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings2" );
        else
            BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings" );
    }
    // unit_test[26] int value check(+loaded files)
    std::string key = sectionstrings[0] + ".key3";
    BOOST_CHECK_EQUAL( intmap.count( key ) , (unsigned int) 0 );
    // unit_test[27] int value check(+loaded files)
    key = sectionstrings[9] + ".key3";
    BOOST_CHECK_EQUAL( intmap.find( key )->second , 1 );
    // unit_test[28] string value check(+loaded files)
    key = sectionstrings[0] + ".key4";
    BOOST_CHECK_EQUAL( strmap.count( key ), (unsigned int) 0 );
    // unit_test[29] string value check(+loaded files)
    key = sectionstrings[9] + ".key2";
    BOOST_CHECK_EQUAL( strmap.count( key ), (unsigned int) 2 );
    // unit_test[30] string value check(+loaded files)
    key = sectionstrings[9] + ".key4";
    BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings" );

    // unit_test[31] init(filename) readed check
    BOOST_CHECK_EQUAL( impl.init( l7vs::PARAM_COMP_SSL, "./add.cf" ), true); 

    // unit_test[32] non read file check
    unlink( PARAMETER_FILE );
    BOOST_CHECK_EQUAL( impl.read_file( l7vs::PARAM_COMP_ALL, PARAMETER_FILE ),
                       false );

    unlink( "./add.cf" );

    // unit_test[33] strmap is no change check
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 12 );
    // unit_test[34] intmap is no change check
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 11 );
    // unit_test[35] sectionmap is change check
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 10);
    for( unsigned int i = 0 ; i < 10; ++i ){
        std::string key = sectionstrings[i] + "." + keystr[0];
        // unit test[36] int value check(loaded file)
        if( i == 0 )
            BOOST_CHECK_EQUAL( intmap.find( key )->second , 2 );
        else
            BOOST_CHECK_EQUAL( intmap.find( key )->second , 1 );
        key = sectionstrings[i] + "." + keystr[1];
        // unit_test[37] string value check(loaded file)
        // unit_test[38] double cautation erase check(loaded file)
        if( i == 0 )
            BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings2" );
        else
            BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings" );
    }
    
}

// ParameterImpl::read_file() test
void parameter_impl_read_file_test()
{
    parameter_impl_test& impl = parameter_impl_test::get_instance();

    std::multimap<std::string, std::string>& strmap = impl.getMultiStringMap();
    std::map<std::string, int>& intmap = impl.getIntMap();
    std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string >&
                                sectionmap = impl.get_section_map();

    // create dummy data
    strmap.insert( std::pair< std::string, std::string >( "abc", "def" ) );
    intmap.insert( std::pair< std::string, int >( "abc", 65000 ) );
    sectionmap.insert( std::pair< l7vs::PARAMETER_COMPONENT_TAG, std::string >( l7vs::PARAM_COMP_NOCAT, "sample" ) );

#ifdef CASE1

    std::ofstream ofs( PARAMETER_FILE );
    ofs.close();

    // unit_test[39] read size 0 file
    BOOST_CHECK_EQUAL( impl.read_file(l7vs::PARAM_COMP_ALL, PARAMETER_FILE),
                       true );
    // stringmap size check (size 0 file)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 0 );
    // intmap size check (size 0 file)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 0 );
    // section size check (size 0 file)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 1 );
    
#elif CASE2

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]" << std::endl;
    ofs.close();

    // unit_test[40] read only seciton file
    BOOST_CHECK_EQUAL( impl.read_file(l7vs::PARAM_COMP_ALL, PARAMETER_FILE),
                       true );
    // stringmap size check (only section file)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 0 );
    // intmap size check (only section file)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 0 );
    // section size check (only section file)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 1 );

#elif CASE3

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "key1=0" << std::endl;
    ofs.close();

    // unit_test[41] read only value file
    BOOST_CHECK_EQUAL( impl.read_file(l7vs::PARAM_COMP_ALL, PARAMETER_FILE),
                       false );
    // check log "don't match first section."
    // stringmap size check (only value file)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 1 );
    // intmap size check (only value file)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 1 );
    // section size check (only value file)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 1 );

#elif CASE4

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]" << std::endl;
    ofs << "key1=0" << std::endl;
    ofs << "[hoge" << std::endl;
    ofs << "key1=0" << std::endl;
    ofs.close();

    // unit_test[42] read unjust section file
    BOOST_CHECK_EQUAL( impl.read_file(l7vs::PARAM_COMP_ALL, PARAMETER_FILE),
                       false );
    // check log "section tag false"
    // stringmap size check (unjust section file)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 1 );
    // intmap size check (unjust section file)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 1 );
    // section size check (unjust section file)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 1 );

#elif CASE5

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]" << std::endl;
    ofs << "key1=0" << std::endl;
    ofs.close();

    // unit_test[43] read file(PARAM_COMP_NOCAT)
    BOOST_CHECK_EQUAL( impl.read_file(l7vs::PARAM_COMP_NOCAT, PARAMETER_FILE),
                       false );
    // check log "parameter_component_none is not suport"
    // stringmap size check (PARAM_COMP_NOCAT)
    BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 1 );
    // intmap size check (PARAM_COMP_NOCAT)
    BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 1 );
    // section size check (PARAM_COMP_NOCAT)
    BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 1 );

#endif

    unlink( PARAMETER_FILE );

}

// ParameterImpl::get_int() test
void parameter_impl_get_int_test(){
    int zero_value = 0;
    int one_value = 1;
    int under_one_value = -1;
    int int_max_value = (INT32_MAX);
    int int_min_value = (INT32_MIN);

    std::ofstream ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "zero_value = " << zero_value << "\n";
    ofs << "one_value  = " << one_value << "\n";
    ofs << "under_one_value = " << under_one_value << "\n";
    ofs << "int_max_value=" << int_max_value << "\n";
    ofs << "int_min_value =" << int_min_value << "\n";
    ofs.close();

    parameter_impl_test& impl = parameter_impl_test::get_instance();
    l7vs::error_code err;
    std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string >&
                                sectionmap = impl.get_section_map();
    sectionmap.insert( std::pair< l7vs::PARAMETER_COMPONENT_TAG, std::string >( l7vs::PARAM_COMP_L7VSD, "l7vsd" ) );

    // unit_test[44] integer file read check
    BOOST_CHECK_EQUAL( impl.read_file( l7vs::PARAM_COMP_ALL, PARAMETER_FILE ),
                       true );
    // unit_test[45] zero value check
    int ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "zero_value", err );
    BOOST_CHECK_EQUAL( ret , zero_value );
    // unit_test[46] one value check
    ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "one_value", err );
    BOOST_CHECK_EQUAL( ret , one_value );
    // unit_test[47] under one value check
    ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "under_one_value", err );
    BOOST_CHECK_EQUAL( ret,  under_one_value );
    // unit_test[48] int max value check
    ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "int_max_value", err );
    BOOST_CHECK_EQUAL( ret, int_max_value );
    // unit_test[49] int min value check
    ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "int_min_value", err );
    BOOST_CHECK_EQUAL( ret, int_min_value );
    // unit_test[50] no key error check
    ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "no_key_value", err );
    BOOST_CHECK_EQUAL( ret, 0 );
    BOOST_CHECK_EQUAL( err, true );

    unlink( PARAMETER_FILE );
}

// ParameterImpl::get_string() test
void parameter_impl_get_string_test(){
    l7vs::error_code   err;
    std::string        str_value = "strvalue";
    std::string        zero_str_value = "";
    std::string        long_str_value = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_+*-/!$%&'()~|";
    std::string        space_str_value = "sample is not report ";

    std::ofstream    ofs( PARAMETER_FILE );
    ofs << "[l7vsd]\n";
    ofs << "str_value = \"" << str_value << "\"\n";
    ofs << "zero_str_value=\"" << zero_str_value << "\"\n";
    ofs << "long_str_value= \"" << long_str_value << "\"\n";
    ofs << "space_str_value=\"" << space_str_value << "\"\n";
    ofs.close();

    parameter_impl_test& impl = parameter_impl_test::get_instance();
    std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string >&
                                sectionmap = impl.get_section_map();
    sectionmap.insert( std::pair< l7vs::PARAMETER_COMPONENT_TAG, std::string >( l7vs::PARAM_COMP_L7VSD, "l7vsd" ) );

    // unit_test[51] string file read check
    BOOST_CHECK_EQUAL( impl.read_file( l7vs::PARAM_COMP_ALL, PARAMETER_FILE ),
                       true );
    // unit_test[52] str value check
    std::string ret = impl.get_string( l7vs::PARAM_COMP_L7VSD,
                                       "str_value",
                                       err );
    BOOST_CHECK_EQUAL( err, false );
    BOOST_CHECK_EQUAL( ret, str_value );
    // unit_test[53] zero size string test
    ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "zero_str_value", err );
    BOOST_CHECK_EQUAL( ret, zero_str_value );
    // unit_test[54] long string test
    ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "long_str_value", err );
    BOOST_CHECK_EQUAL( ret, long_str_value );
    // unit_test[55] space string test
    ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "space_str_value",err );
    BOOST_CHECK_EQUAL( ret , space_str_value );
    // unit_test[56] string no key test
    ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "no_key_value", err );
    BOOST_CHECK_EQUAL( ret, std::string("") );
    BOOST_CHECK_EQUAL( err, true );

    unlink( PARAMETER_FILE );
}

// ParameterImpl::get_multistring() test
void parameter_impl_get_multistring_test(){
    l7vs::error_code   err;
    std::string        str_value1 = "strvalue1";
    std::string        str_value2 = "strvalue2";
    std::vector<std::string> ret_vector;

    std::ofstream    ofs( PARAMETER_FILE );
    ofs << "[ssl]\n";
    ofs << "str_value2 = \"" << str_value2 << "\"\n";
    ofs << "str_value1 = \"" << str_value1 << "\"\n";
    ofs << "str_value2 = \"" << str_value2 << "\"\n";
    ofs.close();

    parameter_impl_test& impl = parameter_impl_test::get_instance();
    std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string >&
                                sectionmap = impl.get_section_map();
    sectionmap.insert( std::pair< l7vs::PARAMETER_COMPONENT_TAG, std::string >( l7vs::PARAM_COMP_SSL, "ssl" ) );

    // unit_test[57] multistring file read check
    BOOST_CHECK_EQUAL( impl.read_file( l7vs::PARAM_COMP_SSL, PARAMETER_FILE ),
                       true );
    // unit_test[58] multistring value check(2 values)
    impl.get_multistring(l7vs::PARAM_COMP_SSL, "str_value2", ret_vector, err);
    BOOST_CHECK_EQUAL( err, false );
    BOOST_CHECK_EQUAL( ret_vector.size(), (unsigned int)2 );
    BOOST_CHECK_EQUAL( ret_vector[0], str_value2 );
    BOOST_CHECK_EQUAL( ret_vector[1], str_value2 );
    // unit_test[59] multistring value check(1 value)
    impl.get_multistring(l7vs::PARAM_COMP_SSL, "str_value1", ret_vector, err);
    BOOST_CHECK_EQUAL( err, false );
    BOOST_CHECK_EQUAL( ret_vector.size(), (unsigned int)1 );
    BOOST_CHECK_EQUAL( ret_vector[0], str_value1 );
    // unit_test[60] multistring no key test
    impl.get_multistring(l7vs::PARAM_COMP_SSL, "no_key_value", ret_vector, err);
    BOOST_CHECK_EQUAL( ret_vector.empty(), true );
    BOOST_CHECK_EQUAL( err, true );

    unlink( PARAMETER_FILE );

}

test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    // Logger initialize
    l7vs::Logger	logger_instance;
    logger_instance.loadConf();

    // create unit test suite
    // !!!! target test case is only one
    test_suite* ts = BOOST_TEST_SUITE( "parameter_impl" );
//    ts->add( BOOST_TEST_CASE( &parameter_impl_init_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_impl_read_file_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_impl_get_int_test ) );
//    ts->add( BOOST_TEST_CASE( &parameter_impl_get_string_test ) );
    ts->add( BOOST_TEST_CASE( &parameter_impl_get_multistring_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
