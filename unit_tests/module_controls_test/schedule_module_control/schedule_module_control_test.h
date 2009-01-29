#ifndef SCHEDULE_MODULE_CONTROL_TEST
#define SCHEDULE_MODULE_CONTROL_TEST

#include "schedule_module_control.h"

#define	PM1	"schedule_module_test1"
#define	PM2	"schedule_module_test2"

class	l7vs::schedule_module_control;

class	schedule_module_control_testclass : public l7vs::schedule_module_control{
public:
	std::string&			get_modulefile_path(){ return modulefile_path; }
	name_module_info_map&	get_loadmodule_map(){ return loadmodule_map; }

	static schedule_module_control_testclass& getInstance(){
		static schedule_module_control_testclass instance_test;
		return instance_test;
	}

};

#endif // SCHEDULE_MODULE_CONTROL_TEST
