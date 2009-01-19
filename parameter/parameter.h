//
//!	@file	parameter.h
//!	@brief	ultramonke-l7 parameter class
//
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include "parameter_enum.h"

namespace l7vs{


//!	@class	Parameter
//!	@brief	configure file controller
//! @brief	set key to get value.
class Parameter{
public:
	Parameter();							//!< default constractor
	~Parameter();							//!< default destractor

	//! reload file function
	//! @param[in] parameter conponent tag
	//! @return true success file read
	//!	@return false failer file read
	bool	rereadFile(const PARAMETER_COMPONENT_TAG);

	//! parameter int value getter
	//! @param[in]	parametercategory
	//!	@param[in]	parameter key
	//! @param[out]	error code
	//!	@return	intvalue
	int		get_int_value(const PARAMETER_COMPONENT_TAG, const std::string&, const parameter::error_code& );

	//! parameter string value getter
	//! @param[in]	parametercategory
	//!	@param[in]	parameter key
	//! @param[out] error code
	//!	@return	string value
	std::string	get_string_value(const PARAMETER_COMPONENT_TAG, const std::string&, const parameter::error_code& );
};

}	//namespace l7vs
	
#endif	//PARAMETER_H
