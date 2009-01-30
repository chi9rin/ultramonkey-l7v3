//
//	@file	http_protocol_module_base.cpp
//	@brief	shared object http protocol module absctract class
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include <iostream>
#include <vector>
#include <boost/xpressive/xpressive.hpp>

#include "http_protocol_module_base.h"

using namespace boost::xpressive;

l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_http_method( const char* buffer, const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
	char*	check_string;
	size_t	line_length		= 0;
	cregex	method_regex
		= (	as_xpr("GET")		| as_xpr("HEAD")		| as_xpr("POST")		|
			as_xpr("PUT")		| as_xpr("PROPFIND")	| as_xpr("PROPPATCH")	|
			as_xpr("OPTIONS")	| as_xpr("CONNECT")		| as_xpr("COPY")		|
			as_xpr("TRACE")		| as_xpr("DELETE")		| as_xpr("LOCK")		|
			as_xpr("UNLOCK")	| as_xpr("MOVE")		| as_xpr("MKCOL")) >> _s >>
			+~_s >> _s >>
			"HTTP/" >> _d >> "." >> _d;

	if( buffer != NULL ){
		for( line_length = 0; line_length < buffer_len; line_length++ ){
			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){
				break;
			}
		}
		if( line_length < buffer_len ){
			check_string = (char*)malloc( line_length + 1 );
			memcpy( check_string, buffer, line_length );
			check_string[line_length] = '\0';
			if( !regex_match( check_string, method_regex )){
				check_result = CHECK_NG;
			}
			free( check_string );
		}
		else{
			check_result = CHECK_INPOSSIBLE;
		}
	}
	else{
		check_result = CHECK_NG;
	}
	return check_result;

// 	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
// 	char	check_string[256];
// 	size_t	check_length	= 16;
// 	cregex method_regex
// 		= (	as_xpr("GET ")		| as_xpr("HEAD ")		| as_xpr("POST ")		|
// 			as_xpr("PUT ")		| as_xpr("PROPFIND ")	| as_xpr("PROPPATCH ")	|
// 			as_xpr("OPTIONS ")	| as_xpr("CONNECT ")	| as_xpr("COPY ")		|
// 			as_xpr("TRACE ")	| as_xpr("DELETE ")		| as_xpr("LOCK ")		|
// 			as_xpr("UNLOCK ")	| as_xpr("MOVE ")		| as_xpr("MKCOL ")) >>
// 			+_;
// 
// 	if( buffer != NULL ){
// 		if( buffer_len >= check_length ){
// 			memcpy( check_string, buffer, check_length );
// 			check_string[check_length]	=	'\0';
// 			if( !regex_match( check_string, method_regex ) ){
// 				check_result = CHECK_NG;
// 			}
// 		}
// 		else{
// 			check_result = CHECK_INPOSSIBLE;
// 		}
// 	}
// 	else{
// 		check_result = CHECK_NG;
// 	}
// 
// 	return check_result;

// 	l7vs::http_procol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
// 	char escape_char;
// 	cregex method_regex
// 		= (	as_xpr("GET ")		| as_xpr("HEAD ")		| as_xpr("POST ")		|
// 			as_xpr("PUT ")		| as_xpr("PROPFIND ")	| as_xpr("PROPPATCH ")	|
// 			as_xpr("OPTIONS ")	| as_xpr("CONNECT ")	| as_xpr("COPY ")		|
// 			as_xpr("TRACE ")	| as_xpr("DELETE ")		| as_xpr("LOCK ")		|
// 			as_xpr("UNLOCK ")	| as_xpr("MOVE ")		| as_xpr("MKCOL ")) >> *_;
// 
// 	if( buffer != NULL ){
// 		if( buffer_len >= 16 ){
// 			escape_char = buffer[15];
// 			buffer[15] = 0x00;
// 			if( !regex_match( buffer, method_regex ) ){
// 				check_result = CHECK_NG;
// 			}
// 			buffer[15] = escape_char;
// 		}
// 		else{
// 			check_result = CHECK_INPOSSIBLE;
// 		}
// 	}
// 	else{
// 		check_result = CHECK_NG;
// 	}
// 	return check_result;

}



l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_http_version( const char* buffer, const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
	char*	check_string;
	size_t	line_length		= 0;
	cregex	version_regex_request
		=	+alpha >> _s >>
			+~_s >> _s >>
			"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1"));
	cregex version_regex_response
		=	"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1")) >> _s >>
			repeat<3>(_d) >> _s >>
			*_;

	if( buffer != NULL ){
		for( line_length = 0; line_length < buffer_len; line_length++ ){
			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){
				break;
			}
		}
		if( line_length < buffer_len ){
			check_string = (char*)malloc( line_length + 1 );
			memcpy( check_string, buffer, line_length );
			check_string[line_length] = '\0';
			if( !regex_match( check_string, version_regex_request ) &&
				!regex_match( check_string, version_regex_response ) ){
				check_result = CHECK_NG;
			}
			free( check_string );
		}
		else{
			check_result = CHECK_INPOSSIBLE;
		}
	}
	else{
		check_result = CHECK_NG;
	}
	return check_result;
}


