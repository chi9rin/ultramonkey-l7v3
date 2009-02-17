//
//			module test utility.
//
//
//
#include <string>
#include <list>
#include <map>
#include <time.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/nondet_random.hpp>
#include <boost/bind.hpp>
// #include <boost/lambda/lambda.hpp>
#include <boost/thread.hpp>
#include <boost/test/included/unit_test.hpp>
#include <boost/filesystem.hpp>
#include "protocol_module_control.h"
#include "schedule_module_control.h"
#include "data_creater.h"

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

string	targetdir;
string	targetfile;

unsigned int get_random_value( unsigned int max, unsigned int min ){
	random_device	dev;
	unsigned int val = dev();
	val %= max + 1;
	if( val < min ) val += min - val;
	return val;
}

string	string_address_generator( bool ipv6 = false ){
	if( !ipv6 ){
		format	frmt( "%1%.%2%.%3%.%4%" );
			frmt
			% get_random_value( 255, 10 )
			% get_random_value( 255, 10 )
			% get_random_value( 255, 10 )
			% get_random_value( 255, 10 );
		return frmt.str();
	}
	else{
		format frmt( "%x:%x:%x:%x:%x:%x:%x" );
			frmt
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 )
			% get_random_value( 0xffff, 0x1 );
		return frmt.str();
	}
}


//
// logger functions
//
LOG_LEVEL_TAG	get_loglevel_func(){ return LOG_LV_DEBUG; }
void			log_put_func( const unsigned int no, const string& mes, const char* file, int line ){
	format	frmt( "%1% : %2% [%3%]   %4%" );
	frmt % file % line % no % mes;
	BOOST_MESSAGE( frmt.str() );
}
//
// list_iterator_func
//
list<realserver>::iterator	begin_func( list<realserver>& inlist ){ return inlist.begin(); }
list<realserver>::iterator	end_func( list<realserver>& inlist ){ return inlist.end(); }
list<realserver>::iterator	next_func( list<realserver>::iterator& itr ){ return itr.operator++(); }
void	listlock(){}
void	listunlock(){}

//
// module thread func
void	module_upthread_func( protocol_module_base* prot ){
	asio::ip::tcp::endpoint			point;

	list< realserver >	serverlist;
	{
		unsigned int num = get_random_value( 50 , 2 );
		for( unsigned int i = 0; i < num; ++i ){
			realserver	server;
			server.tcp_endpoint = asio::ip::tcp::endpoint( asio::ip::address::from_string( string_address_generator() ), get_random_value( 65535, 10000 ) );
			serverlist.push_back( server );
		}
	}
	prot->initialize(	bind( &begin_func, serverlist ),
						bind( &end_func, serverlist ),
						bind( &next_func, _1 ),
						function< void ( void ) >( &listlock ),
						function< void ( void ) >( &listunlock ) );
	data_creater	data;
	array<char, MAX_BUFFER_SIZE>	buf;
	size_t							length;
	string							dataname;
	data.create_data( buf, length, dataname );

	//parameter created
	vector<string>	args;
	prot->check_parameter( args );
	prot->set_parameter( args );

	protocol_module_base::EVENT_TAG	event = protocol_module_base::ACCEPT;
	while( event != protocol_module_base::FINALIZE ){
		switch( event ){
			case	protocol_module_base::ACCEPT:
					event = prot->handle_accept( this_thread::get_id() );
				break;
			case	protocol_module_base::CLIENT_RECV:
					event = prot->handle_client_recv( this_thread::get_id(), buf, length );
				break;
			case	protocol_module_base::REALSERVER_SELECT:
					event = prot->handle_realserver_select( this_thread::get_id(), serverlist.begin()->tcp_endpoint );
				break;
			case	protocol_module_base::REALSERVER_CONNECT:
					event = prot->handle_realserver_connect( this_thread::get_id(), buf, length );
				break;
			case	protocol_module_base::REALSERVER_SEND:
					event = prot->handle_realserver_send( this_thread::get_id() );
				break;
			case	protocol_module_base::SORRYSERVER_SELECT:
					event = prot->handle_sorryserver_select( this_thread::get_id(), point );
				break;
			case	protocol_module_base::SORRYSERVER_CONNECT:
					event = prot->handle_sorryserver_connect(this_thread::get_id(),buf, length );
				break;
			case	protocol_module_base::SORRYSERVER_SEND:
					event = prot->handle_sorryserver_send( this_thread::get_id() );
				break;
			case	protocol_module_base::CLIENT_RESPONSE_SEND:
					event = prot->handle_response_send_inform( this_thread::get_id() );
				break;
			case	protocol_module_base::REALSERVER_DISCONNECT:
					event = prot->handle_realserver_disconnect( this_thread::get_id(), asio::ip::tcp::endpoint() );
				break;
			case	protocol_module_base::SORRYSERVER_DISCONNECT:
					event = prot->handle_sorryserver_disconnect( this_thread::get_id(), asio::ip::tcp::endpoint() );
				break;
			case	protocol_module_base::CLIENT_DISCONNECT:
					event = prot->handle_client_disconnect( this_thread::get_id() );
				break;
			default:
				break;
		}
	}
	prot->finalize();
}

