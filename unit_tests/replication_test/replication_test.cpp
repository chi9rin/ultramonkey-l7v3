#include	<iostream>
#include	<boost/test/included/unit_test.hpp>
#include	"replication.h"
#include	"logger.h"
#include	"parameter.h"
#include	<malloc.h>

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


#include	"logger_stub.cpp"
#include	"parameter_stub.cpp"

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

	bool	locked( const std::string& inid ){
		std::map<std::string, mutex_ptr>::iterator	itr;
		bool	res;
	
		itr = replication_mutex.find( inid );
		if( itr == replication_mutex.end() ){
			return false;
		}

		res = itr->second->try_lock();
		if ( res ){
			itr->second->unlock();
		}
		return !res;
	}
};

}	//namespace l7vs

int		count = 0;

extern "C"
{
	extern void* _start;
	extern void* _etext;
	//--------------------------- mallocスタブ関数 --------------------------------------
	static void*			malloc_dbg(size_t, const void *);
	static void				malloc_dbg_install();
	static void				malloc_dbg_uninstall();
	static void*			(*old_malloc_hook)(size_t, const void *);

	static void malloc_dbg_install ()
	{
		old_malloc_hook = __malloc_hook;
		__malloc_hook = malloc_dbg;
	}

	static void malloc_dbg_uninstall ()
	{
		__malloc_hook = old_malloc_hook;
	}

	//mallocデバッグ
	int		malloc_count = 0;

	static void * malloc_dbg(size_t size, const void *caller){
		void *ret;

		if((&_start < caller) && (caller < &_etext)){
			if (0 < malloc_count){
				--malloc_count;
				malloc_dbg_uninstall();
				ret = malloc(size);
				malloc_dbg_install();
				return ret;
			} else {
				return NULL;
			}
		} else {
			malloc_dbg_uninstall();
			ret = malloc(size);
			malloc_dbg_install();
			return ret;
		}
	}
}

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
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	l7vs::replication	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[2]  initializeのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[3]  finnalizeのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[4]  initializeのテスト(ip_addrが存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 1;
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[5]  initializeのテスト(service_nameが存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 2;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[6]  initializeのテスト(nicが存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 3;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[7]  initializeのテスト(intervalが存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 0;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[8]  initializeのテスト(全部存在しない　initializeはOK)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 1000;
	get_int_stubmode = 1000;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	// unit_test[9]  initializeのテスト(ip_addrが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 101;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[10]  initializeのテスト(service_nameが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 102;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[11]  initializeのテスト(nicが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 103;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[12]  initializeのテスト(intervalが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 0;
	get_int_stubmode = 101;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	// unit_test[13]  initializeのテスト(cmponent_id_00が存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 4;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[14]  initializeのテスト(cmponent_id_00が不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 104;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[15]  initializeのテスト(cmponent_size_00が存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 2;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[16]  initializeのテスト(cmponent_size_00が不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 102;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	get_int_stubmode = 0;
	// unit_test[17]  initializeのテスト(cmponent_id_01が存在しない　OK)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 5;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[18]  initializeのテスト(cmponent_id_01が不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 105;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[19]  initializeのテスト(cmponent_size_01が存在しない　OK)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 3;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[20]  initializeのテスト(cmponent_size_01が不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 103;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	get_int_stubmode = 0;
	// unit_test[21]  initializeのテスト(cmponent_id_02が存在しない　OK)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 6;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[22]  initializeのテスト(cmponent_id_02が不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 106;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_stubmode = 0;
	// unit_test[23]  initializeのテスト(cmponent_size_02が存在しない　OK)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 4;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[24]  initializeのテスト(cmponent_size_02が不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 104;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[25]  initializeのテスト(total_blockが0)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 102;
	get_string_stubmode = 5;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_stubmode = 0;
	get_string_stubmode = 0;
	// unit_test[26]  initializeのテスト(intervalが上限以上)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[0] = 10001;						//	"interval"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[27]  initializeのテスト(intervalが下限未満)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[0] = 9;							//	"interval"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_table[0] = 1000;						//	"interval"
	// unit_test[28]  initializeのテスト(component_idが重複)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_table[4] = "virtualservice";			//	"cmponent_id_01"
	get_string_table[5] = "virtualservice";			//	"cmponent_id_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"

	// unit_test[29]  initializeのテスト(total_blockが上限以上)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[1] = 40000;						//	"cmponent_size_00"
	get_int_table[2] = 40000;						//	"cmponent_size_01"
	get_int_table[3] = 40000;						//	"cmponent_size_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"

	// unit_test[30]  initializeのテスト(Replication memoryのmalloc失敗)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	malloc_count = 0 ;
	malloc_dbg_install();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
	malloc_dbg_uninstall();

	// unit_test[31]  initializeのテスト(Components memoryのmalloc失敗)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	malloc_count = 1 ;
	malloc_dbg_install();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
	malloc_dbg_uninstall();

	// unit_test[32]  initializeのテスト(surface_blockのmalloc失敗)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	malloc_count = 2 ;
	malloc_dbg_install();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
	malloc_dbg_uninstall();

	// unit_test[33]  initializeのテスト(initialize済み)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[34]  finalizeのテスト(finalize済み)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	// unit_test[35]  switch_to_masterのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	
	// unit_test[36]  switch_to_masterのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[37]  switch_to_masterのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	
	// unit_test[38]  switch_to_masterのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	
	// unit_test[39]  switch_to_masterのテスト(SLAVE_STOP)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	repli1.stop();
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );

	// unit_test[40]  switch_to_masterのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	// unit_test[41]  switch_to_slaveのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );
	
	// unit_test[42]  switch_to_slaveのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[43]  switch_to_slaveのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.stop();
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );
	
	// unit_test[44]  switch_to_slaveのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	
	// unit_test[45]  switch_to_slaveのテスト(SLAVE_STOP)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	repli1.stop();
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[46]  switch_to_slaveのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	// unit_test[47]  pay_memoryのテスト(virtualservice時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[1] );

	// unit_test[48]  pay_memoryのテスト(chash時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[2] );

	// unit_test[49]  pay_memoryのテスト(sslid時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	ptr = repli1.pay_memory( "sslid", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[3] );

	// unit_test[50]  pay_memoryのテスト(virtualservice時　サイズ0)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[1] = 0;							//	"cmponent_size_00"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[1] );

	get_int_table[1] = 64;							//	"cmponent_size_00"

	// unit_test[51]  pay_memoryのテスト(chash時　サイズ0)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[2] = 0;							//	"cmponent_size_01"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[2] );

	get_int_table[2] = 1;							//	"cmponent_size_01"

	// unit_test[52]  pay_memoryのテスト(sslid時　サイズ0)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[3] = 0;							//	"cmponent_size_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "sslid", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[3] );

	get_int_table[3] = 200;							//	"cmponent_size_02"

	// unit_test[53]  pay_memoryのテスト(id無効)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "unknown", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, 0U );

	// unit_test[54]  pay_memoryのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, 0U );

	// unit_test[55]  pay_memoryのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	// unit_test[56]  dump_memoryのテスト(正常時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.dump_memory();
	BOOST_CHECK( 1 );

	// unit_test[57]  dump_memoryのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.dump_memory();
	BOOST_CHECK( 1 );

	// unit_test[58]  dump_memoryのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	// unit_test[59]  startのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[60]  startのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[61]  startのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[62]  startのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[63]  startのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[64]  startのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	// unit_test[65]  stopのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[66]  stopのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[67]  stopのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );

	// unit_test[68]  stopのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );


	// unit_test[69]  stopのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[70]  stopのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
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

	get_string_table[0] = "10.144.169.87";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[71]  force_replicateのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[72]  force_replicateのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[73]  force_replicateのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[74]  force_replicateのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
//	repli1.force_replicate();
	BOOST_CHECK( 1 );

	get_string_table[0] = "192.168.0.20";			//	"ip_addr"

	// unit_test[75]  force_replicateのテスト(MASTER時　"compulsorily_interval"が存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 5;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[76]  force_replicateのテスト(MASTER時　"compulsorily_interval"が上限以上)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[4] = 401;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[77]  force_replicateのテスト(MASTER時　"compulsorily_interval"が下限未満)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[4] = 3;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	get_int_table[4] = 10;

	// unit_test[78]  force_replicateのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[79]  force_replicateのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.force_replicate();
	BOOST_CHECK( 1 );
}

//test case9.
void	replication_reset_test(){
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

	// unit_test[80]  resetのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.reset();
	BOOST_CHECK( 1 );

	// unit_test[81]  resetのテスト(intervalが存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 1;
	repli1.reset();
	BOOST_CHECK( 1 );

	// unit_test[82]  resetのテスト(intervalが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 101;
	repli1.reset();
	BOOST_CHECK( 1 );

	get_int_stubmode = 0;
	// unit_test[83]  resetのテスト(intervalが上限以上)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[0] = 10001;						//	"interval"
	repli1.reset();
	BOOST_CHECK( 1 );

	// unit_test[84]  resetのテスト(intervalが下限未満)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[0] = 9;							//	"interval"
	repli1.reset();
	BOOST_CHECK( 1 );

	get_int_table[0] = 1000;						//	"interval"
	repli1.finalize();
}

//test case10.
void	replication_get_status_test(){
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

	// unit_test[85]  get_statusのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[86]  get_statusのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );
	
	// unit_test[87]  get_statusのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[88]  get_statusのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER_STOP );
	
	// unit_test[89]  get_statusのテスト(SLAVE_STOP)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

	// unit_test[90]  get_statusのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
}

//test case11.
void	replication_lock_test(){
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

	l7vs::replication_fake	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	// unit_test[91]  lockのテスト("virtualservice")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.lock( "virtualservice" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	// unit_test[92]  lockのテスト("chash")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.lock( "chash" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	// unit_test[93]  lockのテスト("sslid")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.lock( "sslid" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );

	// unit_test[94]  lockのテスト(不正なid)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.lock( "unknown" ), -1 );

	repli1.unlock( "virtualservice" );
	repli1.unlock( "chash" );
	repli1.unlock( "sslid" );

	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	repli1.finalize();
}

//test case12.
void	replication_unlock_test(){
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

	l7vs::replication_fake	repli1(io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	BOOST_CHECK_EQUAL( repli1.lock( "virtualservice" ), 0 );
	BOOST_CHECK_EQUAL( repli1.lock( "chash" ), 0 );
	BOOST_CHECK_EQUAL( repli1.lock( "sslid" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );

	// unit_test[95]  unlockのテスト("virtualservice")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "virtualservice" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );

	// unit_test[96]  unlockのテスト(再度"virtualservice")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "virtualservice" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );

	// unit_test[97]  unlockのテスト("chash")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "chash" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );

	// unit_test[98]  unlockのテスト(再度"chash")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "chash" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );

	// unit_test[99]  unlockのテスト("sslid")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "sslid" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	// unit_test[100]  unlockのテスト(再度"sslid")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "sslid" ), 0 );
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	// unit_test[101]  unlockのテスト(不正なid)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.unlock( "unknown" ), -1 );

	repli1.finalize();
}







//test caseX1.
void	replication_handle_send_test(){
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

	// unit_test[xx]  get_statusのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );





	repli1.finalize();
}

//test caseX2.
void	replication_handle_receive_test(){
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

	// unit_test[xx]  handle_receiveのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );





	repli1.finalize();
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
	ts->add( BOOST_TEST_CASE( &replication_stop_test ) );

	ts->add( BOOST_TEST_CASE( &replication_force_replicate_test ) );

	ts->add( BOOST_TEST_CASE( &replication_reset_test ) );
	ts->add( BOOST_TEST_CASE( &replication_get_status_test ) );
	ts->add( BOOST_TEST_CASE( &replication_lock_test ) );
	ts->add( BOOST_TEST_CASE( &replication_unlock_test ) );








	ts->add( BOOST_TEST_CASE( &replication_handle_send_test ) );
	ts->add( BOOST_TEST_CASE( &replication_handle_receive_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

