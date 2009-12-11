/*!
 * @file  parameter.cpp
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
#include "parameter_enum.h"
#include "parameter.h"
#include "parameter_impl.h"
#include "logger.h"
#include "utility.h"

#if !defined(LOGGER_PROCESS_VSD) && !defined(LOGGER_PROCESS_ADM) && !defined(LOGGER_PROCESS_SNM)
#define LOGGER_PROCESS_VSD
#endif

#ifdef LOGGER_PROCESS_SNM
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_SNMPAGENT_PARAMETER;
#elif  LOGGER_PROCESS_ADM
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_L7VSADM_PARAMETER;
#else
l7vs::LOG_CATEGORY_TAG param_cat = l7vs::LOG_CAT_L7VSD_PARAMETER;
#endif

//!
//! Constructor of Parameter class
//!
l7vs::Parameter::Parameter(){
    if( !ParameterImpl::get_instance().init() ){
        Logger::putLogFatal( param_cat, 1, "Parameter initialize failure",
                             __FILE__, __LINE__ );
        switch( param_cat ){
        case LOG_CAT_L7VSADM_PARAMETER:
            // DEBUG LOG
            if( unlikely(LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
                std::stringstream buffer;
                buffer << "Call get_instance().init() :false \n";
                buffer << "    param_cat : LOG_CAT_L7VSDADM_PARAMETER";
                Logger::putLogDebug( param_cat, 1, buffer.str(),
                                     __FILE__, __LINE__ );
            }
            // DEBUG LOG END
            break;
        default:
            // DEBUG LOG
            if( unlikely(LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
                std::stringstream buffer;
                buffer << "Call get_instance().init() :false exit(1)ROOT\n";
                buffer << "    param_cat : " << param_cat;
                Logger::putLogDebug( param_cat, 2, buffer.str(),
                                     __FILE__, __LINE__ );
            }
            // DEBUG LOG END
            exit(1);
        }
    }
}

//!
//! Destructor of Parameter class
//!
l7vs::Parameter::~Parameter(){
}

//! Initialize ParameterImpl class(file designation)
//! @param[in]    COMPONENT TAG
//! @param[in]    file name
//! @return true  success
//! @return false failer
bool l7vs::Parameter::init( const PARAMETER_COMPONENT_TAG comp,
                            const std::string& filename ){
    // DEBUG LOG
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
        std::stringstream buffer;
        buffer << "in_function : bool l7vs::Parameter::init( ";
        buffer << "const PARAMETER_COMPONENT_TAG comp = " << comp << " ";
        buffer << "const std::string& filename = " << filename ;
        buffer << " )";
        Logger::putLogDebug( param_cat, 3, buffer.str(), __FILE__, __LINE__ );
    }
    // DEBUG LOG END
    
    ParameterImpl& impl = ParameterImpl::get_instance();
    return impl.init( comp, filename );
}

//! read configuration file
//! @param[in]   comp        section TAG
//! @param[in]   filename    configuration filename
//! @return       true = success read file / false = failure read file
bool l7vs::Parameter::read_file( const PARAMETER_COMPONENT_TAG comp,
                                 const std::string& filename ){
    // DEBUG LOG
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
        std::stringstream buffer;
        buffer << "in_function : bool l7vs::Parameter::read_file( ";
        buffer << "const PARAMETER_COMPONENT_TAG comp = " << comp << " ";
        buffer << "const std::string& filename = " << filename ;
        buffer << " )";
        Logger::putLogDebug( param_cat, 4, buffer.str(), __FILE__, __LINE__ );
    }
    // DEBUG LOG END
    
    ParameterImpl& impl = ParameterImpl::get_instance();
    return impl.read_file( comp, filename );
}

//! get integer data.
//! @param[in]  comp   section TAG
//! @param[in]  key    key string
//! @return     value
int l7vs::Parameter::get_int( const l7vs::PARAMETER_COMPONENT_TAG comp,
                              const std::string& key,
                              l7vs::error_code& err ){
    // DEBUG LOG
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
        std::stringstream buffer;
        buffer << "in_function : int l7vs::Parameter::get_int( ";
        buffer << "const PARAMETER_COMPONENT_TAG comp = " << comp << " ";
        buffer << "const std::string& key = " << key << " ";
        buffer << "l7vs::error_code& err= ";
        buffer << (err ? ("true,"+err.get_message()) : "false") ;
        buffer << " )";
        Logger::putLogDebug( param_cat, 5, buffer.str(), __FILE__, __LINE__ );
    }
    // DEBUG LOG END
    
    ParameterImpl& impl = ParameterImpl::get_instance();
    return impl.get_int( comp, key, err );
}

//! get character data.
//! @param[in]  comp   section TAG
//! @param[in]  key    key string
//! @return     value
std::string l7vs::Parameter::get_string( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                         const std::string& key,
                                         l7vs::error_code& err ){
    // DEBUG LOG
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
        std::stringstream buffer;
        buffer << "in_function : int l7vs::Parameter::get_string( ";
        buffer << "const PARAMETER_COMPONENT_TAG comp = " << comp << " ";
        buffer << "const std::string& key = " << key << " ";
        buffer << "l7vs::error_code& err= ";
        buffer << (err ? ("true,"+err.get_message()) : "false") ;
        buffer << " )";
        Logger::putLogDebug( param_cat, 6, buffer.str(), __FILE__, __LINE__ );
    }
    // DEBUG LOG END
    
    ParameterImpl& impl = ParameterImpl::get_instance();
    return impl.get_string( comp, key, err );
}

//! get character data.(multi)
//! @param[in]    comp    section TAG
//! @param[in]    key     key string
//! @param[inout] retvec  string vector 
//! @param[out]   errr    error code
//! @return       value
void l7vs::Parameter::get_multistring( const l7vs::PARAMETER_COMPONENT_TAG comp,
                                       const std::string& key,
                                       std::vector<std::string>& retvec,
                                       l7vs::error_code& err ){
    // DEBUG LOG
    if( unlikely( LOG_LV_DEBUG == Logger::getLogLevel( param_cat ) ) ) {
        std::stringstream buffer;
        buffer << "in_function : int l7vs::Parameter::get_multistring( ";
        buffer << "const PARAMETER_COMPONENT_TAG comp = " << comp << " ";
        buffer << "const std::string& key = " << key << " ";
        buffer << "std::vector<std::string>& retvec = (size)";
        buffer << retvec.size() << " ";
        buffer << "l7vs::error_code& err= ";
        buffer << (err ? ("true,"+err.get_message()) : "false") ;
        buffer << " )";
        Logger::putLogDebug( param_cat, 7, buffer.str(), __FILE__, __LINE__ );
    }
    // DEBUG LOG END
    
    ParameterImpl&    impl = ParameterImpl::get_instance();
    return impl.get_multistring( comp, key, retvec, err );
}
