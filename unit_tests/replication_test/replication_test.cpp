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

	int		set_master_wrapper(){
		return set_master();
	}

	int		set_slave_wrapper(){
		return set_slave();
	}

	int		check_parameter_wrapper(){
		return check_parameter();
	}

	void*	getrpl_wrapper(){
		replication_state.replication_memory = getrpl();
		return replication_state.replication_memory;
	}

	void*	getcmp_wrapper(){
		replication_state.component_memory = getcmp();
		return replication_state.component_memory;
	}

	uint64_t*	getsrf_wrapper(){
		replication_state.surface_block_array_ptr = getsrf();
		return replication_state.surface_block_array_ptr;
	}

	unsigned long long			make_serial_wrapper(){
		return make_serial();
	}


	void*		releaserpl_wrapper(){
		releaserpl();
		return replication_state.replication_memory;
	}

	void*		releasecmp_wrapper(){
		releasecmp();
		return replication_state.component_memory;
	}

	uint64_t*	releasesrf_wrapper(){
		releasesrf();
		return replication_state.surface_block_array_ptr;
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

//! emun States Type string
static const char* replication_mode[] = {
	"REPLICATION_OUT",
	"REPLICATION_SINGLE",
	"REPLICATION_MASTER",
	"REPLICATION_SLAVE",
	"REPLICATION_MASTER_STOP",
	"REPLICATION_SLAVE_STOP"
};

bool receiver_end = false;
boost::asio::io_service global_io;
boost::asio::io_service global_io2;

void receiver_thread()
{
	boost::asio::ip::udp::endpoint	udp_endpoint( boost::asio::ip::address::from_string( "10.144.169.86" ), 40000 );
	boost::asio::ip::udp::socket	receiver_socket( global_io, udp_endpoint );

	char	*recv_memory;
	size_t	size;

	//! Transfer data between active server and standby server.
	struct replication_data_struct{
		unsigned short					id;							//!< ID of Replication Function
		char							pad1[6];
		unsigned long long				serial;						//!< Serial Number
		unsigned int					block_num;					//!< Block Number in Replication memory
		char							pad2[4];
		uint64_t						size;						//!< Data size for checking packet loss
		char							data[DATA_SIZE];			//!< Raw data
	} replication_data;

	//! State Infomation struct to execute Replication.
	struct replication_state_struct{
		enum l7vs::replication::REPLICATION_MODE_TAG		service_status;				//!< States Type of Replication Function
		unsigned long long				send_time;					//!< Completion last time to send data(no use)
		unsigned int					last_send_block;			//!< Completion last Block Number to send data
		unsigned int					last_recv_block;			//!< Completion last Block Number to receive data
		unsigned int					total_block;				//!< The maximum Block Number of Components memory
		void*							replication_memory;			//!< Top address in Replication memory
		void*							component_memory;			//!< Top address in Component memory
		uint64_t						surface_block_no;			//!< Serial number for respect switch
		uint64_t*						surface_block_array_ptr;	//!< Serial number in received respect at every block

		replication_state_struct() :	service_status(l7vs::replication::REPLICATION_OUT),
										send_time(0),
										last_send_block(0),
										last_recv_block(0),
										total_block(0),
										replication_memory(NULL),
										component_memory(NULL),
										surface_block_no(0),
										surface_block_array_ptr(NULL) {}
	} replication_state;


	l7vs::Parameter	param;
	std::string key_id;
	std::string key_size;
	l7vs::error_code	size_ret;

	// Conponent exists
	// Get Component infomation
	for ( int i=0; i<CMP_MAX; i++)
	{
		key_size = boost::io::str( boost::format( "cmponent_size_%02d" ) % i );
		replication_state.total_block += param.get_int( l7vs::PARAM_COMP_REPLICATION, key_size, size_ret );
	}

	// Replication memory is NULL
	if ( NULL == ( replication_state.replication_memory = malloc( replication_state.total_block*DATA_SIZE ) ))	{
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Replication memory is NULL.", __FILE__, __LINE__ );
		goto END;
	}
//	memset(replication_state.replication_memory,0,replication_state.total_block*DATA_SIZE);
	memset(replication_state.replication_memory,'9',replication_state.total_block*DATA_SIZE);


	// Component memory is NULL
//	if ( NULL == ( replication_state.component_memory = malloc( replication_state.total_block*DATA_SIZE ) ) ){
//		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_SYSTEM_MEMORY, 1, "Component memory is NULL.", __FILE__, __LINE__ );
//		goto END;
//	}
//	memset(replication_state.component_memory,0,replication_state.total_block*DATA_SIZE);

	// Surface block array memory is NULL
	if ( NULL == ( replication_state.surface_block_array_ptr = ( uint64_t* )malloc( replication_state.total_block*sizeof(uint64_t) ) ))	{
		l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Surface block array pointer is NULL.", __FILE__, __LINE__ );
		goto END;
	}
	memset(replication_state.surface_block_array_ptr,0,replication_state.total_block*sizeof(uint64_t));

	for ( ; ; ){
		size = receiver_socket.receive( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ) );
		if ( size != sizeof ( struct replication_data_struct ) ){
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_SYSTEM, 1, "Failed in the reception processing of data because of illegal receive size.", __FILE__, __LINE__ );
			goto END;
		}

//std::cout << "receiver_thread 5-1\n";
//std::cout << replication_data.id << "\n";
//std::cout << replication_data.serial << "\n";
//std::cout << replication_data.block_num << "\n";
//std::cout << replication_data.size << "\n";
//std::cout << replication_data.data[0] << "\n";

		// Check replication ID
		if ( replication_data.id != REPLICATION_ID ){
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Get invalid data.", __FILE__, __LINE__ );
			goto END;
		} else if ( replication_data.block_num > replication_state.total_block ){
			// block number is over
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Recv block number is too large.", __FILE__, __LINE__ );
			goto END;
		} else if ( replication_data.serial < replication_state.surface_block_array_ptr[replication_data.block_num] ){
			// Comparison of serial numbers
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Recv replication data is too old.", __FILE__, __LINE__ );
			goto END;
		} else {
			// Substitution of version
			replication_state.surface_block_array_ptr[replication_data.block_num] = replication_data.serial;
		}

		// set recv data
		recv_memory = ( char * )replication_state.replication_memory + DATA_SIZE * replication_data.block_num;

		// received data.
		memcpy( recv_memory, &replication_data.data, DATA_SIZE );

		// set surface block
		replication_state.surface_block_array_ptr[replication_data.block_num] = replication_data.serial;

		// Surface numbers are compared.
		for ( unsigned int i = 0; i < replication_state.total_block-1; i++ ){
			if ( replication_state.surface_block_array_ptr[i] != replication_state.surface_block_array_ptr[i+1] ){
				break;
			}
			if ( i == replication_state.total_block-2 ){
				// Synchronization is executed.
//				memcpy(replication_state.component_memory, replication_state.replication_memory, replication_state.total_block*DATA_SIZE );
				l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Replication stub receiving is success.", __FILE__, __LINE__ );

				int size;
				char* p;
				char* head;
				int h = 0;
				int i = 0;
				std::string	buf;

				// Dump size
				size = DATA_SIZE * replication_state.total_block;

				// Memory Dump
				p = ( char * )replication_state.replication_memory;

				// Output mode
				l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Replication check Start ----------------------------", __FILE__, __LINE__ );
				buf = boost::io::str( boost::format( "Mode is [ %s ]." ) % replication_mode[(int)replication_state.service_status] );
				l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, buf, __FILE__, __LINE__ );
				buf = boost::io::str( boost::format( "Total Block is [ %u ]" ) % replication_state.total_block );
				l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, buf, __FILE__, __LINE__ );

				// Converts into the binary, and writes it to the file. 
				for ( h = 0; h < size / DATA_SIZE; h++ ){
					buf = boost::io::str( boost::format( "Block Number [ %d ]" ) % h );
					l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, buf, __FILE__, __LINE__ );

					for ( i = 0; i < DATA_SIZE / LOG_DATA_WIDTH; i++ ){
						head = p + h * DATA_SIZE + i * LOG_DATA_WIDTH;

						// have to cast char to int. because boost::format ignore char with appointed width.
						buf = boost::io::str( boost::format(	"%02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX  "
																"%02hhX %02hhX %02hhX %02hhX  %02hhX %02hhX %02hhX %02hhX" )
																% ( int )*head % ( int )*(head+1) % ( int )*(head+2) % ( int )*(head+3)
																% ( int )*(head+4) % ( int )*(head+5) % ( int )*(head+6)% ( int )*(head+7)
																% ( int )*(head+8) % ( int )*(head+9) % ( int )*(head+10) % ( int )*(head+11)
																% ( int )*(head+12) % ( int )*(head+13) % ( int )*(head+14) % ( int )*(head+15) );
						l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, buf, __FILE__, __LINE__ );
					}
				}
				l7vs::Logger::putLogInfo( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Replication check End ------------------------------", __FILE__, __LINE__ );

				goto END;
			}
		}

		// set surface block
		replication_state.surface_block_array_ptr[replication_data.block_num] = replication_data.serial;

		// set last recv block number
		if ( replication_state.last_recv_block < replication_state.total_block-1 ){
			replication_state.last_recv_block += 1;
		}else if(replication_state.last_recv_block == replication_state.total_block-1){
			replication_state.last_recv_block = 0;
		}else{
			l7vs::Logger::putLogError( l7vs::LOG_CAT_L7VSD_REPLICATION, 1, "Last send block number is illegal.", __FILE__, __LINE__ );
			goto END;
		}
	}