void	module_downthread_func( protocol_module_base* prot ){
	asio::ip::tcp::endpoint	point;
	array<char,MAX_BUFFER_SIZE>	buf;
	size_t	length;
	protocol_module_base::EVENT_TAG	event = protocol_module_base::REALSERVER_RECV;
	while( event != protocol_module_base::FINALIZE ){
		switch( event ){
			case	protocol_module_base::REALSERVER_RECV:
				event = prot->handle_realserver_recv( this_thread::get_id(), point, buf, length );
				break;
			case	protocol_module_base::SORRYSERVER_RECV:
				event = prot->handle_sorryserver_recv( this_thread::get_id(), point, buf, length );
				break;
			case	protocol_module_base::CLIENT_CONNECTION_CHECK:
				event = prot->handle_client_connection_check( this_thread::get_id(), buf, length );
				break;
			case	protocol_module_base::CLIENT_SEND:
				event = prot->handle_client_send( this_thread::get_id() );
				break;
			case	protocol_module_base::REALSERVER_DISCONNECT:
				event = prot->handle_realserver_disconnect( this_thread::get_id(), point );
				break;
			case	protocol_module_base::SORRYSERVER_DISCONNECT:
				event = prot->handle_sorryserver_disconnect( this_thread::get_id(), point );
				break;
			case	protocol_module_base::CLIENT_DISCONNECT:
				event = prot->handle_client_disconnect( this_thread::get_id() );
				break;
			default:
				break;
		}
	}
	prot->finalize();
}


// protocol_module_test
void	protocol_module_test(){
	protocol_module_control& ctl = protocol_module_control::getInstance();
	ctl.initialize( targetdir );
	protocol_module_base*	prot = ctl.load_module( targetfile );
	BOOST_CHECK( prot );
	BOOST_MESSAGE( "PROTOCOL_MODULE TEST : init logger function" );
	module_base::logger_func_type		logfunc = &log_put_func;
	module_base::getloglevel_func_type	getlevelfunc = &get_loglevel_func;
	prot->init_logger_functions( get_loglevel_func, logfunc, logfunc, logfunc, logfunc, logfunc );

	BOOST_MESSAGE( "PROTOCOL_MODULE TEST : module initialize" );

	unsigned int thread_count = get_random_value( 100, 10 );
	{
		format	frmt( "PROTOCOL_MODULE TEST : create thread num = %1%" );
		frmt % thread_count;
		BOOST_MESSAGE( frmt.str() );
	}
	thread_group	tg;
	BOOST_MESSAGE( "PROTOCOL_MODULE TEST : create threads" );
	for( unsigned int i = 0 ; i < thread_count; ++i ){
		tg.create_thread( bind( &module_upthread_func, prot ) );
		tg.create_thread( bind( &module_downthread_func, prot ) );
	}
	BOOST_MESSAGE( "PROTOCOL_MODULE TEST : main thread waiting" );
	tg.join_all();
	BOOST_MESSAGE( "PROTOCOL_MODULE TEST : all thread is die. test end." );
}


