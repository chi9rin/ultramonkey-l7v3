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
public:
	typedef	boost::array<char, MAX_BUFFER_SIZE>	contenor_type;
protected:
	//! check http method function
	//! @param const contenor_type	buffer
	//! @param const int			length
	//! @return bool				http_protocol is true, no http_protocol is false 
	bool	check_http_method(	const contenor_type&, const int ) const;
	//! check http version function
	//! @param const contenor_type	buffer
	//! @param const int			length
	//! @return	double 				http version 1.0 or 1.1
	double	check_http_version(	const contenor_type&, const int ) const;
	//! ch1eck http status code function
	//! @param const contenor_type	buffer
	//! @param const int			length
	//! @return	bool				status code is nomal or error
	bool		check_status_code(	const contenor_type&, const int );
	//! check serch uri function
	//! @param const contenor_type	buffer
	//! @param const int			length
	//! @param int& uri length
	//! @return contenor_type::iterator	url head pos. not find url is contenor_type::end()
	contenor_type::iterator	find_uri( const contenor_type&, const int, int&);
	//! check serch status function
	//! @param const contenor_type	buffer
	//! @param const int			length
	//! @param int& status code length
	//! @return contenor_type::iterator status pos. not find status is contenor_type::end()
	contenor_type::iterator find_status_code(	const contenor_type&, const int, int& );
	//! serch http header
	//! @param const contenor_type	buffer
	//! @param const int			length
	//! @param const string&		header name
	//! @param int&  int			header_length
	//! @return contenor_type::iterator http header pos. not find status is contenor_type::end()
	contenor_type::iterator find_http_header(	const contenor_type&, const int, const string&, int& );

public:
	//! constractor
	//! @param logger_func_type		logger_function object
	http_protocol_module_base( logger_func_type inlog ) : logger( inlog );
	//! destractor
	virtual	~http_protocol_module_base() = 0;
};

} // namespace l7vsd

#endif	//HTTP_PROTOCOL_MODULE_BASE_H