END:
	if ( NULL != replication_state.replication_memory){
		free(replication_state.replication_memory);
	}
	replication_state.replication_memory = NULL;
	if ( NULL != replication_state.component_memory){
		free(replication_state.component_memory);
	}
	replication_state.component_memory = NULL;
	if ( NULL != replication_state.surface_block_array_ptr ){
		free(replication_state.surface_block_array_ptr);
	}
	replication_state.surface_block_array_ptr=NULL;

	receiver_end = true;
};

void	sender_thread(){
	l7vs::replication	repli1(global_io);

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	unsigned int	size;
	void*			ptr;

	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli1.lock( "virtualservice" ), 0 );
	memset( ptr, '1', size * DATA_SIZE );
	repli1.unlock( "virtualservice" );

	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli1.lock( "chash" ), 0 );
	memset( ptr, '2', size * DATA_SIZE );
	repli1.unlock( "chash" );

	ptr = repli1.pay_memory( "sslid", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli1.lock( "sslid" ), 0 );
	memset( ptr, '3', size * DATA_SIZE );
	repli1.unlock( "sslid" );

	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.force_replicate();

	repli1.finalize();
}

void	sender_thread2(){
	l7vs::replication	repli2(global_io2);
//	l7vs::replication	repli2(global_io);

	BOOST_CHECK_EQUAL( repli2.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli2.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli2.get_status(), l7vs::replication::REPLICATION_SLAVE );

	unsigned int	size;
	void*			ptr;

	ptr = repli2.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli2.lock( "virtualservice" ), 0 );
	memset( ptr, '7', size * DATA_SIZE );
	repli2.unlock( "virtualservice" );

	ptr = repli2.pay_memory( "chash", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli2.lock( "chash" ), 0 );
	memset( ptr, '8', size * DATA_SIZE );
	repli2.unlock( "chash" );

	ptr = repli2.pay_memory( "sslid", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli2.lock( "sslid" ), 0 );
	memset( ptr, '9', size * DATA_SIZE );
	repli2.unlock( "sslid" );

	repli2.switch_to_master();
	BOOST_CHECK_EQUAL( repli2.get_status(), l7vs::replication::REPLICATION_MASTER );

	sleep(3);

	repli2.finalize();
}

