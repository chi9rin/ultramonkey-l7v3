#ifndef	__L7AG_PARAMETER_H__
#define __L7AG_PARAMETER_H__

#include <string>
#include <map>

#include "logger_wrapper.h"

struct	l7ag_parameter{
	std::string	nic;
	std::string	address;
	int		portno;
	int		interval;
	int		status;    // TODO remove status
	std::map<LOG_CATEGORY_TAG,LOG_LEVEL_TAG>	loglevel;
};

#endif	//__L7AG_PARAMETER_H__

