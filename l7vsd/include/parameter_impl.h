/*!
 * @file  parameter_impl.h
 * @brief ultramonkey-l7 parameter implement class.
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
#ifndef PARAMETER_IMPL_H
#define PARAMETER_IMPL_H

#include "parameter_enum.h"
#include "error_code.h"
#include <string>
#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>

namespace l7vs
{


class ParameterImpl : private boost::noncopyable
{
protected:
        typedef    std::map< std::string, int >               int_map_type;
        typedef    std::multimap< std::string, std::string >  multistring_map_type;

        //! parameter data of multi string
        multistring_map_type                    multistringMap;
        //! parameter data of int
        int_map_type                            intMap;
        //! create mutex
        boost::mutex                            create_mutex;
        //! read / write mutex
        boost::mutex                            param_mutex;
        //! component tag to section name map
        std::map< PARAMETER_COMPONENT_TAG, std::string >
        tag_section_table_map;
        //! constructor
        ParameterImpl() {}
        //! destructor
        ~ParameterImpl() {}
public:
        //! instance getter
        //! @return instance
        static ParameterImpl &get_instance() {
                static ParameterImpl instance;
                return instance;
        }

        //! initialize
        //! @return true success
        //! @return false fail
        bool init();

        //! initialize(file designation)
        //! @param[in]    component tag
        //! @param[in]    file name
        //! @return true success
        //! @return false fail
        bool init(const PARAMETER_COMPONENT_TAG, const std::string &);

        //! target component read
        //! @param[in]    component tag
        //! @param[in]    file name
        //! @return false fail
        //! @return true success
        bool    read_file(const PARAMETER_COMPONENT_TAG, const std::string &);

        //! int value getter
        //! @param[in]    component tag
        //! @param[in]    key string
        //! @param[out]   error code
        //! @param[in]    file name
        //! @return int value
        int get_int(const PARAMETER_COMPONENT_TAG,
                    const std::string &,
                    error_code &,
                    const std::string &);

        //! string value getter
        //! @param[in]    component tag
        //! @param[in]    key string
        //! @param[out]   error code
        //! @param[in]    file name
        //! @return string value
        std::string get_string(const PARAMETER_COMPONENT_TAG,
                               const std::string &,
                               error_code &,
                               const std::string &);

        //! multistring value getter
        //! @param[in]    component tag
        //! @param[in]    key string
        //! @param[inout] string vector
        //! @param[out]   error code
        //! @param[in]    file name
        //! @return       void
        void get_multistring(const PARAMETER_COMPONENT_TAG,
                             const std::string &,
                             std::vector<std::string>&,
                             error_code &,
                             const std::string &);
};

}
#endif //__PARAMETER_IMPL_H__
