#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"
#include "protocol_module_stub.h"
#include "schedule_module_stub.h"

namespace    l7vs{
// protocol module
bool    protocol_module_stub::check_parameter_called(false);
bool    protocol_module_stub::check_parameter_fail(false);

// protocol_module_control
bool    protocol_module_control::load_module_fail(false);

protocol_module_control&
protocol_module_control::getInstance(){
    static    protocol_module_control    instance;
    return    instance;
}

void
protocol_module_control::initialize( const std::string& infile_path ){

}

void
protocol_module_control::finalize(){

}

protocol_module_base*
protocol_module_control::load_module( const    std::string& modulename ){
    protocol_module_base* return_value = NULL;
    if( !load_module_fail )
        return_value = dynamic_cast<l7vs::protocol_module_base*>(new l7vs::protocol_module_stub()); 
    return return_value;
}

void
protocol_module_control::unload_module( protocol_module_base* module_ptr ){
    if( module_ptr )
        delete module_ptr;
}


//    schedule_module_control
bool    schedule_module_control::load_module_fail(false);

schedule_module_control&
schedule_module_control::getInstance(){
    static    schedule_module_control    instance;
    return    instance;
}

void
schedule_module_control::initialize( const std::string& infile_path ){
}

void
schedule_module_control::finalize(){
}

schedule_module_base*
schedule_module_control::load_module( const    std::string& modulename ){
    schedule_module_base* return_value = NULL;
    if( !load_module_fail )
        return_value = dynamic_cast<l7vs::schedule_module_base*>(new l7vs::schedule_module_stub()); 
    return return_value;
}

void
schedule_module_control::unload_module( schedule_module_base* module_ptr ){
    if( module_ptr )
        delete module_ptr;
}

}    //namespace    l7vs