l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_status_code( const char* buffer, const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
	char*	check_string;
	size_t	line_length		= 0;
	cregex	status_code_regex
 		=	"HTTP/" >> _d >> "." >> _d >> _s >>
			range('1', '3') >> repeat<2>(_d) >> _s >>
			*_;

	if( buffer != NULL ){
		for( line_length = 0; line_length < buffer_len; line_length++ ){
			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){
				break;
			}
		}
		if( line_length < buffer_len ){
			check_string = (char*)malloc( line_length + 1 );
			memcpy( check_string, buffer, line_length );
			check_string[line_length] = '\0';
			if( !regex_match( check_string, status_code_regex )){
				check_result = CHECK_NG;
			}
			free( check_string );
		}
		else{
			check_result = CHECK_INPOSSIBLE;
		}
	}
	else{
		check_result = CHECK_NG;
	}
	return check_result;

// 	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
// 	char	check_string[256];
// 	size_t	check_length	= 13;
// 	cregex status_code_regex
// 		= "HTTP/" >> *_ >> _s >> range( '1', '3' ) >> repeat<2>( _d ) >> _s >> *_;
// 
// 	if( buffer != NULL ){
// 		if( buffer_len >= check_length ){
// 			memcpy( check_string, buffer, check_length );
// 			check_string[check_length]	=	'\0';
// 			if( !regex_match( check_string, status_code_regex ) ){
// 				check_result = CHECK_NG;
// 			}
// 		}
// 		else{
// 			check_result = CHECK_INPOSSIBLE;
// 		}
// 	}
// 	else{
// 		check_result = CHECK_NG;
// 	}
// 
// 	return check_result;

}



bool	l7vs::http_protocol_module_base::find_uri( const char* buffer, const size_t buffer_len, size_t& uri_offset, size_t& uri_len){

	char*	find_string;
	size_t	line_length		= 0;
	bool	find_result	= true;
	match_results< const char* >	result;
	cregex	uri_regex
		=	+alpha >> _s >>
			(s1 = +~_s) >> _s >>
			"HTTP/" >> _d >> "." >> _d;

	if( buffer != NULL ){
		for( line_length = 0; line_length < buffer_len; line_length++ ){
			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){
				break;
			}
		}
		if( line_length < buffer_len ){
			find_string = (char*)malloc( line_length + 1 );
			memcpy( find_string, buffer, line_length );
			find_string[line_length] = '\0';
			find_result = regex_search( find_string, result, uri_regex );
			if( find_result == true ){
				uri_offset	= result.position(1);
				uri_len		= result.length(1);
			}
			free( find_string );
		}
		else{
			find_result = false;
		}
	}
	else{
		find_result = false;
	}

	return find_result;


// 	size_t	uri_start	= 0;
// 	size_t	uri_end		= 0;
// 	bool	find_result	= true;
// 
// 	for( uri_start = 0; uri_start < buffer_len - 1; uri_start++ ){
// 		if( buffer[uri_start] == ' ' || buffer[uri_start] == '\x0D' || buffer[uri_start] == '\x0A' ){
// 			break;
// 		}
// 	}
// 
// 	if( uri_start < buffer_len - 1 && buffer[uri_start] == ' ' ){
// 
// 		uri_start++;
// 
// 		for( uri_end = uri_start; uri_end < buffer_len; uri_end++ ){
// 			if( buffer[uri_end] == ' ' || buffer[uri_end] == '\x0D' || buffer[uri_end] == '\x0A' ){
// 				break;
// 			}
// 		}
// 
// 		if( uri_end < buffer_len && buffer[uri_end] == ' ' ){
// 			uri_offset	= uri_start;
// 			uri_len		= uri_end - uri_start;
// 		}
// 		else{
// 			find_result = false;
// 		}
// 	}
// 	else{
// 		find_result = false;
// 	}
// 
// 	return find_result;

}


