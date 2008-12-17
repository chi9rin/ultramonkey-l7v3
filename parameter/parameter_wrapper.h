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

namespace l7vs{

/*!
 * check whether integer data exists.
 * @param[in]	comp	section TAG
 * @param[in]	*key	key string
 * @return	true = exist setting value / false = non exist setting value
 */
inline bool parameter_is_int_exist(const PARAMETER_COMPONENT_TAG comp, const char* key){
	return Parameter::getInstance().isIntExist(comp, key));
}

/*!
 * check whether integer data exists.
 * @param[in]	comp	section TAG
 * @param[in]	key		key string
 * @return	true = exist setting value / false = non exist setting value*/
inline bool parameter_is_int_exist( const PARAMETER_COMPONENT_TAG comp, const std::string key ){
	return Parameter::getInstance().isIntExist(comp,key));
*/

/*!
 * check whether character data exists.
 * @param[in]	comp	section TAG
 * @param[in]	*key	key string
 * @return	true = exist setting value / false = non exist setting value
 */
inline bool parameter_is_char_exist(const PARAMETER_COMPONENT_TAG comp, const char* key){
	return Parameter::getInstance().isStringExist(comp, key));
}

/*!
 * get integer data.
 * @param[in]	comp	section TAG
 * @param[in]	*key	key string
 * @return	value
 */
inline int parameter_get_int_value(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	return l7vs::Parameter::getInstance().getIntValue(comp, key);
}

/*!
 * get character data.
 * @param[in]	comp	section TAG
 * @param[in]	*key	key string
 * @return	value
 */
inline const char* parameter_get_char_value(const PARAMETER_COMPONENT_TAG comp, const char* key)
{
	return (l7vs::Parameter::getInstance().getStringValue(comp, key)).c_str();
}

/*!
 * reload config file
 * @param[in]	comp	section TAG
 * @return	0 = success read file / -1 = failure read file
 */
inline int parameter_reread_file(const PARAMETER_COMPONENT_TAG comp)
{
	if (l7vs::Parameter::getInstance().rereadFile(comp)) {
		return 0;
	}
	return -1;
}

};	//namespace l7vs

#endif	//PARAMETER_WRAPPER

