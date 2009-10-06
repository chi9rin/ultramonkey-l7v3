/*!
 * @file  paramter_impl.h
 * @brief ultramonke-l7 parameter impliment class.
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
	typedef	std::map< std::string, std::string >		string_map_type;
	typedef	std::multimap< std::string, std::string >	multistring_map_type;

	//! parameter data of string
	string_map_type							stringMap;
	//! parameter data of multi string
	multistring_map_type						multistringMap;
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

	//! target component read specified filename
	//!	@param[in]	component tag
	//!	@param[in]	file name
	//! @return false failer
	//! @return true success
	bool	read_specified_file( const PARAMETER_COMPONENT_TAG, const std::string& filename );

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

	//! multistring value getter
	//! @param[in]	component tag
	//! @param[in]	keystring
	//! @param[inout]	multistring map
	//! @param[out]	errorcode
	//! @return	last foundstring value
	std::string	get_multistring( const PARAMETER_COMPONENT_TAG, const std::string&, multistring_map_type&, error_code& );
};

}
#endif	//__PARAMETER_IMPL_H__
