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
