//
//!	@file	l7vsd.cpp
//!	@brief	l7vsd main class
//
//	copyright (c) sdy corporation. 2009
//	mail: a dot takamaru at sdy dot co dot jp
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <boost/shared_ptr.hpp>

#include "l7vsd.h"
#include "error_code.h"

// global function prototype
static void	sig_exit_handler(int sig);
static int	set_sighandler(int sig, void (*handler)(int));
static int	set_sighandlers();
int	l7vsd_main( int, char** );

// global variables
static bool	exit_requested = false;
static int	received_sig = 0;

namespace l7vs{

//! constructor
l7vsd::l7vsd()
	:	help(false),
		debug(false){

	option_dic["-h"]		= boost::bind( &l7vsd::parse_help, this, _1, _2, _3 );
	option_dic["--help"]	= boost::bind( &l7vsd::parse_help, this, _1, _2, _3 );
	option_dic["-d"]		= boost::bind( &l7vsd::parse_debug, this, _1, _2, _3 );
	option_dic["--debug"]	= boost::bind( &l7vsd::parse_debug, this, _1, _2, _3 );

	starttime = boost::posix_time::second_clock::local_time();
}

//! destructor
l7vsd::~l7vsd(){}

//! virtual_service list command
//! @param[out]	arry of vs_element
//! @param[out]	error_code
void	l7vsd::list_virtual_service( vselist_type* out_vslist, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::list_virtual_service", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !out_vslist ){
		std::string msg("out_vslist pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	// make vselement list
	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		out_vslist->push_back( (*itr)->get_element() );
	}
}

//! virtual_service list verbose command
//! @param[out]	arry of vs_element
//! @param[out]	error_code
void	l7vsd::list_virtual_service_verbose( l7vsd_response* response, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::list_virtual_service_verbose", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !response ){
		std::string msg("response pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !bridge ){
		std::string msg("bridge pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !rep ){
		std::string msg("rep pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	unsigned long long	total_client_recv_byte = 0ULL;
	unsigned long long	total_client_send_byte = 0ULL;
	unsigned long long	total_realserver_recv_byte = 0ULL;
	unsigned long long	total_realserver_send_byte = 0ULL;

	// make vselement list
	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		response->virtualservice_status_list.push_back( (*itr)->get_element() );
		// calc send recv bytes
		total_client_recv_byte += (*itr)->get_up_recv_size();
		total_client_send_byte += (*itr)->get_down_send_size();
		total_realserver_recv_byte += (*itr)->get_down_recv_size();
		total_realserver_send_byte += (*itr)->get_up_send_size();
	}

	// get_replication_mode
	response->replication_mode_status = rep->get_status();

	// get all category log level
	Logger::getLogLevelAll( response->log_status_list );

	// get snmp connect status
	response->snmp_connection_status = bridge->get_connectionstate();

	// get snmp all category log level
	typedef std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG > snmplogmap_type; 
	snmplogmap_type snmplogmap;
	bridge->get_loglevel_allcategory( snmplogmap );
	for( snmplogmap_type::iterator itr = snmplogmap.begin();
		 itr != snmplogmap.end();
		 ++itr ){
		response->snmp_log_status_list.push_back( *itr );
	}

	// calc total bps
	unsigned long long	total_bytes =
		total_client_recv_byte +
		total_client_send_byte +
		total_realserver_recv_byte +
		total_realserver_send_byte;

	boost::posix_time::ptime	now =
		boost::posix_time::second_clock::local_time();
	boost::posix_time::time_duration	dur = ( now - starttime );
	if( 0ULL != dur.total_seconds() )
		response->total_bps = ( total_bytes / dur.total_seconds() );
	else
		response->total_bps = 0ULL;

	response->total_client_recv_byte = total_client_recv_byte;
	response->total_client_send_byte = total_client_send_byte;
	response->total_realserver_recv_byte = total_realserver_recv_byte;
	response->total_realserver_send_byte = total_realserver_send_byte;

}

//! virtual_service add command
//! @param[in]	vs_element
//! @param[out]	error_code
void	l7vsd::add_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::add_virtual_service", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !in_vselement ){
		std::string msg("in_vselement pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( vslist.end() == search_vslist( *in_vselement ) ){
		// replication null check
		if( NULL == rep ){
			std::string msg("replication pointer is null.");
			Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
	
			err.setter( true, msg );
			return;
		}
		// create virtualservice
		virtualservice_ptr	vsptr;
		try{
			vsptr.reset( new virtual_service( *this, *rep, *in_vselement ) );
		}
		catch( std::bad_alloc& ){
			std::string msg("virtualservice create failed.");
			Logger::putLogError(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}
		if( NULL == vsptr ){
			std::string msg("virtualservice pointer is null.");
			Logger::putLogError(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}

		// vs initialize
		vsptr->initialize( err );
		if( err )	return;

		// set virtualservice
		vsptr->set_virtualservice( *in_vselement, err );
		if( err )	return;

		// create thread and run
		vs_threads.create_thread( boost::bind( &virtual_service::run, vsptr ) );

		// add to vslist
		vslist.push_back( vsptr );

		// when first vs, replication switch to master
		if( 1U == vslist.size() ){
			rep->switch_to_master();
		}
	}
	else {
		std::string msg("virtual service already exist.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		err.setter( true, msg );
		return;
	}
}

//! virtual_service del command
//! @param[in]	vs_element
//! @param[out]	error_code
void	l7vsd::del_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::del_virtual_service", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !in_vselement ){
		std::string msg("in_vselement pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		// vs stop
		(*vsitr)->stop();
		// vs finalize
		(*vsitr)->finalize( err );

		// when first vs, replication switch to slave
		if( 0U == vslist.size() ){
			rep->switch_to_slave();
		}
	}
	else {
		std::string msg("virtual service not found.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		err.setter( true, msg );
		return;
	}
}

//! virtual_service edit command
//! @param[in]	vs_element
//! @param[out]	error_code
void	l7vsd::edit_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::edit_virtual_service", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !in_vselement ){
		std::string msg("in_vselement pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		// edit virtualservice
		(*vsitr)->edit_virtualservice( *in_vselement, err );
		if( err )	return;
	}
	else {
		std::string msg("virtual service not found.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		err.setter( true, msg );
		return;
	}
}

//! real_server add command
//! @param[in]	vs_element
//! @param[out]	error_code
void	l7vsd::add_real_server( const virtualservice_element* in_vselement, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::add_real_server", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !in_vselement ){
		std::string msg("in_vselement pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		// add realserver
		(*vsitr)->add_realserver( *in_vselement, err );
		if( err )	return;
	}
	else {
		std::string msg("virtual service not found.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		err.setter( true, msg );
		return;
	}
}

//! real_server del command
//! @param[in]	vs_element
//! @param[out]	error_code
void	l7vsd::del_real_server( const virtualservice_element* in_vselement, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::del_real_server", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !in_vselement ){
		std::string msg("in_vselement pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		// del realserver
		(*vsitr)->del_realserver( *in_vselement, err );
		if( err )	return;
	}
	else {
		std::string msg("virtual service not found.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		err.setter( true, msg );
		return;
	}
}

//! real_server edit command
//! @param[in]	vs_element
//! @param[out]	error_code
void	l7vsd::edit_real_server( const virtualservice_element* in_vselement, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::edit_real_server", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !in_vselement ){
		std::string msg("in_vselement pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		// del realserver
		(*vsitr)->edit_realserver( *in_vselement, err );
		if( err )	return;
	}
	else {
		std::string msg("virtual service not found.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		err.setter( true, msg );
		return;
	}
}

//! virtual_service flush command
//! @param[out]	error_code
void	l7vsd::flush_virtual_service( error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::flush_virtual_service", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	// all virtualservice stop and finalize
	for(;;){
		vslist_type::iterator itr = vslist.begin();
		if(vslist.end() == itr){
			break;
		}
		else{
			// vs stop
			(*itr)->stop();
			// vs finalize
			(*itr)->finalize( err );
		}
	}	

	// join virtualservice threads
	vs_threads.join_all();

	// replication switch to slave
	rep->switch_to_slave();
}

//! replication command
//! @param[in]	replicaiton command
//! @param[out]	error_code
void	l7vsd::replication_command( const l7vsadm_request::REPLICATION_COMMAND_TAG* cmd, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::replication_command", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !cmd ){
		std::string msg("cmd pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !rep ){
		std::string msg("rep pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	switch( *cmd ){
	case	l7vsadm_request::REP_START:
		rep->start();
		break;
	case	l7vsadm_request::REP_STOP:
		rep->stop();
		break;
	case	l7vsadm_request::REP_FORCE:
		rep->force_replicate();
		break;
	case	l7vsadm_request::REP_DUMP:
		rep->dump_memory();
		break;
	default:
		std::string msg("invalid replication command.");
		Logger::putLogError(LOG_CAT_L7VSD_REPLICATION, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
}

//! loglevel set command
//! @param[in]	log category
//! @param[in]	log level
//! @param[out]	error_code
void	l7vsd::set_loglevel( const LOG_CATEGORY_TAG* cat, const LOG_LEVEL_TAG* level, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::set_loglevel", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !cat ){
		std::string msg("cat pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !level ){
		std::string msg("level pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( LOG_CAT_END == *cat ){
		// set loglevel all
		if( !Logger::setLogLevelAll( *level ) ){
			std::string msg("set loglevel all failed.");
			Logger::putLogError(LOG_CAT_L7VSD_LOGGER, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}
	}
	else{
		if( !Logger::setLogLevel( *cat, *level ) ){
			std::string msg("set loglevel failed.");
			Logger::putLogError(LOG_CAT_L7VSD_LOGGER, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}
	}
}

//! snmp loglevel set command
//! @param[in]	log category
//! @param[in]	log level
//! @param[out]	error_code
void	l7vsd::snmp_set_loglevel( const LOG_CATEGORY_TAG* cat, const LOG_LEVEL_TAG* level, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::snmp_set_loglevel", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !cat ){
		std::string msg("cat pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !level ){
		std::string msg("level pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !bridge ){
		std::string msg("bridge pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( LOG_CAT_END == *cat ){
		// set loglevel all
		if( 0 != bridge->change_loglevel_allcategory( *level ) ){
			std::string msg("set snmp loglevel all failed.");
			Logger::putLogError(LOG_CAT_L7VSD_LOGGER, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}
	}
	else{
		if( 0 != bridge->change_loglevel( *cat, *level ) ){
			std::string msg("set snmp loglevel failed.");
			Logger::putLogError(LOG_CAT_L7VSD_LOGGER, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}
	}
}

//! reload parameter command
//! @param[in]	reload component
//! @param[out]	error_code
void	l7vsd::reload_parameter( const PARAMETER_COMPONENT_TAG* comp, error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::reload_parameter", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !comp ){
		std::string msg("comp pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !rep ){
		std::string msg("rep pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
	if( !bridge ){
		std::string msg("bridge pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_MAINTHREAD, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	Parameter	param;
	Logger		logger_instance;
	if( param.read_file( *comp ) ){
		switch( *comp ){
		case	PARAM_COMP_REPLICATION:
			rep->reset();
			break;
		case	PARAM_COMP_LOGGER:
			logger_instance.loadConf();
			break;
		case	PARAM_COMP_SNMPAGENT:
			bridge->reload_config();
			break;
		default:
			std::string msg("parameter reload command not found.");
			Logger::putLogWarn(LOG_CAT_L7VSD_PARAMETER, 1, msg, __FILE__, __LINE__);
			err.setter( true, msg );
			return;
		}
	}
	else{
		std::string msg("parameter reload failed.");
		Logger::putLogError(LOG_CAT_L7VSD_PARAMETER, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}
}

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

//! virtualservice release from vslist
//! @param[in]	vs_element
void	l7vsd::release_virtual_service( const virtualservice_element& in_vselement ) const {
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::release_virtual_service", __FILE__, __LINE__ );

	vslist_type::iterator vsitr = search_vslist( in_vselement );
	if( vslist.end() !=  vsitr ){
		// remove from vslist
		vslist.remove( *vsitr );
	}
}
//! virtualservice_list getter
//! @return	vslist
l7vsd::vslist_type&	l7vsd::get_virtualservice_list(){
	return vslist;
}

//! virtualservice_list mutex getter
//! @return	vslist_mutex
boost::mutex&	l7vsd::get_virtualservice_list_mutex(){
	return vslist_mutex;
}

//! l7vsd run method
//! @param[in]	argument count
//! @param[in]	argument value
int	l7vsd::run( int argc, char* argv[] ) {
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::run", __FILE__, __LINE__ );

	// check options
	if( !check_options( argc, argv ) ){
		std::cerr << usage() << std::endl;
		return -1;
	}

	// help mode ?
	if( help ){
		std::cout << usage() <<std::endl;
		return 0;
	}

	// debug mode ?
	if( !debug ){
		int ret = daemon( 0, 0 );
		if( 0 > ret ){
			std::stringstream buf;
			buf << "daemon() failed: " << strerror( errno );
			logger.putLogError( LOG_CAT_L7VSD_MAINTHREAD, 1, buf.str(), __FILE__, __LINE__ );
			return -1;
		}
	}

	// protoclol module control initialize
	protocol_module_control::getInstance().initialize( L7VS_MODULE_PATH );

	// schedule module control initialize
	schedule_module_control::getInstance().initialize( L7VS_MODULE_PATH );

	// receiver initialize
	receiver = command_receiver_ptr( new command_receiver( dispatcher, L7VS_CONFIG_SOCKNAME, *this ) );
	if( NULL ==  receiver ){
		logger.putLogFatal( LOG_CAT_L7VSD_MAINTHREAD, 1, "command receiver create error.", __FILE__, __LINE__ );
		return -1;
	}

	// replication initialize
	rep = replication_ptr( new replication( dispatcher ) );
	if( NULL ==  rep ){
		logger.putLogFatal( LOG_CAT_L7VSD_MAINTHREAD, 1, "replication create error.", __FILE__, __LINE__ );
		return -1;
	}
	if( 0 > rep->initialize() ){
		logger.putLogError( LOG_CAT_L7VSD_MAINTHREAD, 1, "replication initialize failed.", __FILE__, __LINE__ );
		return -1;
	}

	// snmp bridge initialize
	bridge = snmpbridge_ptr( new snmpbridge( *this, dispatcher ) );
	if( NULL ==  bridge ){
		logger.putLogFatal( LOG_CAT_L7VSD_MAINTHREAD, 1, "snmpbridge create error.", __FILE__, __LINE__ );
		return -1;
	}
	if( 0 > bridge->initialize() ){
		logger.putLogError( LOG_CAT_L7VSD_MAINTHREAD, 1, "snmpbridge initialize failed.", __FILE__, __LINE__ );
		return -1;
	}

	// snmp trap function set
	Logger::setSnmpSendtrap( boost::bind( &snmpbridge::send_trap, bridge, _1 ) );

	// main loop
	for(;;){
		if( exit_requested )	break;
		dispatcher.poll();
	}

	// snmp trap function unset
	Logger::setSnmpSendtrap( NULL );

	// snmp bridge finalize
	bridge->finalize();

	// replication finalize
	rep->finalize();

	// schedule module control finalize
	schedule_module_control::getInstance().finalize();

	// protpcol module control finalize
	protocol_module_control::getInstance().finalize();

	return 0;
}

//! command option check
//! @param[in]	argument count
//! @param[in]	argument value
bool	l7vsd::check_options( int argc, char* argv[] ){
	for( int pos = 1; pos < argc; ++pos ){	// check options.
		parse_opt_map_type::iterator itr = option_dic.find( argv[pos] );
		if( itr != option_dic.end() ){	// find option
			if( !itr->second( pos, argc, argv ) ){
				return false;	// option function execute.
			}
		}
		else{	// don't find option function.
			std::stringstream buf;
			buf << "l7vsd: unknown option: " << argv[ pos ] << "\n";
			std::cerr << buf << std::endl;
			return false;
		}
	}
	return true;
}

//! command help parse
//! @param[in]	argument count
//! @param[in]	argument value
bool	l7vsd::parse_help(int& pos, int argc, char* argv[] ){
	help = true;		//help_mode flag on
	return true;
}

//! command debug parse
//! @param[in]	argument count
//! @param[in]	argument value
bool	l7vsd::parse_debug(int& pos, int argc, char* argv[] ){
	debug = true;		//debug_mode flag on
	return true;
}

//! create usage string
//! @return		usage string
std::string	l7vsd::usage(){
	std::stringstream	stream;
	stream <<
	"Usage: l7vsd [-d] [-h]\n"
	"   -d    --debug        run in debug mode (in foreground)\n"
	"   -h    --help         print this help messages and exit\n"
	<< std::endl;
	return stream.str();
}

};// namespace l7vsd

//! signal handler function
//! @param[in]	signal
static void sig_exit_handler( int sig ){
	received_sig = sig;
	exit_requested = true;
}

//! set singal handler function
//! @param[in]	signal
//! @param[in]	handler function pointer
//! @return		0/success, -1/fail
static int set_sighandler( int sig, void ( *handler )( int ) ){
	struct	sigaction act;
	int		ret;

	ret = sigaction( sig, NULL, &act );
	if( 0 > ret )	return ret;

	act.sa_flags &= ~SA_RESETHAND;
	act.sa_handler = handler;

	ret = sigaction( sig, &act, NULL );
	if( 0 > ret )	return ret;

	return 0;
}

//! set all singal handler function
//! @return		0/success, -1/fail
static int set_sighandlers() {
	int ret;

#define SET_SIGHANDLER(sig, handler)				\
	do {											\
		ret = set_sighandler((sig), (handler));		\
		if (ret < 0) {								\
			return ret;								\
		}											\
	} while (0)

	SET_SIGHANDLER( SIGHUP,		sig_exit_handler );
	SET_SIGHANDLER( SIGINT,		sig_exit_handler );
	SET_SIGHANDLER( SIGQUIT,	sig_exit_handler );
	SET_SIGHANDLER( SIGTERM,	sig_exit_handler );
	SET_SIGHANDLER( SIGUSR1,	SIG_IGN );
	SET_SIGHANDLER( SIGUSR2,	SIG_IGN );
	SET_SIGHANDLER( SIGALRM,	SIG_IGN );
	SET_SIGHANDLER( SIGCHLD,	SIG_IGN );

#undef SET_SIGHANDLER

    return 0;
}

//! l7vsd main function
int l7vsd_main( int argc, char* argv[] ){
	int ret = 0;
	try{
		l7vs::Logger	logger_instance;
		l7vs::Parameter	parameter_instance;
		logger_instance.loadConf();

		if ( 0 > set_sighandlers() )	return -1;

		l7vs::l7vsd vsd;
		ret =  vsd.run( argc, argv );

	}
	catch( ... ){
		return -1;
	}
	return ret;
}

#ifndef	TEST_CASE
//! main function
int main( int argc, char* argv[] ){
	return l7vsd_main( argc, argv );
}
#endif	//TEST_CASE
