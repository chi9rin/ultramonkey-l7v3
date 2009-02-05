#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "replication.h"
#include "logger.h"
#include "parameter.h"

int get_string_stubmode = 0;
int get_int_stubmode = 0;

char	*get_string_table[] = {
	"192.168.0.20",				//	"ip_addr"
	"40000",					//	"service_name"
	"eth1",						//	"nic"
	"virtualservice",			//	"cmponent_id_00"
	"chash",					//	"cmponent_id_01"
	"sslid",					//	"cmponent_id_02"
};
int		get_int_table[] = {
	1000,						//	"interval"
	64,							//	"cmponent_size_00"
	1,							//	"cmponent_size_01"
	200,						//	"cmponent_size_02"
	10							//	"compulsorily_interval"
};


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
void	replication_initialize_test(){
//	int	loop;
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	// unit_test[1]  コンストラクタのテスト
	BOOST_MESSAGE( "unit_test[1]" );
	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[2]  initializeのテスト(正常系)
	BOOST_MESSAGE( "unit_test[2]" );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[3]  finnalizeのテスト(正常系)
	BOOST_MESSAGE( "unit_test[3]" );
	repli1.finalize();

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[4]  initializeのテスト(ip_addrが存在しない)
	BOOST_MESSAGE( "unit_test[4]" );
	get_string_stubmode = 1;
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[5]  initializeのテスト(service_nameが存在しない)
	BOOST_MESSAGE( "unit_test[5]" );
	get_string_stubmode = 2;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[6]  initializeのテスト(nicが存在しない)
	BOOST_MESSAGE( "unit_test[6]" );
	get_string_stubmode = 3;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[7]  initializeのテスト(intervalが存在しない)
	BOOST_MESSAGE( "unit_test[7]" );
	get_string_stubmode = 0;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[8]  initializeのテスト(全部存在しない　initializeはOK)
	BOOST_MESSAGE( "unit_test[8]" );
	get_string_stubmode = 1000;
	get_int_stubmode = 1000;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	// unit_test[9]  initializeのテスト(ip_addrが不正)
	BOOST_MESSAGE( "unit_test[9]" );
	get_string_stubmode = 101;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[10]  initializeのテスト(service_nameが不正)
	BOOST_MESSAGE( "unit_test[10]" );
	get_string_stubmode = 102;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[11]  initializeのテスト(nicが不正)
	BOOST_MESSAGE( "unit_test[11]" );
	get_string_stubmode = 103;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[12]  initializeのテスト(intervalが不正)
	BOOST_MESSAGE( "unit_test[12]" );
	get_string_stubmode = 0;
	get_int_stubmode = 101;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	// unit_test[13]  initializeのテスト(cmponent_id_00が存在しない)
	BOOST_MESSAGE( "unit_test[13]" );
	get_string_stubmode = 4;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[14]  initializeのテスト(cmponent_id_00が不正)
	BOOST_MESSAGE( "unit_test[14]" );
	get_string_stubmode = 104;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[15]  initializeのテスト(cmponent_size_00が存在しない)
	BOOST_MESSAGE( "unit_test[15]" );
	get_int_stubmode = 2;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[16]  initializeのテスト(cmponent_size_00が不正)
	BOOST_MESSAGE( "unit_test[16]" );
	get_int_stubmode = 102;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	get_int_stubmode = 0;
	// unit_test[17]  initializeのテスト(cmponent_id_01が存在しない　OK)
	BOOST_MESSAGE( "unit_test[17]" );
	get_string_stubmode = 5;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[18]  initializeのテスト(cmponent_id_01が不正)
	BOOST_MESSAGE( "unit_test[18]" );
	get_string_stubmode = 105;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[19]  initializeのテスト(cmponent_size_01が存在しない　OK)
	BOOST_MESSAGE( "unit_test[19]" );
	get_int_stubmode = 3;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[20]  initializeのテスト(cmponent_size_01が不正)
	BOOST_MESSAGE( "unit_test[20]" );
	get_int_stubmode = 103;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	get_int_stubmode = 0;
	// unit_test[21]  initializeのテスト(cmponent_id_02が存在しない　OK)
	BOOST_MESSAGE( "unit_test[21]" );
	get_string_stubmode = 6;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[22]  initializeのテスト(cmponent_id_02が不正)
	BOOST_MESSAGE( "unit_test[22]" );
	get_string_stubmode = 106;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[23]  initializeのテスト(cmponent_size_02が存在しない　OK)
	BOOST_MESSAGE( "unit_test[23]" );
	get_int_stubmode = 4;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[24]  initializeのテスト(cmponent_size_02が不正)
	BOOST_MESSAGE( "unit_test[24]" );
	get_int_stubmode = 104;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[25]  initializeのテスト(total_blockが0)
	BOOST_MESSAGE( "unit_test[25]" );
	get_int_stubmode = 102;
	get_string_stubmode = 5;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	get_string_stubmode = 0;
	// unit_test[26]  initializeのテスト(intervalが上限以上)
	BOOST_MESSAGE( "unit_test[26]" );
	get_int_table[0] = 10001;						//	"interval"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[27]  initializeのテスト(intervalが下限未満)
	BOOST_MESSAGE( "unit_test[27]" );
	get_int_table[0] = 9;							//	"interval"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_table[0] = 1000;						//	"interval"
	// unit_test[28]  initializeのテスト(component_idが重複)
	BOOST_MESSAGE( "unit_test[28]" );
	get_string_table[4] = "virtualservice";			//	"cmponent_id_01"
	get_string_table[5] = "virtualservice";			//	"cmponent_id_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"

	// unit_test[29]  initializeのテスト(total_blockが上限以上)
	BOOST_MESSAGE( "unit_test[29]" );
	get_int_table[1] = 40000;						//	"cmponent_size_00"
	get_int_table[2] = 40000;						//	"cmponent_size_01"
	get_int_table[3] = 40000;						//	"cmponent_size_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"

	// unit_test[30]  initializeのテスト(initialize済み)
	BOOST_MESSAGE( "unit_test[30]" );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[31]  finalizeのテスト(finalize済み)
	BOOST_MESSAGE( "unit_test[31]" );
	repli1.finalize();
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
}

