#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include "http_protocol_module_base.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

#define	CHECK_METHOD_OK_STRING_NUM	(16)
#define	CHECK_METHOD_NG_STRING_NUM	(8)
#define	CHECK_METHOD_INPOSSIBLE_STRING_NUM	(3)

#define	CHECK_VERSION_OK_STRING_NUM	(10)
#define	CHECK_VERSION_NG_STRING_NUM	(16)
#define	CHECK_VERSION_INPOSSIBLE_STRING_NUM	(4)

#define	CHECK_STATUS_CODE_OK_STRING_NUM	(6)
#define	CHECK_STATUS_CODE_NG_STRING_NUM	(9)
#define	CHECK_STATUS_CODE_INPOSSIBLE_STRING_NUM	(2)

#define	FIND_URI_OK_STRING_NUM	(4)
#define	FIND_URI_NG_STRING_NUM	(4)

#define	FIND_STATUS_CODE_OK_STRING_NUM	(2)
#define	FIND_STATUS_CODE_NG_STRING_NUM	(4)

#define	THREAD_01_LOOP_NUM	(100)
#define	THREAD_02_LOOP_NUM	(0)

//--stub functions--


//--test class--
class	http_protocol_module_base_test : public http_protocol_module_base {
public:
http_protocol_module_base_test( std::string in_modulename ) : http_protocol_module_base( in_modulename ){}
~http_protocol_module_base_test(){}
bool	is_tcp(){ return true; }
bool	is_udp(){ return true; }
void	replication_interrupt(){}
void	initialize(
							rs_list_itr_func_type	inlist_begin,
							rs_list_itr_func_type	inlist_end,
							rs_list_itr_next_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock ){}

void	finalize(){}

bool	is_use_sorry(){ return true; }

void	handle_rslist_update(){}

check_message_result	check_parameter( const std::vector<std::string>& args ){
	check_message_result	result;
	return result;
}
check_message_result	set_parameter( const std::vector<std::string>& args ){
	check_message_result	result;
	return result;
}
check_message_result	add_parameter( const std::vector<std::string>& args ){
	check_message_result	result;
	return result;
}
void	register_schedule( tcp_schedule_func_type inschedule ){}
void	register_schedule( udp_schedule_func_type inschedule ){}
EVENT_TAG	handle_session_initialize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id,
									const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
									const boost::asio::ip::udp::endpoint& client_endpoint_udp ){ return STOP; }
EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id ){ return STOP; }
EVENT_TAG	handle_accept( const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ){ return STOP; }
EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint ){ return STOP; }
EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){ return STOP; }
EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){ return STOP; }
EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint ){ return STOP; }
EVENT_TAG	handle_realserver_send(
									const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& sorry_endpoint ){ return STOP; }
EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){ return STOP; }
EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint ){ return STOP; }
EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ){ return STOP; }
EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint& rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ){ return STOP; }
EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen ){ return STOP; }
EVENT_TAG	handle_response_send_inform(
									const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){ return STOP; }
EVENT_TAG	handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& cl_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){ return STOP; }
EVENT_TAG	handle_client_send( const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_client_disconnect(
									const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_sorry_enable( const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_sorry_disable( const boost::thread::id thread_id ){ return STOP; }
EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& rs_endpoint ){ return STOP; }
EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint& sorry_endpoint ){ return STOP; }
EVENT_TAG	handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint& rs_endpoint ){ return STOP; }

