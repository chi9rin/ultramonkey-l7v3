#ifndef PROTOCOL_MODULE_CONTROL
#define PROTOCOL_MODULE_CONTROL

#include	<string>
// #include	<boost/thread.hpp>
// #include	<boost/asio.hpp>
// #include	<boost/noncopyable.hpp>
// #include	"logger.h"
// #include	"module_control_base.h"
// #include	"protocol_module_base.h"

namespace l7vs{

//
class	protocol_module_control{
protected:

	protocol_module_control(){}
public:
	//! instance getter function.
	static protocol_module_control&	getInstance()
	{
		static	protocol_module_control	instance;
		return	instance;
	}

	//! initialize
	//! @param[in]	shared object file path
	void	initialize( const std::string& infile_path )
	{}

	//! finalize
	void	finalize()
	{}

/*
	//! load module function
	//! @param[in]	protocol module name
	//!	@return		protocol module pointer
	protocol_module_base*	load_module( const std::string& modulename );
	//! unload module function.
	//!	@param[in]	module ptr
	void	unload_module( protocol_module_base* module_ptr )
	{}
*/

};

}	//namespace l7vs
#endif//PROTOCOL_MODULE_CONTROL
