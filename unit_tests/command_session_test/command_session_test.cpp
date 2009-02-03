#include <boost/test/included/unit_test.hpp>

#include "logger.h"
#include "parameter.h"
#include "replication_stub.h"
#include "l7vsd_stub.h"
//#include "command_receiver_stub.h"

#include "../../src/command_session.cpp"

#define	SOCKFILE	"./l7vs"

using namespace boost::unit_test_framework;

// class	command_session_test	:public	l7vs::command_session {
// public:
// 	
// };

//variable

l7vs::l7vsadm_request	request;
l7vs::l7vsd_response	response;

boost::asio::io_service	io;


//--tests--

void	server_thread(){
	std::cout << "sv_sta" << std::endl;
	io.run();
	std::cout << "sv_end" << std::endl;

}


void	client_thread( l7vs::l7vsadm_request::COMMAND_CODE_TAG cmd ){
	using boost::asio::local::stream_protocol;


	boost::array<char,65535>	buf;

	// accept
	boost::asio::io_service			client_io;
	stream_protocol::acceptor	acc( client_io, stream_protocol::endpoint( SOCKFILE ) );
	stream_protocol::socket	s( client_io );

	std::cout << "accept" << std::endl;
	acc.accept( s );

// 	// wait
// 	boost::xtime xt;
// 	xtime_get(&xt, boost::TIME_UTC);
// 	xt.sec += 1;
// 	boost::thread::sleep(xt);


	// create request
	request.command = cmd;

	std::stringstream	send_stream;
	boost::archive::text_oarchive	oa( send_stream );
	oa << (const l7vs::l7vsadm_request&) request;

	std::cout << "send" << std::endl;

	// send request
	boost::asio::write( s, boost::asio::buffer( send_stream.str() ) );


	std::cout << "recv" << std::endl;
	// recv response
	s.read_some( boost::asio::buffer( buf ) );
	
	std::stringstream	recv_stream;
	recv_stream << &(buf[0]);
	boost::archive::text_iarchive	ia( recv_stream );
	//l7vs::l7vsd_response	response;
	ia >> response;

	std::cout << "end" << std::endl;

}

void	command_session_test(){
	l7vs::l7vsd	vsd;

	using boost::asio::local::stream_protocol;

	// CMD_LIST normal case
	{
		//init
		response.status = l7vs::l7vsd_response::RESPONSE_NONE;
		l7vs::l7vsd::list_vs_called = false;

		// create client thread
		boost::thread	cl_thd( boost::bind( &client_thread, l7vs::l7vsadm_request::CMD_LIST) );

		// accept wait
		boost::xtime xt;
		xtime_get(&xt, boost::TIME_UTC);
		xt.sec += 1;
		boost::thread::sleep(xt);

		// create session 
		l7vs::command_session::command_session_ptr session( new l7vs::command_session( io, vsd ) );

		// connect
		session->socket().connect(stream_protocol::endpoint( SOCKFILE ));
		session->start();

		// create server thread
		boost::thread	sv_thd( &server_thread );

		// client thread join
		cl_thd.join();
		
		// unit_test[1] command_session_test normal case CMD_LIST response check
		BOOST_CHECK_EQUAL( response.status, l7vs::l7vsd_response::RESPONSE_OK );
		// unit_test[1] command_session_test normal case CMD_LIST vsd_method call check
		BOOST_CHECK_EQUAL( l7vs::l7vsd::list_vs_called, true );

		//stop server
		io.stop();
		sv_thd.join();

		unlink( SOCKFILE );

	}



	// CMD_LIST_KEY normal case

	// CMD_LIST error case

}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "command_session class test" );
	ts->add( BOOST_TEST_CASE( &command_session_test ) );

	l7vs::Logger	logger;
	l7vs::Parameter	parameter;
	logger.loadConf();

	framework::master_test_suite().add( ts );

	return 0;
}
