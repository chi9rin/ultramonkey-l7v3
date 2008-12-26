//
//	@file	l7vscommand.h
//	@brief	l7vsadm and l7vsd connection data prototype
//
//	copyright (c) sdy corporation. 2008
//	mail: n dot nakai at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#ifndef	L7COMMAND_H
#define	L7COMMAND_H

#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>
#include <boost/asio.hpp>
#include <list>
#include <memory>
#include "logger_enum.h"
#include "parameter_enum.h"
#include "l7vs_replication.h"
#include "l7vs_realserver_element.h"
#include "l7vs_virtualservice_command_element.h"

namespace l7vs{
//
//	l7vsadm -> l7vsd request data class
//	
//
class	l7vsadm_request{
public:
	// command lists
	enum	COMMAND_CODE_TAG{
		CMD_NONE = 0,
		CMD_LIST,               //!< List command(-l,--list)
		CMD_LIST_VERBOSE,       //!< Verbose list command(-V,--verbose)
		CMD_LIST_KEY,           //!< Key list command(-K, --key)
		CMD_ADD_VS,             //!< Add VirtualService command(-A,--add-service)
		CMD_DEL_VS,             //!< Delete VirtualService command(-D, --delete-service)
		CMD_EDIT_VS,            //!< Edit VirtualService command(-E, --edit-service)
		CMD_FLUSH_VS,           //!< Flush VirtualService command(-C, --flush)
		CMD_ADD_RS,             //!< Add RealServer command(-a, --add-server)
		CMD_DEL_RS,             //!< Delete RealServer command(-d, --delete-server)
		CMD_EDIT_RS,            //!< Edit RealServer command(-e.--edit-server)
		CMD_REPLICATION,        //!< Replication command(-R, --replication)
		CMD_LOG,                //!< Logger command(-L, -log)
		CMD_SNMP,               //!< SNMPAgent command(-S, --snmp)
		CMD_PARAMETER,          //!< Parameter command(-P, --parameter)
		CMD_HELP,               //!< Help command(-h, --help)
	};
	enum	REPLICATION_COMMAND_TAG{
		REP_NONE = 0,
		REP_START				// !< REPLICATION START COMMAND
		REP_STOP,				// !< REPLICATION STOP COMMAND
		REP_FORCE,				// !< REPLICATION FORCE COMMAND
		REP_DUMP				// !< REPLICATION DUMP COMMAND
	};

	COMMAND_CODE_TAG			command;
	virtualservice_element		vs_element;
	REPLICATION_CODE_TAG		replication_command;
	LOG_CATEGORY_TAG			log_category;
	LOG_LEVEL_TAG				log_level;
	PARAMETER_COMPONENT_TAG		reload_param;
	PARAMETER_COMPONENT_TAG		snmp_log_category;
	LOG_LEVEL_TAG				snmp_log_level;
	l7vsadm_request() :			command( CMD_NONE ),
								replication_command( REP_NONE ),
								log_category( LOG_CAT_NONE ),
								log_level( LOG_LV_NONE ),
								snmp_log_category( LOG_CAT_NONE ),
								snmp_log_level( LOG_LV_NONE ){}
private:
	friend class	boost::serialization::access;
	template <class Archive > void serialize( Archive& ar, const unsigned int version ){
		ar & command;
		ar & vs_element;
		ar & replication_command;
		ar & log_category;
		ar & log_level;
		ar & snmp_log_category;
		ar & snmp_log_level;
	}
};

//
// l7vsd -> l7vsadm data class
//

class	l7vsd_response{
public:
	enum	COMMAND_RESPONSE_CODE{
		RESPONSE_NONE = 0,
		RESPONSE_OK,
		RESPONSE_LIST_ERROR,
		RESPONSE_LIST_VERBOSE_ERROR,
		RESPONSE_LIST_KEY_ERROR,
		RESPONSE_ADD_VS_ERROR,
		RESPONSE_DEL_VS_ERROR,
		RESPONSE_EDIT_VS_ERROR,
		RESPONSE_FLUSH_VS_ERROR,
		RESPONSE_ADD_RS_ERROR,
		RESPONSE_DEL_RS_ERROR,
		RESPONSE_EDIT_RS_ERROR,
		RESPONSE_REPLICATION_ERROR,
		RESPONSE_LOG_ERROR,
		RESPONSE_SNMP_ERROR,
		RESPONSE_PARAMETER_ERROR,
		RESPONSE_HELP_ERROR
	};
	l7vsadm_request::COMMAND_CODE_TAG
							code;
	
	bool					status;
	
	std::string				message;
	
	std::list< l7vs_virtualservice_element >
							virtualservice_status_list;
	
	REPLICATION_MODE_TAG	replication_mode_status;
	
	std::list< std::pair<LOG_CATEGORY_TAG, LOG_LEVEL_TAG> >
							log_status_list;
							
	bool					snmp_connection_status;
	
	std::list< std::pair<LOG_CATEGORY_TAG, LOG_LEVEL_TAG> >
							snmp_log_status_list;
	
	unsigned long long		total_bps;
	unsigned long long		total_client_recv_byte;
	unsigned long long		total_client_send_byte;
	unsigned long long		total_realserver_recv_byte;
	unsigned long long		total_realserver_send_byte;
	std::vector<virtualservice_element>
							virtualservice_vec;	
private:
	friend class	boost::serialization::access;
	template <class Archive > void serialize( Archive& ar, const unsigned int version ){
		ar & code;
		ar & status;
		ar & message;
		ar & virtualservice_status_list;
		ar & replication_mode_status;
		ar & log_status_list;
		ar & snmp_connection_status;
		ar & snmp_log_status_list;
		ar & total_bps;
		ar & total_client_recv_byte;
		ar & total_client_send_byte;
		ar & total_realserver_recv_byte;
		ar & total_realserver_send_byte;
		ar & virtualservice_vec;
	}
};


}	// namespase l7vsd
#endif	//L7COMMAND_H
