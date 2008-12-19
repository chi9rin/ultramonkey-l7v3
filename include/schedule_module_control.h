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
#include<boost/thread/locks.hpp>
#include"logger.h"
#include"schedule_module_base.h"

class	schedule_module_control : public module_control_base
{
public:
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const std::string ) >	logger_func_type;
	typedef	boost::function< void ( std::string&, unsigned int* ) >				replication_pay_memory_func_type;

	struct	module_info{
		unsigned int	ref_count;
		protocol_module_base*	(*module_create)(
										logger_func_type,
										replication_pay_memory_func_type);
		void					(*module_restroy)(protocol_module_base*);
	};

protected:
	std::map<std::string,int>	loadedmodule_map;
	boost::mutex				loadedmodule_map_mutex;

	schedule_module_control();
	schedule_module_control( const schedule_module_control& );
	schedule_module_control&	operator=( const schedule_module_control& );

public:
	static schedule_module_control&	getInstance();

	bool	load_module( const std::string&	modulename );
	void	unload_module( const std::string&	modulename );

	schedule_module_base*	module_new(
								std::string& modulename,
								logger_func_type	inlog,
								replication_pay_memory_func_type	inpaymemory );
	void	module_delete( schedule_module_base* module_ptr );
};

#endif//SCHEDULE_MODULE_CONTROL