bool	l7vs::http_protocol_module_base::find_status_code( const char* buffer, const size_t buffer_len, size_t& status_code_offset, size_t& status_code_len){

	char*	find_string;
	size_t	line_length		= 0;
	bool	find_result	= true;
	match_results< const char* >	result;
	cregex	status_code_regex
		=	"HTTP/" >> _d >> "." >> _d >> _s >>
			(s1 = repeat<3>(_d)) >> _s >>
			*_;

	if( buffer != NULL ){
		for( line_length = 0; line_length < buffer_len; line_length++ ){
			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){
				break;
			}
		}
		if( line_length < buffer_len ){
			find_string = (char*)malloc( line_length + 1 );
			memcpy( find_string, buffer, line_length );
			find_string[line_length] = '\0';
			find_result = regex_search( find_string, result, status_code_regex );
			if( find_result == true ){
				status_code_offset	= result.position(1);
				status_code_len		= result.length(1);
			}
			free( find_string );
		}
		else{
			find_result = false;
		}
	}
	else{
		find_result = false;
	}

	return find_result;

// 	size_t	status_code_start	= 0;
// 	size_t	status_code_end		= 0;
// 	bool	find_result	= true;
// 
// 	for( status_code_start = 0; status_code_start < buffer_len - 1; status_code_start++ ){
// 		if( buffer[status_code_start] == ' ' || buffer[status_code_start] == '\x0D' || buffer[status_code_start] == '\x0A' ){
// 			break;
// 		}
// 	}
// 
// 	if( status_code_start < buffer_len - 1 && buffer[status_code_start] == ' ' ){
// 		
// 		status_code_start++;
// 
// 		for( status_code_end = status_code_start; status_code_end < buffer_len; status_code_end++ ){
// 			if( buffer[status_code_end] == ' ' || buffer[status_code_end] == '\x0D' || buffer[status_code_end] == '\x0A' ){
// 				break;
// 			}
// 		}
// 
// 		if( status_code_end < buffer_len && buffer[status_code_end] == ' ' ){
// 			status_code_offset	= status_code_start;
// 			status_code_len		= status_code_end - status_code_start;
// 		}
// 		else{
// 			find_result = false;
// 		}
// 	}
// 	else{
// 		find_result = false;
// 	}
// 
// 	return find_result;

}

bool	l7vs::http_protocol_module_base::find_http_header( const char* buffer, const size_t buffer_len, const std::string& http_header_name, size_t& http_header_offset, size_t& http_header_len ){

	char*	find_string;
	int		count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;
	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;
	bool	find_result	= true;
	match_results< const char* >	result;
	cregex	http_header_regex;
// 	cregex	http_header_regex
// 		=	"HTTP/" >> _d >> "." >> _d >> _s >>
// 			(s1 = repeat<3>(_d)) >> _s >>
// 			*_;

	if( buffer != NULL ){
		for( count = 0; count < buffer_len; count++ ){
			if( buffer[count] == '\r' || buffer[count] == '\n' ){
				if( header_begin_flag == 0 ){
					header_begin = count;
					header_begin_flag = 1;
				}
				if( count > 0 ){
					if(	( buffer[count-1] == '\r' && buffer[count] == '\r' ) ||
						( buffer[count-1] == '\n' && buffer[count] == '\n' )	){
						header_end = count;
						header_end_flag = 1;
						break;
					}
				}
				if( count > 2 ){
					if(	buffer[count-3] == '\r' && buffer[count-2] == '\n' &&
						buffer[count-1] == '\r' && buffer[count] == '\n'	){
						header_end = count;
						header_end_flag = 1;
						break;
					}
				}
			}
		}

		if( header_begin_flag == 1 && header_end_flag == 1 ){

			header_length = header_end - header_begin + 1;
			find_string = (char*)malloc( header_length + 1 );
			memcpy( find_string, buffer + header_begin, header_length );
			find_string[header_length] = '\0';

			if( http_header_name.length() > 0 ){
				http_header_regex = _ln >> (s1 = http_header_name >> _ >> _s >> *~_ln);
				find_result = regex_search( find_string, result, http_header_regex );
				if( find_result == true ){
					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);
std::cout << "regex_search_1" << std::endl;
std::cout << "result.position(0) =" << result.position(0) << std::endl;
std::cout << "result.length(0) =" << result.length(0) << std::endl;
std::cout << "result.position(1) =" << result.position(1) << std::endl;
std::cout << "result.length(1) =" << result.length(1) << std::endl;
				}
			}
			else{
				http_header_regex = _ln >> (s1 = *_ >> ~_ln) >> repeat<2>(_ln);
				find_result = regex_search( find_string, result, http_header_regex );
				if( find_result == true ){
					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);
std::cout << "regex_search_2" << std::endl;
std::cout << "result.position(0) =" << result.position(0) << std::endl;
std::cout << "result.length(0) =" << result.length(0) << std::endl;
std::cout << "result.position(1) =" << result.position(1) << std::endl;
std::cout << "result.length(1) =" << result.length(1) << std::endl;
				}
				else{
					http_header_regex = _ln >> (s1 = _ln);
					find_result = regex_search( find_string, result, http_header_regex );
					if( find_result == true ){
						http_header_offset	= result.position(1) + header_begin;
						http_header_len		= 0;
std::cout << "regex_search_3" << std::endl;
std::cout << "result.position(0) =" << result.position(0) << std::endl;
std::cout << "result.length(0) =" << result.length(0) << std::endl;
std::cout << "result.position(1) =" << result.position(1) << std::endl;
std::cout << "result.length(1) =" << result.length(1) << std::endl;
					}
				}
			}
			free( find_string );
		}
		else{
			find_result	= false;
		}
	}
	else{
		find_result	= false;
	}


