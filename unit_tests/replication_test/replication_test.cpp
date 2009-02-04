#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "replication.h"
#include "logger.h"
#include "parameter.h"

int get_string_stubmode = 0;
int get_int_stubmode = 0;

#include "logger_stub.cpp"
#include "parameter_stub.cpp"

class	l7vs::replication;

using namespace boost::unit_test;


namespace l7vs{

class replication_fake : public replication
{
public:
	//!	constractor
	replication_fake( boost::asio::io_service& inreceive_io ) : replication( inreceive_io ) {}
	//! destractor
	~replication_fake(){}
};

}	//namespace l7vs


//test case1.
void	replication_test(){
//	int	loop;
	boost::asio::io_service io;
	std::stringstream buf;

	// unit_test[1]  コンストラクタのテスト
	BOOST_MESSAGE( "unit_test[1]" );
	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[2]  initializeのテスト(正常系)
	BOOST_MESSAGE( "unit_test[2]" );
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[3]  finnalizeのテスト(正常系)
	BOOST_MESSAGE( "unit_test[3]" );
	repli1.finalize();

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[4]  initializeのテスト(ip_addrが存在しない)
	BOOST_MESSAGE( "unit_test[4]" );
	get_string_stubmode = 1;
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[5]  initializeのテスト(service_nameが存在しない)
	BOOST_MESSAGE( "unit_test[5]" );
	get_string_stubmode = 2;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[6]  initializeのテスト(nicが存在しない)
	BOOST_MESSAGE( "unit_test[6]" );
	get_string_stubmode = 3;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[7]  initializeのテスト(intervalが存在しない)
	BOOST_MESSAGE( "unit_test[7]" );
	get_string_stubmode = 0;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[8]  initializeのテスト(全部存在しない　initializeはOK)
	BOOST_MESSAGE( "unit_test[8]" );
	get_string_stubmode = 1000;
	get_int_stubmode = 1000;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	// unit_test[9]  initializeのテスト(ip_addrが不正)
	BOOST_MESSAGE( "unit_test[9]" );
	get_string_stubmode = 101;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[10]  initializeのテスト(service_nameが不正)
	BOOST_MESSAGE( "unit_test[10]" );
	get_string_stubmode = 102;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[11]  initializeのテスト(nicが不正)
	BOOST_MESSAGE( "unit_test[11]" );
	get_string_stubmode = 103;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[12]  initializeのテスト(intervalが不正)
	BOOST_MESSAGE( "unit_test[12]" );
	get_string_stubmode = 0;
	get_int_stubmode = 101;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	// unit_test[13]  initializeのテスト(cmponent_id_00が存在しない)
	BOOST_MESSAGE( "unit_test[13]" );
	get_string_stubmode = 4;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[14]  initializeのテスト(cmponent_id_00が不正)
	BOOST_MESSAGE( "unit_test[14]" );
	get_string_stubmode = 104;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[15]  initializeのテスト(cmponent_size_00が存在しない)
	BOOST_MESSAGE( "unit_test[15]" );
	get_int_stubmode = 2;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[16]  initializeのテスト(cmponent_size_00が不正)
	BOOST_MESSAGE( "unit_test[16]" );
	get_int_stubmode = 102;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	get_int_stubmode = 0;
	// unit_test[17]  initializeのテスト(cmponent_id_01が存在しない　OK)
	BOOST_MESSAGE( "unit_test[17]" );
	get_string_stubmode = 5;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[18]  initializeのテスト(cmponent_id_01が不正)
	BOOST_MESSAGE( "unit_test[18]" );
	get_string_stubmode = 105;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[19]  initializeのテスト(cmponent_size_01が存在しない　OK)
	BOOST_MESSAGE( "unit_test[19]" );
	get_int_stubmode = 3;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[20]  initializeのテスト(cmponent_size_01が不正)
	BOOST_MESSAGE( "unit_test[20]" );
	get_int_stubmode = 103;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	get_int_stubmode = 0;
	// unit_test[21]  initializeのテスト(cmponent_id_02が存在しない　OK)
	BOOST_MESSAGE( "unit_test[21]" );
	get_string_stubmode = 6;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[22]  initializeのテスト(cmponent_id_02が不正)
	BOOST_MESSAGE( "unit_test[22]" );
	get_string_stubmode = 106;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), -1);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[23]  initializeのテスト(cmponent_size_02が存在しない　OK)
	BOOST_MESSAGE( "unit_test[23]" );
	get_int_stubmode = 4;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[24]  initializeのテスト(cmponent_size_02が不正)
	BOOST_MESSAGE( "unit_test[24]" );
	get_int_stubmode = 104;
	repli1.finalize();
	BOOST_CHECK_EQUAL(repli1.initialize(), 0);
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );






}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	l7vs::Logger	logger;
	l7vs::Parameter	parameter;

	logger.loadConf();

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "replication_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &replication_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