// schedule_module_test
//
//
//

void	schedule_thread_func( schedule_module_base* sched ){
	//data setting
	asio::ip::tcp::endpoint	accept_point = asio::ip::tcp::endpoint( asio::ip::address::from_string( string_address_generator() ), get_random_value( 65535, 10000 ) );

//	boost::lambda::placeholder1_type	ph1_type;

	list< realserver >	serverlist;
	{
		unsigned int num = get_random_value( 50 , 2 );
		for( unsigned int i = 0; i < num; ++i ){
			realserver	server;
			server.tcp_endpoint = asio::ip::tcp::endpoint( asio::ip::address::from_string( string_address_generator() ), get_random_value( 65535, 10000 ) );
			serverlist.push_back( server );
		}
	}
	// thread loop
	for( unsigned int i = 0; i < 1024; ++i ){
			asio::ip::tcp::endpoint point;
			sched->handle_schedule( this_thread::get_id(),
				bind( &begin_func, serverlist ),
				bind( &end_func, serverlist ),
				bind( &next_func, _1 ),
				point );
		struct	timespec	spec;
		spec.tv_sec = 0;
		spec.tv_nsec = static_cast<long>( get_random_value( 1000, 10 ) );
		nanosleep( &spec, NULL );
	}
}

void	schedule_module_test(){
	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : controller load" );
	schedule_module_control& ctl = schedule_module_control::getInstance();

	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : controller initialize" );
	ctl.initialize( targetdir );

	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : controller module load" );
	schedule_module_base*	sched = ctl.load_module( targetfile );
	BOOST_CHECK( sched );

	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : init logger function" );
	module_base::logger_func_type		logfunc = &log_put_func;
	module_base::getloglevel_func_type	getlevelfunc = &get_loglevel_func;
	sched->init_logger_functions( get_loglevel_func, logfunc, logfunc, logfunc, logfunc, logfunc );

	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : module initialize" );
	sched->initialize();

	unsigned int thread_count = get_random_value( 100, 10 );
	{
		format	frmt( "SCHEDULE_MODUE TEST : create thread num = %1%" );
		frmt % thread_count;
		BOOST_MESSAGE( frmt.str() );
	}
	thread_group	tg;
	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : create threads" );
	for( unsigned int i = 0 ; i < thread_count; ++i )
		tg.create_thread( bind( &schedule_thread_func, sched ) );
// 	tg.create_thread( &schedule_thread_func );
	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : main thread waiting" );
	tg.join_all();
	BOOST_MESSAGE( "SCHEDULE_MODULE TEST : all thread is die. test end." );
}

void	usage(){
	cout << " modtester [-s|-p] modulename \n"
			"	-s is shcedule module test.\n"
			"	-p is schedule module test.\n"
			"	modulename is loading schedule module file \n"
			"		copyright NTTComware corporation. 2009- " << endl;
}


test_suite*     init_unit_test_suite( int argc, char* argv[] ){
	// argc check
	if( argc != 3 ){
		usage();
		return 0;
	}

	// command check
	if( string( argv[1] ) != string( "-s" ) &&
		string( argv[1] ) != string( "-p" ) ){
		cout << "\n\n commad not found : " << argv[1] << endl;
		usage();
		return 0;
	}

	// module file check
	filesystem::path modulename( argv[2] );
	if( !filesystem::exists( modulename ) ){
		cout << "\n\n not found shared file : " << modulename.string() << "\n\n" << endl;
		usage();
		return 0;
	}

	targetdir  = modulename.branch_path().string();
	targetfile = modulename.leaf();
	targetfile = targetfile.substr( 2, targetfile.length() - 3 );

	test_suite*	ts = BOOST_TEST_SUITE( "modtester" );

	if( string( argv[1] ) == string( "-s" ) ) ts->add( BOOST_TEST_CASE( &schedule_module_test ) );
	if( string( argv[1] ) == string( "-p" ) ) ts->add( BOOST_TEST_CASE( &protocol_module_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
