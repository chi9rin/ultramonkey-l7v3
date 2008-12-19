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
#ifndef SCHEDULE_MODULE_CONTROL
#define SCHEDULE_MODULE_CONTROL

#include<string>
#include"logger.h"
#include"schedule_module_base.h"

class	schedule_module_control : public module_control_base
{
protected:
	schedule_module_control();
	schedule_module_control( const schedule_module_control& );
	schedule_module_control&	operator=( const schedule_module_control& );
public:
	static schedule_module_control&	getInstance();

	bool	load_module( const std::string&	modulename );
	void	unload_module( const std::string&	modulename );

	schedule_module_base*	module_new(
								std::string& modulename,
								boost::function< void ( const LOG_LEVEL_TAG, const std::string ) > inlog,
								boost::function< void ( std::string&, unsigned int* ) >  inpaymemory );
	void	module_delete( schedule_module_base* module_ptr );
};

#endif//SCHEDULE_MODULE_CONTROL
