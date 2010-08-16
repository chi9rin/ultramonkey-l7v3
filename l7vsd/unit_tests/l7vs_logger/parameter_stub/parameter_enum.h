#ifndef __PARAMETER_ENUM_H__
#define __PARAMETER_ENUM_H__

namespace l7vs {
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
	PARAM_COMP_SSL			//!< parameter category sslproxy
};

};
	
#endif	//__PARAMETER_ENUM_H__
