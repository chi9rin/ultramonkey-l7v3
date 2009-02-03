#include "error_code.h"

#include <boost/thread.hpp>
#include "logger.h"
#include "parameter.h"

//#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
#include "realserver_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"

#include "l7vsd_stub.h"

namespace	l7vs{
// l7vsd
l7vsd::l7vsd() {}
l7vsd::~l7vsd(){}

//! vs_list search function
//! @param[in]	vs_element
//! @param[out]	error_code
l7vsd::vslist_type::iterator	l7vsd::search_vslist( const virtualservice_element& in_vselement ) const {
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::search_vslist", __FILE__, __LINE__ );

	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		if( (*itr)->get_element() == in_vselement )	return itr;
	}
	return vslist.end();
}

void	l7vsd::list_virtual_service( vselist_type*, error_code&  )
{
	boost::mutex::scoped_lock lock(command_mutex);

	++call_num_map[l7vs::l7vsadm_request::CMD_LIST];
}

void	l7vsd::list_virtual_service_verbose(	vselist_type*,
										replication::REPLICATION_MODE_TAG*,
										logstatus_list_type*,
										bool*,
										logstatus_list_type*,
										error_code&  )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_LIST_VERBOSE];
}

void	l7vsd::add_virtual_service( const virtualservice_element*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_ADD_VS];
}

void	l7vsd::del_virtual_service( const virtualservice_element*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_DEL_VS];
}

void	l7vsd::edit_virtual_service( const virtualservice_element*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_EDIT_VS];
}


void	l7vsd::add_real_server( const virtualservice_element*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_ADD_RS];
}

void	l7vsd::del_real_server( const virtualservice_element*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_DEL_RS];
}

void	l7vsd::edit_real_server( const virtualservice_element*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_EDIT_RS];
}


void	l7vsd::flush_virtual_service( error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_FLUSH_VS];
}

void	l7vsd::replication_command( const l7vsadm_request::REPLICATION_COMMAND_TAG* rep_cmd, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_REPLICATION];
}

void	l7vsd::log_command( const LOG_CATEGORY_TAG*, const LOG_LEVEL_TAG*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_LOG];
}

void	l7vsd::snmp_log_command( const LOG_CATEGORY_TAG*, const LOG_LEVEL_TAG*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_SNMP];
}

void	l7vsd::reload_parameter( const PARAMETER_COMPONENT_TAG*, error_code& )
{
	boost::mutex::scoped_lock lock(command_mutex);
	++call_num_map[l7vs::l7vsadm_request::CMD_PARAMETER];
}

void	l7vsd::release_virtual_service( const virtualservice_element& )	const
{}


// virtualservice
bool	virtual_service::initialize_fail(false);
bool	virtual_service::set_virtualservice_fail(false);
bool	virtual_service::edit_virtualservice_fail(false);

bool	virtual_service::add_realserver_fail(false);
bool	virtual_service::edit_realserver_fail(false);
bool	virtual_service::del_realserver_fail(false);

bool	virtual_service::finalize_called(false);
bool	virtual_service::finalize_fail(false);
bool	virtual_service::stop_called(false);

void	virtual_service::finalize( error_code& err )
{
	finalize_called = true;
	vsd.release_virtual_service( element );
	if( finalize_fail )	err.setter( true, "finalize_fail!" );
}

// replication
bool	replication::initialize_called(false);
bool	replication::initialize_fail(false);
bool	replication::finalize_called(false);

// snmpbridge
bool	snmpbridge::initialize_called(false);
bool	snmpbridge::initialize_fail(false);
bool	snmpbridge::finalize_called(false);

// protocol_module_control
bool	protocol_module_control::initialize_called(false);
bool	protocol_module_control::finalize_called(false);

// schedule_module_control
bool	schedule_module_control::initialize_called(false);
bool	schedule_module_control::finalize_called(false);

}	//namespace	l7vs
