#define	__STDC_LIMIT_MACROS
#include <stdint.h>
#include <fstream>
#include <unistd.h>
#include <boost/test/included/unit_test.hpp>
#include "../../parameter/parameter_impl.cpp"

using namespace boost::unit_test;

//test case1.

class	param_impl_test : public l7vs::ParameterImpl{
public:
	std::map<std::string, std::string>&	getStringMap(){ return stringMap; }
	std::map<std::string, int>&			getIntMap(){ return intMap; }
	std::map<l7vs::PARAMETER_COMPONENT_TAG, std::string>&
										get_section_map(){ return tag_section_table_map;}
	static	param_impl_test&	get_instance(){
		boost::mutex::scoped_lock( param_mutex );
		static param_impl_test	instance;
		return instance;
	}
};

void	impl_init_test(){
	param_impl_test&		impl = param_impl_test::get_instance();

	std::map< std::string, std::string>&	strmap = impl.getStringMap();
	std::map< std::string, int>&			intmap = impl.getIntMap();
	std::map< l7vs::PARAMETER_COMPONENT_TAG, std::string >&
											sectionmap = impl.get_section_map();

	// create dummy data
	strmap.insert( std::pair< std::string, std::string >( "abc", "def" ) );
	intmap.insert( std::pair< std::string, int >( "abc", 65000 ) );
	sectionmap.insert( std::pair< l7vs::PARAMETER_COMPONENT_TAG, std::string >( l7vs::PARAM_COMP_NOCAT, "sample" ) );
	// create init files.
	std::string	sectionstrings[10];
	sectionstrings[0] = "l7vsd";
	sectionstrings[1] = "command";
	sectionstrings[2] = "session";
	sectionstrings[3] = "virtualservice";
	sectionstrings[4] = "module";
	sectionstrings[5] = "replication";
	sectionstrings[6] = "logger";
	sectionstrings[7] = "l7vsadm";
	sectionstrings[8] = "snmpagent";
	sectionstrings[9] = "sslproxy";
	std::string	keystr[2];
	keystr[0] = "key1";
	keystr[1] = "key2";
	std::string valuestr[2];
	valuestr[0] = "1";
	valuestr[1] = "\"strings\"";

	std::ofstream	ofs( PARAMETER_FILE );
	ofs << "# comment1" << std::endl;
	ofs << " # comment2" << std::endl;
	ofs << "	#comment3" << std::endl;
	for( int i = 0 ; i < 10; ++i ){
		ofs << "[" << sectionstrings[i] << "]\n";
			ofs << keystr[0] << "=" << valuestr[0] << "# comment4\n";
			ofs << keystr[1] << " = " << valuestr[1] << "	#comment5\n";
			ofs << "	#comment 6 \n";
	}
	ofs.close();

	// unit_test[1] file load check
	// unit_test[2] comment line erase check
	// unit_test[3] first space comment line check
	// unit_test[4] first tab comment line check
	// unit_test[5] section - section comment line check
	// unit_test[6] last comment line check
	BOOST_CHECK_EQUAL( impl.init(), true );

	// unit_test[7] stringmap size check(loaded files.)
	// unit_test[8] int last comment erase check(loaded files)
	BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 10 );
	// unit_test[9] integermap size check(loded files)
	BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 10 );
	// unit_test[10] sectionmap size check(loaded files)
	BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 10);
	for( unsigned int i = 0 ; i < 10; ++i ){
		std::string	key = sectionstrings[i] + "." + keystr[0];
		// unit test[11] int value check(loaded file)
		BOOST_CHECK_EQUAL( intmap.find( key )->second , 1 );
		key = sectionstrings[i] + "." + keystr[1];
		// unit_test[12] string value check(loaded file)
		// unit_test[13] double cautation erase check(loaded file)
		BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings" );
	}

	// unit_test[14] non read file check
	unlink( PARAMETER_FILE );
	BOOST_CHECK_EQUAL( impl.read_file( l7vs::PARAM_COMP_ALL ), false );
	// unit_test[15] strmap is not change check
	BOOST_CHECK_EQUAL( strmap.size(), (unsigned int) 10 );
	// unit_test[16] intmap is not change check
	BOOST_CHECK_EQUAL( intmap.size(), (unsigned int) 10 );
	// unit_test[17] sectionmap is not change check
	BOOST_CHECK_EQUAL( sectionmap.size(), (unsigned int) 10);
	for( unsigned int i = 0 ; i < 10; ++i ){
		std::string	key = sectionstrings[i] + "." + keystr[0];
		// unit test[18] int value check(loaded file)
		BOOST_CHECK_EQUAL( intmap.find( key )->second , 1 );
		key = sectionstrings[i] + "." + keystr[1];
		// unit_test[19] string value check(loaded file)
		// unit_test[20] double cautation erase check(loaded file)
		BOOST_CHECK_EQUAL( strmap.find( key )->second , "strings" );
	}
	
}

