/*
 *    @file    http_protocol_module_base.cpp
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

#include "http_protocol_module_base.h"
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

//! check http method function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return CHECK_RESULT_TAG    http method is valid
l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_method(const char *buffer,
                const size_t buffer_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [check_http_method] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(0,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        l7vs::http_protocol_module_base::CHECK_RESULT_TAG    check_result = CHECK_OK;

        size_t    line_length        = 0;

        if (likely(buffer != NULL)) {

                for (line_length = 0; line_length < buffer_len; line_length++) {
                        if (unlikely(buffer[line_length] == '\r' || buffer[line_length] == '\n'))
                                break;
                }

                if (likely(line_length < buffer_len)) {
                        char    *target = const_cast<char *>(buffer);
                        char    backup_c = target[line_length];
                        target[line_length] = '\0';
                        (regex_match(target, method_regex))
                        ? check_result = CHECK_OK
                                         : check_result = CHECK_NG;
                        target[line_length] = backup_c;
                } else {
                        check_result = CHECK_IMPOSSIBLE;
                }
        } else {

                check_result = CHECK_NG;

        }

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [check_http_method] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(1,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;

}

//! check http version function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return    CHECK_RESULT_TAG     http version 1.0 or 1.1
l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_version(const char *buffer,
                const size_t buffer_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [check_http_version] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(2,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        l7vs::http_protocol_module_base::CHECK_RESULT_TAG    check_result = CHECK_OK;

        size_t    line_length        = 0;

        if (likely(buffer != NULL)) {
                for (line_length = 0; line_length < buffer_len; line_length++) {
                        if (unlikely(buffer[line_length] == '\r' || buffer[line_length] == '\n'))
                                break;
                }

                if (likely(line_length < buffer_len)) {
                        char    *target = const_cast<char *>(buffer);
                        char    backup_c = target[line_length];
                        target[line_length] = '\0';

                        if (!regex_match(target, version_regex_request) &&
                            !regex_match(target, version_regex_response)) check_result = CHECK_NG;

                        target[line_length] = backup_c;

                } else {
                        check_result = CHECK_IMPOSSIBLE;
                }
        } else {
                check_result = CHECK_NG;
        }

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [check_http_version] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(3,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;

}

//! check http status code function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return    CHECK_RESULT_TAG    status code is nomal or error
l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_status_code(const char *buffer,
                const size_t buffer_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [check_status_code] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(4,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        l7vs::http_protocol_module_base::CHECK_RESULT_TAG    check_result = CHECK_OK;

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
                        if (!regex_match(target, status_code_regex_check)) check_result = CHECK_NG;
                        target[line_length] = backup_c;
                } else {
                        check_result = CHECK_IMPOSSIBLE;
                }
        } else {
                check_result = CHECK_NG;
        }

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [check_status_code] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(5,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;

}

//! check http method and version function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return CHECK_RESULT_TAG    http method and version is valid
l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_method_and_version(
        const char *buffer,
        const size_t buffer_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [check_http_method_and_version] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(6,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        l7vs::http_protocol_module_base::CHECK_RESULT_TAG    check_result = CHECK_OK;

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [check_http_method_and_version] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(7,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;

}

//! check http version and status code function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @return CHECK_RESULT_TAG    http version and status code is valid
l7vs::http_protocol_module_base::CHECK_RESULT_TAG
l7vs::http_protocol_module_base::check_http_version_and_status_code(
        const char *buffer,
        const size_t buffer_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [check_http_version_and_status_code] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(8,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        l7vs::http_protocol_module_base::CHECK_RESULT_TAG    check_result = CHECK_OK;

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [check_http_version_and_status_code] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(9,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;

}

//! serch uri function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                uri offset
//! @param size_t&                uri length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_uri(const char *buffer,
                const size_t buffer_len,
                size_t &uri_offset,
                size_t &uri_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_uri] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(10,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_uri] : "
                                         "find_result = [%d], "
                                         "uri_offset = [%d], "
                                         "uri_len = [%d]");

                outform % find_result % uri_offset % uri_len;

                putLogDebug(11,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! serch status function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                status offset
//! @param size_t&                status length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_status_code(const char *buffer,
                const size_t buffer_len,
                size_t &status_code_offset,
                size_t &status_code_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_status_code] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(12,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_status_code] : "
                                         "find_result = [%d], "
                                         "status_code_offset = [%d], "
                                         "status_code_len = [%d]");

                outform % find_result % status_code_offset % status_code_len;

                putLogDebug(13,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! serch http header function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param const string&        header name
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_http_header(const char *buffer,
                const size_t buffer_len,
                const std::string &http_header_name,
                size_t &http_header_offset,
                size_t &http_header_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_http_header] : "
                                         "buffer_len = [%d], "
                                         "http_header_name = [%s]");

                outform % buffer_len % http_header_name;

                putLogDebug(14,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_http_header] : "
                                         "find_result = [%d], "
                                         "http_header_offset = [%d], "
                                         "http_header_len = [%d]");

                outform % find_result % http_header_offset % http_header_len;

                putLogDebug(15,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! serch http header Cookie function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_http_header_cookie(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_http_header_cookie] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(16,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_http_header_cookie] : "
                                         "find_result = [%d], "
                                         "http_header_offset = [%d], "
                                         "http_header_len = [%d]");

                outform % find_result % http_header_offset % http_header_len;

                putLogDebug(17,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! serch http header Content_Length function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_http_header_content_length(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_http_header_content_length] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(18,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_http_header_content_length] : "
                                         "find_result = [%d], "
                                         "http_header_offset = [%d], "
                                         "http_header_len = [%d]");

                outform % find_result % http_header_offset % http_header_len;

                putLogDebug(19,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! serch http header X_Forwarded_For function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_http_header_x_forwarded_for(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_http_header_x_forwarded_for] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(20,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_http_header_x_forwarded_for] : "
                                         "find_result = [%d], "
                                         "http_header_offset = [%d], "
                                         "http_header_len = [%d]");

                outform % find_result % http_header_offset % http_header_len;

                putLogDebug(21,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! serch http header all function
//! @param const char*            buffer
//! @param const size_t            buffer_len
//! @param size_t&                header offset
//! @param size_t&                header length
//! @return bool                find is true. not find is false
bool    l7vs::http_protocol_module_base::find_http_header_all(
        const char *buffer,
        const size_t buffer_len,
        size_t &http_header_offset,
        size_t &http_header_len)
{

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in  : [find_http_header_all] : "
                                         "buffer_len = [%d]");

                outform % buffer_len;

                putLogDebug(22,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

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

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [find_http_header_all] : "
                                         "find_result = [%d], "
                                         "http_header_offset = [%d], "
                                         "http_header_len = [%d]");

                outform % find_result % http_header_offset % http_header_len;

                putLogDebug(23,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return find_result;

}

//! check http get method
//! @param  const char*            buffer
//! @return bool                   get method is true. other is false
bool    l7vs::http_protocol_module_base::is_get_request(const char *buffer)
{
        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                putLogDebug(24,
                            "function in  : [is_get_request].",
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        bool    check_result            = false;

        if (likely(buffer != NULL)) {
                if ( strncmp("GET", buffer, 3) == 0 )
                {
                        check_result = true;
                }
        }

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [is_get_request] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(25,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;
}

//! check http post method
//! @param  const char*            buffer
//! @return bool                   post method is true. other is false
bool    l7vs::http_protocol_module_base::is_post_request(const char *buffer)
{
        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                putLogDebug(26,
                            "function in  : [is_post_request].",
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        bool    check_result            = false;

        if (likely(buffer != NULL)) {
                if ( strncmp("POST", buffer, 4) == 0 )
                {
                        check_result = true;
                }
        }

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [is_post_request] : "
                                         "check_result = [%d]");

                outform % check_result;

                putLogDebug(27,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        return check_result;
}

//! increment http statistics
//! @param  const char*            buffer
void    l7vs::http_protocol_module_base::increment_stats(const char *buffer)
{
        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function in : [increment_stats] : "
                             "http_get_requests = [%d], "
                             "http_post_requests = [%d], "
                             "http_requests = [%d]");

                outform % http_stats_info.http_get_requests.get() %
                          http_stats_info.http_post_requests.get() %
                          http_stats_info.http_requests.get();
                putLogDebug(28,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------

        if (buffer != NULL && statistic == true) {
            if (is_get_request(buffer)) {
                    http_stats_info.http_get_requests++;
            }
            else if (is_post_request(buffer)) {
                    http_stats_info.http_post_requests++;
            }

            http_stats_info.http_requests++;
        }

        //---------- DEBUG LOG START ------------------------------
        if (unlikely(LOG_LV_DEBUG == getloglevel())) {
                boost::format    outform("function out : [increment_stats] : "
                                         "http_get_requests = [%d], "
                                         "http_post_requests = [%d], "
                                         "http_requests = [%d]");

                outform % http_stats_info.http_get_requests.get() %
                          http_stats_info.http_post_requests.get() %
                          http_stats_info.http_requests.get();

                putLogDebug(29,
                            outform.str(),
                            __FILE__,
                            __LINE__);
        }
        //---------- DEBUG LOG END ------------------------------
}
