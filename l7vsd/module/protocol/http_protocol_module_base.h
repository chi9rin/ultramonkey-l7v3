/*
 *    @file    http_protocol_module_base.h
 *    @brief    shared object http protocol module abstract class
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
#ifndef    HTTP_PROTOCOL_MODULE_BASE_H
#define    HTTP_PROTOCOL_MODULE_BASE_H

#include "protocol_module_base.h"

namespace l7vs{

class http_protocol_module_base : public protocol_module_base {
protected:
    //! @enum    CHECK_RESULT_TAG
    //! @brief    check tag is return to http protocol module.
    enum    CHECK_RESULT_TAG{
        CHECK_OK = 0,            //!< check ok
        CHECK_NG,                //!< check ng
        CHECK_IMPOSSIBLE        //!< check impossible
    };

    //! check http method function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @return CHECK_RESULT_TAG    http method is valid
    CHECK_RESULT_TAG    check_http_method( const char*, const size_t );

    //! check http version function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @return    CHECK_RESULT_TAG     http version 1.0 or 1.1
    CHECK_RESULT_TAG    check_http_version( const char*, const size_t );

    //! check http status code function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @return    CHECK_RESULT_TAG    status code is nomal or error
    CHECK_RESULT_TAG    check_status_code( const char*, const size_t );

    //! check http method and version function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @return CHECK_RESULT_TAG    http method and version is valid
    CHECK_RESULT_TAG    check_http_method_and_version( const char*, const size_t );

    //! check http version and status code function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @return CHECK_RESULT_TAG    http version and status code is valid
    CHECK_RESULT_TAG    check_http_version_and_status_code( const char*, const size_t );

    //! serch uri function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param size_t&                uri offset
    //! @param size_t&                uri length
    //! @return bool                find is true. not find is false
    bool    find_uri( const char*, const size_t, size_t&, size_t&);

    //! serch status function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param size_t&                status offset
    //! @param size_t&                status length
    //! @return bool                find is true. not find is false
    bool    find_status_code( const char*, const size_t, size_t&, size_t& );

    //! serch http header function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param const string&        header name
    //! @param size_t&                header offset
    //! @param size_t&                header length
    //! @return bool                find is true. not find is false
    bool    find_http_header( const char*, const size_t, const std::string&, size_t&, size_t& );

    //! serch http header Cookie function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param size_t&                header offset
    //! @param size_t&                header length
    //! @return bool                find is true. not find is false
    bool    find_http_header_cookie( const char*, const size_t, size_t&, size_t& );

    //! serch http header Content_Length function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param size_t&                header offset
    //! @param size_t&                header length
    //! @return bool                find is true. not find is false
    bool    find_http_header_content_length( const char*, const size_t, size_t&, size_t& );

    //! serch http header X_Forwarded_For function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param size_t&                header offset
    //! @param size_t&                header length
    //! @return bool                find is true. not find is false
    bool    find_http_header_x_forwarded_for( const char*, const size_t, size_t&, size_t& );

    //! serch http header all function
    //! @param const char*            buffer
    //! @param const size_t            buffer_len
    //! @param size_t&                header offset
    //! @param size_t&                header length
    //! @return bool                find is true. not find is false
    bool    find_http_header_all( const char*, const size_t, size_t&, size_t& );

public:

    //! constractor
    http_protocol_module_base( std::string in_modulename ) : protocol_module_base( in_modulename ){};

    //! destractor
    virtual    ~http_protocol_module_base(){};
};

} // namespace l7vsd

#endif    //HTTP_PROTOCOL_MODULE_BASE_H
