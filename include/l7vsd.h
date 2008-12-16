#ifndef	L7VSD_H
#define L7VSD_H

#include <list>
#include "l7vs_command_receiver.h"
#include "l7vs_virtual_service_command_element.h"
#include "l7vs_virtual_service.h"
#include "l7vs_realserver_element.h"
#include "l7vs_realserver.h"


namespace l7vsd{
class l7vsd{
protected:
	thread_pool tp;
	boost::asio::io_service dispatcher;

	std::list<virtual_service> virtual_service_list;

	shared_ptr<config> config_;
	shared_ptr<replication> replication_;
	shared_ptr<snmp_bridge> snmp_bridge_;

	std::vector<virtualservice_element> list_virtual_service();

	bool add_virtual_service(virtualservice_element in_vselement);
	bool del_virtual_service(virtualservice_element in_vselement);
	bool edit_virtual_service(virtualservice_element in_vselement);

	bool add_real_server(virtualservice_element in_vselement, realserver_element in_rselemnt);
	bool del_real_server(virtualservice_element in_vselement, realserver_element in_rselement);
	bool edit_real_server(virtualservice_element in_vselement, realserver_element in_rselement);

	bool flush_virtual_service();

	bool get_replication_info();
	bool edit_replication();

	bool get_log_info();
	bool set_log_level();
	bool set_log_level_all();

	bool get_snmp_log_info();
	bool get_snmp_connect_status();
	bool set_snmp_log_level();
	bool set_snmp_log_level_all();

	bool edit_parameter();
		

public:
	void run();

};

}	//namespace l7vsd
#endif	//L7VSD_H
