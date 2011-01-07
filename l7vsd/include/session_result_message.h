/*!
 *    @file    session_result_message.h
 *    @brief    session result message class
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

#ifndef SESSION_RESULT_MESSAGE_H
#define SESSION_RESULT_MESSAGE_H

#include <string>

namespace l7vs
{
//!    @struct    session_result_message
//! @brief    this struct is session class use result message.
struct session_result_message {
        //! result flag
        bool flag;
        //! message
        std::string message;
        //! == operator
        //! @param[in]    equal check object.
        //! @return     true is equal
        //! @return     false is not equal
        bool operator==(const session_result_message &in) {
                return ((flag == in.flag) && (message == in.message));
        }
        //! != operator
        //! @param[in]    inequal check object.
        //! @return     true is inequal
        //! @return     false is not inequal
        bool operator!=(const session_result_message &in) {
                return ((flag != in.flag) || (message != in.message));
        }

};// struct session_result_message
}// namespace l7vs

#endif//SESSION_RESULT_MESSAGE_H


