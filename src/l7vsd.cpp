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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
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
void	l7vsd::list_virtual_service_verbose(	vselist_type* out_vslist, 
												replication::REPLICATION_MODE_TAG* repmode,
												logstatus_list_type* logstatus,
												bool*	snmpstatus,
												logstatus_list_type* snmplogstatus,
												error_code& err ){
	Logger	logger( LOG_CAT_L7VSD_MAINTHREAD, 1, "l7vsd::list_virtual_service_verbose", __FILE__, __LINE__ );

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( !out_vslist ){
		std::string msg("out_vslist pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( !repmode ){
		std::string msg("repmode pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( !logstatus ){
		std::string msg("logstatus pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( !snmpstatus ){
		std::string msg("snmpstatus pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( !snmplogstatus ){
		std::string msg("snmplogstatus pointer is null.");
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	// make vselement list
	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		out_vslist->push_back( (*itr)->get_element() );
	}

	// get_replication_mode
	*repmode = rep->get_status();

	// get all category log level
	Logger::getLogLevelAll( *logstatus );

	// get snmp connect status
	*snmpstatus = bridge->get_connectionstate();

	// get snmp all category log level
	typedef std::map< LOG_CATEGORY_TAG, LOG_LEVEL_TAG > snmplogmap_type; 
	snmplogmap_type snmplogmap;
	bridge->get_loglevel_allcategory( snmplogmap );
	for( snmplogmap_type::iterator itr = snmplogmap.begin();
		 itr != snmplogmap.end();
		 ++itr ){
		snmplogstatus->push_back( *itr );
	}
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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
		err.setter( true, msg );
		return;
	}

	if( vslist.end() == search_vslist( *in_vselement ) ){
		// replication null check
		if( NULL == rep ){
			std::string msg("replication pointer is null.");
			Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
	
			err.setter( true, msg );
			return;
		}
		// create virtualservice
		boost::shared_ptr< virtual_service >
			vsptr( new virtual_service( *this, *rep, *in_vselement ) );

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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
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
		Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
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

//! l7vsd run method
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

	// replication initialize
	rep = replication_ptr( new replication( dispatcher ) );
	if( 0 > rep->initialize() ){
		std::stringstream buf;
		buf << "replication initialize failed.";
		logger.putLogError( LOG_CAT_L7VSD_MAINTHREAD, 1, buf.str(), __FILE__, __LINE__ );
		return -1;
	}

	// snmp bridge initialize
	bridge = snmpbridge_ptr( new snmpbridge( *this, dispatcher ) );
	if( 0 > bridge->initialize() ){
		std::stringstream buf;
		buf << "snmpbridge initialize failed.";
		logger.putLogError( LOG_CAT_L7VSD_MAINTHREAD, 1, buf.str(), __FILE__, __LINE__ );
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

bool	l7vsd::parse_help(int& pos, int argc, char* argv[] ){
	help = true;		//help_mode flag on
	return true;
}

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
