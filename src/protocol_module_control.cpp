//
//	@file	protocol_module_control.cpp
//	@brief	control load/unload shared object protocol module
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include	<dlfcn.h>
#include	"protocol_module_control.h"

#define L7VS_MODULE_INITFN "create_module"
namespace l7vs{

/*!
 * returns singleton instance.
 *
 * @param   void
 * @return  instance
 */
protocol_module_control&
protocol_module_control::getInstance(){
	static	protocol_module_control	instance;
	return	instance;
}

/*!
 * initialize class.
 *
 * @param   filepath
 * @return  void
 */
void
protocol_module_control::initialize( const std::string& infile_path ){
	if( &infile_path != NULL ){
		module_control_base::modulefile_path	= infile_path;
	}
}

/*!
 * finalize class.
 *
 * @param   void
 * @return  void
 */
void
protocol_module_control::finalize(){
}

/*!
 * load shared object.
 *
 * @param   modulename
 * @param   loggingfunction
 * @return  pointer for protocol_module_base class instance
 */
protocol_module_base*
protocol_module_control::load_module( const	std::string& modulename ){
	protocol_module_base* return_value = NULL;
	boost::mutex::scoped_lock( loadmodule_map_mutex );
	name_module_info_map::iterator it = loadmodule_map.find( modulename );
	if( it == loadmodule_map.end() ){
//		std::string load_module_name = modulefile_path + modulename + ".so";
//		void* h = dlopen( load_module_name, RTLD_LAZY );
//		if( h == NULL ){
//			return NULL;
//		}
//		protocol_module_base* handle = dlsym( h, L7VS_MODULE_INITFN );
//		void* handle = dlsym( h, L7VS_MODULE_INITFN );
//		if( handle == NULL ){
//			return NULL;
//		}
		protocol_module_control::protocol_module_info module_info;
//		module_info.create_func = ;
//		module_info.destroy_func = ;
		loadmodule_map.insert( std::pair< std::string, protocol_module_control::protocol_module_info >( modulename, module_info ) );
		it = loadmodule_map.find( modulename );
	}
	it->second.ref_count++;
	return_value = it->second.create_func();
	return NULL;
//	return return_value;
}

/*!
 * unload shared object.
 *
 * @param   modulename
 * @param   pointer for protocol_module_base class instance
 * @return  void
 */
void
protocol_module_control::unload_module( protocol_module_base* module_ptr ){
	if( module_ptr == NULL ){
		return;
	}
//	dlerror();
	std::string unload_module_name = module_ptr->get_name();
	boost::mutex::scoped_lock( loadmodule_map_mutex );
	name_module_info_map::iterator it = loadmodule_map.find( unload_module_name );
	if( it == loadmodule_map.end() ){
		return;
	}
	it->second.destroy_func(module_ptr);
	it->second.ref_count--;
	if( it->second.ref_count == 0 ){
//		dlclose();
		loadmodule_map.erase( loadmodule_map.find( unload_module_name ) );
	}
}

}
