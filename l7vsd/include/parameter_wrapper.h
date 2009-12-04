/*!
 * @file  parameter_wrapper.h
 * @brief ultramonke-l7 parameter function prototype.
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
#ifndef PARAMETER_WRAPPER
#define PARAMETER_WRAPPER

#include "parameter.h"
#include <cstring>

namespace l7vs{


//
//! get integer data.
//! @param[in]    section TAG
//! @param[in]    key string
//! @param[out]    errorflag
//! @return    value
inline int parameter_get_int_value(const PARAMETER_COMPONENT_TAG comp, const char* key, int& flag ){
    Parameter    param;
    paramter::error_code    err;
    int ret = param.getIntValue(comp, key, err);
    if( err )    flag = -1;
    else        flag = 0;
    return ret;
}

//
//! get charactor data
//!    @param[in]    section Tag
//! @param[in]    key string
//!    @param[out]    outputstring
//!    @param[in]    output buffer len
//!    @return    0    success
//! @return -1    not found key string
//! @return -2    buffer is short
inline int parameter_get_char_value(    const PARAMETER_COMPONENT_TAG    comp,
                                        const char* key,
                                        char*    targetstr,
                                        const size_t len ){
    Parameter    param;
    parameter::error_code    err;
    std::string    ret = param.get_string_value( comp, key, err );
    if( err ) return -1;
    if( ret.size() > len ) return -2;
    std::strncopy( targetstr, ret.c_str(), ret.size(), len );
    return 0;
}

}    //namespace l7vs

#endif    //PARAMETER_WRAPPER

