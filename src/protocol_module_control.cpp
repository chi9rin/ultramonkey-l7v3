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
#include	"protocol_module_control.h"

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
protocol_module_control::load_module(	const	std::string& modulename,
											logger_func_type inlog ){

}

/*!
 * unload shared object.
 *
 * @param   modulename
 * @param   pointer for protocol_module_base class instance
 * @return  void
 */
void
protocol_module_control::unload_module(	const std::string& modulename,
											protocol_module_base* module_ptr ){
}

}
