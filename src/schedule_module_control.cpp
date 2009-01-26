//
//	@file	schedule_module_control.cpp
//	@brief	control load/unload shared object schedule module
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include	<dlfcn.h>
#include	"schedule_module_control.h"

#define L7VS_MODULE_INITFN "create_module"
#define L7VS_MODULE_FINIFN "destroy_module"
namespace l7vs{

/*!
 * returns singleton instance.
 *
 * @param   void
 * @return  instance
 */
schedule_module_control&
schedule_module_control::getInstance(){
	static	schedule_module_control	instance;
	return	instance;
}

/*!
 * initialize class.
 *
 * @param   filepath
 * @return  void
 */
void
schedule_module_control::initialize( const std::string& infile_path ){
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
schedule_module_control::finalize(){
}

/*!
 * load shared object.
 *
 * @param   modulename
 * @param   loggingfunction
 * @return  pointer for schedule_module_base class instance
 */
schedule_module_base*
schedule_module_control::load_module( const	std::string& modulename ){
	dlerror();
	schedule_module_base* return_value = NULL;
	boost::mutex::scoped_lock( loadmodule_map_mutex );
	name_module_info_map::iterator it = loadmodule_map.find( modulename );
//	std::cout << "load_module_name> " << modulename << std::endl;//debug
	if( it == loadmodule_map.end() ){
		std::string load_module_name = modulefile_path + modulename + ".so";
//		std::cout << "dlopen module_name> " << load_module_name.c_str() << std::endl;//debug
		void* h = dlopen( load_module_name.c_str(), RTLD_LAZY );
//		std::cout << "dlopen = " << h << std::endl;//debug
		if( h == NULL ){
			return NULL;
		}
		dlerror();
		schedule_module_base* (*create_func)(void);
		void (*destroy_func)(schedule_module_base*);

		*(schedule_module_base**) (&create_func) = (schedule_module_base*)dlsym( h, L7VS_MODULE_INITFN );
		if( create_func == NULL ){
//			std::cout << "create_func NULL" << std::endl;//debug
			dlclose(h);
			return NULL;
		}
		dlerror();
		*(void**) (&destroy_func) = dlsym( h, L7VS_MODULE_FINIFN );
		if( destroy_func == NULL ){
//			std::cout << "destroy_func NULL" << std::endl;//debug
			dlclose(h);
			return NULL;
		}
		schedule_module_control::schedule_module_info module_info;
		module_info.handle = h;
		module_info.create_func = create_func;
//		std::cout << "create_func = " << module_info.create_func << std::endl;//debug
		module_info.destroy_func = destroy_func;
//		std::cout << "destroy_func = " << module_info.destroy_func << std::endl;//debug
		loadmodule_map.insert( std::pair< std::string, schedule_module_control::schedule_module_info >( modulename, module_info ) );
		it = loadmodule_map.find( modulename );
	}
	it->second.ref_count++;
//	std::cout << "ref_count = " << it->second.ref_count << std::endl;//debug
	return_value = it->second.create_func();
//	std::cout << "return_value = " << return_value << std::endl;//debug
	return return_value;
}

/*!
 * unload shared object.
 *
 * @param   modulename
 * @param   pointer for schedule_module_base class instance
 * @return  void
 */
void
schedule_module_control::unload_module( schedule_module_base* module_ptr ){
	dlerror();
	if( module_ptr == NULL ){
		return;
	}
//	std::cout << module_ptr << std::endl;//debug

	std::string unload_module_name = module_ptr->get_name();
//	std::cout << "unload_module_name= " << module_ptr->get_name() << std::endl;//debug
	boost::mutex::scoped_lock( loadmodule_map_mutex );
	name_module_info_map::iterator it = loadmodule_map.find( unload_module_name );
	if( it == loadmodule_map.end() ){
		return;
	}
	it->second.destroy_func(module_ptr);
	it->second.ref_count--;
//	std::cout << "ref_count = " << it->second.ref_count << std::endl;//debug
	if( it->second.ref_count == 0 ){
//		std::cout << "dlclose> " << std::endl;//debug
		dlclose(it->second.handle);
		loadmodule_map.erase( loadmodule_map.find( unload_module_name ) );
	}
}

}
