/*
 *    @file    http_utility.h
 *    @brief    shared object http common utility class
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
#ifndef    HTTP_UTILITY_H
#define    HTTP_UTILITY_H

namespace l7vs
{

class http_utility
{
public:
        //! constructor
        http_utility() {};

        //! destructor
        virtual    ~http_utility() {};

        //! @enum    CHECK_RESULT_TAG
        //! @brief    check tag is return to http protocol module.
        enum    CHECK_RESULT_TAG {
                CHECK_OK = 0,            //!< check ok
                CHECK_NG,                //!< check NG
                CHECK_IMPOSSIBLE        //!< check impossible
        };

        //! check http method and version function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @return CHECK_RESULT_TAG    http method and version is valid
        static CHECK_RESULT_TAG    check_http_method_and_version(const char *, const size_t);

        //! check http version and status code function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @return CHECK_RESULT_TAG    http version and status code is valid
        static CHECK_RESULT_TAG    check_http_version_and_status_code(const char *, const size_t);

        //! search uri function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param size_t&                uri offset
        //! @param size_t&                uri length
        //! @return bool                find is true. not find is false
        static bool    find_uri(const char *, const size_t, size_t &, size_t &);

        //! search status function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param size_t&                status offset
        //! @param size_t&                status length
        //! @return bool                find is true. not find is false
        static bool    find_status_code(const char *, const size_t, size_t &, size_t &);

        //! search http header function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param const string&        header name
        //! @param size_t&                header offset
        //! @param size_t&                header length
        //! @return bool                find is true. not find is false
        static bool    find_http_header(const char *, const size_t, const std::string &, size_t &, size_t &);

        //! search http header Cookie function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param size_t&                header offset
        //! @param size_t&                header length
        //! @return bool                find is true. not find is false
        static bool    find_http_header_cookie(const char *, const size_t, size_t &, size_t &);

        //! search http header Content_Length function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param size_t&                header offset
        //! @param size_t&                header length
        //! @return bool                find is true. not find is false
        static bool    find_http_header_content_length(const char *, const size_t, size_t &, size_t &);

        //! search http header X_Forwarded_For function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param size_t&                header offset
        //! @param size_t&                header length
        //! @return bool                find is true. not find is false
        static bool    find_http_header_x_forwarded_for(const char *, const size_t, size_t &, size_t &);

        //! search http header all function
        //! @param const char*            buffer
        //! @param const size_t            buffer_len
        //! @param size_t&                header offset
        //! @param size_t&                header length
        //! @return bool                find is true. not find is false
        static bool    find_http_header_all(const char *, const size_t, size_t &, size_t &);

        //! check http get method
        //! @param  const char*            buffer
        //! @return bool                   get method is true. other is false
        static bool    is_get_request(const char *buffer);

        //! check http post method
        //! @param  const char*            buffer
        //! @return bool                   post method is true. other is false
        static bool    is_post_request(const char *buffer);
};

} // namespace l7vsd

#endif    //HTTP_UTILITY_H
