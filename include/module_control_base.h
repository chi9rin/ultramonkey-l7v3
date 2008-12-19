//
//	@file	protocol_module_base.h
//	@brief	shared object protocol module absctract class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef MODULE_CONTROL_BASE
#define MODULE_CONTROL_BASE

#include<string>
#include<map>
#include<boost/thread/locks.hpp>

class	module_control_base
{
protected:
	std::map<std::string,int>	loadedmodule_map;
	boost::mutex				loadedmodule_map_mutex;

	module_control_base(){}
	virtual	~module_control_base() = 0;

public:
	virtual	bool	load_module( const std::string&	modulename ) = 0;
	virtual	void	unload_module( const std::string&	modulename ) = 0;
};

#endif//MODULE_CONTROL_BASE
