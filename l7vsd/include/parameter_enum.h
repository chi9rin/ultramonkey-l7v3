/*!
 * @file  parameter_enum.h
 * @brief ultramonke-l7 parameter enum prototype.
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
#ifndef PARAMETER_ENUM_H
#define PARAMETER_ENUM_H

namespace l7vs{

//! @enum    PARAMTER_COMPONENT_TAG
//!    @brief    parameter key tags
enum PARAMETER_COMPONENT_TAG {
    PARAM_COMP_NOCAT = 0,        //!< parameter category is none
    PARAM_COMP_ALL,                //!< parameter category is all(caution! this category used l7vsadm and command)
    PARAM_COMP_L7VSD,            //!< parameter category l7vsd
    PARAM_COMP_COMMAND,            //!< parameter category command
    PARAM_COMP_SESSION,            //!< parameter category session
    PARAM_COMP_VIRTUALSERVICE,    //!< parameter category virtualservice
    PARAM_COMP_MODULE,            //!< parameter category module control
    PARAM_COMP_REPLICATION,        //!< parameter category replication
    PARAM_COMP_LOGGER,            //!< parameter category logger
    PARAM_COMP_L7VSADM,            //!< parameter category l7vsadm
    PARAM_COMP_SNMPAGENT,        //!< parameter category snmpagent
    PARAM_COMP_SSLPROXY,            //!< parameter category sslproxy
    PARAM_COMP_SSL            //!< parameter category ssl
};

}    //namespace l7vsd

#endif    //PARAMETER_ENUM