void	impl_get_int_test(){
	int	zero_value = 0;
	int one_value = 1;
	int under_one_value = -1;
	int	int_max_value = (INT32_MAX);
	int int_min_value = (INT32_MIN);

	std::ofstream	ofs( PARAMETER_FILE );
	ofs << "[l7vsd]\n";
	ofs << "zero_value = " << zero_value << "\n";
	ofs << "one_value  = " << one_value << "\n";
	ofs << "under_one_value = " << under_one_value << "\n";
	ofs << "int_max_value=" << int_max_value << "\n";
	ofs << "int_min_value =" << int_min_value << "\n";
	ofs.close();

	param_impl_test&		impl = param_impl_test::get_instance();
	l7vs::parameter::error_code	err;
	// unit_test[21] integer file read check
	BOOST_CHECK_EQUAL( impl.init(), true );
	// unit_test[22] zero value check
	int ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "zero_value", err );
	BOOST_CHECK_EQUAL( ret , zero_value );
	// unit_test[23] one value check
	ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "one_value", err );
	BOOST_CHECK_EQUAL( ret , one_value );
	// unit_test[24] under one value check
	ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "under_one_value", err );
	BOOST_CHECK_EQUAL( ret,  under_one_value );
	// unit_test[25] int max value check
	ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "int_max_value", err );
	BOOST_CHECK_EQUAL( ret, int_max_value );
	// unit_test[26] int min value check
	ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "int_min_value", err );
	BOOST_CHECK_EQUAL( ret, int_min_value );
	// unit_test[27] no key error check
	ret = impl.get_int( l7vs::PARAM_COMP_L7VSD, "no_key_value", err );
	BOOST_CHECK_EQUAL( ret, 0 );
  	BOOST_CHECK_EQUAL( err, false );

	unlink( PARAMETER_FILE );
}

void	impl_get_string_test(){
	l7vs::parameter::error_code	err;
	std::string		str_value = "strvalue";
	std::string		zero_str_value = "";
	std::string		long_str_value = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_+*-/!$%&'()~=~~|";
	std::string		space_str_value = "sample is not report ";

	std::ofstream	ofs( PARAMETER_FILE );
	ofs << "str_value = \"" << str_value << "\"\n";
	ofs << "zero_str_value=\"" << zero_str_value << "\"\n";
	ofs << "long_str_value= \"" << long_str_value << "\"\n";
	ofs << "space_str_value=\"" << space_str_value << "\"\n";
	ofs.close();

	param_impl_test&		impl = param_impl_test::get_instance();
	// unit_test[28] integer file read check
	BOOST_CHECK_EQUAL( impl.init(), true );
	// unit_test[29] str value check
	std::string ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "str_value", err );
	BOOST_CHECK_EQUAL( ret, str_value );
	// unit_test[30] zero size string test
	ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "zero_str_value", err );
	BOOST_CHECK_EQUAL( ret, zero_str_value );
	// unit_test[31] long string test
	ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "long_str_value", err );
	BOOST_CHECK_EQUAL( ret, long_str_value );
	// unit_test[32] space string test
	ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "space_str_value",err );
	BOOST_CHECK_EQUAL( ret , space_str_value );
	// unit_test[33] string no key test
	ret = impl.get_string( l7vs::PARAM_COMP_L7VSD, "no_key_value", err );
	BOOST_CHECK_EQUAL( ret, std::string("") );
 	BOOST_CHECK_EQUAL( err, false );
	unlink( PARAMETER_FILE );
}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "l7vsadm" );
	ts->add( BOOST_TEST_CASE( &impl_init_test ) );
	ts->add( BOOST_TEST_CASE( &impl_get_int_test ) );
	ts->add( BOOST_TEST_CASE( &impl_get_string_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
