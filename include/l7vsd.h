#ifndef	L7VSD_H
#define L7VSD_H

#include <list>
#include <vector>
#include "l7vs_command_receiver.h"
#include "l7vs_virtual_service_command_element.h"
#include "l7vs_virtual_service.h"
#include "l7vs_realserver_element.h"
#include "l7vs_realserver.h"
#include "logger/logger.h"
#include "parameter/parameter.h"

namespace l7vsd{
class l7vsd{
public:
	struct config_message_result{
		bool	flag;
		std::string	message;
		bool	operator==( const config_message_result& in ){ return flag == in.flag; }
		bool	operator!=( const config_message_result& in ){ return flag != in.flag; }
	};

protected:
	thread_pool tp;
	boost::asio::io_service dispatcher;

	std::list<virtual_service> virtual_service_list;

	shared_ptr<command_receiver> command_receiever_;
	shared_ptr<replication> replication_;
	shared_ptr<snmp_bridge> snmp_bridge_;

public:
	config_message_result list_virtual_service(std::vector<virtualservice_element>& out_vslist);

	config_message_result add_virtual_service(virtualservice_element in_vselement);
	config_message_result del_virtual_service(virtualservice_element in_vselement);
	config_message_result edit_virtual_service(virtualservice_element in_vselement);

	config_message_result add_real_server(virtualservice_element in_vselement, realserver_element in_rselemnt);
	config_message_result del_real_server(virtualservice_element in_vselement, realserver_element in_rselement);
	config_message_result edit_real_server(virtualservice_element in_vselement, realserver_element in_rselement);

	config_message_result flush_virtual_service();

	config_message_result get_replication_info(REPLICATION_MODE_TAG& out_status);
	config_message_result set_replication(bool start_replication_flag);
	config_message_result replication_dump_memory();
	config_message_result replication_compulsorily();

	config_message_result get_log_level(LOG_CATEGORY_TAG in_log_category, LOG_LEVEL_TAG& out_log_level);
	config_message_result set_log_level(LOG_CATEGORY_TAG in_log_category, LOG_LEVEL_TAG in_log_level);
	config_message_result set_log_level_all(LOG_LEVEL_TAG in_log_level);

	config_message_result get_snmp_connect_status(int& out_status);
	config_message_result get_snmp_log_level(LOG_CATEGORY_TAG in_log_category, LOG_LEVEL_TAG& out_log_level);
	config_message_result set_snmp_log_level(LOG_CATEGORY_TAG in_log_category, LOG_LEVEL_TAG in_log_level);
	config_message_result set_snmp_log_level_all(LOG_LEVEL_TAG in_log_level);

	config_message_result reload_parameter(PARAMETER_COMPONENT_TAG in_reload_param);

	void start();

protected:
	bool is_exit_requested();

};

}	//namespace l7vsd
#endif	//L7VSD_H