void	sender_thread3(){
	boost::asio::ip::udp::endpoint	udp_endpoint( boost::asio::ip::address::from_string( "10.144.169.86" ), 40000 );
	boost::asio::ip::udp::socket	sender_socket( global_io, udp_endpoint );

	//! Transfer data between active server and standby server.
	struct replication_data_struct{
		unsigned short					id;							//!< ID of Replication Function
		char							pad1[6];
		unsigned long long				serial;						//!< Serial Number
		unsigned int					block_num;					//!< Block Number in Replication memory
		char							pad2[4];
		uint64_t						size;						//!< Data size for checking packet loss
		char							data[DATA_SIZE];			//!< Raw data
	} replication_data;

	size_t send_byte;

std::cout << "sender3 " << udp_endpoint.address() << ":" << udp_endpoint.port() << "\n";
	memset( &replication_data, 0, sizeof( struct replication_data_struct ) );
	// Set replication id
	replication_data.id = REPLICATION_ID;
	replication_data.block_num = 0;
	// set serial
	replication_data.serial = 0;

	// set data size (sizeof(replication_data))
	replication_data.size = sizeof( struct replication_data_struct );

	// set replication data (1 block)
	memset( replication_data.data, '7', DATA_SIZE );

	// send to data
sender_socket.connect( udp_endpoint );
	send_byte = sender_socket.send( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ) );
	if ( sizeof( struct replication_data_struct ) != send_byte ){
		return;
	}
