#ifndef	L7VSD_H
#define	L7VSD_H

#include <list>
#include <boost/shared_ptr.hpp>
#include "error_code.h"
#include "l7vs_command.h"
#include "command_receiver.h"
#include "virtualservice_element.h"
#include "logger.h"
#include "parameter.h"
#include "replication_stub.h"

namespace	l7vs{
class	l7vsd{
public:
	typedef std::list< virtualservice_element >					vselist_type;	//!< virtual service element list typedef

	typedef	std::list< std::pair<LOG_CATEGORY_TAG, LOG_LEVEL_TAG> >	logstatus_list_type;	//!< logstatus list typedef

	static	bool	list_vs_called;
	static	bool	list_vs_v_called;
	static	bool	add_vs_called;
	static	bool	del_vs_called;
	static	bool	edit_vs_called;
	static	bool	add_rs_called;
	static	bool	del_rs_called;
	static	bool	edit_rs_called;
	static	bool	flush_vs_called;
	static	bool	rep_cmd_called;
	static	bool	log_cmd_called;
	static	bool	snm_cmd_called;
	static	bool	param_called;

	static	bool	list_vs_fail;
	static	bool	list_vs_v_fail;
	static	bool	add_vs_fail;
	static	bool	del_vs_fail;
	static	bool	edit_vs_fail;
	static	bool	add_rs_fail;
	static	bool	del_rs_fail;
	static	bool	edit_rs_fail;
	static	bool	flush_vs_fail;
	static	bool	rep_cmd_fail;
	static	bool	log_cmd_fail;
	static	bool	snm_cmd_fail;
	static	bool	param_fail;

public:
	void	list_virtual_service( vselist_type*, error_code& err )
	{
		list_vs_called = true;
		if(list_vs_fail)	err.setter( true, "");
	}
	void	list_virtual_service_verbose( l7vsd_response*, error_code& err )
	{
		list_vs_v_called = true;
		if(list_vs_v_fail)	err.setter( true, "");
	}
	void	add_virtual_service( const virtualservice_element*, error_code& err )
	{
		add_vs_called = true;
		if(add_vs_fail)	err.setter( true, "");
	}
	void	del_virtual_service( const virtualservice_element*, error_code& err )
	{
		del_vs_called = true;
		if(del_vs_fail)	err.setter( true, "");
	}
	void	edit_virtual_service( const virtualservice_element*, error_code& err )
	{
		edit_vs_called = true;
		if(edit_vs_fail)	err.setter( true, "");
	}

	void	add_real_server( const virtualservice_element*, error_code& err )
	{
		add_rs_called = true;
		if(add_rs_fail)	err.setter( true, "");
	}
	void	del_real_server( const virtualservice_element*, error_code& err )
	{
		del_rs_called = true;
		if(del_rs_fail)	err.setter( true, "");
	}
	void	edit_real_server( const virtualservice_element*, error_code& err )
	{
		edit_rs_called = true;
		if(edit_rs_fail)	err.setter( true, "");
	}

	void	flush_virtual_service( error_code& err )
	{
		flush_vs_called = true;
		if(flush_vs_fail)	err.setter( true, "");
	}

	void	replication_command( const l7vsadm_request::REPLICATION_COMMAND_TAG*, error_code& err )
	{
		rep_cmd_called = true;
		if(rep_cmd_fail)	err.setter( true, "");
	}
	void	log_command( const LOG_CATEGORY_TAG*, const LOG_LEVEL_TAG*, error_code& err )
	{
		log_cmd_called = true;
		if(log_cmd_fail)	err.setter( true, "");
	}
	void	snmp_log_command( const LOG_CATEGORY_TAG*, const LOG_LEVEL_TAG*, error_code& err )
	{
		snm_cmd_called = true;
		if(snm_cmd_fail)	err.setter( true, "");
	}
	void	reload_parameter( const PARAMETER_COMPONENT_TAG*, error_code& err )
	{
		param_called = true;
		if(param_fail)	err.setter( true, "");
	}


};
}	//namespace	l7vs
#endif	//L7VSD_H
