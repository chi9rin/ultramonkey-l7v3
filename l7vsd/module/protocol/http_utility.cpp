/*
 *    @file    http_utility.cpp
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
#include <boost/xpressive/xpressive.hpp>

#include "http_utility.h"
#include "utility.h"

using namespace boost::xpressive;

cregex    method_regex
= (as_xpr("GET")        | as_xpr("HEAD")        | as_xpr("POST")        |
   as_xpr("PUT")        | as_xpr("PROPFIND")    | as_xpr("PROPPATCH")    |
   as_xpr("OPTIONS")    | as_xpr("CONNECT")        | as_xpr("COPY")        |
   as_xpr("TRACE")        | as_xpr("DELETE")        | as_xpr("LOCK")        |
   as_xpr("UNLOCK")    | as_xpr("MOVE")        | as_xpr("MKCOL")) >> _s >>
  +~_s >> _s >>
  "HTTP/" >> _d >> "." >> _d;

cregex    version_regex_request
=    +alpha >> _s >>
     +~_s >> _s >>
     "HTTP/" >> (as_xpr("1.0") | as_xpr("1.1"));

cregex    version_regex_response
=    "HTTP/" >> (as_xpr("1.0") | as_xpr("1.1")) >> _s >>
     repeat<3>(_d) >> _s >>
     *_;

cregex    status_code_regex_check
=    "HTTP/" >> _d >> "." >> _d >> _s >>
     range('1', '3') >> repeat<2>(_d) >> _s >>
     *_;

cregex    method_and_version_regex
= (as_xpr("GET")        | as_xpr("HEAD")        | as_xpr("POST")        |
   as_xpr("PUT")        | as_xpr("PROPFIND")    | as_xpr("PROPPATCH")    |
   as_xpr("OPTIONS")    | as_xpr("CONNECT")        | as_xpr("COPY")        |
   as_xpr("TRACE")        | as_xpr("DELETE")        | as_xpr("LOCK")        |
   as_xpr("UNLOCK")    | as_xpr("MOVE")        | as_xpr("MKCOL")) >> _s >>
  +~_s >> _s >>
  "HTTP/" >> (as_xpr("1.0") | as_xpr("1.1"));

cregex    version_and_status_code_regex
=    "HTTP/" >> (as_xpr("1.0") | as_xpr("1.1")) >> _s >>
     range('1', '3') >> repeat<2>(_d) >> _s >>
     *_;

cregex    uri_regex
=    +alpha >> _s >>
     (s1 = *~_s) >> _s >>
     "HTTP/" >> _d >> "." >> _d;

cregex    status_code_regex_find
=    "HTTP/" >> _d >> "." >> _d >> _s >>
     (s1 = repeat<3>(_d)) >> _s >>
     *_;

cregex    http_header_regex_cookie
= _ln >> (s1 = icase("cookie") >> ":" >> *~_ln);

cregex    http_header_regex_content_length
= _ln >> (s1 = icase("content-length") >> ":" >> *~_ln);

cregex    http_header_regex_x_forwarded_for
= _ln >> (s1 = icase("x-forwarded-for") >> ":" >> *~_ln);

cregex    http_header_regex_all
= _ln >> (s1 = *_ >> ~_ln) >> repeat<2>(_ln);

cregex    http_header_regex_none
= _ln >> (s1 = _ln);


//! check http method and version function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return CHECK_RESULT_TAG    http method and version is valid
l7vs::http_utility::CHECK_RESULT_TAG
l7vs::http_utility::check_http_method_and_version(
        const char *buffer,
        const size_t buffer_len)
{

        l7vs::http_utility::CHECK_RESULT_TAG    check_result = CHECK_OK;

        size_t    line_length        = 0;

        if (likely(buffer != NULL)) {
                for (line_length = 0; line_length < buffer_len; line_length++) {
                        if (unlikely(buffer[line_length] == '\r' || buffer[line_length] == '\n')) {
                                break;
                        }
                }
                if (likely(line_length < buffer_len)) {
                        char    *target = const_cast<char *>(buffer);
                        char    backup_c = target[line_length];
                        target[line_length] = '\0';

                        if (!regex_match(target, method_and_version_regex))
                                check_result = CHECK_NG;

                        target[line_length] = backup_c;
                } else {
                        check_result = CHECK_NG;
                }
        } else {
                check_result = CHECK_NG;
        }

        return check_result;

}

//! check http version and status code function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return CHECK_RESULT_TAG    http version and status code is valid
l7vs::http_utility::CHECK_RESULT_TAG
l7vs::http_utility::check_http_version_and_status_code(
        const char *buffer,
        const size_t buffer_len)
{

        l7vs::http_utility::CHECK_RESULT_TAG    check_result = CHECK_OK;

        size_t    line_length        = 0;

        if (likely(buffer != NULL)) {
                for (line_length = 0; line_length < buffer_len; line_length++) {
                        if (unlikely(buffer[line_length] == '\r' || buffer[line_length] == '\n')) {
                                break;
                        }
                }

                if (likely(line_length < buffer_len)) {
                        char    *target = const_cast<char *>(buffer);
                        char    backup_c = target[line_length];
                        target[line_length] = '\0';
                        if (!regex_match(target, version_and_status_code_regex)) check_result = CHECK_NG;
                        target[line_length] = backup_c;
                } else {
                        check_result = CHECK_IMPOSSIBLE;
                }
        } else {
                check_result = CHECK_NG;
        }

        return check_result;

}

//! serch uri function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                uri offset
//! @param size_t&                uri length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_uri(const char *buffer,
                                     const size_t buffer_len,
                                     size_t &uri_offset,
                                     size_t &uri_len)
{

        bool    find_result    = true;

        size_t    line_length    = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {

                for (line_length = 0; line_length < buffer_len; line_length++) {

                        if (unlikely(buffer[line_length] == '\r' || buffer[line_length] == '\n')) {

                                break;

                        }

                }

                if (likely(line_length < buffer_len)) {
                        char    *target = const_cast<char *>(buffer);
                        char    backup_c = target[line_length];
                        find_result = regex_search(target, result, uri_regex);
                        if (find_result) {
                                uri_offset    = result.position(1);
                                uri_len        = result.length(1);
                        }
                        target[line_length] = backup_c;
                } else {
                        find_result = false;
                }
        } else {
                find_result = false;
        }

        return find_result;

}

//! serch status function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                status offset
//! @param size_t&                status length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_status_code(const char *buffer,
                const size_t buffer_len,
                size_t &status_code_offset,
                size_t &status_code_len)
{

        bool    find_result    = true;

        size_t    line_length    = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {

                for (line_length = 0; line_length < buffer_len; line_length++) {
                        if (unlikely(buffer[line_length] == '\r' || buffer[line_length] == '\n')) {
                                break;
                        }
                }

                if (likely(line_length < buffer_len)) {
                        char    *target = const_cast<char *>(buffer);
                        char    backup_c    = target[line_length];
                        target[line_length] = '\0';
                        find_result = regex_search(target, result, status_code_regex_find);
                        if (find_result) {
                                status_code_offset    = result.position(1);
                                status_code_len        = result.length(1);
                        }
                        target[line_length] = backup_c;
                } else {
                        find_result = false;
                }
        } else {
                find_result = false;
        }

        return find_result;

}

//! serch http header function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param const string&        header name
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_http_header(const char *buffer,
                const size_t buffer_len,
                const std::string &http_header_name,
                size_t &http_header_offset,
                size_t &http_header_len)
{

        cregex    http_header_regex;

        bool    find_result            = true;

        size_t    count                = 0;
        size_t    header_begin        = 0;
        size_t    header_end            = 0;
        size_t    header_length        = 0;

        int        header_begin_flag    = 0;
        int        header_end_flag        = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {

                for (count = 0; count < buffer_len; count++) {

                        if (unlikely(buffer[count] == '\r' || buffer[count] == '\n')) {

                                if (unlikely(header_begin_flag == 0)) {

                                        header_begin = count;
                                        header_begin_flag = 1;

                                }

                                if (likely(count > 0)) {

                                        if (unlikely((buffer[count-1] == '\r' && buffer[count] == '\r') ||
                                                     (buffer[count-1] == '\n' && buffer[count] == '\n'))) {

                                                header_end = count;
                                                header_end_flag = 1;
                                                break;

                                        }
                                }

                                if (likely(count > 2)) {

                                        if (unlikely(buffer[count-3] == '\r' && buffer[count-2] == '\n' &&
                                                     buffer[count-1] == '\r' && buffer[count] == '\n')) {

                                                header_end = count;
                                                header_end_flag = 1;
                                                break;

                                        }
                                }
                        }
                }

                if (likely(header_begin_flag == 1 && header_end_flag == 1)) {

                        header_length = header_end - header_begin + 1;
                        char    *ptr = const_cast<char *>(buffer) + header_begin;
                        char    backup_c = *(ptr + header_length);
                        *(ptr + header_length) = '\0';

                        if (http_header_name.length() > 0) {
                                http_header_regex = _ln >> (s1 = icase(http_header_name) >> ":" >> *~_ln);
                                find_result = regex_search(ptr , result, http_header_regex);
                                if (find_result) {
                                        http_header_offset    = result.position(1) + header_begin;
                                        http_header_len        = result.length(1);
                                }
                        } else {
                                http_header_regex = _ln >> (s1 = *_ >> ~_ln) >> repeat<2>(_ln);
                                find_result = regex_search(ptr, result, http_header_regex);
                                if (find_result) {
                                        http_header_offset    = result.position(1) + header_begin;
                                        http_header_len        = result.length(1);
                                } else {
                                        http_header_regex = _ln >> (s1 = _ln);
                                        find_result = regex_search(ptr, result, http_header_regex);
                                        if (find_result) {
                                                http_header_offset    = result.position(1) + header_begin;
                                                http_header_len        = 0;
                                        }
                                }
                        }
                        *(ptr + header_length) = backup_c;
                } else {
                        find_result    = false;
                }
        } else {
                find_result    = false;
        }

        return find_result;

}

//! serch http header Cookie function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_http_header_cookie(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        bool    find_result            = true;

        size_t    count                = 0;
        size_t    header_begin        = 0;
        size_t    header_end            = 0;
        size_t    header_length        = 0;

        int        header_begin_flag    = 0;
        int        header_end_flag        = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {
                for (count = 0; count < buffer_len; count++) {
                        if (unlikely(buffer[count] == '\r' || buffer[count] == '\n')) {
                                if (unlikely(header_begin_flag == 0)) {
                                        header_begin = count;
                                        header_begin_flag = 1;
                                }
                                if (likely(count > 0)) {
                                        if (unlikely((buffer[count-1] == '\r' && buffer[count] == '\r') ||
                                                     (buffer[count-1] == '\n' && buffer[count] == '\n'))) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;
                                        }
                                }
                                if (likely(count > 2)) {
                                        if (unlikely(buffer[count-3] == '\r' && buffer[count-2] == '\n' &&
                                                     buffer[count-1] == '\r' && buffer[count] == '\n')) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;

                                        }
                                }
                        }
                }

                if (likely(header_begin_flag == 1 && header_end_flag == 1)) {
                        header_length = header_end - header_begin + 1;
                        char    *ptr = const_cast<char *>(buffer) + header_begin;
                        char    backup_c = *(ptr + header_length);
                        *(ptr + header_length) = '\0';

                        find_result = regex_search(ptr, result, http_header_regex_cookie);
                        if (find_result) {
                                http_header_offset    = result.position(1) + header_begin;
                                http_header_len        = result.length(1);
                        }
                        *(ptr + header_length) = backup_c;
                } else {
                        find_result    = false;
                }
        } else {
                find_result    = false;
        }

        return find_result;

}

//! serch http header Content_Length function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_http_header_content_length(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        bool    find_result            = true;

        size_t    count                = 0;
        size_t    header_begin        = 0;
        size_t    header_end            = 0;
        size_t    header_length        = 0;

        int        header_begin_flag    = 0;
        int        header_end_flag        = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {
                for (count = 0; count < buffer_len; count++) {
                        if (unlikely(buffer[count] == '\r' || buffer[count] == '\n')) {
                                if (unlikely(header_begin_flag == 0)) {
                                        header_begin = count;
                                        header_begin_flag = 1;
                                }
                                if (likely(count > 0)) {
                                        if (unlikely((buffer[count-1] == '\r' && buffer[count] == '\r') ||
                                                     (buffer[count-1] == '\n' && buffer[count] == '\n'))) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;
                                        }
                                }
                                if (likely(count > 2)) {
                                        if (unlikely(buffer[count-3] == '\r' && buffer[count-2] == '\n' &&
                                                     buffer[count-1] == '\r' && buffer[count] == '\n')) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;
                                        }
                                }
                        }
                }

                if (likely(header_begin_flag == 1 && header_end_flag == 1)) {
                        header_length = header_end - header_begin + 1;

                        char    *ptr = const_cast<char *>(buffer) + header_begin;
                        char    backup_c = *(ptr + header_length);
                        *(ptr + header_length) = '\0';

                        find_result = regex_search(ptr, result, http_header_regex_content_length);
                        if (find_result) {
                                http_header_offset    = result.position(1) + header_begin;
                                http_header_len        = result.length(1);
                        }

                        *(ptr + header_length) = backup_c;
                } else {

                        find_result    = false;

                }
        } else {

                find_result    = false;

        }

        return find_result;

}

//! serch http header X_Forwarded_For function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_http_header_x_forwarded_for(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        bool    find_result            = true;

        size_t    count                = 0;
        size_t    header_begin        = 0;
        size_t    header_end            = 0;
        size_t    header_length        = 0;

        int        header_begin_flag    = 0;
        int        header_end_flag        = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {
                for (count = 0; count < buffer_len; count++) {
                        if (unlikely(buffer[count] == '\r' || buffer[count] == '\n')) {
                                if (unlikely(header_begin_flag == 0)) {
                                        header_begin = count;
                                        header_begin_flag = 1;
                                }
                                if (likely(count > 0)) {
                                        if (unlikely((buffer[count-1] == '\r' && buffer[count] == '\r') ||
                                                     (buffer[count-1] == '\n' && buffer[count] == '\n'))) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;
                                        }
                                }

                                if (likely(count > 2)) {
                                        if (unlikely(buffer[count-3] == '\r' && buffer[count-2] == '\n' &&
                                                     buffer[count-1] == '\r' && buffer[count] == '\n')) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;

                                        }
                                }
                        }
                }

                if (likely(header_begin_flag == 1 && header_end_flag == 1)) {

                        header_length = header_end - header_begin + 1;
                        char    *ptr = const_cast<char *>(buffer) + header_begin;
                        char    backup_c = *(ptr + header_length);
                        *(ptr + header_length) = '\0';

                        find_result = regex_search(ptr, result, http_header_regex_x_forwarded_for);
                        if (find_result == true) {
                                http_header_offset    = result.position(1) + header_begin;
                                http_header_len        = result.length(1);
                        }

                        *(ptr + header_length) = backup_c;
                } else {

                        find_result    = false;

                }
        } else {

                find_result    = false;

        }

        return find_result;

}

//! serch http header all function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_utility::find_http_header_all(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        bool    find_result            = true;

        size_t    count                = 0;
        size_t    header_begin        = 0;
        size_t    header_end            = 0;
        size_t    header_length        = 0;

        int        header_begin_flag    = 0;
        int        header_end_flag        = 0;

        match_results< const char * >    result;

        if (likely(buffer != NULL)) {
                for (count = 0; count < buffer_len; count++) {
                        if (unlikely(buffer[count] == '\r' || buffer[count] == '\n')) {
                                if (unlikely(header_begin_flag == 0)) {
                                        header_begin = count;
                                        header_begin_flag = 1;
                                }

                                if (likely(count > 0)) {
                                        if (unlikely((buffer[count-1] == '\r' && buffer[count] == '\r') ||
                                                     (buffer[count-1] == '\n' && buffer[count] == '\n'))) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;
                                        }
                                }

                                if (likely(count > 2)) {
                                        if (unlikely(buffer[count-3] == '\r' && buffer[count-2] == '\n' &&
                                                     buffer[count-1] == '\r' && buffer[count] == '\n')) {
                                                header_end = count;
                                                header_end_flag = 1;
                                                break;
                                        }
                                }
                        }
                }

                if (likely(header_begin_flag == 1 && header_end_flag == 1)) {
                        header_length = header_end - header_begin + 1;
                        char    *ptr = const_cast<char *>(buffer) + header_begin;
                        char    backup_c = *(ptr + header_length);
                        *(ptr + header_length) = '\0';

                        find_result = regex_search(ptr, result, http_header_regex_all);

                        if (find_result) {
                                http_header_offset    = result.position(1) + header_begin;
                                http_header_len        = result.length(1);
                        } else {
                                find_result = regex_search(ptr, result, http_header_regex_none);
                                if (find_result == true) {
                                        http_header_offset    = result.position(1) + header_begin;
                                        http_header_len        = 0;
                                }
                        }
                        *(ptr + header_length) = backup_c;
                } else {

                        find_result    = false;

                }
        } else {

                find_result    = false;

        }

        return find_result;

}