//test case2.
void	replication_switch_to_master_test(){
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[32]  switch_to_masterのテスト(SLAVE時)
	BOOST_MESSAGE( "unit_test[32]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	
	// unit_test[33]  switch_to_masterのテスト(MASTER時)
	BOOST_MESSAGE( "unit_test[33]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[34]  switch_to_masterのテスト(MASTER_STOP時)
	BOOST_MESSAGE( "unit_test[34]" );
	repli1.stop();
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	
	// unit_test[35]  switch_to_masterのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[35]" );
	repli1.finalize();
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	
	// unit_test[36]  switch_to_masterのテスト(SLAVE_STOP)
	BOOST_MESSAGE( "unit_test[36]" );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	repli1.stop();
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );

	// unit_test[37]  switch_to_masterのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[37]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
}

//test case3.
void	replication_switch_to_slave_test(){
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[38]  switch_to_slaveのテスト(SLAVE時)
	BOOST_MESSAGE( "unit_test[38]" );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );
	
	// unit_test[39]  switch_to_slaveのテスト(MASTER時)
	BOOST_MESSAGE( "unit_test[39]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[40]  switch_to_slaveのテスト(MASTER_STOP時)
	BOOST_MESSAGE( "unit_test[40]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.stop();
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );
	
	// unit_test[41]  switch_to_slaveのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[41]" );
	repli1.finalize();
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	
	// unit_test[42]  switch_to_slaveのテスト(SLAVE_STOP)
	BOOST_MESSAGE( "unit_test[42]" );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	repli1.stop();
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[43]  switch_to_slaveのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[43]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
}

//test case4.
void	replication_pay_memory_test(){
	boost::asio::io_service io;
	unsigned int	size;
	void*			ptr;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[44]  pay_memoryのテスト(virtualservice時)
	BOOST_MESSAGE( "unit_test[44]" );
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[1] );

	// unit_test[45]  pay_memoryのテスト(chash時)
	BOOST_MESSAGE( "unit_test[45]" );
	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[2] );

	// unit_test[46]  pay_memoryのテスト(sslid時)
	BOOST_MESSAGE( "unit_test[46]" );
	ptr = repli1.pay_memory( "sslid", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[3] );

	// unit_test[47]  pay_memoryのテスト(virtualservice時　サイズ0)
	BOOST_MESSAGE( "unit_test[47]" );
	get_int_table[1] = 0;							//	"cmponent_size_00"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[1] );

	get_int_table[1] = 64;							//	"cmponent_size_00"

	// unit_test[48]  pay_memoryのテスト(chash時　サイズ0)
	BOOST_MESSAGE( "unit_test[48]" );
	get_int_table[2] = 0;							//	"cmponent_size_01"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[2] );

	get_int_table[2] = 1;							//	"cmponent_size_01"

	// unit_test[49]  pay_memoryのテスト(sslid時　サイズ0)
	BOOST_MESSAGE( "unit_test[49]" );
	get_int_table[3] = 0;							//	"cmponent_size_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "sslid", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[3] );

	get_int_table[3] = 200;							//	"cmponent_size_02"

	// unit_test[50]  pay_memoryのテスト(id無効)
	BOOST_MESSAGE( "unit_test[50]" );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "unknown", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, 0U );

	// unit_test[51]  pay_memoryのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[51]" );
	repli1.finalize();
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, 0U );

	// unit_test[52]  pay_memoryのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[52]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, 0U );
}

