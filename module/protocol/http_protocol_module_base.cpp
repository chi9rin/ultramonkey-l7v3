//
//	@file	http_protocol_module_base.cpp
//	@brief	shared object http protocol module absctract class
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef	HTTP_PROTOCOL_MODULE_BASE_H
#define	HTTP_PROTOCOL_MODULE_BASE_H

#include "http_protocol_module_base.h"

namespace l7vs{

CHECK_RESULT_TAG	check_http_method( const char* buffer, const size_t buffer_len ){

	CHECK_RESULT_TAG	check_result = CHECK_OK;
	char escape_char;
	cregex method_regex = ( "GET "		|| "HEAD "		|| "POST "	|| "PUT "	|| "PROPFIND "	|| "PROPPATCH "	||
							"OPTIONS "	|| "CONNECT "	|| "COPY "	|| "TRACE "	|| "DELETE "	|| "LOCK "		||
							"UNLOCK "	|| "MOVE "		|| "MKCOL " ) >> *_;
	if( buffer_len >= 16 ){
		escape_char = buffer[15];
		buffer[15] = NULL;
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



CHECK_RESULT_TAG	check_http_version( const char* buffer, const size_t buffer_len ){

	CHECK_RESULT_TAG	check_result = CHECK_OK;
	char escape_char;
	cregex version_regex_request	= +_ >> _s >> +_ >> " HTTP/" >> ( "1.0" || "1.1" ) >> _ln;
	cregex version_regex_response	= "HTTP/" >> ( "1.0" || "1.1" ) >> +_ >> +_ >> _ln;

	if( buffer_len >= 16 ){

		for( i = 0; i < buffer_len - 1; i++ ){
			if( buffer[i] == '\x0D' || buffer[i] == '\x0A' ){
				break;
			}
		}

		if( i < buffer_len - 1 ){
			escape_char = buffer[i+1];
			buffer[i+1] = NULL;
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


CHECK_RESULT_TAG	check_status_code( const char* buffer, const size_t buffer_len ){

	CHECK_RESULT_TAG	check_result = CHECK_OK;
	char escape_char;
	cregex status_code_regex	= "HTTP/" >> _d >> "." >> _d >> _s >> range( '1', '3' ) >> repeat<2>( _d ) >> _s;

	if( buffer_len >= 14 ){

		escape_char = buffer[13];
		buffer[13] = NULL;
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



bool	find_uri( const char* buffer, const size_t buffer_len, size_t& uri_offset, size_t& uri_len){

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


bool	find_status_code( const char* buffer, const size_t buffer_len, size_t& uri_offset, size_t& uri_len){

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

bool	find_http_header( const char* buffer, const size_t buffer_len, const std::string& http_header_name, size_t& http_header_offset, size_t& http_header_len ){

	size_t http_header_start	= 0;
	size_t http_header_end	= 0;

	for( http_header_start = 0; http_header_start < buffer_len - 1; http_header_start++ ){
		if( buffer[http_header_start] == '\x0D' || buffer[http_header_start] == '\x0A' ){
			break;
		}
	}

	if( http_header_start < buffer_len - 1 ){

		http_header_start++;

		for( http_header_end = http_header_start; http_header_end < buffer_len; http_header_end++ ){
			if( buffer[http_header_end] == '\x0D' || buffer[http_header_end] == '\x0A' ){
				break;
			}
		}

		if( http_header_end < buffer_len ){
			http_header_offset	= status_code_start;
			http_header_len		= status_code_end - status_code_start;
		}
		else{
			find_result = false;
		}
























namespace l7vs{

class http_protocol_module_base : public protocol_module_base {
protected:
	//! @enum	CHECK_RESULT_TAG
	//! @brief	check tag is return to http protocol module.
	enum	CHECK_RESULT_TAG{
		CHECK_OK = 0,			//!< check ok
		CHECK_NG,				//!< check ng
		CHECK_INPOSSIBLE		//!< check inpossible
	};

	//! check http method function
	//! @param const char*			buffer
	//! @param const size_t			buffer_len
	//! @return CHECK_RESULT_TAG	http method is valid
	CHECK_RESULT_TAG	check_http_method(	const char*, const size_t ) const;
	//! check http version function
	//! @param const char*			buffer
	//! @param const size_t			buffer_len
	//! @return	CHECK_RESULT_TAG 	http version 1.0 or 1.1
	CHECK_RESULT_TAG	check_http_version(	const char*, const size_t ) const;
	//! check http status code function
	//! @param const char*			buffer
	//! @param const size_t			buffer_len
	//! @return	CHECK_RESULT_TAG	status code is nomal or error
	CHECK_RESULT_TAG	check_status_code(	const char*, const size_t ) const;
	//! serch uri function
	//! @param const char*			buffer
	//! @param const size_t			buffer_len
	//! @param size_t&				uri offset
	//! @param size_t&				uri length
	//! @return bool				find is true. not find is false
	bool	find_uri(	const char*, const size_t, size_t&, size_t&);
	//! serch status function
	//! @param const char*			buffer
	//! @param const size_t			buffer_len
	//! @param size_t&				status offset
	//! @param size_t&				status length
	//! @return bool				find is true. not find is false
	bool	find_status_code(	const char*, const size_t, size_t&, size_t& );
	//! serch http header
	//! @param const char*			buffer
	//! @param const size_t			buffer_len
	//! @param const string&		header name
	//! @param size_t&				header offset
	//! @param size_t&				header length
	//! @return bool				find is true. not find is false
	bool	find_http_header(	const char*, const size_t, const std::string&, size_t&, size_t& );

public:
	//! constractor
	http_protocol_module_base( std::string in_modulename ) : protocol_module_base( in_modulename ){};
	//! destractor
	virtual	~http_protocol_module_base(){};
};

} // namespace l7vsd

#endif	//HTTP_PROTOCOL_MODULE_BASE_H