// 			if( http_header_name.length() > 0 ){
// 				if( header_begin == header_end ){
// 					find_result	= false;
// 				}
// 				else{
// 					header_length = header_end - header_begin + 1;
// 					find_string = (char*)malloc( header_length + 1 );
// 					memcpy( find_string, buffer + header_begin, header_length );
// 					find_string[header_length] = '\0';
// 
// 
// 					http_header_regex =	_ln >>
// 										(s1 = http_header_name >> _ >> _s >> *~_ln);
// 					find_result = regex_search( find_string, result, http_header_regex );
// 					if( find_result == true ){
// 						http_header_offset	= result.position(1) + header_begin;
// 						http_header_len		= result.length(1);
// 
// std::cout << "result.position(0) =" << result.position(0) << std::endl;
// std::cout << "result.length(0) =" << result.length(0) << std::endl;
// std::cout << "result.position(1) =" << result.position(1) << std::endl;
// std::cout << "result.length(1) =" << result.length(1) << std::endl;
// 
// 					}
// 					free( find_string );
// 				}
// 			}
// 			else{
// 				http_header_offset	= header_begin + 1;
// 				if( header_begin == header_end ){
// 					http_header_len	= 0;
// 				}
// 				else{
// 					http_header_len	= header_end - header_begin -1;
// 				}
// 			}
// 		}
// 		else{
// 			find_result	= false;
// 		}
// 	}
// 	else{
// 		find_result	= false;
// 	}

	return find_result;


// 	char*	find_string;
// 	size_t	http_header_start	= 0;
// 	size_t	http_header_end		= 0;
// 	cregex version_regex_request
// 		= +_ >> _s >> +_ >> " HTTP/" >> (as_xpr("1.0")|as_xpr("1.1"));
// 
// 	cregex start_search_regex	= _ln;
// 	cregex end_search_regex	= repeat<2>(_ln);
// 	cregex header_search_regex	= _ln >> "X-Forwarded-For:" >> *~_ln;
// 	match_results< const char* >	result;
// 
// 	bool	ret;
// 
// 	ret = regex_search( buffer, result, start_search_regex );
// 
// 	std::cout << "Result = [" << ret << "]" << std::endl;
// 	std::cout << "Length = [" << result.length(0) << "]" << std::endl;
// 	std::cout << "Position = [" << result.position(0) << "]" << std::endl;
// 
// 	ret = regex_search( buffer, result, end_search_regex );
// 
// 	std::cout << "Result = [" << ret << "]" << std::endl;
// 	std::cout << "Length = [" << result.length(0) << "]" << std::endl;
// 	std::cout << "Position = [" << result.position(0) << "]" << std::endl;
// 
// 	ret = regex_search( buffer, result, header_search_regex );
// 
// 	std::cout << "Result = [" << ret << "]" << std::endl;
// 	std::cout << "Length = [" << result.length(0) << "]" << std::endl;
// 	std::cout << "Position = [" << result.position(0) << "]" << std::endl;
//---------------------------------------------------------------------
// 	size_t http_header_start	= 0;
// 	size_t http_header_end	= 0;
// 
// 	for( http_header_start = 0; http_header_start < buffer_len - 1; http_header_start++ ){
// 		if( buffer[http_header_start] == '\x0D' || buffer[http_header_start] == '\x0A' ){
// 			break;
// 		}
// 	}
// 
// 	if( http_header_start < buffer_len - 1 ){
// 
// 		http_header_start++;
// 
// 		for( http_header_end = http_header_start; http_header_end < buffer_len; http_header_end++ ){
// 			if( buffer[http_header_end] == '\x0D' || buffer[http_header_end] == '\x0A' ){
// 				break;
// 			}
// 		}
// 
// 		if( http_header_end < buffer_len ){
// 			http_header_offset	= status_code_start;
// 			http_header_len		= status_code_end - status_code_start;
// 		}
// 		else{
// 			find_result = false;
// 		}

}