//test case5.
void	replication_dump_memory_test(){
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[53]  dump_memoryのテスト(正常時)
	BOOST_MESSAGE( "unit_test[53]" );
	repli1.dump_memory();
	BOOST_CHECK( 1 );

	// unit_test[54]  dump_memoryのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[54]" );
	repli1.finalize();
	repli1.dump_memory();
	BOOST_CHECK( 1 );

	// unit_test[55]  dump_memoryのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[55]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.dump_memory();
	BOOST_CHECK( 1 );
}

//test case6.
void	replication_start_test(){
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[56]  startのテスト(SLAVE時)
	BOOST_MESSAGE( "unit_test[56]" );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[57]  startのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( "unit_test[57]" );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[58]  startのテスト(MASTER時)
	BOOST_MESSAGE( "unit_test[58]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[59]  startのテスト(MASTER_STOP時)
	BOOST_MESSAGE( "unit_test[59]" );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[60]  startのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[60]" );
	repli1.finalize();
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[61]  startのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[61]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
}

//test case7.
void	replication_stop_test(){
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[62]  stopのテスト(SLAVE時)
	BOOST_MESSAGE( "unit_test[62]" );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[63]  stopのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( "unit_test[63]" );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[64]  stopのテスト(MASTER_STOP時)
	BOOST_MESSAGE( "unit_test[64]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );

	// unit_test[65]  stopのテスト(MASTER時)
	BOOST_MESSAGE( "unit_test[65]" );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );


	// unit_test[67]  stopのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[67]" );
	repli1.finalize();
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[68]  stopのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[68]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
}

//test case8.
void	replication_force_replicate_test(){
	boost::asio::io_service io;

	get_string_stubmode = 0;
	get_int_stubmode = 0;
	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_string_table[2] = "eth1";					//	"nic"
	get_string_table[3] = "virtualservice";			//	"cmponent_id_00"
	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"
	get_int_table[0] = 1000;						//	"interval"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"
	get_int_table[4] = 10;							//	"compulsorily_interval"

	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[69]  force_replicateのテスト(SLAVE時)
	BOOST_MESSAGE( "unit_test[70]" );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[71]  force_replicateのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( "unit_test[71]" );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[72]  force_replicateのテスト(MASTER_STOP時)
	BOOST_MESSAGE( "unit_test[72]" );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[73]  force_replicateのテスト(MASTER時)
	BOOST_MESSAGE( "unit_test[73]" );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
//	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[74]  force_replicateのテスト(MASTER時　"compulsorily_interval"が存在しない)
	BOOST_MESSAGE( "unit_test[74]" );
	get_int_stubmode = 5;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[75]  force_replicateのテスト(MASTER時　"compulsorily_interval"が上限以上)
	BOOST_MESSAGE( "unit_test[75]" );
	get_int_table[4] = 401;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[76]  force_replicateのテスト(MASTER時　"compulsorily_interval"が下限未満)
	BOOST_MESSAGE( "unit_test[76]" );
	get_int_table[4] = 3;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	get_int_table[4] = 10;

	// unit_test[77]  force_replicateのテスト(未初期化)
	BOOST_MESSAGE( "unit_test[77]" );
	repli1.finalize();
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[78]  force_replicateのテスト(SINGLE時)
	BOOST_MESSAGE( "unit_test[78]" );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.force_replicate();
	BOOST_CHECK( 1 );
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	l7vs::Logger	logger;
	l7vs::Parameter	parameter;

	logger.loadConf();

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "replication_test" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &replication_initialize_test ) );
	ts->add( BOOST_TEST_CASE( &replication_switch_to_master_test ) );
	ts->add( BOOST_TEST_CASE( &replication_switch_to_slave_test ) );
	ts->add( BOOST_TEST_CASE( &replication_pay_memory_test ) );
	ts->add( BOOST_TEST_CASE( &replication_dump_memory_test ) );
	ts->add( BOOST_TEST_CASE( &replication_start_test ) );

	ts->add( BOOST_TEST_CASE( &replication_force_replicate_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

