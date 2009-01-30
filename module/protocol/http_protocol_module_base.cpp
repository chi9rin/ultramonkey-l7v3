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

l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_http_method(	const char* buffer,
													const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
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

}


l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_http_version(	const char* buffer,
														const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
	size_t	line_length		= 0;

	cregex	version_regex_request
		=	+alpha >> _s >>
			+~_s >> _s >>
			"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1"));

	cregex	version_regex_response
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


l7vs::http_protocol_module_base::CHECK_RESULT_TAG	l7vs::http_protocol_module_base::check_status_code(	const char* buffer,
													const size_t buffer_len ) const {

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
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

}


bool	l7vs::http_protocol_module_base::find_uri(	const char* buffer,
													const size_t buffer_len,
													size_t& uri_offset,
													size_t& uri_len){

	bool	find_result	= true;

	char*	find_string	= NULL;
	size_t	line_length	= 0;

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

}


bool	l7vs::http_protocol_module_base::find_status_code(	const char* buffer,
															const size_t buffer_len,
															size_t& status_code_offset,
															size_t& status_code_len){

	bool	find_result	= true;

	char*	find_string	= NULL;
	size_t	line_length	= 0;

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

}


bool	l7vs::http_protocol_module_base::find_http_header(	const char* buffer,
															const size_t buffer_len,
															const std::string& http_header_name,
															size_t& http_header_offset,
															size_t& http_header_len ){

	bool	find_result			= true;

	char*	find_string			= NULL;
	size_t	count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;

	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;

	match_results< const char* >	result;

	cregex	http_header_regex;

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

				}
			}
			else{

				http_header_regex = _ln >> (s1 = *_ >> ~_ln) >> repeat<2>(_ln);

				find_result = regex_search( find_string, result, http_header_regex );

				if( find_result == true ){

					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);

				}
				else{

					http_header_regex = _ln >> (s1 = _ln);

					find_result = regex_search( find_string, result, http_header_regex );

					if( find_result == true ){

						http_header_offset	= result.position(1) + header_begin;
						http_header_len		= 0;

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

	return find_result;

}


