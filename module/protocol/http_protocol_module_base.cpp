//
//	@file	http_protocol_module_base.cpp
//	@brief	shared object http protocol module absctract class
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include <iostream>
#include <boost/xpressive/xpressive.hpp>

#include "http_protocol_module_base.h"

using namespace boost::xpressive;

l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_http_method( char* buffer, const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;
	char escape_char;
	cregex method_regex
		= (	as_xpr("GET ")		| as_xpr("HEAD ")		| as_xpr("POST ")		|
			as_xpr("PUT ")		| as_xpr("PROPFIND ")	| as_xpr("PROPPATCH ")	|
			as_xpr("OPTIONS ")	| as_xpr("CONNECT ")	| as_xpr("COPY ")		|
			as_xpr("TRACE ")	| as_xpr("DELETE ")		| as_xpr("LOCK ")		|
			as_xpr("UNLOCK ")	| as_xpr("MOVE ")		| as_xpr("MKCOL ")) >> *_;

	if( buffer_len >= 16 ){
		escape_char = buffer[15];
		buffer[15] = 0x00;
		if( !regex_match( buffer, method_regex ) ){
			check_result = CHECK_NG;
		}
		buffer[15] = escape_char;
	}
	else{
		check_result = CHECK_INPOSSIBLE;
	}

	return check_result;

}



l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_http_version( char* buffer, const size_t buffer_len ) const {

	CHECK_RESULT_TAG	check_result = CHECK_OK;
	char escape_char;
	size_t	i;
	cregex version_regex_request
		= +_ >> _s >> +_ >> " HTTP/" >> (as_xpr("1.0")|as_xpr("1.1")) >> _ln;
	cregex version_regex_response
		= "HTTP/" >> (as_xpr("1.0")|as_xpr("1.1")) >> +_ >> +_ >> _ln;

	if( buffer_len >= 16 ){

		for( i = 0; i < buffer_len - 1; i++ ){
			if( buffer[i] == '\x0D' || buffer[i] == '\x0A' ){
				break;
			}
		}

		if( i < buffer_len - 1 ){
			escape_char = buffer[i+1];
			buffer[i+1] = 0x00;
			if( !regex_match( buffer, version_regex_request ) && !regex_match( buffer, version_regex_response ) ){
				check_result = CHECK_NG;
			}
			buffer[i+1] = escape_char;
		}
		else{
			check_result = CHECK_INPOSSIBLE;
		}
	}
	else{
		check_result = CHECK_INPOSSIBLE;
	}

	return check_result;

}


l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_status_code( char* buffer, const size_t buffer_len ) const {

	CHECK_RESULT_TAG	check_result = CHECK_OK;
	char escape_char;
	cregex status_code_regex
		= "HTTP/" >> _d >> "." >> _d >> _s >> range( '1', '3' ) >> repeat<2>( _d ) >> _s;

	if( buffer_len >= 14 ){

		escape_char = buffer[13];
		buffer[13] = 0x00;
		if( !regex_match( buffer, status_code_regex ) ){
			check_result = CHECK_NG;
		}
		buffer[13] = escape_char;
	}
	else{
		check_result = CHECK_INPOSSIBLE;
	}

	return check_result;

}



bool	l7vs::http_protocol_module_base::find_uri( char* buffer, const size_t buffer_len, size_t& uri_offset, size_t& uri_len){

	size_t	uri_start	= 0;
	size_t	uri_end		= 0;
	bool	find_result	= true;

	for( uri_start = 0; uri_start < buffer_len - 1; uri_start++ ){
		if( buffer[uri_start] == ' ' || buffer[uri_start] == '\x0D' || buffer[uri_start] == '\x0A' ){
			break;
		}
	}

	if( uri_start < buffer_len - 1 && buffer[uri_start] == ' ' ){

		uri_start++;

		for( uri_end = uri_start; uri_end < buffer_len; uri_end++ ){
			if( buffer[uri_end] == ' ' || buffer[uri_end] == '\x0D' || buffer[uri_end] == '\x0A' ){
				break;
			}
		}

		if( uri_end < buffer_len && buffer[uri_end] == ' ' ){
			uri_offset	= uri_start;
			uri_len		= uri_end - uri_start;
		}
		else{
			find_result = false;
		}
	}
	else{
		find_result = false;
	}

	return find_result;

}


bool	l7vs::http_protocol_module_base::find_status_code( char* buffer, const size_t buffer_len, size_t& status_code_offset, size_t& status_code_len){

	size_t	status_code_start	= 0;
	size_t	status_code_end		= 0;
	bool	find_result	= true;

	for( status_code_start = 0; status_code_start < buffer_len - 1; status_code_start++ ){
		if( buffer[status_code_start] == ' ' || buffer[status_code_start] == '\x0D' || buffer[status_code_start] == '\x0A' ){
			break;
		}
	}

	if( status_code_start < buffer_len - 1 && buffer[status_code_start] == ' ' ){
		
		status_code_start++;

		for( status_code_end = status_code_start; status_code_end < buffer_len; status_code_end++ ){
			if( buffer[status_code_end] == ' ' || buffer[status_code_end] == '\x0D' || buffer[status_code_end] == '\x0A' ){
				break;
			}
		}

		if( status_code_end < buffer_len && buffer[status_code_end] == ' ' ){
			status_code_offset	= status_code_start;
			status_code_len		= status_code_end - status_code_start;
		}
		else{
			find_result = false;
		}
	}
	else{
		find_result = false;
	}

	return find_result;

}

bool	l7vs::http_protocol_module_base::find_http_header( char* buffer, const size_t buffer_len, const std::string& http_header_name, size_t& http_header_offset, size_t& http_header_len ){

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

	return true;
}


