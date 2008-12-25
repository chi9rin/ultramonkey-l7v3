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
#ifndef PROTOCOL_MODULE_CONTROL
#define PROTOCOL_MODULE_CONTROL

#include<string>
#include<boost/thread/locks.hpp>
#include"logger.h"
#include "realserver.h"
#include "protocol_module_base.h"

namespace l7vsd
{

class	protocol_module_control : public module_control_base{
public:
	typedef	list<realserver>	realserverlist_type;
	typedef	boost::function< realserverlist_type::iretarot( void ) >	rs_list_itr_func_type;
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const std::string ) >	logger_func_type;
	typedef	boost::function< void ( std::string&, unsigned int* ) >				replication_pay_memory_func_type;

	struct	protocol_module_info{
		unsigned int	ref_count;
		boost::function<protocol_module_base*(logger_func_type)>	create_func;
		boost::function<void(protocol_module_base*)>				destroy_func;
	};

protected:
	std::map<std::string,protocol_module_info>	loadmodule_map;
	boost::mutex								loadmodule_map_mutex;

	protocol_module_control();
	protocol_module_control( const protocol_module_control& );
	protocol_module_control&	operator=( const protocol_module_control& );

public:
	static protocol_module_control&	getInstance();

	protocol_module_base*	load_module( const std::string& modulename, logger_func_type inlog );
	void					unload_module( const std::string& modulename, protocol_module_base* module_ptr );
};

};	//namespace l7vsd
#endif//PROTOCOL_MODULE_CONTROL
