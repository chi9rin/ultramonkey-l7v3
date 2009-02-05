#include "error_code.h"

#include "logger.h"
#include "parameter.h"

#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
#include "realserver_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"

#include "l7vsd.h"

namespace	l7vs{

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

bool	replication::dump_memory_called(false);
bool	replication::start_called(false);
bool	replication::stop_called(false);
bool	replication::force_replicate_called(false);
bool	replication::reset_called(false);

// snmpbridge
bool	snmpbridge::initialize_called(false);
bool	snmpbridge::initialize_fail(false);
bool	snmpbridge::finalize_called(false);
bool	snmpbridge::reload_config_called(false);
LOG_CATEGORY_TAG	snmpbridge::change_loglevel_category(LOG_CAT_NONE);
LOG_LEVEL_TAG		snmpbridge::change_loglevel_level(LOG_LV_NONE);
LOG_LEVEL_TAG		snmpbridge::change_loglevel_all_level(LOG_LV_NONE);

// protocol_module_control
bool	protocol_module_control::initialize_called(false);
bool	protocol_module_control::finalize_called(false);

// schedule_module_control
bool	schedule_module_control::initialize_called(false);
bool	schedule_module_control::finalize_called(false);

}	//namespace	l7vs
