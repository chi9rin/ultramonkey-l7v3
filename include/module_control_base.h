//
//!	@file	protocol_module_base.h
//!	@brief	shared object protocol module absctract class
//
//	copyright (c) sdy corporation. 2008
//	mail: h dot okada at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#ifndef MODULE_CONTROL_BASE
#define MODULE_CONTROL_BASE

#include	<string>
#include	<boost/noncopyable.hpp>

namespace l7vs{

//
//! @class	module_control_base_alloc
//! @brief	this class throw then don't allocate memory.
class	module_control_bad_alloc : public std::bad_alloc {
public:
//	module_control_bad_alloc( ) : std::bad_alloc( ){
		
//	}
};

//
//! @class	module_control_error
//!	@brief	this class throw then logical error
class	module_control_error : public std::logic_error {

};

//
//! @class	module_control_base
//!	@brief	base abstract class. all module control is extend this class. 
class	module_control_base : private boost::noncopyable{
protected:
	std::string	modulefile_path;		//!<	shared object path
	virtual	~module_control_base(){};	//!<	destractor
public:
	//!	@brief	initialize prototype function is fist called
	virtual	void	initialize( const std::string& infile_path ) = 0;
	//! @brief	finalize prototype function is last called
	virtual	void	finalize() = 0;
};

}	// namespace l7vs
#endif//MODULE_CONTROL_BASE