void	check_http_method_test_t1(){

	std::string		thread_name	= "[Thread_01] ";

	int count	= 1;

	char	buffer_ok[CHECK_METHOD_OK_STRING_NUM][256]
				=	{
// ## test [01] check_http_method( CheckData = "GET /abc/def/ HTTP/1.0" / Result = OK )
// ## test [02] check_http_method( CheckData = "HEAD /abc/def/ HTTP/1.0" / Result = OK )
// ## test [03] check_http_method( CheckData = "POST /abc/def/ HTTP/1.0" / Result = OK )
// ## test [04] check_http_method( CheckData = "PUT /abc/def/ HTTP/1.0" / Result = OK )
// ## test [05] check_http_method( CheckData = "PROPFIND /abc/def/ HTTP/1.0" / Result = OK )
// ## test [06] check_http_method( CheckData = "PROPPATCH /abc/def/ HTTP/1.0" / Result = OK )
// ## test [07] check_http_method( CheckData = "OPTIONS /abc/def/ HTTP/1.0" / Result = OK )
// ## test [08] check_http_method( CheckData = "CONNECT /abc/def/ HTTP/1.0" / Result = OK )
// ## test [09] check_http_method( CheckData = "COPY /abc/def/ HTTP/1.0" / Result = OK )
// ## test [10] check_http_method( CheckData = "TRACE /abc/def/ HTTP/1.0" / Result = OK )
// ## test [11] check_http_method( CheckData = "DELETE /abc/def/ HTTP/1.0" / Result = OK )
// ## test [12] check_http_method( CheckData = "LOCK /abc/def/ HTTP/1.0" / Result = OK )
// ## test [13] check_http_method( CheckData = "UNLOCK /abc/def/ HTTP/1.0" / Result = OK )
// ## test [14] check_http_method( CheckData = "MOVE /abc/def/ HTTP/1.0" / Result = OK )
// ## test [15] check_http_method( CheckData = "MKCOL /abc/def/ HTTP/1.0" / Result = OK )
// ## test [16] check_http_method( CheckData = "GET /a HTTP/1.0" / Result = OK )
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
						"GET /a HTTP/1.0",
					};
	char	buffer_ng[CHECK_METHOD_NG_STRING_NUM][256]
				=	{
// ## test [17] check_http_method( CheckData = "get /abc/def/ HTTP/1.0" / Result = NG )
// ## test [18] check_http_method( CheckData = "Get /abc/def/ HTTP/1.0" / Result = NG )
// ## test [19] check_http_method( CheckData = "GET/abc/def/ HTTP/1.0" / Result = NG )
// ## test [20] check_http_method( CheckData = "GGET /abc/def/ HTTP/1.0" / Result = NG )
// ## test [21] check_http_method( CheckData = " GET /abc/def/ HTTP/1.0" / Result = NG )
// ## test [22] check_http_method( CheckData = "get GET /abc/def/ HTTP/1.0" / Result = NG )
// ## test [23] check_http_method( CheckData = "get /abc/GET /abc/def/ HTTP/1.0" / Result = NG )
// ## test [24] check_http_method( CheckData = "GET /abc/def/ HTTP/1.0 GET /abc/def/ HTTP/1.0" / Result = NG )
						"get /abc/def/ HTTP/1.0",
						"Get /abc/def/ HTTP/1.0",
						"GET/abc/def/ HTTP/1.0",
						"GGET /abc/def/ HTTP/1.0",
						" GET /abc/def/ HTTP/1.0",
						"get GET /abc/def/ HTTP/1.0",
						"get /abc/GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ HTTP/1.0 GET /abc/def/ HTTP/1.0",
					};
	char	buffer_inpossible[CHECK_METHOD_INPOSSIBLE_STRING_NUM][256]
				=	{
// ## test [25] check_http_method( CheckData = "GET / HTTP/1.0" / Result = INPOSSIBLE )
// ## test [26] check_http_method( CheckData = "Get / HTTP/1.0" / Result = INPOSSIBLE )
// ## test [27] check_http_method( CheckData = "" / Result = INPOSSIBLE )
						"GET / HTTP/1.0",
						"Get / HTTP/1.0",
						"",
					};
	size_t	buffer_len	= 0;

	BOOST_MESSAGE( "----- check_http_method test start -----" );

	for( int i = 0; i < CHECK_METHOD_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [01] - [16]
		BOOST_CHECK( check_http_method( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_METHOD_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [17] - [24]
		BOOST_CHECK( check_http_method( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_METHOD_INPOSSIBLE_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_inpossible[i] << "]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [25] - [27]
		BOOST_CHECK( check_http_method( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [28] check_http_method( CheckData = NULL, Length = 0 / Result = NG )
	BOOST_CHECK( check_http_method( NULL, buffer_len ) == CHECK_NG );
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [29] check_http_method( CheckData = NULL, Length = 100 / Result = NG )
	BOOST_CHECK( check_http_method( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_http_method test end -----" );
}

void	check_http_method_test_t2(){

	std::string		thread_name	= "[Thread_02] ";

	int count	= 1;

	char	buffer_ok[CHECK_METHOD_OK_STRING_NUM][256]
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
						"GET /a HTTP/1.0",
					};
	char	buffer_ng[CHECK_METHOD_NG_STRING_NUM][256]
				=	{
						"get /abc/def/ HTTP/1.0",
						"Get /abc/def/ HTTP/1.0",
						"GET/abc/def/ HTTP/1.0",
						"GGET /abc/def/ HTTP/1.0",
						" GET /abc/def/ HTTP/1.0",
						"get GET /abc/def/ HTTP/1.0",
						"get /abc/GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ HTTP/1.0 GET /abc/def/ HTTP/1.0",
					};
	char	buffer_inpossible[CHECK_METHOD_INPOSSIBLE_STRING_NUM][256]
				=	{
						"GET / HTTP/1.0",
						"Get / HTTP/1.0",
						"",
					};
	size_t	buffer_len	= 0;

	BOOST_MESSAGE( "----- check_http_method test start -----" );

	for( int i = 0; i < CHECK_METHOD_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_http_method( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_METHOD_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_http_method( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_METHOD_INPOSSIBLE_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_inpossible[i] << "]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_http_method( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( check_http_method( NULL, buffer_len ) == CHECK_NG );
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( check_http_method( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_http_method test end -----" );
}


void	check_http_version_test_t1(){

	std::string		thread_name	= "[Thread_01] ";

	int count	= 1;

	char	buffer_ok[CHECK_VERSION_OK_STRING_NUM][256]
				=	{
// ## test [01] check_http_version( CheckData = "GET /abc/def/ HTTP/1.0" / Result = OK )
// ## test [02] check_http_version( CheckData = "GET /abc/def/ HTTP/1.1" / Result = OK )
// ## test [03] check_http_version( CheckData = "GET /abc/def/ghi HTTP/1.0" / Result = OK )
// ## test [04] check_http_version( CheckData = "GET /abc/def/ghi HTTP/1.1" / Result = OK )
// ## test [05] check_http_version( CheckData = "get /abc/def/ghi HTTP/1.0" / Result = OK )
// ## test [06] check_http_version( CheckData = "get /abc/def/ghi HTTP/1.1" / Result = OK )
// ## test [07] check_http_version( CheckData = "HTTP/1.0 100 abcdff" / Result = OK )
// ## test [08] check_http_version( CheckData = "HTTP/1.1 100 abcdff" / Result = OK )
// ## test [09] check_http_version( CheckData = "HTTP/1.0 404 abcdff" / Result = OK )
// ## test [10] check_http_version( CheckData = "HTTP/1.1 404 abcdff" / Result = OK )
						"GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ HTTP/1.1",
						"GET /abc/def/ghi HTTP/1.0",
						"GET /abc/def/ghi HTTP/1.1",
						"get /abc/def/ghi HTTP/1.0",
						"get /abc/def/ghi HTTP/1.1",
						"HTTP/1.0 100 abcdff",
						"HTTP/1.1 100 abcdff",
						"HTTP/1.0 404 abcdff",
						"HTTP/1.1 404 abcdff",
					};
	char	buffer_ng[CHECK_VERSION_NG_STRING_NUM][256]
				=	{
// ## test [11] check_http_version( CheckData = "GET /abc/def/ HTTP/0.0" / Result = NG )
// ## test [12] check_http_version( CheckData = "GET /abc/def/ HTTP/0.9" / Result = NG )
// ## test [13] check_http_version( CheckData = "GET /abc/def/ HTTP/1.2" / Result = NG )
// ## test [14] check_http_version( CheckData = "GET /abc/def/ HTTP/2.0" / Result = NG )
// ## test [15] check_http_version( CheckData = "GET /abc/def/ghiHTTP/1.0" / Result = NG )
// ## test [16] check_http_version( CheckData = "GET/abc/def/ghi HTTP/1.1" / Result = NG )
// ## test [17] check_http_version( CheckData = "/abc/def/ghi HTTP/1.0" / Result = NG )
// ## test [18] check_http_version( CheckData = "GET /abc/def/ http/1.0" / Result = NG )
// ## test [19] check_http_version( CheckData = "HTTP/0.0 100 abcdff" / Result = NG )
// ## test [20] check_http_version( CheckData = "HTTP/0.9 100 abcdff" / Result = NG )
// ## test [21] check_http_version( CheckData = "HTTP/1.2 100 abcdff" / Result = NG )
// ## test [22] check_http_version( CheckData = "HTTP/2.0 100 abcdff" / Result = NG )
// ## test [23] check_http_version( CheckData = "HTTP/1.0 404abcdff" / Result = NG )
// ## test [24] check_http_version( CheckData = "HTTP/1.1404 abcdff" / Result = NG )
// ## test [25] check_http_version( CheckData = "HTTP/1.1404abcdff" / Result = NG )
// ## test [26] check_http_version( CheckData = "http/1.0 100 abcdff" / Result = NG )
						"GET /abc/def/ HTTP/0.0",
						"GET /abc/def/ HTTP/0.9",
						"GET /abc/def/ HTTP/1.2",
						"GET /abc/def/ HTTP/2.0",
						"GET /abc/def/ghiHTTP/1.0",
						"GET/abc/def/ghi HTTP/1.1",
						"/abc/def/ghi HTTP/1.0",
						"GET /abc/def/ http/1.0",
						"HTTP/0.0 100 abcdff",
						"HTTP/0.9 100 abcdff",
						"HTTP/1.2 100 abcdff",
						"HTTP/2.0 100 abcdff",
						"HTTP/1.0 404abcdff",
						"HTTP/1.1404 abcdff",
						"HTTP/1.1404abcdff",
						"http/1.0 100 abcdff",
					};
	char	buffer_inpossible[CHECK_VERSION_INPOSSIBLE_STRING_NUM][256]
				=	{
// ## test [27] check_http_version( CheckData = "GET /abc/def/ HTTP/1.0" / Result = INPOSSIBLE )
// ## test [28] check_http_version( CheckData = "GET /abc/def/ H" / Result = INPOSSIBLE )
// ## test [29] check_http_version( CheckData = "Get /abc/def/ H" / Result = INPOSSIBLE )
// ## test [30] check_http_version( CheckData = "" / Result = INPOSSIBLE )
						"GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ H",
						"Get /abc/def/ H",
						"",
					};
	size_t	buffer_len	= 0;

	BOOST_MESSAGE( "----- check_http_version test start -----" );

	for( int i = 0; i < CHECK_VERSION_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [01] - [10]
		BOOST_CHECK( check_http_version( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_VERSION_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [11] - [26]
		BOOST_CHECK( check_http_version( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_VERSION_INPOSSIBLE_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_inpossible[i] << "]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [27] - [30]
		BOOST_CHECK( check_http_version( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [31] check_http_version( CheckData = NULL, Length = 0 / Result = NG )
	BOOST_CHECK( check_http_version( NULL, buffer_len ) == CHECK_NG );
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [32] check_http_version( CheckData = NULL, Length = 100 / Result = NG )
	BOOST_CHECK( check_http_version( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_http_version test end -----" );
}

void	check_http_version_test_t2(){

	std::string		thread_name	= "[Thread_02] ";

	int count	= 1;

	char	buffer_ok[CHECK_VERSION_OK_STRING_NUM][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ HTTP/1.1",
						"GET /abc/def/ghi HTTP/1.0",
						"GET /abc/def/ghi HTTP/1.1",
						"get /abc/def/ghi HTTP/1.0",
						"get /abc/def/ghi HTTP/1.1",
						"HTTP/1.0 100 abcdff",
						"HTTP/1.1 100 abcdff",
						"HTTP/1.0 404 abcdff",
						"HTTP/1.1 404 abcdff",
					};
	char	buffer_ng[CHECK_VERSION_NG_STRING_NUM][256]
				=	{
						"GET /abc/def/ HTTP/0.0",
						"GET /abc/def/ HTTP/0.9",
						"GET /abc/def/ HTTP/1.2",
						"GET /abc/def/ HTTP/2.0",
						"GET /abc/def/ghiHTTP/1.0",
						"GET/abc/def/ghi HTTP/1.1",
						"/abc/def/ghi HTTP/1.0",
						"GET /abc/def/ http/1.0",
						"HTTP/0.0 100 abcdff",
						"HTTP/0.9 100 abcdff",
						"HTTP/1.2 100 abcdff",
						"HTTP/2.0 100 abcdff",
						"HTTP/1.0 404abcdff",
						"HTTP/1.1404 abcdff",
						"HTTP/1.1404abcdff",
						"http/1.0 100 abcdff",
					};
	char	buffer_inpossible[CHECK_VERSION_INPOSSIBLE_STRING_NUM][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ H",
						"Get /abc/def/ H",
						"",
					};
	size_t	buffer_len	= 0;

	BOOST_MESSAGE( "----- check_http_version test start -----" );

	for( int i = 0; i < CHECK_VERSION_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_http_version( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_VERSION_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_http_version( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_VERSION_INPOSSIBLE_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_inpossible[i] << "]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_http_version( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( check_http_version( NULL, buffer_len ) == CHECK_NG );
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( check_http_version( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_http_version test end -----" );
}


void	check_status_code_test_t1(){

	std::string		thread_name	= "[Thread_01] ";

	int count	= 1;

	char	buffer_ok[CHECK_STATUS_CODE_OK_STRING_NUM][256]
				=	{
// ## test [01] check_status_code( CheckData = "HTTP/1.0 100 abcdff" / Result = OK )
// ## test [02] check_status_code( CheckData = "HTTP/1.0 199 abcdff" / Result = OK )
// ## test [03] check_status_code( CheckData = "HTTP/1.0 200 abcdff" / Result = OK )
// ## test [04] check_status_code( CheckData = "HTTP/1.0 299 abcdff" / Result = OK )
// ## test [05] check_status_code( CheckData = "HTTP/1.0 300 abcdff" / Result = OK )
// ## test [06] check_status_code( CheckData = "HTTP/1.0 399 abcdff" / Result = OK )
						"HTTP/1.0 100 abcdff",
						"HTTP/1.0 199 abcdff",
						"HTTP/1.0 200 abcdff",
						"HTTP/1.0 299 abcdff",
						"HTTP/1.0 300 abcdff",
						"HTTP/1.0 399 abcdff",
					};
	char	buffer_ng[CHECK_STATUS_CODE_NG_STRING_NUM][256]
				=	{
// ## test [07] check_status_code( CheckData = "HTTP/1.0 000 abcdff" / Result = NG )
// ## test [08] check_status_code( CheckData = "HTTP/1.0 099 abcdff" / Result = NG )
// ## test [09] check_status_code( CheckData = "HTTP/1.0 400 abcdff" / Result = NG )
// ## test [10] check_status_code( CheckData = "HTTP/1.0 999 abcdff" / Result = NG )
// ## test [11] check_status_code( CheckData = "HTTP/1.0 10 abcdff" / Result = NG )
// ## test [12] check_status_code( CheckData = "HTTP/1.0 1000 abcdff" / Result = NG )
// ## test [13] check_status_code( CheckData = "HTTP/1.0 a00 abcdff" / Result = NG )
// ## test [14] check_status_code( CheckData = "HTTP/1.0 1a0 abcdff" / Result = NG )
// ## test [15] check_status_code( CheckData = "HTTP/1.0 10a abcdff" / Result = NG )
						"HTTP/1.0 000 abcdff",
						"HTTP/1.0 099 abcdff",
						"HTTP/1.0 400 abcdff",
						"HTTP/1.0 999 abcdff",
						"HTTP/1.0 10 abcdff",
						"HTTP/1.0 1000 abcdff",
						"HTTP/1.0 a00 abcdff",
						"HTTP/1.0 1a0 abcdff",
						"HTTP/1.0 10a abcdff",
					};
	char	buffer_inpossible[CHECK_STATUS_CODE_INPOSSIBLE_STRING_NUM][256]
				=	{
// ## test [16] check_status_code( CheckData = "HTTP/1.0 100" / Result = INPOSSIBLE )
// ## test [17] check_status_code( CheckData = "" / Result = INPOSSIBLE )
						"HTTP/1.0 100",
						"",
					};
	size_t	buffer_len	= 0;

	BOOST_MESSAGE( "----- check_status_code test start -----" );

	for( int i = 0; i < CHECK_STATUS_CODE_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [01] - [06]
		BOOST_CHECK( check_status_code( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_STATUS_CODE_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [07] - [15]
		BOOST_CHECK( check_status_code( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_STATUS_CODE_INPOSSIBLE_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_inpossible[i] << "] + [CR]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [16] - [17]
		BOOST_CHECK( check_status_code( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [18] check_status_code( CheckData = NULL, Length = 0 / Result = NG )
	BOOST_CHECK( check_status_code( NULL, buffer_len ) == CHECK_NG );
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [19] check_status_code( CheckData = NULL, Length = 100 / Result = NG )
	BOOST_CHECK( check_status_code( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_status_code test end -----" );
}

void	check_status_code_test_t2(){

	std::string		thread_name	= "[Thread_02] ";

	int count	= 1;

	char	buffer_ok[CHECK_STATUS_CODE_OK_STRING_NUM][256]
				=	{
						"HTTP/1.0 100 abcdff",
						"HTTP/1.0 199 abcdff",
						"HTTP/1.0 200 abcdff",
						"HTTP/1.0 299 abcdff",
						"HTTP/1.0 300 abcdff",
						"HTTP/1.0 399 abcdff",
					};
	char	buffer_ng[CHECK_STATUS_CODE_NG_STRING_NUM][256]
				=	{
						"HTTP/1.0 000 abcdff",
						"HTTP/1.0 099 abcdff",
						"HTTP/1.0 400 abcdff",
						"HTTP/1.0 999 abcdff",
						"HTTP/1.0 10 abcdff",
						"HTTP/1.0 1000 abcdff",
						"HTTP/1.0 a00 abcdff",
						"HTTP/1.0 1a0 abcdff",
						"HTTP/1.0 10a abcdff",
					};
	char	buffer_inpossible[CHECK_STATUS_CODE_INPOSSIBLE_STRING_NUM][256]
				=	{
						"HTTP/1.0 100",
						"",
					};
	size_t	buffer_len	= 0;

	BOOST_MESSAGE( "----- check_status_code test start -----" );

	for( int i = 0; i < CHECK_STATUS_CODE_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_status_code( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_STATUS_CODE_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_status_code( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_STATUS_CODE_INPOSSIBLE_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		std::cout << thread_name << "String = [" << buffer_inpossible[i] << "] + [CR]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( check_status_code( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( check_status_code( NULL, buffer_len ) == CHECK_NG );
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	buffer_len = 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( check_status_code( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_status_code test end -----" );
}


void	find_uri_test_t1(){

	std::string		thread_name	= "[Thread_01] ";

	int count	= 1;

	char	disp_uri[256];

	char	buffer_ok[FIND_URI_OK_STRING_NUM][256]
				=	{
// ## test [01] find_uri( FindData = "GET /abc/def/ HTTP/1.0" / Result = true )
// ## test [02] find_uri( FindData = "GET /abc/def/ghi HTTP/1.0" / Result = true )
// ## test [03] find_uri( FindData = "HEAD abcdef HTTP/1.0" / Result = true )
// ## test [04] find_uri( FindData = "HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0" / Result = true )
						"GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ghi HTTP/1.0",
						"HEAD abcdef HTTP/1.0",
						"HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0",
					};

	char	buffer_ng[FIND_URI_NG_STRING_NUM][256]
				=	{
// ## test [05] find_uri( FindData = "GET/abc/def/ HTTP/1.0" / Result = false )
// ## test [06] find_uri( FindData = "GET /abc/def/HTTP/1.0" / Result = false )
// ## test [07] find_uri( FindData = "HEAD/abc/def/HTTP/1.0" / Result = false )
// ## test [08] find_uri( FindData = "" / Result = false )
						"GET/abc/def/ HTTP/1.0",
						"GET /abc/def/HTTP/1.0",
						"HEAD/abc/def/HTTP/1.0",
						"",
					};

	size_t	buffer_len	= 0;
	size_t	uri_offset	= 0;
	size_t	uri_len		= 0;

	BOOST_MESSAGE( "----- find_uri test start -----" );

	for( int i = 0; i < FIND_URI_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_uri, '\0', 256 );
		uri_offset	= 0;
		uri_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [01] - [04]
		BOOST_CHECK( find_uri( (const char*)buffer_ok[i], buffer_len, uri_offset, uri_len ) == true );
		memcpy( disp_uri, buffer_ok[i] + uri_offset, uri_len );
		std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
		std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
		std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	}

	for( int i = 0; i < FIND_URI_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_uri, '\0', 256 );
		uri_offset	= 0;
		uri_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [05] - [07]
		BOOST_CHECK( find_uri( (const char*)buffer_ng[i], buffer_len, uri_offset, uri_len ) == false );
		memcpy( disp_uri, buffer_ng[i] + uri_offset, uri_len );
		std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
		std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
		std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_uri, '\0', 256 );
	uri_offset	= 0;
	uri_len		= 0;
	buffer_len	= 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [09] find_uri( FindData = NULL, Length = 0 / Result = false )
	BOOST_CHECK( find_uri( NULL, buffer_len, uri_offset, uri_len ) == false );
	std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
	std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
	std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_uri, '\0', 256 );
	uri_offset	= 0;
	uri_len		= 0;
	buffer_len	= 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [10] find_uri( FindData = NULL, Length = 100 / Result = false )
	BOOST_CHECK( find_uri( NULL, buffer_len, uri_offset, uri_len ) == false );
	std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
	std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
	std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	count++;

	BOOST_MESSAGE( "----- find_uri test end -----" );
}

void	find_uri_test_t2(){

	std::string		thread_name	= "[Thread_02] ";

	int count	= 1;

	char	disp_uri[256];

	char	buffer_ok[FIND_URI_OK_STRING_NUM][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"GET /abc/def/ghi HTTP/1.0",
						"HEAD abcdef HTTP/1.0",
						"HEAD /abc/def/ghi/jkl/mno/pqr/stu/vwx/yz0/123/456/789/ HTTP/1.0",
					};
	size_t	size_OK[FIND_URI_OK_STRING_NUM][2];

	char	buffer_ng[FIND_URI_NG_STRING_NUM][256]
				=	{
						"GET/abc/def/ HTTP/1.0",
						"GET /abc/def/HTTP/1.0",
						"HEAD/abc/def/HTTP/1.0",
						"",
					};

	size_t	buffer_len	= 0;
	size_t	uri_offset	= 0;
	size_t	uri_len		= 0;

	BOOST_MESSAGE( "----- find_uri test start -----" );

	for( int i = 0; i < FIND_URI_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_uri, '\0', 256 );
		uri_offset	= 0;
		uri_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( find_uri( (const char*)buffer_ok[i], buffer_len, uri_offset, uri_len ) == true );
		memcpy( disp_uri, buffer_ok[i] + uri_offset, uri_len );
		std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
		std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
		std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	}

	for( int i = 0; i < FIND_URI_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_uri, '\0', 256 );
		uri_offset	= 0;
		uri_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( find_uri( (const char*)buffer_ng[i], buffer_len, uri_offset, uri_len ) == false );
		memcpy( disp_uri, buffer_ng[i] + uri_offset, uri_len );
		std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
		std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
		std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_uri, '\0', 256 );
	uri_offset	= 0;
	uri_len		= 0;
	buffer_len	= 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( find_uri( NULL, buffer_len, uri_offset, uri_len ) == false );
	std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
	std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
	std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_uri, '\0', 256 );
	uri_offset	= 0;
	uri_len		= 0;
	buffer_len	= 100;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( find_uri( NULL, buffer_len, uri_offset, uri_len ) == false );
	std::cout << thread_name << "URI Offset = [" << uri_offset << "]" << std::endl;
	std::cout << thread_name << "URI Length = [" << uri_len << "]" << std::endl;
	std::cout << thread_name << "URI String = [" << disp_uri << "]" << std::endl;
	count++;

	BOOST_MESSAGE( "----- find_uri test end -----" );
}


void	find_status_code_test_t1(){

	std::string		thread_name	= "[Thread_01] ";

	int count	= 1;

	char	disp_status_code[256];

	char	buffer_ok[FIND_STATUS_CODE_OK_STRING_NUM][256]
				=	{
// ## test [01] find_status_code( FindData = "HTTP/1.0 000 abcdff" / Result = true )
// ## test [02] find_status_code( FindData = "HTTP/1.0 999 abcdff" / Result = true )
						"HTTP/1.0 000 abcdff",
						"HTTP/1.0 999 abcdff",
					};
	size_t	size_OK[FIND_STATUS_CODE_OK_STRING_NUM][2];

	char	buffer_ng[FIND_STATUS_CODE_NG_STRING_NUM][256]
				=	{
// ## test [03] find_status_code( FindData = "HTTP/1.0 10 abcdff" / Result = false )
// ## test [04] find_status_code( FindData = "HTTP/1.0 1000 abcdff" / Result = false )
// ## test [05] find_status_code( FindData = "HTTP/1.0 aaa abcdff" / Result = false )
// ## test [06] find_status_code( FindData = "" / Result = false )
						"HTTP/1.0 10 abcdff",
						"HTTP/1.0 1000 abcdff",
						"HTTP/1.0 aaa abcdff",
						"",
					};

	size_t	buffer_len			= 0;
	size_t	status_code_offset	= 0;
	size_t	status_code_len		= 0;

	BOOST_MESSAGE( "----- find_status_code test start -----" );

	for( int i = 0; i < FIND_STATUS_CODE_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_status_code, '\0', 256 );
		status_code_offset	= 0;
		status_code_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [01] - [02]
		BOOST_CHECK( find_status_code( (const char*)buffer_ok[i], buffer_len, status_code_offset, status_code_len ) == true );
		memcpy( disp_status_code, buffer_ok[i] + status_code_offset, status_code_len );
		std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	}

	for( int i = 0; i < FIND_STATUS_CODE_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_status_code, '\0', 256 );
		status_code_offset	= 0;
		status_code_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		// [03] - [06]
		BOOST_CHECK( find_status_code( (const char*)buffer_ng[i], buffer_len, status_code_offset, status_code_len ) == false );
		memcpy( disp_status_code, buffer_ng[i] + status_code_offset, status_code_len );
		std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_status_code, '\0', 256 );
	status_code_offset	= 0;
	status_code_len		= 0;
	buffer_len			= 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [07] find_status_code( FindData = NULL, Length = 0 / Result = false )
	BOOST_CHECK( find_status_code( NULL, buffer_len, status_code_offset, status_code_len ) == false );
	std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_status_code, '\0', 256 );
	status_code_offset	= 0;
	status_code_len		= 0;
	buffer_len			= 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
// ## test [08] find_status_code( FindData = NULL, Length = 100 / Result = false )
	BOOST_CHECK( find_status_code( NULL, buffer_len, status_code_offset, status_code_len ) == false );
	std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	count++;

	BOOST_MESSAGE( "----- find_status_code test end -----" );
}

void	find_status_code_test_t2(){

	std::string		thread_name	= "[Thread_02] ";

	int count	= 1;

	char	disp_status_code[256];

	char	buffer_ok[FIND_STATUS_CODE_OK_STRING_NUM][256]
				=	{
						"HTTP/1.0 000 abcdff",
						"HTTP/1.0 999 abcdff",
					};
	size_t	size_OK[FIND_STATUS_CODE_OK_STRING_NUM][2];

	char	buffer_ng[FIND_STATUS_CODE_NG_STRING_NUM][256]
				=	{
						"HTTP/1.0 10 abcdff",
						"HTTP/1.0 1000 abcdff",
						"HTTP/1.0 aaa abcdff",
						"",
					};

	size_t	buffer_len			= 0;
	size_t	status_code_offset	= 0;
	size_t	status_code_len		= 0;

	BOOST_MESSAGE( "----- find_status_code test start -----" );

	for( int i = 0; i < FIND_STATUS_CODE_OK_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_status_code, '\0', 256 );
		status_code_offset	= 0;
		status_code_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( find_status_code( (const char*)buffer_ok[i], buffer_len, status_code_offset, status_code_len ) == true );
		memcpy( disp_status_code, buffer_ok[i] + status_code_offset, status_code_len );
		std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	}

	for( int i = 0; i < FIND_STATUS_CODE_NG_STRING_NUM; i++, count++ ){
 		std::cout << thread_name;
		std::cout.width(2);
		std::cout.fill('0');
		std::cout << count << "---------------------------------------" << std::endl;
		memset( disp_status_code, '\0', 256 );
		status_code_offset	= 0;
		status_code_len		= 0;
		std::cout << thread_name << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
		BOOST_CHECK( find_status_code( (const char*)buffer_ng[i], buffer_len, status_code_offset, status_code_len ) == false );
		memcpy( disp_status_code, buffer_ng[i] + status_code_offset, status_code_len );
		std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
		std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_status_code, '\0', 256 );
	status_code_offset	= 0;
	status_code_len		= 0;
	buffer_len			= 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( find_status_code( NULL, buffer_len, status_code_offset, status_code_len ) == false );
	std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	memset( disp_status_code, '\0', 256 );
	status_code_offset	= 0;
	status_code_len		= 0;
	buffer_len			= 0;
	std::cout << thread_name << "String = [NULL]" << std::endl;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	BOOST_CHECK( find_status_code( NULL, buffer_len, status_code_offset, status_code_len ) == false );
	std::cout << thread_name << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
	std::cout << thread_name << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	count++;

	BOOST_MESSAGE( "----- find_status_code test end -----" );
}

void	find_http_header_test_t1(){

	std::string		thread_name	= "[Thread_01] ";

	int		count	= 1;
	int		i		= 0;
	int		j		= 0;

	char	disp_http_header[4096];

	std::string	http_header_name[10]
				=	{
						"Cookie",
						"X-Forwarded-For",
						"Cookie2",
						"X-Forwarded-For2",
						"",
						"cookie",
						"COOKIE",
						"content-length",
						"Content-Length",
						"CONTENT-LENGTH",
					};

	bool	result_1[10]
				=	{
// ## test [01] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie", ReturnCode = "\r" / Result = true )
// ## test [02] find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For", ReturnCode = "\r" / Result = true )
// ## test [03] find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r" / Result = false )
// ## test [04] find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r" / Result = false )
// ## test [05] find_http_header( FindData = "...BlankLine...", HeaderName = "", ReturnCode = "\r" / Result = true )
// ## test [06] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "cookie", ReturnCode = "\r" / Result = true )
// ## test [07] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "COOKIE", ReturnCode = "\r" / Result = true )
// ## test [08] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "content-length", ReturnCode = "\r" / Result = true )
// ## test [09] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "Content-Length", ReturnCode = "\r" / Result = true )
// ## test [10] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r" / Result = true )

// ## test [11] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie", ReturnCode = "\n" / Result = true )
// ## test [12] find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For", ReturnCode = "\n" / Result = true )
// ## test [13] find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\n" / Result = false )
// ## test [14] find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\n" / Result = false )
// ## test [15] find_http_header( FindData = "...BlankLine...", HeaderName = "", ReturnCode = "\n" / Result = true )
// ## test [16] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "cookie", ReturnCode = "\n" / Result = true )
// ## test [17] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "COOKIE", ReturnCode = "\n" / Result = true )
// ## test [18] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "content-length", ReturnCode = "\n" / Result = true )
// ## test [19] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "Content-Length", ReturnCode = "\n" / Result = true )
// ## test [20] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\n" / Result = true )

// ## test [21] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "Cookie", ReturnCode = "\r\n" / Result = true )
// ## test [22] find_http_header( FindData = "...X-Forwarded-For: 20.20.20.20...BlankLine...", HeaderName = "X-Forwarded-For", ReturnCode = "\r\n" / Result = true )
// ## test [23] find_http_header( FindData = "...BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r\n" / Result = false )
// ## test [24] find_http_header( FindData = "...BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r\n" / Result = false )
// ## test [25] find_http_header( FindData = "...BlankLine...", HeaderName = "", ReturnCode = "\r\n" / Result = true )
// ## test [26] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "cookie", ReturnCode = "\r\n" / Result = true )
// ## test [27] find_http_header( FindData = "...Cookie: 10.10.10.10:11111...BlankLine...", HeaderName = "COOKIE", ReturnCode = "\r\n" / Result = true )
// ## test [28] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "content-length", ReturnCode = "\r\n" / Result = true )
// ## test [29] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "Content-Length", ReturnCode = "\r\n" / Result = true )
// ## test [30] find_http_header( FindData = "...CONTENT-LENGTH: 1000...BlankLine...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r\n" / Result = true )
						true,
						true,
						false,
						false,
						true,
						true,
						true,
						true,
						true,
						true,
					};

	bool	result_2[10]
				=	{
// ## test [31] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "Cookie", ReturnCode = "\r" / Result = false )
// ## test [32] find_http_header( FindData = "BlankLine...X-Forwarded-For: 20.20.20.20...", HeaderName = "X-Forwarded-For", ReturnCode = "\r" / Result = false )
// ## test [33] find_http_header( FindData = "BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r" / Result = false )
// ## test [34] find_http_header( FindData = "BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r" / Result = false )
// ## test [35] find_http_header( FindData = "BlankLine...", HeaderName = "", ReturnCode = "\r" / Result = true )
// ## test [36] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "cookie", ReturnCode = "\r" / Result = false )
// ## test [37] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "COOKIE", ReturnCode = "\r" / Result = false )
// ## test [38] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "content-length", ReturnCode = "\r" / Result = false )
// ## test [39] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "Content-Length", ReturnCode = "\r" / Result = false )
// ## test [40] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r" / Result = false )

// ## test [41] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "Cookie", ReturnCode = "\n" / Result = false )
// ## test [42] find_http_header( FindData = "BlankLine...X-Forwarded-For: 20.20.20.20...", HeaderName = "X-Forwarded-For", ReturnCode = "\n" / Result = false )
// ## test [43] find_http_header( FindData = "BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\n" / Result = false )
// ## test [44] find_http_header( FindData = "BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\n" / Result = false )
// ## test [45] find_http_header( FindData = "BlankLine...", HeaderName = "", ReturnCode = "\n" / Result = true )
// ## test [46] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "cookie", ReturnCode = "\n" / Result = false )
// ## test [47] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "COOKIE", ReturnCode = "\n" / Result = false )
// ## test [48] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "content-length", ReturnCode = "\n" / Result = false )
// ## test [49] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "Content-Length", ReturnCode = "\n" / Result = false )
// ## test [50] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\n" / Result = false )

// ## test [51] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "Cookie", ReturnCode = "\r\n" / Result = false )
// ## test [52] find_http_header( FindData = "BlankLine...X-Forwarded-For: 20.20.20.20...", HeaderName = "X-Forwarded-For", ReturnCode = "\r\n" / Result = false )
// ## test [53] find_http_header( FindData = "BlankLine...Cookie2: 30.30.30.30:33333...", HeaderName = "Cookie2", ReturnCode = "\r\n" / Result = false )
// ## test [54] find_http_header( FindData = "BlankLine...X-Forwarded-For2: 40.40.40.40...", HeaderName = "X-Forwarded-For2", ReturnCode = "\r\n" / Result = false )
// ## test [55] find_http_header( FindData = "BlankLine...", HeaderName = "", ReturnCode = "\r\n" / Result = true )
// ## test [56] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "cookie", ReturnCode = "\r\n" / Result = false )
// ## test [57] find_http_header( FindData = "BlankLine...Cookie: 10.10.10.10:11111...", HeaderName = "COOKIE", ReturnCode = "\r\n" / Result = false )
// ## test [58] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "content-length", ReturnCode = "\r\n" / Result = false )
// ## test [59] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "Content-Length", ReturnCode = "\r\n" / Result = false )
// ## test [60] find_http_header( FindData = "BlankLine...CONTENT-LENGTH: 1000...", HeaderName = "CONTENT-LENGTH", ReturnCode = "\r\n" / Result = false )
						false,
						false,
						false,
						false,
						true,
						false,
						false,
						false,
						false,
						false,
					};

	char	buffer_all_1[3][4096];
	char	buffer_all_2[3][4096];

	char	buffer_line_1[8][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"Cookie: 10.10.10.10:11111",
						"X-Forwarded-For: 20.20.20.20",
						"CONTENT-LENGTH: 1000",
						"",
						"GET /abc/def/ HTTP/1.0",
						"Cookie2: 30.30.30.30:33333",
						"X-Forwarded-For2: 40.40.40.40",
					};


	char	buffer_line_2[8][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"",
						"Cookie: 10.10.10.10:11111",
						"X-Forwarded-For: 20.20.20.20",
						"CONTENT-LENGTH: 1000",
						"GET /abc/def/ HTTP/1.0",
						"Cookie2: 30.30.30.30:33333",
						"X-Forwarded-For2: 40.40.40.40",
					};

	size_t	buffer_len			= 0;
	size_t	http_header_offset	= 0;
	size_t	http_header_len		= 0;

	BOOST_MESSAGE( "----- find_http_header test start -----" );

	memset( buffer_all_1, '\0', sizeof(buffer_all_1));
	memset( buffer_all_2, '\0', sizeof(buffer_all_2));
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 3; j++ ){
			memcpy( buffer_all_1[j] + strlen( buffer_all_1[j] ), buffer_line_1[i], strlen( buffer_line_1[i] ));
			if( j == 0 || j == 2 ){
				buffer_all_1[j][strlen( buffer_all_1[j] )] = '\r';
			}
			if( j == 1 || j == 2 ){
				buffer_all_1[j][strlen( buffer_all_1[j] )] = '\n';
			}
			
		}
	}

	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 3; j++ ){
			memcpy( buffer_all_2[j] + strlen( buffer_all_2[j] ), buffer_line_2[i], strlen( buffer_line_1[i] ));
			if( j == 0 || j == 2 ){
				buffer_all_2[j][strlen( buffer_all_2[j] )] = '\r';
			}
			if( j == 1 || j == 2 ){
				buffer_all_2[j][strlen( buffer_all_2[j] )] = '\n';
			}
			
		}
	}


	for( i = 0; i < 3; i++ ){
		buffer_len = strlen( buffer_all_1[i] );
		for( j = 0; j < 10; j++, count++ ){
			std::cout << thread_name;
			std::cout.width(2);
			std::cout.fill('0');
			std::cout << count << "---------------------------------------" << std::endl;
			http_header_offset	= 0;
			http_header_len		= 0;
			std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
			std::cout << thread_name << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
			std::cout << thread_name << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
			// [01] - [30]
			BOOST_CHECK( find_http_header( (const char*)buffer_all_1[i], buffer_len, http_header_name[j], http_header_offset, http_header_len ) == result_1[j] );
			memset( disp_http_header, '\0', sizeof(disp_http_header));
			memcpy( disp_http_header, buffer_all_1[i] + http_header_offset, http_header_len );
			std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
			std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
			std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
		}
	}

	for( i = 0; i < 3; i++ ){
		buffer_len = strlen( buffer_all_2[i] );
		for( j = 0; j < 10; j++, count++ ){
			std::cout << thread_name;
			std::cout.width(2);
			std::cout.fill('0');
			std::cout << count << "---------------------------------------" << std::endl;
			http_header_offset	= 0;
			http_header_len		= 0;
			std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
			std::cout << thread_name << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
			std::cout << thread_name << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
			// [31] - [60]
			BOOST_CHECK( find_http_header( (const char*)buffer_all_2[i], buffer_len, http_header_name[j], http_header_offset, http_header_len ) == result_2[j] );
			memset( disp_http_header, '\0', sizeof(disp_http_header));
			memcpy( disp_http_header, buffer_all_2[i] + http_header_offset, http_header_len );
			std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
			std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
			std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
		}
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	http_header_offset	= 0;
	http_header_len		= 0;
	buffer_len = 0;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	std::cout << thread_name << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
	std::cout << thread_name << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
// ## test [61] find_http_header( FindData = NULL, HeaderName = "Cookie", Length = 0 / Result = false )
	BOOST_CHECK( find_http_header( NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len ) == false );
	memset( disp_http_header, '\0', sizeof(disp_http_header));
	std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
	std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
	std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	http_header_offset	= 0;
	http_header_len		= 0;
	buffer_len = 100;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	std::cout << thread_name << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
	std::cout << thread_name << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
// ## test [62] find_http_header( FindData = NULL, HeaderName = "Cookie", Length = 100 / Result = false )
	BOOST_CHECK( find_http_header( NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len ) == false );
	memset( disp_http_header, '\0', sizeof(disp_http_header));
	std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
	std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
	std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
	count++;

	BOOST_MESSAGE( "----- find_http_header test end -----" );
}

void	find_http_header_test_t2(){

	std::string		thread_name	= "[Thread_02] ";

	int		count	= 1;
	int		i		= 0;
	int		j		= 0;

	char	disp_http_header[4096];

	std::string	http_header_name[10]
				=	{
						"Cookie",
						"X-Forwarded-For",
						"Cookie2",
						"X-Forwarded-For2",
						"",
						"cookie",
						"COOKIE",
						"content-length",
						"Content-Length",
						"CONTENT-LENGTH",
					};

	bool	result_1[10]
				=	{
						true,
						true,
						false,
						false,
						true,
						true,
						true,
						true,
						true,
						true,
					};

	bool	result_2[10]
				=	{
						false,
						false,
						false,
						false,
						true,
						false,
						false,
						false,
						false,
						false,
					};

	char	buffer_all_1[3][4096];
	char	buffer_all_2[3][4096];

	char	buffer_line_1[8][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"Cookie: 10.10.10.10:11111",
						"X-Forwarded-For: 20.20.20.20",
						"CONTENT-LENGTH: 1000",
						"",
						"GET /abc/def/ HTTP/1.0",
						"Cookie2: 30.30.30.30:33333",
						"X-Forwarded-For2: 40.40.40.40",
					};


	char	buffer_line_2[8][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"",
						"Cookie: 10.10.10.10:11111",
						"X-Forwarded-For: 20.20.20.20",
						"CONTENT-LENGTH: 1000",
						"GET /abc/def/ HTTP/1.0",
						"Cookie2: 30.30.30.30:33333",
						"X-Forwarded-For2: 40.40.40.40",
					};

	size_t	buffer_len			= 0;
	size_t	http_header_offset	= 0;
	size_t	http_header_len		= 0;

	BOOST_MESSAGE( "----- find_http_header test start -----" );

	memset( buffer_all_1, '\0', sizeof(buffer_all_1));
	memset( buffer_all_2, '\0', sizeof(buffer_all_2));
	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 3; j++ ){
			memcpy( buffer_all_1[j] + strlen( buffer_all_1[j] ), buffer_line_1[i], strlen( buffer_line_1[i] ));
			if( j == 0 || j == 2 ){
				buffer_all_1[j][strlen( buffer_all_1[j] )] = '\r';
			}
			if( j == 1 || j == 2 ){
				buffer_all_1[j][strlen( buffer_all_1[j] )] = '\n';
			}
			
		}
	}

	for( i = 0; i < 8; i++ ){
		for( j = 0; j < 3; j++ ){
			memcpy( buffer_all_2[j] + strlen( buffer_all_2[j] ), buffer_line_2[i], strlen( buffer_line_1[i] ));
			if( j == 0 || j == 2 ){
				buffer_all_2[j][strlen( buffer_all_2[j] )] = '\r';
			}
			if( j == 1 || j == 2 ){
				buffer_all_2[j][strlen( buffer_all_2[j] )] = '\n';
			}
			
		}
	}


	for( i = 0; i < 3; i++ ){
		buffer_len = strlen( buffer_all_1[i] );
		for( j = 0; j < 10; j++, count++ ){
			std::cout << thread_name;
			std::cout.width(2);
			std::cout.fill('0');
			std::cout << count << "---------------------------------------" << std::endl;
			http_header_offset	= 0;
			http_header_len		= 0;
			std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
			std::cout << thread_name << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
			std::cout << thread_name << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
			BOOST_CHECK( find_http_header( (const char*)buffer_all_1[i], buffer_len, http_header_name[j], http_header_offset, http_header_len ) == result_1[j] );
			memset( disp_http_header, '\0', sizeof(disp_http_header));
			memcpy( disp_http_header, buffer_all_1[i] + http_header_offset, http_header_len );
			std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
			std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
			std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
		}
	}

	for( i = 0; i < 3; i++ ){
		buffer_len = strlen( buffer_all_2[i] );
		for( j = 0; j < 10; j++, count++ ){
			std::cout << thread_name;
			std::cout.width(2);
			std::cout.fill('0');
			std::cout << count << "---------------------------------------" << std::endl;
			http_header_offset	= 0;
			http_header_len		= 0;
			std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
			std::cout << thread_name << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
			std::cout << thread_name << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
			BOOST_CHECK( find_http_header( (const char*)buffer_all_2[i], buffer_len, http_header_name[j], http_header_offset, http_header_len ) == result_2[j] );
			memset( disp_http_header, '\0', sizeof(disp_http_header));
			memcpy( disp_http_header, buffer_all_2[i] + http_header_offset, http_header_len );
			std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
			std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
			std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
		}
	}

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	http_header_offset	= 0;
	http_header_len		= 0;
	buffer_len = 0;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	std::cout << thread_name << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
	std::cout << thread_name << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
	BOOST_CHECK( find_http_header( NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len ) == false );
	memset( disp_http_header, '\0', sizeof(disp_http_header));
	std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
	std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
	std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
	count++;

	std::cout << thread_name;
	std::cout.width(2);
	std::cout.fill('0');
	std::cout << count << "---------------------------------------" << std::endl;
	http_header_offset	= 0;
	http_header_len		= 0;
	buffer_len = 100;
	std::cout << thread_name << "Length = [" << buffer_len << "]" << std::endl;
	std::cout << thread_name << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
	std::cout << thread_name << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
	BOOST_CHECK( find_http_header( NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len ) == false );
	memset( disp_http_header, '\0', sizeof(disp_http_header));
	std::cout << thread_name << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
	std::cout << thread_name << "Http Header Length = [" << http_header_len << "]" << std::endl;
	std::cout << thread_name << "Http Header String = [" << disp_http_header << "]" << std::endl;
	count++;

	BOOST_MESSAGE( "----- find_http_header test end -----" );
}

};

//--test functions--
//-------------------------------------------------------------------
void	check_http_method_test_t1( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.check_http_method_test_t1();
	}

}
void	check_http_method_test_t2( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.check_http_method_test_t2();
	}

}
void	check_http_method_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	boost::thread	t1(	boost::bind(	&check_http_method_test_t1,
										http_protocol_module_base_test_1));
	boost::thread	t2(	boost::bind(	&check_http_method_test_t2,
										http_protocol_module_base_test_1));

	t1.join();
	t2.join();

}
//-------------------------------------------------------------------
void	check_http_version_test_t1( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.check_http_version_test_t1();
	}

}
void	check_http_version_test_t2( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.check_http_version_test_t2();
	}

}
void	check_http_version_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	boost::thread	t1(	boost::bind(	&check_http_version_test_t1,
										http_protocol_module_base_test_1));
	boost::thread	t2(	boost::bind(	&check_http_version_test_t2,
										http_protocol_module_base_test_1));

	t1.join();
	t2.join();

}
//-------------------------------------------------------------------
void	check_status_code_test_t1( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.check_status_code_test_t1();
	}

}
void	check_status_code_test_t2( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.check_status_code_test_t2();
	}

}
void	check_status_code_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	boost::thread	t1(	boost::bind(	&check_status_code_test_t1,
										http_protocol_module_base_test_1));
	boost::thread	t2(	boost::bind(	&check_status_code_test_t2,
										http_protocol_module_base_test_1));

	t1.join();
	t2.join();

}
//-------------------------------------------------------------------
void	find_uri_test_t1( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.find_uri_test_t1();
	}

}
void	find_uri_test_t2( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.find_uri_test_t2();
	}

}
void	find_uri_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	boost::thread	t1(	boost::bind(	&find_uri_test_t1,
										http_protocol_module_base_test_1));
	boost::thread	t2(	boost::bind(	&find_uri_test_t2,
										http_protocol_module_base_test_1));

	t1.join();
	t2.join();

}
//-------------------------------------------------------------------
void	find_status_code_test_t1( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.find_status_code_test_t1();
	}

}
void	find_status_code_test_t2( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.find_status_code_test_t2();
	}

}
void	find_status_code_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	boost::thread	t1(	boost::bind(	&find_status_code_test_t1,
										http_protocol_module_base_test_1));
	boost::thread	t2(	boost::bind(	&find_status_code_test_t2,
										http_protocol_module_base_test_1));

	t1.join();
	t2.join();

}
//-------------------------------------------------------------------
void	find_http_header_test_t1( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.find_http_header_test_t1();
	}

}
void	find_http_header_test_t2( http_protocol_module_base_test http_protocol_module_base_test_1 ){

	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
		http_protocol_module_base_test_1.find_http_header_test_t2();
	}

}
void	find_http_header_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	boost::thread	t1(	boost::bind(	&find_http_header_test_t1,
										http_protocol_module_base_test_1));
	boost::thread	t2(	boost::bind(	&find_http_header_test_t2,
										http_protocol_module_base_test_1));

	t1.join();
	t2.join();

}
//-------------------------------------------------------------------
// void	multi_thread_test_t1(){
// 
// 	for( int i = 0; i < THREAD_01_LOOP_NUM; i++ ){
// 		BOOST_CHECK( true );
// 	}
// 
// }
// void	multi_thread_test_t2(){
// 
// 	for( int i = 0; i < THREAD_02_LOOP_NUM; i++ ){
// 		BOOST_CHECK( true );
// 	}
// 
// }
// void	multi_thread_test(){
// 
// 	boost::thread	t1(	&multi_thread_test_t1);
// 	boost::thread	t2(	&multi_thread_test_t2);
// 
// 	t1.join();
// 	t2.join();
// 
// }
//-------------------------------------------------------------------

//-------------------------------------------------------------------
test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "http_protocol_module_base class test" );

	ts->add( BOOST_TEST_CASE( &check_http_method_test ) );
	ts->add( BOOST_TEST_CASE( &check_http_version_test ) );
	ts->add( BOOST_TEST_CASE( &check_status_code_test ) );
	ts->add( BOOST_TEST_CASE( &find_uri_test ) );
	ts->add( BOOST_TEST_CASE( &find_status_code_test ) );
	ts->add( BOOST_TEST_CASE( &find_http_header_test ) );
	
// 	ts->add( BOOST_TEST_CASE( &multi_thread_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