sender_socket.close();
	usleep( 1000 );
	
	replication_data.block_num = 1;
	// set serial

	// set replication data (1 block)
	memset( replication_data.data, '8', DATA_SIZE );

	// send to data
sender_socket.connect( udp_endpoint );
	send_byte = sender_socket.send( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ) );
	if ( sizeof( struct replication_data_struct ) != send_byte ){
		return;
	}
sender_socket.close();
	usleep( 1000 );

	replication_data.block_num = 2;
	// set serial

	// set replication data (1 block)
	memset( replication_data.data, '9', DATA_SIZE );

	// send to data
sender_socket.connect( udp_endpoint );
	send_byte = sender_socket.send( boost::asio::buffer( &replication_data, sizeof( struct replication_data_struct ) ) );
	if ( sizeof( struct replication_data_struct ) != send_byte ){
		return;
	}
sender_socket.close();
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

	// unit_test[8]  initializeのテスト(全部存在しない initializeはOK)
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
	// unit_test[13]  check_parameterのテスト(cmponent_id_00が存在しない)
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
	// unit_test[17]  initializeのテスト(cmponent_id_01が存在しない OK)
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
	// unit_test[19]  initializeのテスト(cmponent_size_01が存在しない OK)
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
	// unit_test[21]  initializeのテスト(cmponent_id_02が存在しない OK)
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
	// unit_test[23]  initializeのテスト(cmponent_size_02が存在しない OK)
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
	// unit_test[26]  check_parameterのテスト(intervalが上限以上)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[0] = 10001;						//	"interval"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	// unit_test[27]  check_parameterのテスト(intervalが下限未満)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[0] = 9;							//	"interval"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_int_table[0] = 1000;						//	"interval"
	// unit_test[28]  check_parameterのテスト(component_idが重複)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_table[4] = "virtualservice";			//	"cmponent_id_01"
	get_string_table[5] = "virtualservice";			//	"cmponent_id_02"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );

	get_string_table[4] = "chash";					//	"cmponent_id_01"
	get_string_table[5] = "sslid";					//	"cmponent_id_02"

	// unit_test[29]  check_parameterのテスト(total_blockが上限以上)
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

	// unit_test[30]  getrplのテスト(Replication memoryのmalloc失敗)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	malloc_count = 0 ;
	malloc_dbg_install();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
	malloc_dbg_uninstall();

	// unit_test[31]  getcmpのテスト(Components memoryのmalloc失敗)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	malloc_count = 1 ;
	malloc_dbg_install();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SINGLE );
	malloc_dbg_uninstall();

	// unit_test[32]  getsrfのテスト(surface_blockのmalloc失敗)
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

	// unit_test[35]  switch_to_master&set_masterのテスト(SLAVE時)
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
	
	// unit_test[39]  switch_to_master&set_masterのテスト(SLAVE_STOP)
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
	
	// unit_test[42]  switch_to_slave&set_slaveのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.switch_to_slave();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[43]  switch_to_slave&set_slaveのテスト(MASTER_STOP時)
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

	// unit_test[50]  pay_memoryのテスト(virtualservice時 サイズ0)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[1] = 0;							//	"cmponent_size_00"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[1] );

	get_int_table[1] = 64;							//	"cmponent_size_00"

	// unit_test[51]  pay_memoryのテスト(chash時 サイズ0)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[2] = 0;							//	"cmponent_size_01"
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL == ptr );
	BOOST_CHECK_EQUAL( size, ( unsigned int )get_int_table[2] );

	get_int_table[2] = 1;							//	"cmponent_size_01"

	// unit_test[52]  pay_memoryのテスト(sslid時 サイズ0)
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

	get_int_table[1] = 1;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 1;							//	"cmponent_size_02"

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

	get_string_table[0] = "10.144.169.86";			//	"ip_addr"
	get_string_table[1] = "40000";					//	"service_name"
	get_int_table[1] = 1;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 1;							//	"cmponent_size_02"

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

	// unit_test[74]  force_replicateのテスト(MASTER時 "compulsorily_interval"が存在しない)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_stubmode = 5;
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[75]  force_replicateのテスト(MASTER時 "compulsorily_interval"が上限以上)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[4] = 401;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[76]  force_replicateのテスト(MASTER時 "compulsorily_interval"が下限未満)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_int_table[4] = 3;
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	get_int_table[4] = 10;

	// unit_test[77]  force_replicateのテスト(未初期化)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.finalize();
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	// unit_test[78]  force_replicateのテスト(SINGLE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	get_string_stubmode = 1;
	get_int_stubmode = 1;
	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.initialize(), -1 );
	repli1.force_replicate();
	BOOST_CHECK( 1 );

	get_string_stubmode = 0;
	get_int_stubmode = 0;

	// unit_test[79]  force_replicate&handle_sendのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	receiver_end = false;
	boost::thread	thread_item1( boost::bind ( &receiver_thread ) );
	boost::thread	thread_item2( boost::bind ( &sender_thread ) );

	while( !receiver_end ){
		global_io.poll();
	}

	thread_item2.join();
	thread_item1.join();

	global_io.stop();
	global_io.reset();

	BOOST_CHECK( 1 );

	get_string_table[0] = "192.168.0.20";			//	"ip_addr"

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

