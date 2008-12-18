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
#include"logger.h"
#include "realserver.h"
#include "protocol_module_base.h"

class	protocol_module_control : public module_control_base
{
public:
	typedef	list<realserver>	realserverlist_type;
	typedef	boost::function< realserverlist_type::iretarot( void ) > rs_list_itr_func_type;
protected:
	protocol_module_control();
	protocol_module_control( const protocol_module_control& );
	protocol_module_control&	operator=( const protocol_module_control& );
public:
	static protocol_module_control&	getInstance();

	void	load_module( const std::string&	modulename );
	void	unload_module( const std::string&	modulename );

	protocol_module_base*	module_new(
								std::string& modulename,
								rs_list_itr_func_type rslist_begin,
								rs_list_itr_func_type rslist_end,
								rs_list_itr_func_type rslist_next,
								boost::function< void ( const LOG_LEVEL_TAG, const std::string ) > inlog,
								boost::function< void ( std::string&, unsigned int* ) >  inpaymemory );
	void	module_delete( protocol_module_base* module_ptr );
};

#endif//PROTOCOL_MODULE_CONTROL
