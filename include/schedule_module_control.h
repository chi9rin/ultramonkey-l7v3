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

namespace l7vsd
{

class	schedule_module_control : public module_control_base
{
public:
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const unsigned int log_id, const std::string ) >
										logger_func_type;
	typedef	boost::function< void ( std::string&, unsigned int* ) >
										replication_pay_memory_func_type;

	struct	schedule_module_info
	{
		unsigned int	ref_count;
		boost::function<schedule_module_base*(logger_func_type)>	create_func;
		boost::function<void(schedule_module_base*)>				destroy_func;
	};

protected:
	std::map<std::string,schedule_module_info>	loadmodule_map;
	boost::mutex								loadmodule_map_mutex;

	schedule_module_control();
	schedule_module_control( const schedule_module_control& );
	schedule_module_control&	operator=( const schedule_module_control& );

public:
	static schedule_module_control&	getInstance();

	schedule_module_base*	load_module( const std::string&	modulename, logger_func_type inlog );
	void					unload_module( const std::string& modulename, schedule_module_base* module_ptr );
};

};	//namespace l7vsd
#endif//SCHEDULE_MODULE_CONTROL
