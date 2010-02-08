/*!
 * @file  schedule_module_control.h
 * @brief control load/unload shared object schedule module.
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
#ifndef SCHEDULE_MODULE_CONTROL
#define SCHEDULE_MODULE_CONTROL

#include    <string>
#include    <boost/thread.hpp>
#include    <boost/noncopyable.hpp>
#include    "logger.h"
#include    "module_control_base.h"
#include    "schedule_module_base.h"

namespace l7vs{

//
//!    @class    schedule_module_control    
//! @brief    schedule module control class is load schedule module from shared object file.
//! @brief    many virtual service class used schedule module instance.
//! @brief    but, shared object load is once. and unload when vitual service refarence count is zero.
class    schedule_module_control : public module_control_base{
public:
    //! logger function object typedef
    typedef    boost::function< void ( const LOG_LEVEL_TAG, const std::string ) >
                                logger_func_type;
    //! module create function object typedef
    typedef    boost::function< schedule_module_base* ( void ) >
                                create_func_type;
    //! module destroy function object typedef
    typedef    boost::function< void ( schedule_module_base* ) >
                                destroy_func_type;

    //! @struct    schedule_module_information
    //! @brief    schedule module information structure.
    struct    schedule_module_info
    {
        void *                handle;                //!< dlopen handle
        unsigned int        ref_count;            //!< refarence count
        create_func_type    create_func;        //!< create function object
        destroy_func_type    destroy_func;        //!< destroy function object
        schedule_module_info() : ref_count(0) {}    //!< constractor
    };

    //! name module map type typedef
    typedef    std::map<std::string,schedule_module_info>
                                name_module_info_map;

protected:
    //! load module information hash map,
    name_module_info_map    loadmodule_map;
    //! this mutex used loadmodule_map read/write/change.
    boost::mutex            loadmodule_map_mutex;

    schedule_module_control(){}
public:
    //! instance getter function.
    static schedule_module_control&    getInstance();
    //! initialize
    //! @param[in]    shared object file path
    void    initialize( const std::string& infile_path );
    //! finalize
    void    finalize();
    //! load module function
    //! @param[in]    schedule module name
    //!    @return        schedule module pointer
    schedule_module_base*    load_module( const std::string& modulename );
    //! unload module function.
    //!    @param[in]    module ptr
    void                    unload_module( schedule_module_base* module_ptr );
};

}    //namespace l7vs
#endif//SCHEDULE_MODULE_CONTROL
