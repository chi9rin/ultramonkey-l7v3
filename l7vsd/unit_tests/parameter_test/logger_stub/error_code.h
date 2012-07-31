/*!
 *  @file   error_code.h
 *  @brief  error code and error messege impliment class
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

#ifndef ERROR_CODE_H
#define ERROR_CODE_H

#include <string>

namespace l7vs
{
//! @class  error_code
//! @brief  getValue error
//! @brief  this class is POD
class   error_code
{
protected:
        bool        flag;   //!<    errorcode flag
        std::string msg;    //!<    errorcode message
public:
        error_code() : flag(false) {}   //!< constractor
        //! setter constractor
        //! @param[in]  flags
        //! @param[in]  error message
        error_code(const bool inflg, const std::string &instr) {
                flag = inflg;
                msg = instr;
        }
        bool    operator==(const bool in)const {
                return (flag == in);        //!< operator== orverload
        }
        bool    operator!=(const bool in)const {
                return (flag != in);        //!< operator!= orverload
        }
        bool    operator!() const {
                return !flag;        //!< operator! orverload
        }
        typedef void (*unspecified_bool_type)();    //!< if return function
        static void unspecified_bool_true() {}      //!< if true orverload function
        operator unspecified_bool_type() const {
                return flag == 0 ? 0 : unspecified_bool_true;        //!< if() orverload
        }
        const std::string  &get_message() const {
                return msg;        //!< message getter
        }
        //! error setter
        //! @param[in]  flags
        //! @param[in]  error message
        void    setter(const bool flg, const std::string &instr) {
                flag = flg;
                msg = instr;
        }
};

}

#endif // ERROR_CODE_H

