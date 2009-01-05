#include	"protocol_module_control.h"

namespace l7vs{

protocol_module_control&
protocol_module_control::getInstance(){
	static	protocol_module_control	instance;
	return	instance;
}

void
protocol_module_control::initialize( const std::string& infile_path ){
}

void
protocol_module_control::finalize(){
}

protocol_module_base*
protocol_module_control::load_module(	const	std::string& modulename,
											logger_func_type inlog ){

}

void
protocol_module_control::unload_module(	const std::string& modulename,
											protocol_module_base* module_ptr ){
}

}
