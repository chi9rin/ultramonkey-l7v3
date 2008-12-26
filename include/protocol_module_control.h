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

#include	<string>
#include	<boost/thread.hpp>
#include	<boost/noncopyable.hpp>
#include	"logger.h"
#include	"protocol_module_base.h"

namespace l7vs{

class	protocol_module_control : public module_control_base : public boost::noncopyable{
public:
	typedef	std::map<std::string,protocol_module_info>
								name_module_info_map;
	typedef	boost::function< void ( const LOG_LEVEL_TAG, const std::string ) >
								logger_func_type;

	struct	protocol_module_info{
		unsigned int	ref_count;
		boost::function<protocol_module_base*(logger_func_type)>
						create_func;
		boost::function<void(protocol_module_base*)>
						destroy_func;
		protocol_module_info() : ref_count(0) {}
	};

protected:
	name_module_info_map	loadmodule_map;
	boost::mutex			loadmodule_map_mutex;
public:
	static protocol_module_control&	getInstance();
	protocol_module_base*	load_module(	const	std::string& modulename,
											logger_func_type inlog );
	void					unload_module(	const std::string& modulename,
											protocol_module_base* module_ptr );
}

};	//namespace l7vsd
#endif//PROTOCOL_MODULE_CONTROL
