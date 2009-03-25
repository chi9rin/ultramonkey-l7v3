/*
 *	@file	http_protocol_module_base.cpp
 *	@brief	shared object protocol module abstract class
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/
#include <boost/xpressive/xpressive.hpp>

#include "http_protocol_module_base.h"

using namespace boost::xpressive;

cregex	method_regex
		= (	as_xpr("GET")		| as_xpr("HEAD")		| as_xpr("POST")		|
			as_xpr("PUT")		| as_xpr("PROPFIND")	| as_xpr("PROPPATCH")	|
			as_xpr("OPTIONS")	| as_xpr("CONNECT")		| as_xpr("COPY")		|
			as_xpr("TRACE")		| as_xpr("DELETE")		| as_xpr("LOCK")		|
			as_xpr("UNLOCK")	| as_xpr("MOVE")		| as_xpr("MKCOL")) >> _s >>
			+~_s >> _s >>
			"HTTP/" >> _d >> "." >> _d;

cregex	version_regex_request
		=	+alpha >> _s >>
			+~_s >> _s >>
			"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1"));

cregex	version_regex_response
		=	"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1")) >> _s >>
			repeat<3>(_d) >> _s >>
			*_;

cregex	status_code_regex_check
		=	"HTTP/" >> _d >> "." >> _d >> _s >>
			range('1', '3') >> repeat<2>(_d) >> _s >>
			*_;

cregex	method_and_version_regex
		= (	as_xpr("GET")		| as_xpr("HEAD")		| as_xpr("POST")		|
			as_xpr("PUT")		| as_xpr("PROPFIND")	| as_xpr("PROPPATCH")	|
			as_xpr("OPTIONS")	| as_xpr("CONNECT")		| as_xpr("COPY")		|
			as_xpr("TRACE")		| as_xpr("DELETE")		| as_xpr("LOCK")		|
			as_xpr("UNLOCK")	| as_xpr("MOVE")		| as_xpr("MKCOL")) >> _s >>
			+~_s >> _s >>
			"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1"));

cregex	version_and_status_code_regex
		=	"HTTP/" >> (as_xpr("1.0")|as_xpr("1.1")) >> _s >>
			range('1', '3') >> repeat<2>(_d) >> _s >>
			*_;

cregex	uri_regex
		=	+alpha >> _s >>
			(s1 = *~_s) >> _s >>
			"HTTP/" >> _d >> "." >> _d;

cregex	status_code_regex_find
		=	"HTTP/" >> _d >> "." >> _d >> _s >>
			(s1 = repeat<3>(_d)) >> _s >>
			*_;

cregex	http_header_regex_cookie
		= _ln >> (s1 = icase("cookie") >> ":" >> *~_ln);

cregex	http_header_regex_content_length
		= _ln >> (s1 = icase("content-length") >> ":" >> *~_ln);

cregex	http_header_regex_x_forwarded_for
		= _ln >> (s1 = icase("x-forwarded-for") >> ":" >> *~_ln);

cregex	http_header_regex_all
		= _ln >> (s1 = *_ >> ~_ln) >> repeat<2>(_ln);

cregex	http_header_regex_none
		= _ln >> (s1 = _ln);

l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_method(	const char* buffer,
													const size_t buffer_len ) const {

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [check_http_method] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
	size_t	line_length		= 0;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			check_string = (char*)malloc( line_length + 1 );

			if( check_string != NULL ){
				memcpy( check_string, buffer, line_length );
	
				check_string[line_length] = '\0';
	
				if( !regex_match( check_string, method_regex )){
	
					check_result = CHECK_NG;
	
				}
	
				free( check_string );
			}
			else{

				check_result = CHECK_NG;

			}

		}
		else{

			check_result = CHECK_INPOSSIBLE;

		}

	}
	else{

		check_result = CHECK_NG;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [check_http_method] : "
									"check_result = [%d]" );

		outform % check_result;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;

}

l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_version(	const char* buffer,
														const size_t buffer_len ) const {

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [check_http_version] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
	size_t	line_length		= 0;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			check_string = (char*)malloc( line_length + 1 );

			if( check_string != NULL ){

				memcpy( check_string, buffer, line_length );
	
				check_string[line_length] = '\0';
	
				if( !regex_match( check_string, version_regex_request ) &&
					!regex_match( check_string, version_regex_response ) ){
	
					check_result = CHECK_NG;
	
				}
	
				free( check_string );

			}
			else{

				check_result = CHECK_NG;

			}

		}
		else{

			check_result = CHECK_INPOSSIBLE;

		}

	}
	else{

		check_result = CHECK_NG;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [check_http_version] : "
									"check_result = [%d]" );

		outform % check_result;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;

}


l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_status_code(	const char* buffer,
													const size_t buffer_len ) const {

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [check_status_code] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
	size_t	line_length		= 0;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			check_string = (char*)malloc( line_length + 1 );

			if( check_string != NULL ){

				memcpy( check_string, buffer, line_length );
	
				check_string[line_length] = '\0';
	
				if( !regex_match( check_string, status_code_regex_check )){
	
					check_result = CHECK_NG;
	
				}
	
				free( check_string );

			}
			else{

				check_result = CHECK_NG;

			}

		}
		else{

			check_result = CHECK_INPOSSIBLE;

		}

	}
	else{

		check_result = CHECK_NG;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [check_status_code] : "
									"check_result = [%d]" );

		outform % check_result;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;

}

l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_method_and_version(
													const char* buffer,
													const size_t buffer_len ) const {

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [check_http_method_and_version] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
	size_t	line_length		= 0;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			check_string = (char*)malloc( line_length + 1 );

			if( check_string != NULL ){
				memcpy( check_string, buffer, line_length );
	
				check_string[line_length] = '\0';
	
				if( !regex_match( check_string, method_and_version_regex )){
	
					check_result = CHECK_NG;
	
				}
	
				free( check_string );
			}
			else{

				check_result = CHECK_NG;

			}

		}
		else{

			check_result = CHECK_INPOSSIBLE;

		}

	}
	else{

		check_result = CHECK_NG;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [check_http_method_and_version] : "
									"check_result = [%d]" );

		outform % check_result;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;

}

l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_version_and_status_code(
													const char* buffer,
													const size_t buffer_len ) const {

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [check_http_version_and_status_code] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	l7vs::http_protocol_module_base::CHECK_RESULT_TAG	check_result = CHECK_OK;

	char*	check_string	= NULL;
	size_t	line_length		= 0;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			check_string = (char*)malloc( line_length + 1 );

			if( check_string != NULL ){
				memcpy( check_string, buffer, line_length );
	
				check_string[line_length] = '\0';
	
				if( !regex_match( check_string, version_and_status_code_regex )){
	
					check_result = CHECK_NG;
	
				}
	
				free( check_string );
			}
			else{

				check_result = CHECK_NG;

			}

		}
		else{

			check_result = CHECK_INPOSSIBLE;

		}

	}
	else{

		check_result = CHECK_NG;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [check_http_version_and_status_code] : "
									"check_result = [%d]" );

		outform % check_result;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return check_result;

}

bool	l7vs::http_protocol_module_base::find_uri(	const char* buffer,
													const size_t buffer_len,
													size_t& uri_offset,
													size_t& uri_len){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_uri] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	bool	find_result	= true;

	char*	find_string	= NULL;
	size_t	line_length	= 0;

	match_results< const char* >	result;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			find_string = (char*)malloc( line_length + 1 );

			if( find_string != NULL ){

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

	}
	else{

		find_result = false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_uri] : "
									"find_result = [%d], "
									"uri_offset = [%d], "
									"uri_len = [%d]" );

		outform % find_result % uri_offset % uri_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}


bool	l7vs::http_protocol_module_base::find_status_code(	const char* buffer,
															const size_t buffer_len,
															size_t& status_code_offset,
															size_t& status_code_len){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_status_code] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	bool	find_result	= true;

	char*	find_string	= NULL;
	size_t	line_length	= 0;

	match_results< const char* >	result;

	if( buffer != NULL ){

		for( line_length = 0; line_length < buffer_len; line_length++ ){

			if( buffer[line_length] == '\r' || buffer[line_length] == '\n' ){

				break;

			}

		}

		if( line_length < buffer_len ){

			find_string = (char*)malloc( line_length + 1 );

			if( find_string != NULL ){

				memcpy( find_string, buffer, line_length );
	
				find_string[line_length] = '\0';
	
				find_result = regex_search( find_string, result, status_code_regex_find );
	
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

	}
	else{

		find_result = false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_status_code] : "
									"find_result = [%d], "
									"status_code_offset = [%d], "
									"status_code_len = [%d]" );

		outform % find_result % status_code_offset % status_code_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}


bool	l7vs::http_protocol_module_base::find_http_header(	const char* buffer,
															const size_t buffer_len,
															const std::string& http_header_name,
															size_t& http_header_offset,
															size_t& http_header_len ){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_http_header] : "
									"buffer_len = [%d], "
									"http_header_name = [%s]" );

		outform % buffer_len % http_header_name;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	cregex	http_header_regex;

	bool	find_result			= true;

	char*	find_string			= NULL;
	size_t	count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;

	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;

	match_results< const char* >	result;

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

			if( find_string != NULL ){

				memcpy( find_string, buffer + header_begin, header_length );
	
				find_string[header_length] = '\0';

				if( http_header_name.length() > 0 ){

					http_header_regex = _ln >> (s1 = icase(http_header_name) >> ":" >> *~_ln);

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
	}
	else{

		find_result	= false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_http_header] : "
									"find_result = [%d], "
									"http_header_offset = [%d], "
									"http_header_len = [%d]" );

		outform % find_result % http_header_offset % http_header_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}

bool	l7vs::http_protocol_module_base::find_http_header_cookie(
															const char* buffer,
															const size_t buffer_len,
															size_t& http_header_offset,
															size_t& http_header_len ){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_http_header_cookie] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	bool	find_result			= true;

	char*	find_string			= NULL;
	size_t	count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;

	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;

	match_results< const char* >	result;

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

			if( find_string != NULL ){

				memcpy( find_string, buffer + header_begin, header_length );
	
				find_string[header_length] = '\0';

				find_result = regex_search( find_string, result, http_header_regex_cookie );

				if( find_result == true ){

					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);

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
	}
	else{

		find_result	= false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_http_header_cookie] : "
									"find_result = [%d], "
									"http_header_offset = [%d], "
									"http_header_len = [%d]" );

		outform % find_result % http_header_offset % http_header_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}

bool	l7vs::http_protocol_module_base::find_http_header_content_length(
															const char* buffer,
															const size_t buffer_len,
															size_t& http_header_offset,
															size_t& http_header_len ){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_http_header_content_length] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	bool	find_result			= true;

	char*	find_string			= NULL;
	size_t	count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;

	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;

	match_results< const char* >	result;

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

			if( find_string != NULL ){

				memcpy( find_string, buffer + header_begin, header_length );
	
				find_string[header_length] = '\0';

				find_result = regex_search( find_string, result, http_header_regex_content_length );

				if( find_result == true ){

					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);

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
	}
	else{

		find_result	= false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_http_header_content_length] : "
									"find_result = [%d], "
									"http_header_offset = [%d], "
									"http_header_len = [%d]" );

		outform % find_result % http_header_offset % http_header_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}

bool	l7vs::http_protocol_module_base::find_http_header_x_forwarded_for(
															const char* buffer,
															const size_t buffer_len,
															size_t& http_header_offset,
															size_t& http_header_len ){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_http_header_x_forwarded_for] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	bool	find_result			= true;

	char*	find_string			= NULL;
	size_t	count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;

	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;

	match_results< const char* >	result;

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

			if( find_string != NULL ){

				memcpy( find_string, buffer + header_begin, header_length );
	
				find_string[header_length] = '\0';

				find_result = regex_search( find_string, result, http_header_regex_x_forwarded_for );

				if( find_result == true ){

					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);

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
	}
	else{

		find_result	= false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_http_header_x_forwarded_for] : "
									"find_result = [%d], "
									"http_header_offset = [%d], "
									"http_header_len = [%d]" );

		outform % find_result % http_header_offset % http_header_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}

bool	l7vs::http_protocol_module_base::find_http_header_all(
															const char* buffer,
															const size_t buffer_len,
															size_t& http_header_offset,
															size_t& http_header_len ){

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function in  : [find_http_header_all] : "
									"buffer_len = [%d]" );

		outform % buffer_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	bool	find_result			= true;

	char*	find_string			= NULL;
	size_t	count				= 0;
	size_t	header_begin		= 0;
	size_t	header_end			= 0;
	size_t	header_length		= 0;

	int		header_begin_flag	= 0;
	int		header_end_flag		= 0;

	match_results< const char* >	result;

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

			if( find_string != NULL ){

				memcpy( find_string, buffer + header_begin, header_length );
	
				find_string[header_length] = '\0';

				find_result = regex_search( find_string, result, http_header_regex_all );

				if( find_result == true ){

					http_header_offset	= result.position(1) + header_begin;
					http_header_len		= result.length(1);

				}
				else{

					find_result = regex_search( find_string, result, http_header_regex_none );

					if( find_result == true ){

						http_header_offset	= result.position(1) + header_begin;
						http_header_len		= 0;

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
	}
	else{

		find_result	= false;

	}

	//---------- DEBUG LOG START ------------------------------
	if( LOG_LV_DEBUG == getloglevel())
	{
		boost::format	outform(	"function out : [find_http_header_all] : "
									"find_result = [%d], "
									"http_header_offset = [%d], "
									"http_header_len = [%d]" );

		outform % find_result % http_header_offset % http_header_len;

		putLogDebug(	0,
						outform.str(),
						__FILE__,
						__LINE__ );
	}
	//---------- DEBUG LOG END ------------------------------

	return find_result;

}
