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

class	protocol_module_control : public module_control_base
{
public:
	typedef	list<realserver>	realserverlist_type;
	typedef	boost::function< realserverlist_type::iretarot( void ) >	rs_list_itr_func_type;
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const std::string ) >	logger_func_type;
	typedef	boost::function< void ( std::string&, unsigned int* ) >				replication_pay_memory_func_type;

	struct	module_info{
		unsigned int	ref_count;
		protocol_module_base*	(*module_create)(
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										rs_list_itr_func_type,
										logger_func_type,
										replication_pay_memory_func_type);
		void					(*module_restroy)(protocol_module_base*);
	};

protected:
	std::map<std::string,module_info>	loadedmodule_map;
	boost::mutex						loadedmodule_map_mutex;

	protocol_module_control();
	protocol_module_control( const protocol_module_control& );
	protocol_module_control&	operator=( const protocol_module_control& );

public:
	static protocol_module_control&	getInstance();

	bool	load_module( const std::string& );
	void	unload_module( const std::string& );

	protocol_module_base*	module_create(
								std::string& modulename,
								rs_list_itr_func_type	rslist_begin,
								rs_list_itr_func_type	rslist_end,
								rs_list_itr_func_type	rslist_next,
								logger_func_type		inlog,
								replication_pay_memory_func_type	inpaymemory );
	void	module_destroy( protocol_module_base* module_ptr );
};

#endif//PROTOCOL_MODULE_CONTROL
