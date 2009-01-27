#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include "http_protocol_module_base.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

#define	CHECK_METHOD_OK_STRING_NUM	(16)
#define	CHECK_METHOD_NG_STRING_NUM	(10)
#define	CHECK_METHOD_INPOSSIBLE_STRING_NUM	(2)

//--stub functions--


//--test class--
class	http_protocol_module_base_test : public http_protocol_module_base {
public:
http_protocol_module_base_test( std::string in_modulename ) : http_protocol_module_base( in_modulename ){}
~http_protocol_module_base_test(){}
bool	is_tcp(){ return true; }
bool	is_udp(){ return true; }
void	replication_interrupt(){}
// void	initialize(
// 							rs_list_itr_func_type	inlist_begin,
// 							rs_list_itr_func_type	inlist_end,
// 							rs_list_itr_func_type	inlist_next,
// 							boost::function< void( void ) >	inlist_lock,
// 							boost::function< void( void ) >	inlist_unlock ){}
// 
// void	finalize(){}
// 
// bool	is_use_sorry(){ return true; }
// 
// void	handle_rslist_update(){}

// check_message_result	check_parameter( const std::vector<std::string>& args ){
// 	check_message_result	result;
// 	return result;
// }
// check_message_result	set_parameter( const std::vector<std::string>& args ){
// 	check_message_result	result;
// 	return result;
// }
// check_message_result	add_parameter( const std::vector<std::string>& args ){
// 	check_message_result	result;
// 	return result;
// }
// void	register_schedule( tcp_schedule_func_type inschedule ){}
// void	register_schedule( udp_schedule_func_type inschedule ){}
// EVENT_TAG	handle_session_initialize(
// 									const boost::thread::id up_thread_id,
// 									const boost::thread::id down_thread_id ){ return STOP; }
// EVENT_TAG	handle_session_finalize(
// 									const boost::thread::id up_thread_id,
// 									const boost::thread::id down_thread_id ){ return STOP; }
// EVENT_TAG	handle_accept( const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_client_recv(
// 									const boost::thread::id thread_id,
// 									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
// 									const size_t recvlen ){ return STOP; }
// EVENT_TAG	handle_realserver_select(
// 									const boost::thread::id thread_id,
// 									boost::asio::ip::tcp::endpoint& rs_endpoint ){ return STOP; }
// EVENT_TAG	handle_realserver_select(
// 									const boost::thread::id thread_id,
// 									boost::asio::ip::udp::endpoint& rs_endpoint,
// 									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
// 									size_t& datalen ){ return STOP; }
// EVENT_TAG	handle_realserver_connect(
// 									const boost::thread::id thread_id,
// 									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
// 									size_t& datalen ){ return STOP; }
// EVENT_TAG	handle_realserver_connection_fail(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::tcp::endpoint& rs_endpoint ){ return STOP; }
// EVENT_TAG	handle_realserver_send(
// 									const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_sorryserver_select(
// 									const boost::thread::id thread_id,
// 									boost::asio::ip::tcp::endpoint& sorry_endpoint ){ return STOP; }
// EVENT_TAG	handle_sorryserver_connect(
// 									const boost::thread::id thread_id,
// 									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
// 									size_t& datalen ){ return STOP; }
// EVENT_TAG	handle_sorryserver_connection_fail(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::tcp::endpoint& sorry_endpoint ){ return STOP; }
// EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_realserver_recv(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::tcp::endpoint& rs_endpoint,
// 									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
// 									const size_t recvlen ){ return STOP; }
// EVENT_TAG	handle_realserver_recv(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::udp::endpoint& rs_endpoint,
// 									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
// 									const size_t recvlen ){ return STOP; }
// EVENT_TAG	handle_sorryserver_recv(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::tcp::endpoint& sorry_endpoint,
// 									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
// 									const size_t recvlen ){ return STOP; }
// EVENT_TAG	handle_response_send_inform(
// 									const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_client_connection_check(
// 									const boost::thread::id thread_id,
// 									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
// 									size_t& datalen ){ return STOP; }
// EVENT_TAG	handle_client_select(
// 									const boost::thread::id thread_id,
// 									boost::asio::ip::udp::endpoint& cl_endpoint,
// 									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
// 									size_t& datalen ){ return STOP; }
// EVENT_TAG	handle_client_send( const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_client_disconnect(
// 									const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_sorry_enable( const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_sorry_disable( const boost::thread::id thread_id ){ return STOP; }
// EVENT_TAG	handle_realserver_disconnect(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::tcp::endpoint& rs_endpoint ){ return STOP; }
// EVENT_TAG	handle_sorryserver_disconnect(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::tcp::endpoint& sorry_endpoint ){ return STOP; }
// EVENT_TAG	handle_realserver_close(
// 									const boost::thread::id thread_id,
// 									const boost::asio::ip::udp::endpoint& rs_endpoint ){ return STOP; }

void	check_http_method_test(){

	int count					= 1;

	char	buffer_OK[CHECK_METHOD_OK_STRING_NUM][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"HEAD /abc/def/ HTTP/1.0",
						"POST /abc/def/ HTTP/1.0",
						"PUT /abc/def/ HTTP/1.0",
						"PROPFIND /abc/def/ HTTP/1.0",
						"PROPPATCH /abc/def/ HTTP/1.0",
						"OPTIONS /abc/def/ HTTP/1.0",
						"CONNECT /abc/def/ HTTP/1.0",
						"COPY /abc/def/ HTTP/1.0",
						"TRACE /abc/def/ HTTP/1.0",
						"DELETE /abc/def/ HTTP/1.0",
						"LOCK /abc/def/ HTTP/1.0",
						"UNLOCK /abc/def/ HTTP/1.0",
						"MOVE /abc/def/ HTTP/1.0",
						"MKCOL /abc/def/ HTTP/1.0",
						"MKCOL /abc/def/ ",
					};
	char	buffer_NG[CHECK_METHOD_NG_STRING_NUM][256]
				=	{
						"get /abc/def/ HTTP/1.0",
						"Get /abc/def/ HTTP/1.0",
						"GET/abc/def/ HTTP/1.0",
						"GGET /abc/def/ HTTP/1.0",
						" GET /abc/def/ HTTP/1.0",
						"get GET /abc/def/ HTTP/1.0",
						"get /abc/GET /abc/def/ HTTP/1.0",
						"get /abc/def/ HTTP/1.0 Get ",
						"get /abc/def/ HTTP/1.0 Get",
						"get /abc/def/ HTTP/1.0 Get /abc/def/ HTTP/1.0",
					};
	char	buffer_INPOSSIBLE[CHECK_METHOD_INPOSSIBLE_STRING_NUM][256]
				=	{
						"GET /abc/def/ H",
						"Get /abc/def/ H",
					};
	size_t	buffer_len		= 0;

	BOOST_MESSAGE( "----- check_http_method test start -----" );

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	for( int i = 0; i < CHECK_METHOD_OK_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		buffer_len = strlen( buffer_OK[i] );
		std::cout << "String = [" << buffer_OK[i] << "]  Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( buffer_OK[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_METHOD_NG_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		buffer_len = strlen( buffer_NG[i] );
		std::cout << "String = [" << buffer_NG[i] << "]  Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( buffer_NG[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_METHOD_INPOSSIBLE_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		buffer_len = strlen( buffer_INPOSSIBLE[i] );
		std::cout << "String = [" << buffer_INPOSSIBLE[i] << "]  Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( buffer_INPOSSIBLE[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

//		check_result = http_protocol_module_base_test_1.check_http_method( buffer_OK[i], buffer_len );
//	http_protocol_module_base_test_1.check_http_method( (const char*)&buffer[0], ( const size_t )18 );
//	BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( buffer, buffer_len ) );

//	buffer_len = strlen( buffer2[0] );
//	http_protocol_module_base_test_1.check_http_method( buffer2[0], buffer_len );
//	BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( buffer2[0], buffer_len ) );
//	BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( buffer2[1], buffer_len ) );
//	http_protocol_module_base_test_1.check_http_method( (const char*)&buffer[0], (const size_t)18 );
//	BOOST_CHECK( http_protocol_module_base_test_1.find_http_header( buffer, buffer_len, http_header_name, http_header_offset, http_header_len ) );

	BOOST_MESSAGE( "----- check_http_method test end -----" );
}

void	find_http_header_test(){

	char* buffer					= "GET /abc/defg/ HTTP/1.0";
	size_t buffer_len				= 0;
	std::string http_header_name	= "Cookie";
	size_t http_header_offset		= 0;
	size_t http_header_len			= 0;


	BOOST_MESSAGE( "----- find_http_header test start -----" );

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
//	BOOST_CHECK( http_protocol_module_base_test_1.find_http_header( buffer, buffer_len, http_header_name, http_header_offset, http_header_len ) );

	BOOST_MESSAGE( "----- find_http_header test end -----" );
}

// void	constractor_test(){
// 
// 	BOOST_MESSAGE( "----- constractor test start -----" );
// 
// 	std::string module_name_1 = "cinsert";
// 	std::string module_name_2 = "";
// 
// 	module_base_test	module_base_test_1( module_name_1 );
// 	module_base_test	module_base_test_2( module_name_2 );
// 
// 	// ## test [1]  constractor parameter set test ("cinsert")
// 	
// 	BOOST_CHECK_EQUAL( module_base_test_1.name, module_name_1 );
// 
// 	// ## test [2]  constractor parameter set test ("")
// 	std::cout << "2----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_2.name, module_name_2 );
// 
// 	BOOST_MESSAGE( "----- constractor test end -----" );
// }
// 
// void	get_name_test(){
// 
// 	BOOST_MESSAGE( "----- get_name test start -----" );
// 
// 	std::string module_name_1 = "cinsert";
// 	std::string module_name_2 = "";
// 
// 	module_base_test	module_base_test_1( module_name_1 );
// 	module_base_test	module_base_test_2( module_name_2 );
// 
// 	// ## test [1]  get_name get test ("cinsert")
// 	std::cout << "1----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_1.get_name(), module_name_1 );
// 
// 	// ## test [2]  get_name get test ("cinsert")
// 	std::cout << "2----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_1.get_name(), module_base_test_1.name );
// 
// 	// ## test [3]  get_name get test ("")
// 	std::cout << "3----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_2.get_name(), module_name_2 );
// 
// 	// ## test [4]  get_name get test ("")
// 	std::cout << "4----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_2.get_name(), module_base_test_2.name );
// 
// 	BOOST_MESSAGE( "----- get_name test end -----" );
// }
// 
// void	init_logger_functions_test(
// 							getloglevel_func_type	ingetloglevel,
// 							logger_func_type		inputLogFatal,
// 							logger_func_type		inputLogError,
// 							logger_func_type		inputLogWarn,
// 							logger_func_type		inputLogInfo,
// 							logger_func_type		inputLogDebug ){
// 
// 	BOOST_MESSAGE( "----- init_logger_functions test start -----" );
// 
// 	module_base_test	module_base_test_1( "cinsert" );
// 
// 	std::cout << "0----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.getloglevel	== NULL );
// 	BOOST_CHECK( module_base_test_1.putLogFatal	== NULL );
// 	BOOST_CHECK( module_base_test_1.putLogError	== NULL );
// 	BOOST_CHECK( module_base_test_1.putLogWarn	== NULL );
// 	BOOST_CHECK( module_base_test_1.putLogInfo	== NULL );
// 	BOOST_CHECK( module_base_test_1.putLogDebug	== NULL );
// 
// 	module_base_test_1.init_logger_functions(
// 												ingetloglevel,
// 												inputLogFatal,
// 												inputLogError,
// 												inputLogWarn,
// 												inputLogInfo,
// 												inputLogDebug );
// 
// 	// ## test [1]  init_logger_functions call test (getloglevel)
// 	std::cout << "1----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.getloglevel != NULL );
// 	module_base_test_1.getloglevel();
// 
// 	// ## test [2]  init_logger_functions call test (putLogFatal)
// 	std::cout << "2----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.putLogFatal != NULL );
// 	module_base_test_1.putLogFatal( 0, "", NULL, 0 );
// 
// 	// ## test [3]  init_logger_functions call test (putLogError)
// 	std::cout << "3----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.putLogError != NULL );
// 	module_base_test_1.putLogError( 0, "", NULL, 0 );
// 
// 	// ## test [4]  init_logger_functions call test (putLogWarn)
// 	std::cout << "4----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.putLogWarn != NULL );
// 	module_base_test_1.putLogWarn( 0, "", NULL, 0 );
// 
// 	// ## test [5]  init_logger_functions call test (putLogInfo)
// 	std::cout << "5----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.putLogInfo != NULL );
// 	module_base_test_1.putLogInfo( 0, "", NULL, 0 );
// 
// 	// ## test [6]  init_logger_functions call test (putLogDebug)
// 	std::cout << "6----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.putLogDebug != NULL );
// 	module_base_test_1.putLogDebug( 0, "", NULL, 0 );
// 
// 	BOOST_MESSAGE( "----- init_logger_functions test end -----" );
// }
// 
// void	init_replication_functions_test(
// 							replicationpaymemory_func_type	inreplication_pay_memory,
// 							boost::function< void( void ) >	inreplication_area_lock,
// 							boost::function< void( void ) >	inreplication_area_unlock,
// 							const boost::asio::ip::tcp::endpoint&	invirtual_service_endpoint_tcp,
// 							const boost::asio::ip::udp::endpoint&	invirtual_service_endpoint_udp	){
// 
// 	BOOST_MESSAGE( "----- init_replication_functions test start -----" );
// 
// 	module_base_test	module_base_test_1( "cinsert" );
// 
// 	std::cout << "0----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.replication_pay_memory	== NULL );
// 	BOOST_CHECK( module_base_test_1.replication_area_lock	== NULL );
// 	BOOST_CHECK( module_base_test_1.replication_area_unlock	== NULL );
// 
// 	module_base_test_1.init_replication_functions(
// 												inreplication_pay_memory,
// 												inreplication_area_lock,
// 												inreplication_area_unlock,
// 												invirtual_service_endpoint_tcp,
// 												invirtual_service_endpoint_udp );
// 
// 	// ## test [1]  init_replication_functions parameter set test (replication_pay_memory)
// 	std::cout << "1----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.replication_pay_memory	!= NULL );
// 	module_base_test_1.replication_pay_memory( "", NULL );
// 
// 	// ## test [1]  init_replication_functions call test (replication_area_lock)
// 	std::cout << "2----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.replication_area_lock	!= NULL );
// 	module_base_test_1.replication_area_lock();
// 
// 	// ## test [1]  init_replication_functions call test (replication_area_unlock)
// 	std::cout << "3----------------------------------------" << std::endl;
// 	BOOST_CHECK( module_base_test_1.replication_area_unlock	!= NULL );
// 	module_base_test_1.replication_area_unlock();
// 
// 	// ## test [1]  init_replication_functions parameter set test (virtual_service_endpoint_tcp)
// 	std::cout << "4----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_1.virtual_service_endpoint_tcp, invirtual_service_endpoint_tcp );
// 
// 	// ## test [1]  init_replication_functions parameter set test (virtual_service_endpoint_udp)
// 	std::cout << "5----------------------------------------" << std::endl;
// 	BOOST_CHECK_EQUAL( module_base_test_1.virtual_service_endpoint_udp, invirtual_service_endpoint_udp );
// 
// 	BOOST_MESSAGE( "----- init_replication_functions test end -----" );
// }

};

//--test functions--
void	check_http_method_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.check_http_method_test();

}

