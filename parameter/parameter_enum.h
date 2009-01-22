//
//!	@file	parameter_enum.h
//!	@brief	ultramonke-l7 parameter enum prototype
//
//	copyright(c) sdy corporation.2008
//	mail: n.nakai at sdy.co.jp
//	Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//	Distributed under the Boost Software License, Version 1.0. (See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef PARAMETER_ENUM_H
#define PARAMETER_ENUM_H

namespace l7vs{

//! @enum	PARAMTER_COMPONENT_TAG
//!	@brief	parameter key tags
enum PARAMETER_COMPONENT_TAG {
	PARAM_COMP_NOCAT = 0,		//!< parameter category is none
	PARAM_COMP_ALL,				//!< parameter category is all(caution! this category used l7vsadm and command)
	PARAM_COMP_L7VSD,			//!< parameter category l7vsd
	PARAM_COMP_COMMAND,			//!< parameter category command
	PARAM_COMP_SESSION,			//!< parameter category session
	PARAM_COMP_VIRTUALSERVICE,	//!< parameter category virtualservice
	PARAM_COMP_MODULE,			//!< parameter category module control
	PARAM_COMP_REPLICATION,		//!< parameter category replication
	PARAM_COMP_LOGGER,			//!< parameter category logger
	PARAM_COMP_L7VSADM,			//!< parameter category l7vsadm
	PARAM_COMP_SNMPAGENT,		//!< parameter category snmpagent
	PARAM_COMP_SSLPROXY			//!< parameter category sslproxy
};

}	//namespace l7vsd

#endif	//PARAMETER_ENUM
