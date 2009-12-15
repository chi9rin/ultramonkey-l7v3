/*!
 * @file  paramter_impl.h
 * @brief ultramonke-l7 parameter impliment class.
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

namespace l7vs{


class ParameterImpl : private boost::noncopyable {
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
    //! componenttag to section name map
    std::map< PARAMETER_COMPONENT_TAG, std::string >
                                            tag_section_table_map;
    //! constractor
    ParameterImpl(){}
    //! destractor
    ~ParameterImpl(){}
public:
    //! instansgetter
    //! @return instance
    static ParameterImpl & get_instance(){
        static ParameterImpl instance;
        return instance;
    }

    //! initialize
    //! @return true success
    //! @return false failer
    bool init();

    //! initialize(file designation)
    //! @param[in]    component tag
    //! @param[in]    file name
    //! @return true success
    //! @return false failer
    bool init( const PARAMETER_COMPONENT_TAG, const std::string& );

    //! target component read
    //! @param[in]    component tag
    //! @param[in]    file name
    //! @return false failer
    //! @return true success
    bool    read_file( const PARAMETER_COMPONENT_TAG, const std::string& );

    //! int value getter
    //! @param[in]    component tag
    //! @param[in]    keystring
    //! @param[out]   errorcode
    //! @return int value
    int get_int( const PARAMETER_COMPONENT_TAG,
                 const std::string&,
                 error_code& );

    void set_int( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                  const std::string& key,
                                  int set_value,
                                  l7vs::error_code& err ){
        // find section
        std::map<PARAMETER_COMPONENT_TAG, std::string>::iterator
            section_table_iterator = tag_section_table_map.find( comp );
        int_map_type::iterator intmap_iterator =
            intMap.find( section_table_iterator->second + "." + key );
        if( intmap_iterator != intMap.end() ) {
            intmap_iterator->second = set_value;
        } else {
            std::string str_key = section_table_iterator->second + "." + key;
            std::pair< std::string, int > add_para;
            add_para.first = str_key;
            add_para.second = set_value;
            intMap.insert(add_para);
//            std::cout << "Parameter set Error" << std::endl;
        }
    };

    void rm_int_parameter( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                  const std::string& key,
                                  l7vs::error_code& err ){
        // find section
        std::map<PARAMETER_COMPONENT_TAG, std::string>::iterator
            section_table_iterator = tag_section_table_map.find( comp );
        int_map_type::iterator intmap_iterator =
            intMap.find( section_table_iterator->second + "." + key );
        if( intmap_iterator != intMap.end() ) {
            intMap.erase(section_table_iterator->second + "." + key);
        }
    };

    //! string value getter
    //! @param[in]    component tag
    //! @param[in]    keystring
    //! @param[out]   errorcode
    //! @return string value
    std::string get_string( const PARAMETER_COMPONENT_TAG,
                            const std::string&,
                            error_code& );

    //! multistring value getter
    //! @param[in]    component tag
    //! @param[in]    keystring
    //! @param[inout] string vector
    //! @param[out]   errorcode
    //! @return       void
    void get_multistring( const PARAMETER_COMPONENT_TAG,
                          const std::string&,
                          std::vector<std::string>&,
                          error_code& );
};

}
#endif //__PARAMETER_IMPL_H__

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

namespace l7vs{


//! @class Parameter
//! @brief configure file controller
//! @brief set key to get value.
class Parameter{
public:
    Parameter();                            //!< default constractor
    ~Parameter();                           //!< default destractor

    //! initialize(file designation)
    //! @param[in]    component tag
    //! @param[in]    file name
    //! @return true success
    //! @return false failer
    bool init( const PARAMETER_COMPONENT_TAG, const std::string& );

    //! reload file function
    //! @param[in]    parameter conponent tag
    //! @param[in]    configuration filename
    //!                   default:PARAMETER_FILE
    //! @return true  success file read
    //! @return false failure file read
    bool read_file(const PARAMETER_COMPONENT_TAG,
                   const std::string& = PARAMETER_FILE );

    //! parameter int value getter
    //! @param[in]    parametercategory
    //! @param[in]    parameter key
    //! @param[out]   error code
    //! @return int value
    int get_int(const PARAMETER_COMPONENT_TAG,
                const std::string&,
                error_code& );

    //! parameter string value getter
    //! @param[in]    parametercategory
    //! @param[in]    parameter key
    //! @param[out]   error code
    //! @return string value
    std::string get_string(const PARAMETER_COMPONENT_TAG,
                           const std::string&,
                           error_code& );

    //! parameter multistring value getter
    //! @param[in]    parametercategory
    //! @param[in]    parameter key
    //! @param[inout] string vector
    //! @param[out]   error code
    //! @return string value
    void get_multistring(const PARAMETER_COMPONENT_TAG,
                         const std::string&,
                         std::vector<std::string>&,
                         error_code& );

    void set_int( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                  const std::string& key,
                                  int set_value,
                                  l7vs::error_code& err ){
        ParameterImpl& impl = ParameterImpl::get_instance();
        impl.set_int( comp, key, set_value, err );
    };

    void rm_int_parameter( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                  const std::string& key,
                                  l7vs::error_code& err ){
        ParameterImpl& impl = ParameterImpl::get_instance();
        impl.rm_int_parameter( comp, key, err );
    };
};

} //namespace l7vs
    
#endif //PARAMETER_H
