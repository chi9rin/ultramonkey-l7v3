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
#define	FIND_URI_NG_STRING_NUM	(3)

#define	FIND_STATUS_CODE_OK_STRING_NUM	(2)
#define	FIND_STATUS_CODE_NG_STRING_NUM	(2)

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
							rs_list_itr_func_type	inlist_next,
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

void	check_http_method_test(){

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

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	for( int i = 0; i < CHECK_METHOD_OK_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [1]  http method check test (check result = OK)
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_METHOD_NG_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [2]  http method check test (check result = NG)
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_METHOD_INPOSSIBLE_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_inpossible[i] << "]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [3]  http method check test (check result = INPOSSIBLE)
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	buffer_len = 0;
	std::cout << "String = [NULL]" << std::endl;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	// ## test [4]  http method check test (check result = NG / buffer = NULL / buffer_len = 0)
	BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( NULL, buffer_len ) == CHECK_NG );
	count++;

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	buffer_len = 100;
	std::cout << "String = [NULL]" << std::endl;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	// ## test [5]  http method check test (check result = NG / buffer = NULL / buffer_len = 100)
	BOOST_CHECK( http_protocol_module_base_test_1.check_http_method( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_http_method test end -----" );
}

void	check_http_version_test(){

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

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	for( int i = 0; i < CHECK_VERSION_OK_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [1]  http version check test (check result = OK)
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_version( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_VERSION_NG_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [2]  http version check test (check result = NG)
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_version( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_VERSION_INPOSSIBLE_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_inpossible[i] << "]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [3]  http version check test (check result = INPOSSIBLE)
		BOOST_CHECK( http_protocol_module_base_test_1.check_http_version( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	buffer_len = 0;
	std::cout << "String = [NULL]" << std::endl;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	// ## test [4]  http version check test (check result = NG / buffer = NULL / buffer_len = 0)
	BOOST_CHECK( http_protocol_module_base_test_1.check_http_version( NULL, buffer_len ) == CHECK_NG );
	count++;

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	buffer_len = 100;
	std::cout << "String = [NULL]" << std::endl;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	// ## test [5]  http version check test (check result = NG / buffer = NULL / buffer_len = 100)
	BOOST_CHECK( http_protocol_module_base_test_1.check_http_version( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_http_version test end -----" );
}

void	check_status_code_test(){

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

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	for( int i = 0; i < CHECK_STATUS_CODE_OK_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [1]  status code check test (check result = OK)
		BOOST_CHECK( http_protocol_module_base_test_1.check_status_code( (const char*)buffer_ok[i], buffer_len ) == CHECK_OK );
	}

	for( int i = 0; i < CHECK_STATUS_CODE_NG_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [2]  status code check test (check result = NG)
		BOOST_CHECK( http_protocol_module_base_test_1.check_status_code( (const char*)buffer_ng[i], buffer_len ) == CHECK_NG );
	}

	for( int i = 0; i < CHECK_STATUS_CODE_INPOSSIBLE_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		std::cout << "String = [" << buffer_inpossible[i] << "] + [CR]" << std::endl;
		buffer_len = strlen( buffer_inpossible[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [3]  status code check test (check result = INPOSSIBLE)
		BOOST_CHECK( http_protocol_module_base_test_1.check_status_code( (const char*)buffer_inpossible[i], buffer_len ) == CHECK_INPOSSIBLE );
	}

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	buffer_len = 0;
	std::cout << "String = [NULL]" << std::endl;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	// ## test [4]  status code check test (check result = NG / buffer = NULL / buffer_len = 0)
	BOOST_CHECK( http_protocol_module_base_test_1.check_status_code( NULL, buffer_len ) == CHECK_NG );
	count++;

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	buffer_len = 100;
	std::cout << "String = [NULL]" << std::endl;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	// ## test [5]  status code check test (check result = NG / buffer = NULL / buffer_len = 100)
	BOOST_CHECK( http_protocol_module_base_test_1.check_status_code( NULL, buffer_len ) == CHECK_NG );
	count++;

	BOOST_MESSAGE( "----- check_status_code test end -----" );
}

void	find_uri_test(){

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
					};

	size_t	buffer_len	= 0;
	size_t	uri_offset	= 0;
	size_t	uri_len		= 0;

	BOOST_MESSAGE( "----- find_uri test start -----" );

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	for( int i = 0; i < FIND_URI_OK_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		memset( disp_uri, '\0', 256 );
		uri_offset	= 0;
		uri_len		= 0;
		std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [1]  uri find test (find result = TRUE)
		BOOST_CHECK( http_protocol_module_base_test_1.find_uri( (const char*)buffer_ok[i], buffer_len, uri_offset, uri_len ) == true );
		memcpy( disp_uri, buffer_ok[i] + uri_offset, uri_len );
		std::cout << "URI Offset = [" << uri_offset << "]" << std::endl;
		std::cout << "URI Length = [" << uri_len << "]" << std::endl;
		std::cout << "URI String = [" << disp_uri << "]" << std::endl;
	}

	for( int i = 0; i < FIND_URI_NG_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		memset( disp_uri, '\0', 256 );
		uri_offset	= 0;
		uri_len		= 0;
		std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [2]  uri find test (find result = FALSE)
		BOOST_CHECK( http_protocol_module_base_test_1.find_uri( (const char*)buffer_ng[i], buffer_len, uri_offset, uri_len ) == false );
		memcpy( disp_uri, buffer_ng[i] + uri_offset, uri_len );
		std::cout << "URI Offset = [" << uri_offset << "]" << std::endl;
		std::cout << "URI Length = [" << uri_len << "]" << std::endl;
		std::cout << "URI String = [" << disp_uri << "]" << std::endl;
	}

	BOOST_MESSAGE( "----- find_uri test end -----" );
}

void	find_status_code_test(){

	int count	= 1;

	char	disp_status_code[256];

	char	buffer_ok[FIND_STATUS_CODE_OK_STRING_NUM][256]
				=	{
						"HTTP/1.0 000 abcdff",
						"HTTP/1.0 099 abcdff",
					};
	size_t	size_OK[FIND_STATUS_CODE_OK_STRING_NUM][2];

	char	buffer_ng[FIND_STATUS_CODE_NG_STRING_NUM][256]
				=	{
						"HTTP/1.0000 abcdff",
						"HTTP/1.0 099abcdff",
					};

	size_t	buffer_len			= 0;
	size_t	status_code_offset	= 0;
	size_t	status_code_len		= 0;

	BOOST_MESSAGE( "----- find_status_code test start -----" );

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	for( int i = 0; i < FIND_STATUS_CODE_OK_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		memset( disp_status_code, '\0', 256 );
		status_code_offset	= 0;
		status_code_len		= 0;
		std::cout << "String = [" << buffer_ok[i] << "] + [CR]" << std::endl;
		buffer_ok[i][strlen( buffer_ok[i] )] = '\r';
		buffer_len = strlen( buffer_ok[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [1]  status code find test (find result = TRUE)
		BOOST_CHECK( http_protocol_module_base_test_1.find_status_code( (const char*)buffer_ok[i], buffer_len, status_code_offset, status_code_len ) == true );
		memcpy( disp_status_code, buffer_ok[i] + status_code_offset, status_code_len );
		std::cout << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
		std::cout << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
		std::cout << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	}

	for( int i = 0; i < FIND_STATUS_CODE_NG_STRING_NUM; i++, count++ ){
		if( count < 10 ){
 			std::cout << count << "---------------------------------------" << std::endl;
		}
		else{
			std::cout << count << "--------------------------------------" << std::endl;
		}
		memset( disp_status_code, '\0', 256 );
		status_code_offset	= 0;
		status_code_len		= 0;
		std::cout << "String = [" << buffer_ng[i] << "] + [CR]" << std::endl;
		buffer_ng[i][strlen( buffer_ng[i] )] = '\r';
		buffer_len = strlen( buffer_ng[i] );
		std::cout << "Length = [" << buffer_len << "]" << std::endl;
		// ## test [2]  status code find test (find result = FALSE)
		BOOST_CHECK( http_protocol_module_base_test_1.find_status_code( (const char*)buffer_ng[i], buffer_len, status_code_offset, status_code_len ) == false );
		memcpy( disp_status_code, buffer_ng[i] + status_code_offset, status_code_len );
		std::cout << "STATUS CODE Offset = [" << status_code_offset << "]" << std::endl;
		std::cout << "STATUS CODE Length = [" << status_code_len << "]" << std::endl;
		std::cout << "STATUS CODE String = [" << disp_status_code << "]" << std::endl;
	}

	BOOST_MESSAGE( "----- find_status_code test end -----" );
}

void	find_http_header_test(){

	int		count	= 1;
	int		i		= 0;
	int		j		= 0;

	char	disp_http_header[4096];

	std::string	http_header_name[5]
				=	{
						"Cookie",
						"X-Forwarded-For",
						"Cookie2",
						"X-Forwarded-For2",
						"",
					};

	bool	result_1[5]
				=	{
						true,
						true,
						false,
						false,
						true,
					};

	bool	result_2[5]
				=	{
						false,
						false,
						false,
						false,
						true,
					};

	char	buffer_all_1[3][4096];
	char	buffer_all_2[3][4096];

	char	buffer_line_1[7][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"Cookie: 10.10.10.10:11111",
						"X-Forwarded-For: 20.20.20.20",
						"",
						"GET /abc/def/ HTTP/1.0",
						"Cookie2: 30.30.30.30:33333",
						"X-Forwarded-For2: 40.40.40.40",
					};


	char	buffer_line_2[7][256]
				=	{
						"GET /abc/def/ HTTP/1.0",
						"",
						"Cookie: 10.10.10.10:11111",
						"X-Forwarded-For: 20.20.20.20",
						"GET /abc/def/ HTTP/1.0",
						"Cookie2: 30.30.30.30:33333",
						"X-Forwarded-For2: 40.40.40.40",
					};

	size_t	buffer_len			= 0;
	size_t	http_header_offset	= 0;
	size_t	http_header_len		= 0;

	BOOST_MESSAGE( "----- find_http_header test start -----" );

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );

	memset( buffer_all_1, '\0', sizeof(buffer_all_1));
	memset( buffer_all_2, '\0', sizeof(buffer_all_2));
	for( i = 0; i < 7; i++ ){
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

	for( i = 0; i < 7; i++ ){
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
		for( j = 0; j < 5; j++, count++ ){
			if( count < 10 ){
				std::cout << count << "---------------------------------------" << std::endl;
			}
			else{
				std::cout << count << "--------------------------------------" << std::endl;
			}
			http_header_offset	= 0;
			http_header_len		= 0;
			std::cout << "Length = [" << buffer_len << "]" << std::endl;
			std::cout << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
			std::cout << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
			// ## test [1]  http header find test (http header valid)
			BOOST_CHECK( http_protocol_module_base_test_1.find_http_header( (const char*)buffer_all_1[i], buffer_len, http_header_name[j], http_header_offset, http_header_len ) == result_1[j] );
			memset( disp_http_header, '\0', sizeof(disp_http_header));
			memcpy( disp_http_header, buffer_all_1[i] + http_header_offset, http_header_len );
			std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
			std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
			std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
		}
	}

	for( i = 0; i < 3; i++ ){
		buffer_len = strlen( buffer_all_2[i] );
		for( j = 0; j < 5; j++, count++ ){
			if( count < 10 ){
				std::cout << count << "---------------------------------------" << std::endl;
			}
			else{
				std::cout << count << "--------------------------------------" << std::endl;
			}
			http_header_offset	= 0;
			http_header_len		= 0;
			std::cout << "Length = [" << buffer_len << "]" << std::endl;
			std::cout << "Http Header Name = [" << http_header_name[j] << "]" << std::endl;
			std::cout << "Http Header Name Length = [" << http_header_name[j].length() << "]" << std::endl;
			// ## test [2]  http header find test (http header invalid)
			BOOST_CHECK( http_protocol_module_base_test_1.find_http_header( (const char*)buffer_all_2[i], buffer_len, http_header_name[j], http_header_offset, http_header_len ) == result_2[j] );
			memset( disp_http_header, '\0', sizeof(disp_http_header));
			memcpy( disp_http_header, buffer_all_2[i] + http_header_offset, http_header_len );
			std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
			std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
			std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;
		}
	}

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	http_header_offset	= 0;
	http_header_len		= 0;
	buffer_len = 0;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	std::cout << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
	std::cout << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
	// ## test [3]  http header find test (buffer = NULL / buffer_len = 0)
	BOOST_CHECK( http_protocol_module_base_test_1.find_http_header( NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len ) == false );
	memset( disp_http_header, '\0', sizeof(disp_http_header));
	std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
	std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
	std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;

	if( count < 10 ){
		std::cout << count << "---------------------------------------" << std::endl;
	}
	else{
		std::cout << count << "--------------------------------------" << std::endl;
	}
	http_header_offset	= 0;
	http_header_len		= 0;
	buffer_len = 100;
	std::cout << "Length = [" << buffer_len << "]" << std::endl;
	std::cout << "Http Header Name = [" << http_header_name[0] << "]" << std::endl;
	std::cout << "Http Header Name Length = [" << http_header_name[0].length() << "]" << std::endl;
	// ## test [4]  http header find test (buffer = NULL / buffer_len = 100)
	BOOST_CHECK( http_protocol_module_base_test_1.find_http_header( NULL, buffer_len, http_header_name[0], http_header_offset, http_header_len ) == false );
	memset( disp_http_header, '\0', sizeof(disp_http_header));
	std::cout << "Http Header Offset = [" << http_header_offset << "]" << std::endl;
	std::cout << "Http Header Length = [" << http_header_len << "]" << std::endl;
	std::cout << "Http Header String = [" << disp_http_header << "]" << std::endl;

	BOOST_MESSAGE( "----- find_http_header test end -----" );
}

};

//--test functions--
void	check_http_method_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.check_http_method_test();

}

void	check_http_version_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.check_http_version_test();

}

void	check_status_code_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.check_status_code_test();

}

void	find_uri_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.find_uri_test();

}

void	find_status_code_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.find_status_code_test();

}

void	find_http_header_test(){

	http_protocol_module_base_test	http_protocol_module_base_test_1( "cinsert" );
	http_protocol_module_base_test_1.find_http_header_test();

}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "http_protocol_module_base class test" );

	ts->add( BOOST_TEST_CASE( &check_http_method_test ) );
	ts->add( BOOST_TEST_CASE( &check_http_version_test ) );
	ts->add( BOOST_TEST_CASE( &check_status_code_test ) );
	ts->add( BOOST_TEST_CASE( &find_uri_test ) );
	ts->add( BOOST_TEST_CASE( &find_status_code_test ) );
	ts->add( BOOST_TEST_CASE( &find_http_header_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