//test case13.
void	replication_refer_lock_mutex_test(){
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

	l7vs::replication::mutex_ptr mutex1, mutex2, mutex3, mutex4;

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );


	// unit_test[102]  refer_lock_mutexのテスト("virtualservice")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.refer_lock_mutex( "virtualservice", mutex1 ), 0 );
	mutex1->lock();
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );
	mutex1->unlock();
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );

	// unit_test[103]  refer_lock_mutexのテスト("chash")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.refer_lock_mutex( "chash", mutex2 ), 0 );
	mutex2->lock();
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), true );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );
	mutex2->unlock();
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );

	// unit_test[104]  refer_lock_mutexのテスト("sslid")
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.refer_lock_mutex( "sslid", mutex3 ), 0 );
	mutex3->lock();
	BOOST_CHECK_EQUAL( repli1.locked( "virtualservice" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "chash" ), false );
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), true );
	mutex3->unlock();
	BOOST_CHECK_EQUAL( repli1.locked( "sslid" ), false );

	// unit_test[105]  refer_lock_mutexのテスト(不正なid)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.refer_lock_mutex( "unknown", mutex4 ), -1 );

	repli1.finalize();
}

//test case14.
void	replication_handle_send_test(){
//	boost::asio::io_service io;

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

	l7vs::replication	repli1(global_io);

	get_string_table[0] = "10.144.169.86";			//	"ip_addr"
	get_int_table[1] = 1;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 1;							//	"cmponent_size_02"

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

	// unit_test[106]  send_thread&handle_sendのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	receiver_end = false;
	boost::thread	thread_item1( boost::bind ( &receiver_thread ) );

	unsigned int	size;
	void*			ptr;

	ptr = repli1.pay_memory( "virtualservice", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli1.lock( "virtualservice" ), 0 );
	memset( ptr, '4', size * DATA_SIZE );
	repli1.unlock( "virtualservice" );

	ptr = repli1.pay_memory( "chash", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli1.lock( "chash" ), 0 );
	memset( ptr, '5', size * DATA_SIZE );
	repli1.unlock( "chash" );

	ptr = repli1.pay_memory( "sslid", size );
	BOOST_CHECK( NULL != ptr );
	BOOST_CHECK_EQUAL( repli1.lock( "sslid" ), 0 );
	memset( ptr, '6', size * DATA_SIZE );
	repli1.unlock( "sslid" );

//	global_io.poll();

	// unit_test[107]  send_thread&handle_sendのテスト(SLAVE_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.stop();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE_STOP );

//	global_io.poll();

	// unit_test[108]  send_thread&handle_sendのテスト(MASTER_STOP時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.switch_to_master();

//	global_io.poll();

	// unit_test[109]  send_thread&handle_sendのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );
	repli1.start();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );
	while( !receiver_end ){
		global_io.poll();
	}

	thread_item1.join();
	BOOST_CHECK( 1 );

	repli1.finalize();

	global_io.stop();
	global_io.reset();

	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
}

