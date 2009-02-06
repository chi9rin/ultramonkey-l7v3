#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>

#include "protocol_module_cinsert.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

#define	THREAD_LOOP_NUM	(1)

//--stub functions--
void	thread_method()
{
}

//--test class--
class	protocol_module_cinsert_test : public protocol_module_cinsert
{
public:
boost::mutex		sync_mutex;
boost::condition	sync_condition;
boost::condition	session_thread_data_map_condition;
protocol_module_cinsert_test() : protocol_module_cinsert() {}
~protocol_module_cinsert_test(){}
//-------------------------------------------------------------------
void	session_thread_data_map_lock()
{
	boost::mutex::scoped_lock	lk( session_thread_data_map_mutex );
	session_thread_data_map_condition.wait( lk );
}
//-------------------------------------------------------------------
void	get_name_test()
{
	// unit_test[01]  get_name( Result = "cinsert" )
	BOOST_CHECK( get_name() == "cinsert" );
}

void	get_name_test_thread( int thread_no, bool* ret )
{
	// unit_test[02]  get_name( Result = "cinsert" / ThreadNo = 1 )
	// unit_test[03]  get_name( Result = "cinsert" / ThreadNo = 2 )
	// unit_test[04]  get_name( Result = "cinsert" / ThreadNo = 3 )
	// unit_test[05]  get_name( Result = "cinsert" / ThreadNo = 4 )
	// unit_test[06]  get_name( Result = "cinsert" / ThreadNo = 5 )
	{
		boost::mutex::scoped_lock	lk( sync_mutex );
		sync_condition.wait( lk );
	}
	if( get_name() == "cinsert" )
	{
		*ret = true;
	}
	else
	{
		*ret = false;
	}
}
//-------------------------------------------------------------------
void	is_tcp_test()
{
	// unit_test[01]  is_tcp( Result = true )
	BOOST_CHECK( is_tcp() == true );
}

void	is_tcp_test_thread( int thread_no, bool* ret )
{
	// unit_test[02]  is_tcp( Result = true / ThreadNo = 1 )
	// unit_test[03]  is_tcp( Result = true / ThreadNo = 2 )
	// unit_test[04]  is_tcp( Result = true / ThreadNo = 3 )
	// unit_test[05]  is_tcp( Result = true / ThreadNo = 4 )
	// unit_test[06]  is_tcp( Result = true / ThreadNo = 5 )
	{
		boost::mutex::scoped_lock	lk( sync_mutex );
		sync_condition.wait( lk );
	}
	if( is_tcp() == true )
	{
		*ret = true;
	}
	else
	{
		*ret = false;
	}
}
//-------------------------------------------------------------------
void	is_udp_test()
{
	// unit_test[01]  is_udp( Result = false )
	BOOST_CHECK( is_udp() == false );
}

void	is_udp_test_thread( int thread_no, bool* ret )
{
	// unit_test[02]  is_udp( Result = false / ThreadNo = 1 )
	// unit_test[03]  is_udp( Result = false / ThreadNo = 2 )
	// unit_test[04]  is_udp( Result = false / ThreadNo = 3 )
	// unit_test[05]  is_udp( Result = false / ThreadNo = 4 )
	// unit_test[06]  is_udp( Result = false / ThreadNo = 5 )
	{
		boost::mutex::scoped_lock	lk( sync_mutex );
		sync_condition.wait( lk );
	}
	if( is_udp() == false )
	{
		*ret = true;
	}
	else
	{
		*ret = false;
	}
}

//-------------------------------------------------------------------
void	is_use_sorry_test()
{
	// unit_test[01]  is_use_sorry( Result = true )
	BOOST_CHECK( is_use_sorry() == true );
}

void	is_use_sorry_test_thread( int thread_no, bool* ret )
{
	// unit_test[02]  is_use_sorry( Result = true / ThreadNo = 1 )
	// unit_test[03]  is_use_sorry( Result = true / ThreadNo = 2 )
	// unit_test[04]  is_use_sorry( Result = true / ThreadNo = 3 )
	// unit_test[05]  is_use_sorry( Result = true / ThreadNo = 4 )
	// unit_test[06]  is_use_sorry( Result = true / ThreadNo = 5 )
	{
		boost::mutex::scoped_lock	lk( sync_mutex );
		sync_condition.wait( lk );
	}
	if( is_use_sorry() == true )
	{
		*ret = true;
	}
	else
	{
		*ret = false;
	}
}
//-------------------------------------------------------------------
void	handle_session_initialize_test()
{

	protocol_module_cinsert::EVENT_TAG	status;

	boost::thread	up_thread( &thread_method );
	boost::thread	down_thread( &thread_method );

	boost::asio::ip::address	address;
	boost::asio::ip::tcp::endpoint client_endpoint_tcp( address, 11111 );
	boost::asio::ip::udp::endpoint client_endpoint_udp( address, 22222 );

	session_thread_data_map_itr		up_thread_data_itr;
	session_thread_data_map_itr		down_thread_data_itr;
	recive_data_map_itr				recive_data_itr;

	status = handle_session_initialize(
						up_thread.get_id(),
						down_thread.get_id(),
						client_endpoint_tcp,
						client_endpoint_udp );

	std::cout << "<<< parameter >>>" << std::endl;
	std::cout << "up_thread_id        = [" << up_thread.get_id() << "]" << std::endl;
	std::cout << "down_thread_id      = [" << down_thread.get_id() << "]" << std::endl;
	std::cout << "client_endpoint_tcp = [" << client_endpoint_tcp << "]" << std::endl;
	std::cout << "client_endpoint_udp = [" << client_endpoint_udp << "]" << std::endl;
	std::cout << std::endl;

	up_thread_data_itr = session_thread_data_map.find( up_thread.get_id());

	BOOST_CHECK( up_thread_data_itr != session_thread_data_map.end());

	if( up_thread_data_itr != session_thread_data_map.end())
	{
		std::cout	<< "<< up_thread_data >>" << std::endl;
		std::cout	<< "thread_id               = ["
					<< up_thread_data_itr->second->thread_id << "]" << std::endl;
		std::cout	<< "thread_division         = ["
					<< up_thread_data_itr->second->thread_division << "]" << std::endl;
		std::cout	<< "pair_thread_id          = ["
					<< up_thread_data_itr->second->pair_thread_id << "]" << std::endl;
		std::cout	<< "end_flag                = ["
					<< up_thread_data_itr->second->end_flag << "]" << std::endl;
		std::cout	<< "accept_end_flag         = ["
					<< up_thread_data_itr->second->accept_end_flag << "]" << std::endl;
		std::cout	<< "sorry_flag              = ["
					<< up_thread_data_itr->second->sorry_flag << "]" << std::endl;
		std::cout	<< "sorryserver_switch_flag = ["
					<< up_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
		std::cout	<< "realserver_switch_flag  = ["
					<< up_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
		std::cout	<< "client_endpoint_tcp     = ["
					<< up_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
		std::cout << std::endl;

		BOOST_CHECK( up_thread_data_itr->second->thread_id == up_thread.get_id());
		BOOST_CHECK( up_thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM );
		BOOST_CHECK( up_thread_data_itr->second->pair_thread_id == down_thread.get_id());
		BOOST_CHECK( up_thread_data_itr->second->end_flag == END_FLAG_OFF );
		BOOST_CHECK( up_thread_data_itr->second->accept_end_flag == ACCEPT_END_FLAG_OFF );
		BOOST_CHECK( up_thread_data_itr->second->sorry_flag == SORRY_FLAG_OFF );
		BOOST_CHECK( up_thread_data_itr->second->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_OFF );
		BOOST_CHECK( up_thread_data_itr->second->realserver_switch_flag == REALSERVER_SWITCH_FLAG_OFF );
		BOOST_CHECK( up_thread_data_itr->second->client_endpoint_tcp == client_endpoint_tcp);

		recive_data_itr = up_thread_data_itr->second->recive_data_map.find( client_endpoint_tcp );

		BOOST_CHECK( recive_data_itr != up_thread_data_itr->second->recive_data_map.end());

		if( recive_data_itr != up_thread_data_itr->second->recive_data_map.end())
		{
			std::cout	<< "< recive_data >" << std::endl;
			std::cout	<< "recive_buffer_max_size  = ["
						<< recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
			std::cout	<< "recive_buffer_rest_size = ["
						<< recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
			std::cout	<< "recive_buffer           = ["
						<< (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
			std::cout	<< "recive_buffer_1         = ["
						<< (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
			std::cout	<< "recive_buffer_2         = ["
						<< (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
			std::cout << std::endl;

			BOOST_CHECK( recive_data_itr->second.recive_buffer_max_size == MAX_BUFFER_SIZE );
			BOOST_CHECK( recive_data_itr->second.recive_buffer_rest_size == MAX_BUFFER_SIZE );
			BOOST_CHECK( recive_data_itr->second.recive_buffer == recive_data_itr->second.recive_buffer_1 );
			BOOST_CHECK( recive_data_itr->second.recive_buffer_1 != NULL );
			BOOST_CHECK( recive_data_itr->second.recive_buffer_2 != NULL );

		}

	}

	down_thread_data_itr = session_thread_data_map.find( down_thread.get_id());

	BOOST_CHECK( down_thread_data_itr != session_thread_data_map.end());

	if( down_thread_data_itr != session_thread_data_map.end())
	{
		std::cout	<< "<< down_thread_data >>" << std::endl;
		std::cout	<< "thread_id               = ["
					<< down_thread_data_itr->second->thread_id << "]" << std::endl;
		std::cout	<< "thread_division         = ["
					<< down_thread_data_itr->second->thread_division << "]" << std::endl;
		std::cout	<< "pair_thread_id          = ["
					<< down_thread_data_itr->second->pair_thread_id << "]" << std::endl;
		std::cout	<< "end_flag                = ["
					<< down_thread_data_itr->second->end_flag << "]" << std::endl;
		std::cout	<< "accept_end_flag         = ["
					<< down_thread_data_itr->second->accept_end_flag << "]" << std::endl;
		std::cout	<< "sorry_flag              = ["
					<< down_thread_data_itr->second->sorry_flag << "]" << std::endl;
		std::cout	<< "sorryserver_switch_flag = ["
					<< down_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
		std::cout	<< "realserver_switch_flag  = ["
					<< down_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
		std::cout	<< "client_endpoint_tcp     = ["
					<< down_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
		std::cout << std::endl;

		BOOST_CHECK( down_thread_data_itr->second->thread_id == down_thread.get_id());
		BOOST_CHECK( down_thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM );
		BOOST_CHECK( down_thread_data_itr->second->pair_thread_id == up_thread.get_id());
		BOOST_CHECK( down_thread_data_itr->second->end_flag == END_FLAG_OFF );
		BOOST_CHECK( down_thread_data_itr->second->accept_end_flag == ACCEPT_END_FLAG_OFF );
		BOOST_CHECK( down_thread_data_itr->second->sorry_flag == SORRY_FLAG_OFF );
		BOOST_CHECK( down_thread_data_itr->second->sorryserver_switch_flag == SORRYSERVER_SWITCH_FLAG_OFF );
		BOOST_CHECK( down_thread_data_itr->second->realserver_switch_flag == REALSERVER_SWITCH_FLAG_OFF );
		BOOST_CHECK( down_thread_data_itr->second->client_endpoint_tcp == client_endpoint_tcp);

		BOOST_CHECK( down_thread_data_itr->second->recive_data_map.empty() == true );

	}

	BOOST_CHECK( status == ACCEPT );

}

void	handle_session_initialize_test_thread( int thread_no, bool* ret )
{

	boost::xtime	start_time;
	boost::xtime	end_time;

	protocol_module_cinsert::EVENT_TAG	status;

	boost::thread	up_thread( &thread_method );
	boost::thread	down_thread( &thread_method );

	boost::asio::ip::address	address;
	boost::asio::ip::tcp::endpoint client_endpoint_tcp( address, thread_no * 11111 );
	boost::asio::ip::udp::endpoint client_endpoint_udp( address, 00000 );

	session_thread_data_map_itr		up_thread_data_itr;
	session_thread_data_map_itr		down_thread_data_itr;
	recive_data_map_itr				recive_data_itr;

	{
		boost::mutex::scoped_lock	lk( sync_mutex );
		sync_condition.wait( lk );
	}

	boost::xtime_get(&start_time, boost::TIME_UTC);
	status = handle_session_initialize(
						up_thread.get_id(),
						down_thread.get_id(),
						client_endpoint_tcp,
						client_endpoint_udp );
	boost::xtime_get(&end_time, boost::TIME_UTC);

	boost::mutex::scoped_lock	lk( sync_mutex );
	sync_condition.wait( lk );

	*ret = false;

	std::cout << "<<<< ThreadNo = [" << thread_no << "] >>>>" << std::endl;
	std::cout << "StartTime = [" << start_time.nsec << "]" << std::endl;
	std::cout << "EndTime   = [" << end_time.nsec << "]" << std::endl;
	std::cout << "<<< parameter >>>" << std::endl;
	std::cout << "up_thread_id        = [" << up_thread.get_id() << "]" << std::endl;
	std::cout << "down_thread_id      = [" << down_thread.get_id() << "]" << std::endl;
	std::cout << "client_endpoint_tcp = [" << client_endpoint_tcp << "]" << std::endl;
	std::cout << "client_endpoint_udp = [" << client_endpoint_udp << "]" << std::endl;
	std::cout << std::endl;

	up_thread_data_itr = session_thread_data_map.find( up_thread.get_id());

	if( up_thread_data_itr == session_thread_data_map.end()){ return; }

	std::cout	<< "<< up_thread_data >>" << std::endl;
	std::cout	<< "thread_id               = ["
				<< up_thread_data_itr->second->thread_id << "]" << std::endl;
	std::cout	<< "thread_division         = ["
				<< up_thread_data_itr->second->thread_division << "]" << std::endl;
	std::cout	<< "pair_thread_id          = ["
				<< up_thread_data_itr->second->pair_thread_id << "]" << std::endl;
	std::cout	<< "end_flag                = ["
				<< up_thread_data_itr->second->end_flag << "]" << std::endl;
	std::cout	<< "accept_end_flag         = ["
				<< up_thread_data_itr->second->accept_end_flag << "]" << std::endl;
	std::cout	<< "sorry_flag              = ["
				<< up_thread_data_itr->second->sorry_flag << "]" << std::endl;
	std::cout	<< "sorryserver_switch_flag = ["
				<< up_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
	std::cout	<< "realserver_switch_flag  = ["
				<< up_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
	std::cout	<< "client_endpoint_tcp     = ["
				<< up_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
	std::cout << std::endl;

	if( up_thread_data_itr->second->thread_id != up_thread.get_id()){ return; }
	if( up_thread_data_itr->second->thread_division != THREAD_DIVISION_UP_STREAM ){ return; }
	if( up_thread_data_itr->second->pair_thread_id != down_thread.get_id()){ return; }
	if( up_thread_data_itr->second->end_flag != END_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->accept_end_flag != ACCEPT_END_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->sorry_flag != SORRY_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->sorryserver_switch_flag != SORRYSERVER_SWITCH_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->realserver_switch_flag != REALSERVER_SWITCH_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->client_endpoint_tcp != client_endpoint_tcp){ return; }

	recive_data_itr = up_thread_data_itr->second->recive_data_map.find( client_endpoint_tcp );

	if( recive_data_itr == up_thread_data_itr->second->recive_data_map.end()){ return; }

	std::cout	<< "< recive_data >" << std::endl;
	std::cout	<< "recive_buffer_max_size  = ["
				<< recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
	std::cout	<< "recive_buffer_rest_size = ["
				<< recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
	std::cout	<< "recive_buffer           = ["
				<< (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
	std::cout	<< "recive_buffer_1         = ["
				<< (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
	std::cout	<< "recive_buffer_2         = ["
				<< (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
	std::cout << std::endl;

	if( recive_data_itr->second.recive_buffer_max_size != MAX_BUFFER_SIZE ){ return; }
	if( recive_data_itr->second.recive_buffer_rest_size != MAX_BUFFER_SIZE ){ return; }
	if( recive_data_itr->second.recive_buffer != recive_data_itr->second.recive_buffer_1 ){ return; }
	if( recive_data_itr->second.recive_buffer_1 == NULL ){ return; }
	if( recive_data_itr->second.recive_buffer_2 == NULL ){ return; }

	down_thread_data_itr = session_thread_data_map.find( down_thread.get_id());

	if( down_thread_data_itr == session_thread_data_map.end()){ return; }

	std::cout	<< "<< down_thread_data >>" << std::endl;
	std::cout	<< "thread_id               = ["
				<< down_thread_data_itr->second->thread_id << "]" << std::endl;
	std::cout	<< "thread_division         = ["
				<< down_thread_data_itr->second->thread_division << "]" << std::endl;
	std::cout	<< "pair_thread_id          = ["
				<< down_thread_data_itr->second->pair_thread_id << "]" << std::endl;
	std::cout	<< "end_flag                = ["
				<< down_thread_data_itr->second->end_flag << "]" << std::endl;
	std::cout	<< "accept_end_flag         = ["
				<< down_thread_data_itr->second->accept_end_flag << "]" << std::endl;
	std::cout	<< "sorry_flag              = ["
				<< down_thread_data_itr->second->sorry_flag << "]" << std::endl;
	std::cout	<< "sorryserver_switch_flag = ["
				<< down_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
	std::cout	<< "realserver_switch_flag  = ["
				<< down_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
	std::cout	<< "client_endpoint_tcp     = ["
				<< down_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
	std::cout << std::endl;

	if( down_thread_data_itr->second->thread_id != down_thread.get_id()){ return; }
	if( down_thread_data_itr->second->thread_division != THREAD_DIVISION_DOWN_STREAM ){ return; }
	if( down_thread_data_itr->second->pair_thread_id != up_thread.get_id()){ return; }
	if( down_thread_data_itr->second->end_flag != END_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->accept_end_flag != ACCEPT_END_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->sorry_flag != SORRY_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->sorryserver_switch_flag != SORRYSERVER_SWITCH_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->realserver_switch_flag != REALSERVER_SWITCH_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->client_endpoint_tcp != client_endpoint_tcp){ return; }

	if( down_thread_data_itr->second->recive_data_map.empty() != true ){ return; }

	if( status != ACCEPT ){ return; }

	*ret = true;

	return;

}

void	handle_session_finalize_test()
{

	int	i = 0;
	int	j = 0;
	size_t	recive_data_num = 0;

	protocol_module_cinsert::EVENT_TAG	status;

	boost::thread	up_thread_1( &thread_method );
	boost::thread	up_thread_2( &thread_method );

	boost::thread	down_thread_1( &thread_method );
	boost::thread	down_thread_2( &thread_method );

	boost::asio::ip::address	address;

	boost::asio::ip::tcp::endpoint client_endpoint_tcp_1( address, 11111 );
	boost::asio::ip::tcp::endpoint client_endpoint_tcp_2( address, 12222 );
	boost::asio::ip::tcp::endpoint client_endpoint_tcp_3( address, 13333 );

	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_1( address, 21111 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_2( address, 22222 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_3( address, 23333 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_4( address, 24444 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_5( address, 25555 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_6( address, 26666 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_7( address, 27777 );

	session_thread_data_map_itr		thread_data_itr;
	recive_data_map_itr				recive_data_itr;

	session_thread_data_cinsert*	up_thread_data_1	= NULL;
	session_thread_data_cinsert*	up_thread_data_2	= NULL;
	session_thread_data_cinsert*	down_thread_data_1	= NULL;
	session_thread_data_cinsert*	down_thread_data_2	= NULL;
	recive_data						client_recv_data_1;
	recive_data						client_recv_data_2;
	recive_data						client_recv_data_3;
	recive_data						realserver_recv_data_1;
	recive_data						realserver_recv_data_2;
	recive_data						realserver_recv_data_3;
	recive_data						realserver_recv_data_4;
	recive_data						realserver_recv_data_5;
	recive_data						realserver_recv_data_6;
	recive_data						realserver_recv_data_7;
	char*							buffer				= NULL;

	//----------------------------------------------------------------------
	up_thread_data_1 = new session_thread_data_cinsert;

	up_thread_data_1->thread_id				= up_thread_1.get_id();
	up_thread_data_1->thread_division		= THREAD_DIVISION_UP_STREAM;
	up_thread_data_1->pair_thread_id		= down_thread_1.get_id();
	up_thread_data_1->client_endpoint_tcp	= client_endpoint_tcp_1;

	buffer = (char*)malloc( client_recv_data_1.recive_buffer_max_size );

	client_recv_data_1.recive_buffer	= buffer;
	client_recv_data_1.recive_buffer_1	= buffer;

	buffer = (char*)malloc( client_recv_data_1.recive_buffer_max_size );

	client_recv_data_1.recive_buffer_2	= buffer;

	up_thread_data_1->recive_data_map[ client_endpoint_tcp_1 ] = client_recv_data_1;

	buffer = (char*)malloc( client_recv_data_2.recive_buffer_max_size );

	client_recv_data_2.recive_buffer	= buffer;
	client_recv_data_2.recive_buffer_1	= buffer;

	buffer = (char*)malloc( client_recv_data_2.recive_buffer_max_size );

	client_recv_data_2.recive_buffer_2	= buffer;

	up_thread_data_1->recive_data_map[ client_endpoint_tcp_2 ] = client_recv_data_2;

	session_thread_data_map[ up_thread_1.get_id() ]	= up_thread_data_1;

	//----------------------------------------------------------------------
	up_thread_data_2 = new session_thread_data_cinsert;

	up_thread_data_2->thread_id				= up_thread_2.get_id();
	up_thread_data_2->thread_division		= THREAD_DIVISION_UP_STREAM;
	up_thread_data_2->pair_thread_id		= down_thread_2.get_id();
	up_thread_data_2->client_endpoint_tcp	= client_endpoint_tcp_3;

	buffer = (char*)malloc( client_recv_data_3.recive_buffer_max_size );

	client_recv_data_3.recive_buffer	= buffer;
	client_recv_data_3.recive_buffer_1	= buffer;

	buffer = (char*)malloc( client_recv_data_3.recive_buffer_max_size );

	client_recv_data_3.recive_buffer_2	= buffer;

	up_thread_data_2->recive_data_map[ client_endpoint_tcp_3 ] = client_recv_data_3;

	session_thread_data_map[ up_thread_2.get_id() ]	= up_thread_data_2;

	//----------------------------------------------------------------------
	down_thread_data_1 = new session_thread_data_cinsert;

	down_thread_data_1->thread_id			= down_thread_1.get_id();
	down_thread_data_1->thread_division		= THREAD_DIVISION_DOWN_STREAM;
	down_thread_data_1->pair_thread_id		= up_thread_1.get_id();
	down_thread_data_1->client_endpoint_tcp	= client_endpoint_tcp_1;

	buffer = (char*)malloc( realserver_recv_data_1.recive_buffer_max_size );

	realserver_recv_data_1.recive_buffer	= buffer;
	realserver_recv_data_1.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_1.recive_buffer_max_size );

	realserver_recv_data_1.recive_buffer_2	= buffer;

	down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_1 ] = realserver_recv_data_1;

	buffer = (char*)malloc( realserver_recv_data_2.recive_buffer_max_size );

	realserver_recv_data_2.recive_buffer	= buffer;
	realserver_recv_data_2.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_2.recive_buffer_max_size );

	realserver_recv_data_2.recive_buffer_2	= buffer;

	down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_2 ] = realserver_recv_data_2;

	buffer = (char*)malloc( realserver_recv_data_3.recive_buffer_max_size );

	realserver_recv_data_3.recive_buffer	= buffer;
	realserver_recv_data_3.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_3.recive_buffer_max_size );

	realserver_recv_data_3.recive_buffer_2	= buffer;

	down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_3 ] = realserver_recv_data_3;

	buffer = (char*)malloc( realserver_recv_data_4.recive_buffer_max_size );

	realserver_recv_data_4.recive_buffer	= buffer;
	realserver_recv_data_4.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_4.recive_buffer_max_size );

	realserver_recv_data_4.recive_buffer_2	= buffer;

	down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_4 ] = realserver_recv_data_4;

	session_thread_data_map[ down_thread_1.get_id() ]	= down_thread_data_1;

	//----------------------------------------------------------------------
	down_thread_data_2 = new session_thread_data_cinsert;

	down_thread_data_2->thread_id			= down_thread_2.get_id();
	down_thread_data_2->thread_division		= THREAD_DIVISION_DOWN_STREAM;
	down_thread_data_2->pair_thread_id		= up_thread_2.get_id();
	down_thread_data_2->client_endpoint_tcp	= client_endpoint_tcp_3;

	buffer = (char*)malloc( realserver_recv_data_5.recive_buffer_max_size );

	realserver_recv_data_5.recive_buffer	= buffer;
	realserver_recv_data_5.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_5.recive_buffer_max_size );

	realserver_recv_data_5.recive_buffer_2	= buffer;

	down_thread_data_2->recive_data_map[ realserver_endpoint_tcp_5 ] = realserver_recv_data_5;

	buffer = (char*)malloc( realserver_recv_data_6.recive_buffer_max_size );

	realserver_recv_data_6.recive_buffer	= buffer;
	realserver_recv_data_6.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_6.recive_buffer_max_size );

	realserver_recv_data_6.recive_buffer_2	= buffer;

	down_thread_data_2->recive_data_map[ realserver_endpoint_tcp_6 ] = realserver_recv_data_6;

	buffer = (char*)malloc( realserver_recv_data_7.recive_buffer_max_size );

	realserver_recv_data_7.recive_buffer		= buffer;
	realserver_recv_data_7.recive_buffer_1	= buffer;

	buffer = (char*)malloc( realserver_recv_data_7.recive_buffer_max_size );

	realserver_recv_data_7.recive_buffer_2	= buffer;

	down_thread_data_2->recive_data_map[ realserver_endpoint_tcp_7 ] = realserver_recv_data_7;

	session_thread_data_map[ down_thread_2.get_id() ]	= down_thread_data_2;

	//----------------------------------------------------------------------

	std::cout	<< "<<< handle_session_finalize before >>>" << std::endl;

	thread_data_itr = session_thread_data_map.begin();

	std::cout	<< "session_thread_data_map entry = ["
				<< session_thread_data_map.size() << "]" << std::endl;

	BOOST_CHECK( session_thread_data_map.size() == 4 );

	std::cout << std::endl;

	i = 0;

	while( thread_data_itr != session_thread_data_map.end() )
	{
		i++;

		BOOST_CHECK(	( thread_data_itr->first == up_thread_1.get_id())	||
						( thread_data_itr->first == up_thread_2.get_id())	||
						( thread_data_itr->first == down_thread_1.get_id())	||
						( thread_data_itr->first == down_thread_2.get_id()) );

		std::cout	<< "<< thread_data[" << i << "] >>" << std::endl;
		std::cout	<< "key                     = ["
					<< thread_data_itr->first << "]" << std::endl;
		std::cout	<< "thread_id               = ["
					<< thread_data_itr->second->thread_id << "]" << std::endl;
		std::cout	<< "thread_division         = ["
					<< thread_data_itr->second->thread_division << "]" << std::endl;
		std::cout	<< "pair_thread_id          = ["
					<< thread_data_itr->second->pair_thread_id << "]" << std::endl;
		std::cout	<< "end_flag                = ["
					<< thread_data_itr->second->end_flag << "]" << std::endl;
		std::cout	<< "accept_end_flag         = ["
					<< thread_data_itr->second->accept_end_flag << "]" << std::endl;
		std::cout	<< "sorry_flag              = ["
					<< thread_data_itr->second->sorry_flag << "]" << std::endl;
		std::cout	<< "sorryserver_switch_flag = ["
					<< thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
		std::cout	<< "realserver_switch_flag  = ["
					<< thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
		std::cout	<< "client_endpoint_tcp     = ["
					<< thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
		std::cout	<< "recive_data_map entry = ["
					<< thread_data_itr->second->recive_data_map.size() << "]" << std::endl;
		std::cout << std::endl;

		recive_data_num = 0;

		if( thread_data_itr->first == up_thread_1.get_id() )
		{
			recive_data_num = 2;
		}
		else if( thread_data_itr->first == up_thread_2.get_id() )
		{
			recive_data_num = 1;
		}
		else if( thread_data_itr->first == down_thread_1.get_id() )
		{
			recive_data_num = 4;
		}
		else if( thread_data_itr->first == down_thread_2.get_id() )
		{
			recive_data_num = 3;
		}

		BOOST_CHECK( thread_data_itr->second->recive_data_map.size() == recive_data_num );

		recive_data_itr = thread_data_itr->second->recive_data_map.begin();

		j = 0;

		while( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
		{
			j++;

			std::cout	<< "< recive_data[" << j << "] >" << std::endl;
			std::cout	<< "key                     = ["
						<< recive_data_itr->first << "]" << std::endl;
			std::cout	<< "recive_buffer_max_size  = ["
						<< recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
			std::cout	<< "recive_buffer_rest_size = ["
						<< recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
			std::cout	<< "recive_buffer           = ["
						<< (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
			std::cout	<< "recive_buffer_1         = ["
						<< (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
			std::cout	<< "recive_buffer_2         = ["
						<< (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
			std::cout << std::endl;

			recive_data_itr++;

		}

		thread_data_itr++;

	}

	std::cout << std::endl;
	std::cout	<< "<<< handle_session_finalize after >>>" << std::endl;
	std::cout	<< "finalize up_thread_id   = ["
				<< up_thread_1.get_id() << "]" << std::endl;
	std::cout	<< "finalize down_thread_id = ["
				<< down_thread_1.get_id() << "]" << std::endl;
	//------------------------------------------------------------------
	status = handle_session_finalize(
						up_thread_1.get_id(),
						down_thread_1.get_id());
	//------------------------------------------------------------------

	thread_data_itr = session_thread_data_map.begin();

	std::cout	<< "session_thread_data_map entry = ["
				<< session_thread_data_map.size() << "]" << std::endl;

	BOOST_CHECK( session_thread_data_map.size() == 2 );

	std::cout << std::endl;

	i = 0;

	while( thread_data_itr != session_thread_data_map.end() )
	{
		i++;

		BOOST_CHECK(	( thread_data_itr->first == up_thread_2.get_id())	||
						( thread_data_itr->first == down_thread_2.get_id()) );

		std::cout	<< "<< thread_data[" << i << "] >>" << std::endl;
		std::cout	<< "key                     = ["
					<< thread_data_itr->first << "]" << std::endl;
		std::cout	<< "thread_id               = ["
					<< thread_data_itr->second->thread_id << "]" << std::endl;
		std::cout	<< "thread_division         = ["
					<< thread_data_itr->second->thread_division << "]" << std::endl;
		std::cout	<< "pair_thread_id          = ["
					<< thread_data_itr->second->pair_thread_id << "]" << std::endl;
		std::cout	<< "end_flag                = ["
					<< thread_data_itr->second->end_flag << "]" << std::endl;
		std::cout	<< "accept_end_flag         = ["
					<< thread_data_itr->second->accept_end_flag << "]" << std::endl;
		std::cout	<< "sorry_flag              = ["
					<< thread_data_itr->second->sorry_flag << "]" << std::endl;
		std::cout	<< "sorryserver_switch_flag = ["
					<< thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
		std::cout	<< "realserver_switch_flag  = ["
					<< thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
		std::cout	<< "client_endpoint_tcp     = ["
					<< thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
		std::cout	<< "recive_data_map entry = ["
					<< thread_data_itr->second->recive_data_map.size() << "]" << std::endl;
		std::cout << std::endl;

		recive_data_num = 0;

		if( thread_data_itr->first == up_thread_1.get_id() )
		{
			recive_data_num = 2;
		}
		else if( thread_data_itr->first == up_thread_2.get_id() )
		{
			recive_data_num = 1;
		}
		else if( thread_data_itr->first == down_thread_1.get_id() )
		{
			recive_data_num = 4;
		}
		else if( thread_data_itr->first == down_thread_2.get_id() )
		{
			recive_data_num = 3;
		}

		BOOST_CHECK( thread_data_itr->second->recive_data_map.size() == recive_data_num );

		recive_data_itr = thread_data_itr->second->recive_data_map.begin();

		j = 0;

		while( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
		{
			j++;

			std::cout	<< "< recive_data[" << j << "] >" << std::endl;
			std::cout	<< "key                     = ["
						<< recive_data_itr->first << "]" << std::endl;
			std::cout	<< "recive_buffer_max_size  = ["
						<< recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
			std::cout	<< "recive_buffer_rest_size = ["
						<< recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
			std::cout	<< "recive_buffer           = ["
						<< (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
			std::cout	<< "recive_buffer_1         = ["
						<< (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
			std::cout	<< "recive_buffer_2         = ["
						<< (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
			std::cout << std::endl;

			recive_data_itr++;

		}

		thread_data_itr++;

	}

	std::cout << std::endl;

 	BOOST_CHECK( status == STOP );

}

void	handle_session_finalize_test_thread( int thread_no, bool* ret )
{

	boost::xtime	start_time;
	boost::xtime	end_time;

	protocol_module_cinsert::EVENT_TAG	status;

	boost::thread	up_thread( &thread_method );
	boost::thread	down_thread( &thread_method );

	boost::asio::ip::address	address;
	boost::asio::ip::tcp::endpoint client_endpoint_tcp( address, thread_no * 11111 );
	boost::asio::ip::udp::endpoint client_endpoint_udp( address, 00000 );

	session_thread_data_map_itr		up_thread_data_itr;
	session_thread_data_map_itr		down_thread_data_itr;
	recive_data_map_itr				recive_data_itr;

	{
		boost::mutex::scoped_lock	lk( sync_mutex );
		sync_condition.wait( lk );
	}

	boost::xtime_get(&start_time, boost::TIME_UTC);
	status = handle_session_initialize(
						up_thread.get_id(),
						down_thread.get_id(),
						client_endpoint_tcp,
						client_endpoint_udp );
	boost::xtime_get(&end_time, boost::TIME_UTC);

	boost::mutex::scoped_lock	lk( sync_mutex );
	sync_condition.wait( lk );

	*ret = false;

	std::cout << "<<<< ThreadNo = [" << thread_no << "] >>>>" << std::endl;
	std::cout << "StartTime = [" << start_time.nsec << "]" << std::endl;
	std::cout << "EndTime   = [" << end_time.nsec << "]" << std::endl;
	std::cout << "<<< parameter >>>" << std::endl;
	std::cout << "up_thread_id        = [" << up_thread.get_id() << "]" << std::endl;
	std::cout << "down_thread_id      = [" << down_thread.get_id() << "]" << std::endl;
	std::cout << "client_endpoint_tcp = [" << client_endpoint_tcp << "]" << std::endl;
	std::cout << "client_endpoint_udp = [" << client_endpoint_udp << "]" << std::endl;
	std::cout << std::endl;

	up_thread_data_itr = session_thread_data_map.find( up_thread.get_id());

	if( up_thread_data_itr == session_thread_data_map.end()){ return; }

	std::cout	<< "<< up_thread_data >>" << std::endl;
	std::cout	<< "thread_id               = ["
				<< up_thread_data_itr->second->thread_id << "]" << std::endl;
	std::cout	<< "thread_division         = ["
				<< up_thread_data_itr->second->thread_division << "]" << std::endl;
	std::cout	<< "pair_thread_id          = ["
				<< up_thread_data_itr->second->pair_thread_id << "]" << std::endl;
	std::cout	<< "end_flag                = ["
				<< up_thread_data_itr->second->end_flag << "]" << std::endl;
	std::cout	<< "accept_end_flag         = ["
				<< up_thread_data_itr->second->accept_end_flag << "]" << std::endl;
	std::cout	<< "sorry_flag              = ["
				<< up_thread_data_itr->second->sorry_flag << "]" << std::endl;
	std::cout	<< "sorryserver_switch_flag = ["
				<< up_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
	std::cout	<< "realserver_switch_flag  = ["
				<< up_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
	std::cout	<< "client_endpoint_tcp     = ["
				<< up_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
	std::cout << std::endl;

	if( up_thread_data_itr->second->thread_id != up_thread.get_id()){ return; }
	if( up_thread_data_itr->second->thread_division != THREAD_DIVISION_UP_STREAM ){ return; }
	if( up_thread_data_itr->second->pair_thread_id != down_thread.get_id()){ return; }
	if( up_thread_data_itr->second->end_flag != END_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->accept_end_flag != ACCEPT_END_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->sorry_flag != SORRY_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->sorryserver_switch_flag != SORRYSERVER_SWITCH_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->realserver_switch_flag != REALSERVER_SWITCH_FLAG_OFF ){ return; }
	if( up_thread_data_itr->second->client_endpoint_tcp != client_endpoint_tcp){ return; }

	recive_data_itr = up_thread_data_itr->second->recive_data_map.find( client_endpoint_tcp );

	if( recive_data_itr == up_thread_data_itr->second->recive_data_map.end()){ return; }

	std::cout	<< "< recive_data >" << std::endl;
	std::cout	<< "recive_buffer_max_size  = ["
				<< recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
	std::cout	<< "recive_buffer_rest_size = ["
				<< recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
	std::cout	<< "recive_buffer           = ["
				<< (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
	std::cout	<< "recive_buffer_1         = ["
				<< (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
	std::cout	<< "recive_buffer_2         = ["
				<< (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
	std::cout << std::endl;

	if( recive_data_itr->second.recive_buffer_max_size != MAX_BUFFER_SIZE ){ return; }
	if( recive_data_itr->second.recive_buffer_rest_size != MAX_BUFFER_SIZE ){ return; }
	if( recive_data_itr->second.recive_buffer != recive_data_itr->second.recive_buffer_1 ){ return; }
	if( recive_data_itr->second.recive_buffer_1 == NULL ){ return; }
	if( recive_data_itr->second.recive_buffer_2 == NULL ){ return; }

	down_thread_data_itr = session_thread_data_map.find( down_thread.get_id());

	if( down_thread_data_itr == session_thread_data_map.end()){ return; }

	std::cout	<< "<< down_thread_data >>" << std::endl;
	std::cout	<< "thread_id               = ["
				<< down_thread_data_itr->second->thread_id << "]" << std::endl;
	std::cout	<< "thread_division         = ["
				<< down_thread_data_itr->second->thread_division << "]" << std::endl;
	std::cout	<< "pair_thread_id          = ["
				<< down_thread_data_itr->second->pair_thread_id << "]" << std::endl;
	std::cout	<< "end_flag                = ["
				<< down_thread_data_itr->second->end_flag << "]" << std::endl;
	std::cout	<< "accept_end_flag         = ["
				<< down_thread_data_itr->second->accept_end_flag << "]" << std::endl;
	std::cout	<< "sorry_flag              = ["
				<< down_thread_data_itr->second->sorry_flag << "]" << std::endl;
	std::cout	<< "sorryserver_switch_flag = ["
				<< down_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
	std::cout	<< "realserver_switch_flag  = ["
				<< down_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
	std::cout	<< "client_endpoint_tcp     = ["
				<< down_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
	std::cout << std::endl;

	if( down_thread_data_itr->second->thread_id != down_thread.get_id()){ return; }
	if( down_thread_data_itr->second->thread_division != THREAD_DIVISION_DOWN_STREAM ){ return; }
	if( down_thread_data_itr->second->pair_thread_id != up_thread.get_id()){ return; }
	if( down_thread_data_itr->second->end_flag != END_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->accept_end_flag != ACCEPT_END_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->sorry_flag != SORRY_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->sorryserver_switch_flag != SORRYSERVER_SWITCH_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->realserver_switch_flag != REALSERVER_SWITCH_FLAG_OFF ){ return; }
	if( down_thread_data_itr->second->client_endpoint_tcp != client_endpoint_tcp){ return; }

	if( down_thread_data_itr->second->recive_data_map.empty() != true ){ return; }

	if( status != ACCEPT ){ return; }

	*ret = true;

	return;

}

};



//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//--test functions--
//-------------------------------------------------------------------
void	get_name_test_thread(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1,
				int thread_no,
				bool* ret )
{
	for( int i = 0; i < THREAD_LOOP_NUM; i++ )
	{
		protocol_module_cinsert_test_1->get_name_test_thread( thread_no, ret );
	}
}

void	get_name_test()
{
	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- get_name test start -----" );
	protocol_module_cinsert_test_1.get_name_test();
	BOOST_MESSAGE( "----- get_name test end -----" );

	BOOST_MESSAGE( "----- get_name test multi thread start -----" );
	boost::thread	t1(	boost::bind(	&get_name_test_thread,
										&protocol_module_cinsert_test_1, 1, &thread_1));
	boost::thread	t2(	boost::bind(	&get_name_test_thread,
										&protocol_module_cinsert_test_1, 2, &thread_2));
	boost::thread	t3(	boost::bind(	&get_name_test_thread,
										&protocol_module_cinsert_test_1, 3, &thread_3));
	boost::thread	t4(	boost::bind(	&get_name_test_thread,
										&protocol_module_cinsert_test_1, 4, &thread_4));
	boost::thread	t5(	boost::bind(	&get_name_test_thread,
										&protocol_module_cinsert_test_1, 5, &thread_5));

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

	BOOST_MESSAGE( "----- get_name test multi thread end -----" );
}
//-------------------------------------------------------------------
void	is_tcp_test_thread(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1,
				int thread_no,
				bool* ret )
{
	for( int i = 0; i < THREAD_LOOP_NUM; i++ )
	{
		protocol_module_cinsert_test_1->is_tcp_test_thread( thread_no, ret );
	}
}

void	is_tcp_test()
{
	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- is_tcp test start -----" );
	protocol_module_cinsert_test_1.is_tcp_test();
	BOOST_MESSAGE( "----- is_tcp test end -----" );

	BOOST_MESSAGE( "----- is_tcp test multi thread start -----" );
	boost::thread	t1(	boost::bind(	&is_tcp_test_thread,
										&protocol_module_cinsert_test_1, 1, &thread_1));
	boost::thread	t2(	boost::bind(	&is_tcp_test_thread,
										&protocol_module_cinsert_test_1, 2, &thread_2));
	boost::thread	t3(	boost::bind(	&is_tcp_test_thread,
										&protocol_module_cinsert_test_1, 3, &thread_3));
	boost::thread	t4(	boost::bind(	&is_tcp_test_thread,
										&protocol_module_cinsert_test_1, 4, &thread_4));
	boost::thread	t5(	boost::bind(	&is_tcp_test_thread,
										&protocol_module_cinsert_test_1, 5, &thread_5));

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

	BOOST_MESSAGE( "----- is_tcp test multi thread end -----" );
}
//-------------------------------------------------------------------
void	is_udp_test_thread(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1,
				int thread_no,
				bool* ret )
{
	for( int i = 0; i < THREAD_LOOP_NUM; i++ )
	{
		protocol_module_cinsert_test_1->is_udp_test_thread( thread_no, ret );
	}
}

void	is_udp_test()
{
	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- is_udp test start -----" );
	protocol_module_cinsert_test_1.is_udp_test();
	BOOST_MESSAGE( "----- is_udp test end -----" );

	BOOST_MESSAGE( "----- is_udp test multi thread start -----" );
	boost::thread	t1(	boost::bind(	&is_udp_test_thread,
										&protocol_module_cinsert_test_1, 1, &thread_1));
	boost::thread	t2(	boost::bind(	&is_udp_test_thread,
										&protocol_module_cinsert_test_1, 2, &thread_2));
	boost::thread	t3(	boost::bind(	&is_udp_test_thread,
										&protocol_module_cinsert_test_1, 3, &thread_3));
	boost::thread	t4(	boost::bind(	&is_udp_test_thread,
										&protocol_module_cinsert_test_1, 4, &thread_4));
	boost::thread	t5(	boost::bind(	&is_udp_test_thread,
										&protocol_module_cinsert_test_1, 5, &thread_5));

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

	BOOST_MESSAGE( "----- is_udp test multi thread end -----" );
}
//-------------------------------------------------------------------
void	is_use_sorry_test_thread(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1,
				int thread_no,
				bool* ret )
{
	for( int i = 0; i < THREAD_LOOP_NUM; i++ )
	{
		protocol_module_cinsert_test_1->is_use_sorry_test_thread( thread_no, ret );
	}
}

void	is_use_sorry_test()
{
	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- is_use_sorry test start -----" );
	protocol_module_cinsert_test_1.is_use_sorry_test();
	BOOST_MESSAGE( "----- is_use_sorry test end -----" );

	BOOST_MESSAGE( "----- is_use_sorry test multi thread start -----" );
	boost::thread	t1(	boost::bind(	&is_use_sorry_test_thread,
										&protocol_module_cinsert_test_1, 1, &thread_1));
	boost::thread	t2(	boost::bind(	&is_use_sorry_test_thread,
										&protocol_module_cinsert_test_1, 2, &thread_2));
	boost::thread	t3(	boost::bind(	&is_use_sorry_test_thread,
										&protocol_module_cinsert_test_1, 3, &thread_3));
	boost::thread	t4(	boost::bind(	&is_use_sorry_test_thread,
										&protocol_module_cinsert_test_1, 4, &thread_4));
	boost::thread	t5(	boost::bind(	&is_use_sorry_test_thread,
										&protocol_module_cinsert_test_1, 5, &thread_5));

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

	BOOST_MESSAGE( "----- is_use_sorry test multi thread end -----" );
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void	session_thread_data_map_lock(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1 )
{
	protocol_module_cinsert_test_1->session_thread_data_map_lock();
}

void	handle_session_initialize_test_thread(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1,
				int thread_no,
				bool* ret )
{
	for( int i = 0; i < THREAD_LOOP_NUM; i++ )
	{
		protocol_module_cinsert_test_1->handle_session_initialize_test_thread( thread_no, ret );
	}
}

void	handle_session_initialize_test()
{

	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- handle_session_initialize test start -----" );
	protocol_module_cinsert_test_1.handle_session_initialize_test();
	BOOST_MESSAGE( "----- handle_session_initialize test end -----" );

	BOOST_MESSAGE( "----- handle_session_initialize test multi thread start -----" );
	boost::thread	t0(	boost::bind(	&session_thread_data_map_lock,
										&protocol_module_cinsert_test_1));

	sleep(1);

	boost::thread	t1(	boost::bind(	&handle_session_initialize_test_thread,
										&protocol_module_cinsert_test_1, 1, &thread_1));
	boost::thread	t2(	boost::bind(	&handle_session_initialize_test_thread,
										&protocol_module_cinsert_test_1, 2, &thread_2));
	boost::thread	t3(	boost::bind(	&handle_session_initialize_test_thread,
										&protocol_module_cinsert_test_1, 3, &thread_3));
	boost::thread	t4(	boost::bind(	&handle_session_initialize_test_thread,
										&protocol_module_cinsert_test_1, 4, &thread_4));
	boost::thread	t5(	boost::bind(	&handle_session_initialize_test_thread,
										&protocol_module_cinsert_test_1, 5, &thread_5));

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	sleep(1);

	protocol_module_cinsert_test_1.session_thread_data_map_condition.notify_all();

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

	BOOST_MESSAGE( "----- handle_session_initialize test multi thread end -----" );
}
//-------------------------------------------------------------------
void	handle_session_finalize_test_thread(
				protocol_module_cinsert_test* protocol_module_cinsert_test_1,
				int thread_no,
				bool* ret )
{
	for( int i = 0; i < THREAD_LOOP_NUM; i++ )
	{
		protocol_module_cinsert_test_1->handle_session_finalize_test_thread( thread_no, ret );
	}
}

void	handle_session_finalize_test()
{

	bool	thread_1;
	bool	thread_2;
	bool	thread_3;
	bool	thread_4;
	bool	thread_5;

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- handle_session_finalize test start -----" );
	protocol_module_cinsert_test_1.handle_session_finalize_test();
	BOOST_MESSAGE( "----- handle_session_initialize test end -----" );

	BOOST_MESSAGE( "----- handle_session_finalize test multi thread start -----" );
	boost::thread	t0(	boost::bind(	&session_thread_data_map_lock,
										&protocol_module_cinsert_test_1));

	sleep(1);

	boost::thread	t1(	boost::bind(	&handle_session_finalize_test_thread,
										&protocol_module_cinsert_test_1, 1, &thread_1));
	boost::thread	t2(	boost::bind(	&handle_session_finalize_test_thread,
										&protocol_module_cinsert_test_1, 2, &thread_2));
	boost::thread	t3(	boost::bind(	&handle_session_finalize_test_thread,
										&protocol_module_cinsert_test_1, 3, &thread_3));
	boost::thread	t4(	boost::bind(	&handle_session_finalize_test_thread,
										&protocol_module_cinsert_test_1, 4, &thread_4));
	boost::thread	t5(	boost::bind(	&handle_session_finalize_test_thread,
										&protocol_module_cinsert_test_1, 5, &thread_5));

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	sleep(1);

	protocol_module_cinsert_test_1.session_thread_data_map_condition.notify_all();

	sleep(1);

	protocol_module_cinsert_test_1.sync_condition.notify_all();

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();

	BOOST_CHECK( thread_1 == true );
	BOOST_CHECK( thread_2 == true );
	BOOST_CHECK( thread_3 == true );
	BOOST_CHECK( thread_4 == true );
	BOOST_CHECK( thread_5 == true );

	BOOST_MESSAGE( "----- handle_session_finalize test multi thread end -----" );
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_cinsert class test" );

// 	ts->add( BOOST_TEST_CASE( &get_name_test ) );
// 	ts->add( BOOST_TEST_CASE( &is_tcp_test ) );
// 	ts->add( BOOST_TEST_CASE( &is_udp_test ) );
// 	ts->add( BOOST_TEST_CASE( &is_use_sorry_test ) );
// 
// 
// 
// 	ts->add( BOOST_TEST_CASE( &handle_session_initialize_test ) );
	ts->add( BOOST_TEST_CASE( &handle_session_finalize_test ) );


	framework::master_test_suite().add( ts );

	return 0;
}