void	find_http_header_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.find_http_header_test();

}
// void	constractor_test(){
// 
// 	module_base_test	module_base_test_1( "cinsert" );
// 	module_base_test_1.constractor_test();
// 
// }
// 
// void	get_name_test(){
// 
// 	module_base_test	module_base_test_1( "cinsert" );
// 	module_base_test_1.get_name_test();
// 
// }
// 
// void	init_logger_functions_test(){
// 
// 	boost::function< LOG_LEVEL_TAG(void) >	getloglevel = &stb_getloglevel;
// 	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogFatal	= &stb_putLogFatal;
// 	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogError	= &stb_putLogError;
// 	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogWarn		= &stb_putLogWarn;
// 	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogInfo		= &stb_putLogInfo;
// 	boost::function< void ( const unsigned int, const std::string&, const char*, int ) >	putLogDebug	= &stb_putLogDebug;
// 
// 	module_base_test	module_base_test_1( "cinsert" );
// 	module_base_test_1.init_logger_functions_test(	getloglevel,
// 													putLogFatal,
// 													putLogError,
// 													putLogWarn,
// 													putLogInfo,
// 													putLogDebug);
// 
// }
// 
// void	init_replication_functions_test(){
// 
// 	boost::function< void* ( const std::string&, unsigned int* ) >	replication_pay_memory	=	&stb_replication_pay_memory;
// 	boost::function< void( void ) >	replication_area_lock	=	&stb_replication_area_lock;
// 	boost::function< void( void ) >	replication_area_unlock	=	&stb_replication_area_unlock;
// 	boost::asio::ip::address	address_1;
// 	unsigned short				port_1 = 1111;
// 	boost::asio::ip::address	address_2;
// 	unsigned short				port_2 = 2222;
// 	boost::asio::ip::tcp::endpoint virtual_service_endpoint_tcp( address_1, port_1 );
// 	boost::asio::ip::udp::endpoint virtual_service_endpoint_udp( address_2, port_2 );
// 
// 	module_base_test	module_base_test_1( "cinsert" );
// 	module_base_test_1.init_replication_functions_test(
// 													replication_pay_memory,
// 													replication_area_lock,
// 													replication_area_unlock,
// 													virtual_service_endpoint_tcp,
// 													virtual_service_endpoint_udp );
// 
// }

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "http_protocol_module_base class test" );

// 	ts->add( BOOST_TEST_CASE( &constractor_test ) );
// 	ts->add( BOOST_TEST_CASE( &get_name_test ) );
// 	ts->add( BOOST_TEST_CASE( &init_logger_functions_test ) );
// 	ts->add( BOOST_TEST_CASE( &init_replication_functions_test ) );

	ts->add( BOOST_TEST_CASE( &check_http_method_test ) );
	ts->add( BOOST_TEST_CASE( &find_http_header_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
