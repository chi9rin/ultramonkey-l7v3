/*!
 *	@file	protocol_module_base.h
 *	@brief	shared object protocol module absctract class
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

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
