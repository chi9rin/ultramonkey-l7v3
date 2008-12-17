//
//	@file	parameter_enum.h
//	@brief	ultramonke-l7 parameter enum prototype
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

namespace l7vsd{

enum PARAMETER_COMPONENT_TAG
{
	PARAM_COMP_NOCAT = 0,
	PARAM_COMP_ALL,
	PARAM_COMP_L7VSD,
	PARAM_COMP_COMMAND,
	PARAM_COMP_SESSION,
	PARAM_COMP_VIRTUALSERVICE,
	PARAM_COMP_SCHEDULEMODULE,
	PARAM_COMP_PROTOCOLMODULE,
	PARAM_COMP_MODULE,
	PARAM_COMP_REPLICATION,
	PARAM_COMP_LOGGER,
	PARAM_COMP_L7VSADM,
	PARAM_COMP_SNMPAGENT
};

};	//namespace l7vsd
	
#endif	//PARAMETER_ENUM
