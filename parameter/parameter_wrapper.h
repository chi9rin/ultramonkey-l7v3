//
//	@file	parameter_wrapper.h
//	@brief	ultramonke-l7 parameter function prototype
//
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef PARAMETER_WRAPPER
#define PARAMETER_WRAPPER

#include "parameter.h"
#include <cstring>

namespace l7vs{


//
//! get integer data.
//! @param[in]	section TAG
//! @param[in]	key string
//! @param[out]	errorflag
//! @return	value
inline int parameter_get_int_value(const PARAMETER_COMPONENT_TAG comp, const char* key, int& flag ){
	Parameter	param;
	paramter::error_code	err;
	int ret = param.getIntValue(comp, key, err);
	if( err )	flag = -1;
	else		flag = 0;
	return ret;
}

//
//! get charactor data
//!	@param[in]	section Tag
//! @param[in]	key string
//!	@param[out]	outputstring
//!	@param[in]	output buffer len
//!	@return	0	success
//! @return -1	not found key string
//! @return -2	buffer is short
inline int parameter_get_char_value(	const PARAMETER_COMPONENT_TAG	comp,
										const char* key,
										char*	targetstr,
										const size_t len ){
	Parameter	param;
	parameter::error_code	err;
	std::string	ret = param.get_string_value( comp, key, err );
	if( err ) return -1;
	if( ret.size() > len ) return -2;
	std::strncopy( targetstr, ret.c_str(), ret.size(), len );
	return 0;
}

}	//namespace l7vs

#endif	//PARAMETER_WRAPPER

