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
#ifndef MODULE_CONTROL_BASE
#define MODULE_CONTROL_BASE

#include	<string>

namespace l7vs{

class	module_control_bad_alloc : public std::bad_alloc {
public:
//	module_control_bad_alloc( ) : std::bad_alloc( ){
		
//	}
};

class	module_control_error : public std::logic_error {

};

class	module_control_base{
protected:
	std::string	modulefile_path;
	module_control_base(){}
	virtual	~module_control_base(){};
public:
	virtual	void	initialize( const std::string& infile_path ) = 0;
	virtual	void	finalize() = 0;
};

}	// namespace l7vs
#endif//MODULE_CONTROL_BASE