//test case15.
void	replication_set_master_test(){
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

	// unit_test[110]  set_masterのテスト(SLAVE時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK_EQUAL( repli1.set_master_wrapper(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );	// switch_toではないのでSLAVEのまま
	BOOST_CHECK_EQUAL( repli1.set_slave_wrapper(), 0 );								// socket.cancel2重回避

	// unit_test[111]  set_masterのテスト(ip_adrが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	get_string_table[0] = "192:168:0:20";			//	"ip_addr"

	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
//	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
//	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

//	BOOST_CHECK_EQUAL( repli1.set_master_wrapper(), -1 );							// 不正なip_addrはruntime errorになるのでオミット

	get_string_table[0] = "192.168.0.20";			//	"ip_addr"

	repli1.finalize();
}

//test case16.
void	replication_set_slave_test(){
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
	repli1.switch_to_master();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );

	// unit_test[112]  set_slaveのテスト(MASTER時)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK_EQUAL( repli1.set_slave_wrapper(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_MASTER );	// switch_toではないのでMASTERのまま

	// unit_test[113]  set_slaveのテスト(ip_adrが不正)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	get_string_table[0] = "192:168:0:20";			//	"ip_addr"

	repli1.finalize();
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );

//	BOOST_CHECK_EQUAL( repli1.set_slave_wrapper(), -1 );								// 不正なip_addrはruntime errorになるのでオミット

	get_string_table[0] = "192.168.0.20";			//	"ip_addr"

	repli1.finalize();
}

//test case17.
void	replication_check_parameter_test(){
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

	// unit_test[114]  check_parameterのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK_EQUAL( repli1.check_parameter_wrapper(), 0 );	// 異常系はinitializeにて評価済み

	repli1.finalize();
}

//test case18.
void	replication_getrpl_test(){
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
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );	// param読み込みのため1回実行
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	repli1.finalize();

	// unit_test[115]  getrplのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK( NULL != repli1.getrpl_wrapper() );	// 異常系はinitializeにて評価済み

	repli1.finalize();
}

//test case19.
void	replication_getcmp_test(){
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
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );	// param読み込みのため1回実行
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	repli1.finalize();

	// unit_test[116]  getcmpのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK( NULL != repli1.getcmp_wrapper() );	// 異常系はinitializeにて評価済み

	repli1.finalize();
}

//test case20.
void	replication_getsrf_test(){
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
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );	// param読み込みのため1回実行
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	repli1.finalize();

	// unit_test[117]  getsrfのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK( NULL != repli1.getsrf_wrapper() );	// 異常系はinitializeにて評価済み

	repli1.finalize();
}

//test case21.
void	replication_make_serial_test(){
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
	unsigned long long		value1, value2;

	// unit_test[118]  make_serialのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	value1 = repli1.make_serial_wrapper();
	BOOST_CHECK( 0 != value1 );

	// unit_test[119]  make_serialのテスト(再実行)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	value2 = repli1.make_serial_wrapper();
	BOOST_CHECK( 0 != value1 );
	BOOST_CHECK( value1 != value2 );

	repli1.finalize();
}

