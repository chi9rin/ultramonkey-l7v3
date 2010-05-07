/*!
 * @file  parameter.h
 * @brief ultramonke-l7 parameter class.
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
#ifndef PARAMETER_H
#define PARAMETER_H

#include <string>
#include <map>
#include <vector>
#include "parameter_enum.h"
#include "error_code.h"

#ifndef PARAMETER_FILE
#define PARAMETER_FILE "/etc/l7vs/l7vs.cf"
#endif //PARAMETER_FILE

namespace l7vs
{


//! @class Parameter
//! @brief configure file controller
//! @brief set key to get value.
class Parameter
{
public:
        Parameter();                            //!< default constractor
        ~Parameter();                           //!< default destractor

        //! initialize(file designation)
        //! @param[in]    component tag
        //! @param[in]    file name
        //! @return true success
        //! @return false failer
        bool init(const PARAMETER_COMPONENT_TAG, const std::string &);

        //! reload file function
        //! @param[in]    parameter conponent tag
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return true  success file read
        //! @return false failure file read
        bool read_file(const PARAMETER_COMPONENT_TAG,
                       const std::string& = PARAMETER_FILE);

        //! parameter int value getter
        //! @param[in]    parametercategory
        //! @param[in]    parameter key
        //! @param[out]   error code
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return int value
        int get_int(const PARAMETER_COMPONENT_TAG,
                    const std::string &,
                    error_code &,
                    const std::string& = PARAMETER_FILE);

        //! parameter string value getter
        //! @param[in]    parametercategory
        //! @param[in]    parameter key
        //! @param[out]   error code
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return string value
        std::string get_string(const PARAMETER_COMPONENT_TAG,
                               const std::string &,
                               error_code &,
                               const std::string& = PARAMETER_FILE);

        //! parameter multistring value getter
        //! @param[in]    parametercategory
        //! @param[in]    parameter key
        //! @param[inout] string vector
        //! @param[out]   error code
        //! @param[in]    configuration filename
        //!                   default:PARAMETER_FILE
        //! @return string value
        void get_multistring(const PARAMETER_COMPONENT_TAG,
                             const std::string &,
                             std::vector<std::string>&,
                             error_code &,
                             const std::string& = PARAMETER_FILE);
};

} //namespace l7vs

#endif //PARAMETER_H
