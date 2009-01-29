#ifndef PROTOCOL_MODULE_CONTROL_TEST
#define PROTOCOL_MODULE_CONTROL_TEST

#include "protocol_module_control.h"

#define	PM1	"protocol_module_test1"
#define	PM2	"protocol_module_test2"

class	l7vs::protocol_module_control;

class	protocol_module_control_testclass : public l7vs::protocol_module_control{
public:
	std::string&			get_modulefile_path(){ return modulefile_path; }
	name_module_info_map&	get_loadmodule_map(){ return loadmodule_map; }

	static protocol_module_control_testclass& getInstance(){
		static protocol_module_control_testclass instance_test;
		return instance_test;
	}

};

#endif // PROTOCOL_MODULE_CONTROL_TEST
