#include <boost/test/included/unit_test.hpp>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "error_code.h"
#include "logger.h"
#include "parameter.h"

#include "l7vs_command.h"
//#include "command_receiver_stub.h"
#include "virtualservice_stub.h"
#include "realserver_stub.h"
#include "replication_stub.h"
#include "snmpbridge_stub.h"
#include "protocol_module_control_stub.h"
#include "schedule_module_control_stub.h"
#include "l7vsd_stub.h"
#include "test_data.h"

#include "../../src/command_receiver.cpp"
#include "../../src/command_session.cpp"

using namespace boost::unit_test_framework;

// class	l7vsd_test	:public	l7vs::l7vsd {
// public:
// 	l7vsd_test() : list_vs_called(false){}
// 
// 	vslist_type&	get_vslist(){ return vslist; }
// 	boost::thread_group&	get_tg(){ return vs_threads; }
// 
// 	bool&	get_help() { return help; }
// 	bool&	get_debug() { return debug; }
// 
// 	vslist_type::iterator	search_vslist( l7vs::virtualservice_element& in_elem ){
// 		return l7vsd::search_vslist( in_elem );
// 	}
// 	void	set_replication( boost::shared_ptr< l7vs::replication > inrep ){
// 		rep = inrep;
// 	}
// 
// };

//variable
boost::asio::io_service			client_io;
unsigned int list_err_cnt;
unsigned int	method_called_err;

bool	exit_flag = false;

using boost::asio::local::stream_protocol;
stream_protocol::socket	unixsocket( client_io );

//util
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint< InternetProtocol > string_to_endpoint( std::string str ){
	std::string::size_type pos = str.find( ":" );
	std::string	hostname = str.substr( 0, pos );
	std::string	portname = str.substr( pos+1, str.length() );
	boost::asio::io_service		io_service;
	typename InternetProtocol::resolver				resolver(io_service);
	typename InternetProtocol::resolver::query		query( hostname, portname );
	typename InternetProtocol::resolver::iterator	end;
	typename InternetProtocol::resolver::iterator	itr = resolver.resolve( query );
	if( itr == end ) return typename InternetProtocol::endpoint();
	return *itr;
}

//--tests--


void	cmd_thread( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd ){
	using boost::asio::local::stream_protocol;

	boost::array<char,65535>	buf;

	stream_protocol::socket	s( client_io );
	s.connect(stream_protocol::endpoint("./l7vs"));
	std::cout << "connected" << std::endl;

	for(unsigned int i = 0; i < 100; ++i){
		std::cout << "cmd:" << cmd << "n:" << i << std::endl;

		l7vs::l7vsadm_request	request;
		request.command = cmd;
		std::cout << "request_created" << std::endl;
	
		std::stringstream	send_stream;
		boost::archive::text_oarchive	oa( send_stream );
		oa << (const l7vs::l7vsadm_request&) request;
		std::cout << "write data is " << send_stream.str() << std::endl;
	
		std::cout << "send" << std::endl;
		boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );
		std::cout << "recv" << std::endl;
		s.read_some( boost::asio::buffer( buf ) );
		
		std::stringstream	recv_stream;
		recv_stream << &(buf[0]);
		boost::archive::text_iarchive	ia( recv_stream );
		l7vs::l7vsd_response	response;
		ia >> response;
		if(response.status != l7vs::l7vsd_response::RESPONSE_OK){
			++list_err_cnt;
		}
	}
	
}

void	rep_call(){
	using boost::asio::local::stream_protocol;

	boost::array<char,65535>	buf;

	stream_protocol::socket	s( client_io );
	s.connect(stream_protocol::endpoint("./l7vs"));
	std::cout << "connected" << std::endl;

	for(unsigned int i = 0; i < 1; ++i){

	
		l7vs::l7vsadm_request	request;
		request.command = l7vs::l7vsadm_request::CMD_REPLICATION;
		request.replication_command = l7vs::l7vsadm_request::REP_FORCE;
		std::cout << "request_created" << std::endl;
	
		std::stringstream	send_stream;
		boost::archive::text_oarchive	oa( send_stream );
		oa << (const l7vs::l7vsadm_request&) request;
	
		std::cout << "write data is " << send_stream.str() << std::endl;
	
		std::cout << "send" << std::endl;
		boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );
		s.read_some( boost::asio::buffer( buf ) );
	
		
		std::stringstream	recv_stream;
		recv_stream << &(buf[0]);
		boost::archive::text_iarchive	ia( recv_stream );
		l7vs::l7vsd_response	response;
		ia >> response;
		if(response.status != l7vs::l7vsd_response::RESPONSE_OK){
			++list_err_cnt;
		}
	}
	
}




void	thread_test(){
	BOOST_MESSAGE( "----- thread_test start -----" );
	
	boost::thread	thd1( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_LIST ) );
	boost::thread	thd2( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_LIST_VERBOSE ) );
	boost::thread	thd3( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_LIST_KEY ) );
	boost::thread	thd4( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_ADD_VS ) );
	boost::thread	thd5( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_DEL_VS ) );
	boost::thread	thd6( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_EDIT_VS ) );
	boost::thread	thd7( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_FLUSH_VS ) );
	boost::thread	thd8( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_ADD_RS ) );
	boost::thread	thd9( boost::bind( &cmd_thread, l7vs::l7vsadm_request::CMD_DEL_RS ) );



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

	std::cout << "join" << std::endl;
	thd1.join();
	//thd2.join();

	rep_call();

	BOOST_CHECK_EQUAL( list_err_cnt, 0U );


	BOOST_MESSAGE( "----- thread_test end -----" );

}

test_suite*	init_unit_test_suite( int argc, char* argv[] ){
	test_suite* ts = BOOST_TEST_SUITE( "l7vsd class test" );

	l7vs::Logger	logger_instance;
	l7vs::Parameter	parameter_instance;
	logger_instance.loadConf();

	ts->add( BOOST_TEST_CASE( &thread_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}


