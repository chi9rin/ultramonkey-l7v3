//
//	@file	parameter_imp;.h
//	@brief	ultramonke-l7 parameter impliment class
//
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PARAMETER_IMPL_H
#define PARAMETER_IMPL_H

#include "parameter_enum.h"
#include "error_code.h"
#include <string>
#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

namespace l7vs{


class ParameterImpl : private boost::noncopyable {
protected:
	typedef	std::map< std::string, int >			int_map_type;
	typedef	std::map< std::string, std::string >	string_map_type;

	//! parameter data of string
	string_map_type							stringMap;
	//! parameter data of int
	int_map_type							intMap;
	//! create mutex
	boost::mutex							create_mutex;
	//! read / write mutex
	boost::mutex							param_mutex;
	//! componenttag to section name map
	std::map< PARAMETER_COMPONENT_TAG, std::string >
											tag_section_table_map;
	//! constractor
	ParameterImpl(){}
	//! destractor
	~ParameterImpl(){}
public:
	//! instansgetter
	//! @return instance
	static ParameterImpl & get_instance(){
		static ParameterImpl instance;
		return instance;
	}
	//! initialize
	//! @return true success
	//! @return false failer
	bool	init();

	//! target component read
	//!	@param[in]	component tag
	//! @return false failer
	//! @return true success
	bool	read_file( const PARAMETER_COMPONENT_TAG );

	//! int value getter
	//! @param[in]	component tag
	//! @param[in]	keystring
	//! @param[out]	errorcode
	//! @return		int value
	int	get_int( const PARAMETER_COMPONENT_TAG, const std::string&, error_code& );

	//! string value getter
	//! @param[in]	component tag
	//! @param[in]	keystring
	//! @param[out]	errorcode
	//! @return		string value
	std::string	get_string( const PARAMETER_COMPONENT_TAG, const std::string&, error_code& );
};

}
#endif	//__PARAMETER_IMPL_H__
