//
//	@file	http_protocol_module_base.h
//	@brief	shared object http protocol module absctract class
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef	HTTP_PROTOCOL_MODULE_BASE_H
#define	HTTP_PROTOCOL_MODULE_BASE_H

#include "protocol_module_base.h"

namespace l7vs{

class http_protocol_module_base : public protocol_module_base {
protected:

	// http function
	int		check_http_method(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len );
	int		check_http_version(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len );
	int		check_status_code(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len );
	bool	find_uri(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len,
								int& uri_offset,
								int& uri_len );
	bool	find_status_code(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len,
								int& status_code_offset,
								int& status_code_len );
	bool	find_http_header(
								const boost::array<char,MAX_BUFFER_SIZE>& buffer,
								const int buffer_len,
								const boost::array<char,MAX_BUFFER_SIZE>& http_header_name,
								const int http_header_name_len,
								int& http_header_offset,
								int& http_header_len );

public:

	http_protocol_module_base( logger_func_type inlog ) : logger( inlog );

	virtual	~http_protocol_module_base() = 0;

};

} // namespace l7vsd

#endif	//HTTP_PROTOCOL_MODULE_BASE_H
