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

// grobal function prototype
static void	sig_exit_handler(int sig);
static int	set_sighandler(int sig, void (*handler)(int));
static int	set_sighandlers();
static void	usage(FILE* p);
static int	l7vsd_main( int, char* );

// grobal variables
static bool	exit_requested = false;
static int	received_sig = 0;

namespace l7vs{

//! virtual_service list command
//! @param[out]	arry of vs_element
//! @return		l7vsd_operation_result
l7vsd::l7vsd_operation_result	l7vsd::list_virtual_service( vsvec_type& out_vslist ){
	l7vsd_operation_result	res;

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	// make vselement array
	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		out_vslist.push_back( (*itr)->get_element() );
	}
	return res;
}

//! virtual_service add command
//! @param[in]	vs_element
//! @return		l7vsd_operation_result
l7vsd::l7vsd_operation_result	l7vsd::add_virtual_service( const virtualservice_element& in_vselement ){
	l7vsd_operation_result	res;

	boost::mutex::scoped_lock command_lock( command_mutex );
	boost::mutex::scoped_lock vslist_lock( vslist_mutex );

	if( vslist.end() == search_vslist( in_vselement ) ){
		// replication null check
		if( NULL == rep ){
			std::string msg("replication pointer is null.");
			Logger::putLogFatal(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);
	
			res.flag = false;
			res.message = msg;
			return res;
		}
		// create virtualservice
		boost::shared_ptr< virtual_service >
			vsptr( new virtual_service( *this, *rep, in_vselement ) );

		virtualservice_base::vs_operation_result	vsres;
		// vs initialize
		vsres = vsptr->initialize();
		if( !vsres ){
			res.flag = false;
			res.message = vsres.message;
			return res;
		}
		// set virtualservice
		vsres = vsptr->set_virtualservice( in_vselement );
		if( !vsres ){
			res.flag = false;
			res.message = vsres.message;
			return res;
		}
		// create thread and run
		vs_threads.create_thread( boost::bind( &virtual_service::run, vsptr ) );

		// add to vslist
		vslist.push_back( vsptr );

		// when first vs, replication switch to master
		if( 1 == vslist.size() ){
			rep->switch_to_master();
		}
	}
	else {
		std::string msg("virtual service already exist.");
		Logger::putLogWarn(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, msg, __FILE__, __LINE__);

		res.flag = false;
		res.message = msg;
		return res;
	}
	return res;
}

//! virtual_service del command
//! @param[in]	vs_element
//! @return		l7vsd_operation_result
l7vsd::l7vsd_operation_result	l7vsd::del_virtual_service( const virtualservice_element& in_vselement ){
}

//! virtual_service edit command
//! @param[in]	vs_element
//! @return		l7vsd_operation_result
l7vsd::l7vsd_operation_result	l7vsd::edit_virtual_service( const virtualservice_element& in_vselement ){
}

//! real_server add command
//! @param[in]	vs_element
//! @return		l7vsd_operation_result
l7vsd::l7vsd_operation_result	l7vsd::add_real_server( const virtualservice_element& in_vselement ){
}

//! vs_list search function
//! @param[in]	vs_element
//! @return		vslist iterator
l7vsd::vslist_type::iterator	l7vsd::search_vslist( const virtualservice_element& in_vselement ){
	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		if( (*itr)->get_element() == in_vselement )	return itr;
	}
	return vslist.end();
}

//! l7vsd run method
void l7vsd::run() {

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

//! display usage
//! @param[in]	file pointer
static void usage( FILE* p ){

}

//! l7vsd main function
int l7vsd_main( int argc, char* argv[] ){
	try{
		l7vs::Logger	logger_instance;
		l7vs::Parameter	parameter_instance;
		

		if (0 > set_sighandlers()) {
			exit(-1);
		}

		l7vs::l7vsd vsd;
		vsd.run();

	}
	catch( ... ){
		return -1;
	}
	return 0;
}

#ifndef	TEST_CASE
//! main function
int main( int argc, char* argv[] ){
	return l7vsd_main( argc, argv );
}
#endif	//TEST_CASE