//test case22.
void	replication_releaserpl_test(){
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
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );	// param読み込みのため1回実行
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	repli1.finalize();

	// unit_test[120]  releaserplのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK( NULL != repli1.getrpl_wrapper() );
	BOOST_CHECK( NULL == repli1.releaserpl_wrapper() );

	repli1.finalize();
}

//test case23.
void	replication_releasecmp_test(){
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
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );	// param読み込みのため1回実行
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	repli1.finalize();

	// unit_test[121]  releasecmpのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK( NULL != repli1.getcmp_wrapper() );
	BOOST_CHECK( NULL == repli1.releasecmp_wrapper() );
	repli1.finalize();
}

//test case24.
void	replication_releasesrf_test(){
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
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );	// param読み込みのため1回実行
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	repli1.finalize();

	// unit_test[122]  releasesrfのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	BOOST_CHECK( NULL != repli1.getsrf_wrapper() );
	BOOST_CHECK( NULL == repli1.releasesrf_wrapper() );

	repli1.finalize();
}

//test case25.
void	replication_handle_receive_test(){
//	boost::asio::io_service io;

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

	l7vs::replication	repli1(global_io);

	get_string_table[0] = "10.144.169.86";			//	"ip_addr"
	get_int_table[1] = 1;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 1;							//	"cmponent_size_02"

	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_OUT );
	BOOST_CHECK_EQUAL( repli1.initialize(), 0 );
	BOOST_CHECK_EQUAL( repli1.get_status(), l7vs::replication::REPLICATION_SLAVE );

	// unit_test[123]  handle_receiveのテスト(正常系)
	BOOST_MESSAGE( boost::format( "unit_test[%d]" ) % ++count );

	repli1.dump_memory();
	BOOST_CHECK( 1 );
	
//	boost::thread	thread_item( boost::bind ( &sender_thread2 ) );
	boost::thread	thread_item( boost::bind ( &sender_thread3 ) );


	for ( int i = 0 ; i < 3 ; i++ ){
		sleep(1);
//std::cout << "global1 poll\n";
		global_io.poll();
//std::cout << "global2 poll\n";
		global_io2.poll();
//std::cout << "global loop\n";
	}

std::cout << "sender2 wait\n";
	thread_item.join();
std::cout << "sender2 exit\n";

	repli1.dump_memory();
	BOOST_CHECK( 1 );

	get_string_table[0] = "192.168.0.20";			//	"ip_addr"
	get_int_table[1] = 64;							//	"cmponent_size_00"
	get_int_table[2] = 1;							//	"cmponent_size_01"
	get_int_table[3] = 200;							//	"cmponent_size_02"

	repli1.finalize();

	global_io.stop();
	global_io.reset();
	global_io2.stop();
	global_io2.reset();
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
	ts->add( BOOST_TEST_CASE( &replication_refer_lock_mutex_test ) );

	ts->add( BOOST_TEST_CASE( &replication_handle_send_test ) );
	ts->add( BOOST_TEST_CASE( &replication_set_master_test ) );
	ts->add( BOOST_TEST_CASE( &replication_set_slave_test ) );
	ts->add( BOOST_TEST_CASE( &replication_check_parameter_test ) );
	ts->add( BOOST_TEST_CASE( &replication_getrpl_test ) );
	ts->add( BOOST_TEST_CASE( &replication_getcmp_test ) );
	ts->add( BOOST_TEST_CASE( &replication_getsrf_test ) );
	ts->add( BOOST_TEST_CASE( &replication_make_serial_test ) );
	ts->add( BOOST_TEST_CASE( &replication_releaserpl_test ) );
	ts->add( BOOST_TEST_CASE( &replication_releasecmp_test ) );
	ts->add( BOOST_TEST_CASE( &replication_releasesrf_test ) );

	ts->add( BOOST_TEST_CASE( &replication_handle_receive_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}

