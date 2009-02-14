#define	TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

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
void	session_thread_data_set()
{

	int	i = 0;
	int	j = 0;
	size_t	recive_data_num = 0;

	boost::thread	up_thread_1( &thread_method );
	boost::thread	up_thread_2( &thread_method );

	boost::thread	down_thread_1( &thread_method );
	boost::thread	down_thread_2( &thread_method );

	boost::asio::ip::address	address;

	boost::asio::ip::tcp::endpoint client_endpoint_tcp_1( boost::asio::ip::address::from_string("10.10.10.10"), 1111 );
	boost::asio::ip::tcp::endpoint client_endpoint_tcp_2( boost::asio::ip::address::from_string("10.10.10.10"), 2222 );
	boost::asio::ip::tcp::endpoint client_endpoint_tcp_3( boost::asio::ip::address::from_string("10.10.10.10"), 3333 );

	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_1( boost::asio::ip::address::from_string("20.20.20.20"), 1111 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_2( boost::asio::ip::address::from_string("20.20.20.20"), 2222 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_3( boost::asio::ip::address::from_string("20.20.20.20"), 3333 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_4( boost::asio::ip::address::from_string("20.20.20.20"), 4444 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_5( boost::asio::ip::address::from_string("20.20.20.20"), 5555 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_6( boost::asio::ip::address::from_string("20.20.20.20"), 6666 );
	boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_7( boost::asio::ip::address::from_string("20.20.20.20"), 7777 );

	t_session_thread_data_map_itr		thread_data_itr;
	t_recive_data_map_itr				recive_data_itr;

	t_session_thread_data_cinsert	up_thread_data_1( new session_thread_data_cinsert );
	t_session_thread_data_cinsert	up_thread_data_2( new session_thread_data_cinsert );
	t_session_thread_data_cinsert	down_thread_data_1( new session_thread_data_cinsert );
	t_session_thread_data_cinsert	down_thread_data_2( new session_thread_data_cinsert );

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
//	up_thread_data_1 = new session_thread_data_cinsert;

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
//	up_thread_data_2 = new session_thread_data_cinsert;

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
//	down_thread_data_1 = new session_thread_data_cinsert;

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
//	down_thread_data_2 = new session_thread_data_cinsert;

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

	client_recv_data_1.recive_buffer_1		= NULL;
	client_recv_data_1.recive_buffer_2		= NULL;
	client_recv_data_2.recive_buffer_1		= NULL;
	client_recv_data_2.recive_buffer_2		= NULL;
	client_recv_data_3.recive_buffer_1		= NULL;
	client_recv_data_3.recive_buffer_2		= NULL;
	realserver_recv_data_1.recive_buffer_1	= NULL;
	realserver_recv_data_1.recive_buffer_2	= NULL;
	realserver_recv_data_2.recive_buffer_1	= NULL;
	realserver_recv_data_2.recive_buffer_2	= NULL;
	realserver_recv_data_3.recive_buffer_1	= NULL;
	realserver_recv_data_3.recive_buffer_2	= NULL;
	realserver_recv_data_4.recive_buffer_1	= NULL;
	realserver_recv_data_4.recive_buffer_2	= NULL;
	realserver_recv_data_5.recive_buffer_1	= NULL;
	realserver_recv_data_5.recive_buffer_2	= NULL;
	realserver_recv_data_6.recive_buffer_1	= NULL;
	realserver_recv_data_6.recive_buffer_2	= NULL;
	realserver_recv_data_7.recive_buffer_1	= NULL;
	realserver_recv_data_7.recive_buffer_2	= NULL;

	thread_data_itr = session_thread_data_map.begin();

	std::cout	<< "session_thread_data_map entry = ["
				<< session_thread_data_map.size() << "]" << std::endl;

	std::cout << std::endl;

	i = 0;

	while( thread_data_itr != session_thread_data_map.end() )
	{
		i++;

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

}

void	session_thread_data_disp()
{

	int	i = 0;
	int	j = 0;
	int	k = 0;
	int	l = 0;

	t_session_thread_data_map_itr		thread_data_itr;
	t_recive_data_map_itr				recive_data_itr;
	t_send_status_list_itr				send_status_itr;
	t_edit_data_list_itr				edit_data_itr;

std::cout	<< "session_thread_data_disp" << std::endl;

	thread_data_itr = session_thread_data_map.begin();

	std::cout	<< "session_thread_data_map entry = ["
				<< session_thread_data_map.size() << "]" << std::endl;

	std::cout << std::endl;

	i = 0;

	while( thread_data_itr != session_thread_data_map.end() )
	{
		i++;

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
			std::cout	<< "send_status_list entry = ["
						<< recive_data_itr->second.send_status_list.size() << "]" << std::endl;
			std::cout << std::endl;

			send_status_itr = recive_data_itr->second.send_status_list.begin();
	
			k = 0;
	
			while( send_status_itr != recive_data_itr->second.send_status_list.end() )
			{
				k++;

				std::cout	<< "< send_status[" << k << "] >" << std::endl;
				std::cout	<< "status  = ["
							<< send_status_itr->status << "]" << std::endl;
				std::cout	<< "send_end_size = ["
							<< send_status_itr->send_end_size << "]" << std::endl;
				std::cout	<< "send_rest_size = ["
							<< send_status_itr->send_rest_size << "]" << std::endl;
				std::cout	<< "send_possible_size = ["
							<< send_status_itr->send_possible_size << "]" << std::endl;
				std::cout	<< "send_offset = ["
							<< send_status_itr->send_offset << "]" << std::endl;
				std::cout	<< "unsend_size = ["
							<< send_status_itr->unsend_size << "]" << std::endl;
				std::cout	<< "edit_division = ["
							<< send_status_itr->edit_division << "]" << std::endl;
				std::cout	<< "send_endpoint = ["
							<< send_status_itr->send_endpoint << "]" << std::endl;
				std::cout	<< "edit_data_list entry = ["
							<< send_status_itr->edit_data_list.size() << "]" << std::endl;
				std::cout << std::endl;

				edit_data_itr = send_status_itr->edit_data_list.begin();
		
				l = 0;
		
				while( edit_data_itr != send_status_itr->edit_data_list.end() )
				{
					l++;
	
					std::cout	<< "< edit_data[" << l << "] >" << std::endl;
					std::cout	<< "data  = ["
								<< edit_data_itr->data << "]" << std::endl;
					std::cout	<< "data_size  = ["
								<< edit_data_itr->data_size << "]" << std::endl;
					std::cout	<< "insert_position  = ["
								<< edit_data_itr->insert_position << "]" << std::endl;
					std::cout	<< "replace_size  = ["
								<< edit_data_itr->replace_size << "]" << std::endl;
					std::cout << std::endl;

					edit_data_itr++;

				}

				send_status_itr++;

			}

			recive_data_itr++;

		}

		thread_data_itr++;

	}

	std::cout << std::endl;

}

void	session_thread_data_send(	boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint endpoint)
{

	t_session_thread_data_map_itr		thread_data_itr;
	t_recive_data_map_itr				recive_data_itr;
	t_send_status_list_itr				send_status_itr;

	thread_data_itr = session_thread_data_map.find( thread_id );

	if( thread_data_itr != session_thread_data_map.end() )
	{

		recive_data_itr = thread_data_itr->second->recive_data_map.find(endpoint);

		if( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
		{

			send_status_itr = recive_data_itr->second.send_status_list.begin();
	
			while( send_status_itr != recive_data_itr->second.send_status_list.end() )
			{

				if( send_status_itr->status == SEND_OK )
				{

					if( send_status_itr->send_rest_size > 0 )
					{
						send_status_itr->status = SEND_CONTINUE;
						send_status_itr->send_end_size = send_status_itr->send_possible_size;
						send_status_itr->send_possible_size = 0;
						send_status_itr->edit_division = EDIT_DIVISION_NO_EDIT;
					}
					else
					{
						send_status_itr->status = SEND_END;
					}
				}

				send_status_itr++;

			}

		}

	}

}


void session_thread_data_erase()
{

	t_session_thread_data_map_itr		thread_data_itr;

	thread_data_itr = session_thread_data_map.begin();

std::cout	<< "session_thread_data_erase" << std::endl;

session_thread_data_cinsert* thread_data;

thread_data = new session_thread_data_cinsert;

delete thread_data;

	while( thread_data_itr != session_thread_data_map.end() )
	{
		session_thread_data_map.erase(thread_data_itr);
		thread_data_itr = session_thread_data_map.begin();
	}

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

	t_session_thread_data_map_itr		up_thread_data_itr;
	t_session_thread_data_map_itr		down_thread_data_itr;
	t_recive_data_map_itr				recive_data_itr;

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

	t_session_thread_data_map_itr	up_thread_data_itr;
	t_session_thread_data_map_itr	down_thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;

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

	t_session_thread_data_map_itr	thread_data_itr;
	t_recive_data_map_itr			recive_data_itr;

	t_session_thread_data_cinsert	up_thread_data_1( new session_thread_data_cinsert );
	t_session_thread_data_cinsert	up_thread_data_2( new session_thread_data_cinsert );
	t_session_thread_data_cinsert	down_thread_data_1( new session_thread_data_cinsert );
	t_session_thread_data_cinsert	down_thread_data_2( new session_thread_data_cinsert );
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
//	up_thread_data_1 = new session_thread_data_cinsert;

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
//	up_thread_data_2 = new session_thread_data_cinsert;

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
//	down_thread_data_1 = new session_thread_data_cinsert;

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
//	down_thread_data_2 = new session_thread_data_cinsert;

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

	client_recv_data_1.recive_buffer_1		= NULL;
	client_recv_data_1.recive_buffer_2		= NULL;
	client_recv_data_2.recive_buffer_1		= NULL;
	client_recv_data_2.recive_buffer_2		= NULL;
	client_recv_data_3.recive_buffer_1		= NULL;
	client_recv_data_3.recive_buffer_2		= NULL;
	realserver_recv_data_1.recive_buffer_1	= NULL;
	realserver_recv_data_1.recive_buffer_2	= NULL;
	realserver_recv_data_2.recive_buffer_1	= NULL;
	realserver_recv_data_2.recive_buffer_2	= NULL;
	realserver_recv_data_3.recive_buffer_1	= NULL;
	realserver_recv_data_3.recive_buffer_2	= NULL;
	realserver_recv_data_4.recive_buffer_1	= NULL;
	realserver_recv_data_4.recive_buffer_2	= NULL;
	realserver_recv_data_5.recive_buffer_1	= NULL;
	realserver_recv_data_5.recive_buffer_2	= NULL;
	realserver_recv_data_6.recive_buffer_1	= NULL;
	realserver_recv_data_6.recive_buffer_2	= NULL;
	realserver_recv_data_7.recive_buffer_1	= NULL;
	realserver_recv_data_7.recive_buffer_2	= NULL;

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

//	realserver_recv_data_1.recive_buffer_1[0] = 'a';
//	realserver_recv_data_1.recive_buffer_1[0] = 'a';

	std::cout << std::endl;

 	BOOST_CHECK( status == STOP );

}

void	handle_session_finalize_test_thread( int thread_no, bool* ret )
{

// 	boost::xtime	start_time;
// 	boost::xtime	end_time;
// 
// 	protocol_module_cinsert::EVENT_TAG	status;
// 
// 	boost::thread	up_thread( &thread_method );
// 	boost::thread	down_thread( &thread_method );
// 
// 	boost::asio::ip::address	address;
// 	boost::asio::ip::tcp::endpoint client_endpoint_tcp( address, thread_no * 11111 );
// 	boost::asio::ip::udp::endpoint client_endpoint_udp( address, 00000 );
// 
// 	t_session_thread_data_map_itr	up_thread_data_itr;
// 	t_session_thread_data_map_itr	down_thread_data_itr;
// 	t_recive_data_map_itr				recive_data_itr;
// 
// 	{
// 		boost::mutex::scoped_lock	lk( sync_mutex );
// 		sync_condition.wait( lk );
// 	}
// 
// 	boost::xtime_get(&start_time, boost::TIME_UTC);
// 	status = handle_session_initialize(
// 						up_thread.get_id(),
// 						down_thread.get_id(),
// 						client_endpoint_tcp,
// 						client_endpoint_udp );
// 	boost::xtime_get(&end_time, boost::TIME_UTC);
// 
// 	boost::mutex::scoped_lock	lk( sync_mutex );
// 	sync_condition.wait( lk );
// 
// 	*ret = false;
// 
// 	std::cout << "<<<< ThreadNo = [" << thread_no << "] >>>>" << std::endl;
// 	std::cout << "StartTime = [" << start_time.nsec << "]" << std::endl;
// 	std::cout << "EndTime   = [" << end_time.nsec << "]" << std::endl;
// 	std::cout << "<<< parameter >>>" << std::endl;
// 	std::cout << "up_thread_id        = [" << up_thread.get_id() << "]" << std::endl;
// 	std::cout << "down_thread_id      = [" << down_thread.get_id() << "]" << std::endl;
// 	std::cout << "client_endpoint_tcp = [" << client_endpoint_tcp << "]" << std::endl;
// 	std::cout << "client_endpoint_udp = [" << client_endpoint_udp << "]" << std::endl;
// 	std::cout << std::endl;
//
// 	up_thread_data_itr = session_thread_data_map.find( up_thread.get_id());
// 
// 	if( up_thread_data_itr == session_thread_data_map.end()){ return; }
// 
// 	std::cout	<< "<< up_thread_data >>" << std::endl;
// 	std::cout	<< "thread_id               = ["
// 				<< up_thread_data_itr->second->thread_id << "]" << std::endl;
// 	std::cout	<< "thread_division         = ["
// 				<< up_thread_data_itr->second->thread_division << "]" << std::endl;
// 	std::cout	<< "pair_thread_id          = ["
// 				<< up_thread_data_itr->second->pair_thread_id << "]" << std::endl;
// 	std::cout	<< "end_flag                = ["
// 				<< up_thread_data_itr->second->end_flag << "]" << std::endl;
// 	std::cout	<< "accept_end_flag         = ["
// 				<< up_thread_data_itr->second->accept_end_flag << "]" << std::endl;
// 	std::cout	<< "sorry_flag              = ["
// 				<< up_thread_data_itr->second->sorry_flag << "]" << std::endl;
// 	std::cout	<< "sorryserver_switch_flag = ["
// 				<< up_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
// 	std::cout	<< "realserver_switch_flag  = ["
// 				<< up_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
// 	std::cout	<< "client_endpoint_tcp     = ["
// 				<< up_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
// 	std::cout << std::endl;
// 
// 	if( up_thread_data_itr->second->thread_id != up_thread.get_id()){ return; }
// 	if( up_thread_data_itr->second->thread_division != THREAD_DIVISION_UP_STREAM ){ return; }
// 	if( up_thread_data_itr->second->pair_thread_id != down_thread.get_id()){ return; }
// 	if( up_thread_data_itr->second->end_flag != END_FLAG_OFF ){ return; }
// 	if( up_thread_data_itr->second->accept_end_flag != ACCEPT_END_FLAG_OFF ){ return; }
// 	if( up_thread_data_itr->second->sorry_flag != SORRY_FLAG_OFF ){ return; }
// 	if( up_thread_data_itr->second->sorryserver_switch_flag != SORRYSERVER_SWITCH_FLAG_OFF ){ return; }
// 	if( up_thread_data_itr->second->realserver_switch_flag != REALSERVER_SWITCH_FLAG_OFF ){ return; }
// 	if( up_thread_data_itr->second->client_endpoint_tcp != client_endpoint_tcp){ return; }
// 
// 	recive_data_itr = up_thread_data_itr->second->recive_data_map.find( client_endpoint_tcp );
// 
// 	if( recive_data_itr == up_thread_data_itr->second->recive_data_map.end()){ return; }
// 
// 	std::cout	<< "< recive_data >" << std::endl;
// 	std::cout	<< "recive_buffer_max_size  = ["
// 				<< recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
// 	std::cout	<< "recive_buffer_rest_size = ["
// 				<< recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
// 	std::cout	<< "recive_buffer           = ["
// 				<< (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
// 	std::cout	<< "recive_buffer_1         = ["
// 				<< (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
// 	std::cout	<< "recive_buffer_2         = ["
// 				<< (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
// 	std::cout << std::endl;
// 
// 	if( recive_data_itr->second.recive_buffer_max_size != MAX_BUFFER_SIZE ){ return; }
// 	if( recive_data_itr->second.recive_buffer_rest_size != MAX_BUFFER_SIZE ){ return; }
// 	if( recive_data_itr->second.recive_buffer != recive_data_itr->second.recive_buffer_1 ){ return; }
// 	if( recive_data_itr->second.recive_buffer_1 == NULL ){ return; }
// 	if( recive_data_itr->second.recive_buffer_2 == NULL ){ return; }
// 
// 	down_thread_data_itr = session_thread_data_map.find( down_thread.get_id());
// 
// 	if( down_thread_data_itr == session_thread_data_map.end()){ return; }
// 
// 	std::cout	<< "<< down_thread_data >>" << std::endl;
// 	std::cout	<< "thread_id               = ["
// 				<< down_thread_data_itr->second->thread_id << "]" << std::endl;
// 	std::cout	<< "thread_division         = ["
// 				<< down_thread_data_itr->second->thread_division << "]" << std::endl;
// 	std::cout	<< "pair_thread_id          = ["
// 				<< down_thread_data_itr->second->pair_thread_id << "]" << std::endl;
// 	std::cout	<< "end_flag                = ["
// 				<< down_thread_data_itr->second->end_flag << "]" << std::endl;
// 	std::cout	<< "accept_end_flag         = ["
// 				<< down_thread_data_itr->second->accept_end_flag << "]" << std::endl;
// 	std::cout	<< "sorry_flag              = ["
// 				<< down_thread_data_itr->second->sorry_flag << "]" << std::endl;
// 	std::cout	<< "sorryserver_switch_flag = ["
// 				<< down_thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
// 	std::cout	<< "realserver_switch_flag  = ["
// 				<< down_thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
// 	std::cout	<< "client_endpoint_tcp     = ["
// 				<< down_thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
// 	std::cout << std::endl;
// 
// 	if( down_thread_data_itr->second->thread_id != down_thread.get_id()){ return; }
// 	if( down_thread_data_itr->second->thread_division != THREAD_DIVISION_DOWN_STREAM ){ return; }
// 	if( down_thread_data_itr->second->pair_thread_id != up_thread.get_id()){ return; }
// 	if( down_thread_data_itr->second->end_flag != END_FLAG_OFF ){ return; }
// 	if( down_thread_data_itr->second->accept_end_flag != ACCEPT_END_FLAG_OFF ){ return; }
// 	if( down_thread_data_itr->second->sorry_flag != SORRY_FLAG_OFF ){ return; }
// 	if( down_thread_data_itr->second->sorryserver_switch_flag != SORRYSERVER_SWITCH_FLAG_OFF ){ return; }
// 	if( down_thread_data_itr->second->realserver_switch_flag != REALSERVER_SWITCH_FLAG_OFF ){ return; }
// 	if( down_thread_data_itr->second->client_endpoint_tcp != client_endpoint_tcp){ return; }
// 
// 	if( down_thread_data_itr->second->recive_data_map.empty() != true ){ return; }
// 
// 	if( status != ACCEPT ){ return; }

	*ret = true;

	return;

}

void check_parameter_test()
{

	check_message_result check_result;
	std::vector<std::string> args;

//-------------------------------------------------------------
	args.push_back("-C");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-R");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_01>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("--cookie-name");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("--cookie-expire");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("--forwarded-for");
	args.push_back("--reschedule");
	args.push_back("--sorry-uri");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_02>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_03>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("--cookie-name");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("--cookie-expire");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("--forwarded-for");
	args.push_back("--no-reschedule");
	args.push_back("--sorry-uri");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_04>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_05>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-C/--cookie-name'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_06>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-C/--cookie-name' option value 'ABCDEfghij1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678' is too long." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij_1234567890");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_07>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-C/--cookie-name' option value 'ABCDEfghij_1234567890' is not alphanumeric character." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-C");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_08>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-C/--cookie-name'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_09>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot set multiple option '-C/--cookie-name'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("-R");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_10>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-E/--cookie-expire'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("123abc");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_11>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-E/--cookie-expire' option value '123abc' is not numeric character." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("2147483648");	//INT_MAX + 1

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_12>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-E/--cookie-expire' option value '2147483648' is too large." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_13>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-E/--cookie-expire'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_14>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot set multiple option '-E/--cookie-expire'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-R");
	args.push_back("-N");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_15>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to choose either of reschedule or no-reschedule." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-N");
	args.push_back("-R");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_16>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to choose either of reschedule or no-reschedule." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-S");
	args.push_back("-F");
	args.push_back("-N");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_17>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-S/--sorry-uri'." );

//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F900");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_18>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-S/--sorry-uri' option value '/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F900' is too long." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/~a0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_19>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-S/--sorry-uri' option value '/~a0$-_.+!*'(),%00%0a%0A%A0%Aa%AA' is not a valid URI." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_20>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-S/--sorry-uri'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_21>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot set multiple option '-S/--sorry-uri'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");
	args.push_back("-X");

	check_result = check_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<check_parameter_22>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Option error." );

}

void set_parameter_test()
{

	check_message_result check_result;
	std::vector<std::string> args;
	

//-------------------------------------------------------------
	args.push_back("-C");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-R");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_01>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );

	BOOST_CHECK( memcmp( cookie_name.data(),  "ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", strlen(cookie_name.data())) == 0 );
	BOOST_CHECK( cookie_expire == INT_MAX );
	BOOST_CHECK( forwarded_for == 1 );
	BOOST_CHECK( reschedule == 1 );
	BOOST_CHECK( memcmp( sorry_uri.data(), "/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90", strlen(sorry_uri.data())) == 0 );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("--cookie-name");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("--cookie-expire");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("--forwarded-for");
	args.push_back("--reschedule");
	args.push_back("--sorry-uri");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_02>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );

	BOOST_CHECK( memcmp( cookie_name.data(),  "ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", strlen(cookie_name.data())) == 0 );
	BOOST_CHECK( cookie_expire == INT_MAX );
	BOOST_CHECK( forwarded_for == 1 );
	BOOST_CHECK( reschedule == 1 );
	BOOST_CHECK( memcmp( sorry_uri.data(), "/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90", strlen(sorry_uri.data())) == 0 );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_03>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );

	BOOST_CHECK( memcmp( cookie_name.data(),  "ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", strlen(cookie_name.data())) == 0 );
	BOOST_CHECK( cookie_expire == INT_MAX );
	BOOST_CHECK( forwarded_for == 1 );
	BOOST_CHECK( reschedule == 0 );
	BOOST_CHECK( memcmp( sorry_uri.data(), "/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90", strlen(sorry_uri.data())) == 0 );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("--cookie-name");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("--cookie-expire");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("--forwarded-for");
	args.push_back("--no-reschedule");
	args.push_back("--sorry-uri");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_04>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );

	BOOST_CHECK( memcmp( cookie_name.data(),  "ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567", strlen(cookie_name.data())) == 0 );
	BOOST_CHECK( cookie_expire == INT_MAX );
	BOOST_CHECK( forwarded_for == 1 );
	BOOST_CHECK( reschedule == 0 );
	BOOST_CHECK( memcmp( sorry_uri.data(), "/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90", strlen(sorry_uri.data())) == 0 );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_05>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-C/--cookie-name'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_06>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-C/--cookie-name' option value 'ABCDEfghij1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678' is too long." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij_1234567890");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_07>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-C/--cookie-name' option value 'ABCDEfghij_1234567890' is not alphanumeric character." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-C");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_08>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-C/--cookie-name'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_09>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot set multiple option '-C/--cookie-name'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("-R");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_10>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-E/--cookie-expire'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("123abc");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_11>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-E/--cookie-expire' option value '123abc' is not numeric character." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("2147483648");	//INT_MAX + 1

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_12>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-E/--cookie-expire' option value '2147483648' is too large." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_13>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-E/--cookie-expire'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_14>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot set multiple option '-E/--cookie-expire'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-R");
	args.push_back("-N");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_15>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to choose either of reschedule or no-reschedule." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-N");
	args.push_back("-R");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_16>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to choose either of reschedule or no-reschedule." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-S");
	args.push_back("-F");
	args.push_back("-N");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_17>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-S/--sorry-uri'." );

//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F900");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_18>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-S/--sorry-uri' option value '/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F900' is too long." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/~a0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_19>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "'-S/--sorry-uri' option value '/~a0$-_.+!*'(),%00%0a%0A%A0%Aa%AA' is not a valid URI." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_20>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "You have to set option value '-S/--sorry-uri'." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_21>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot set multiple option '-S/--sorry-uri'." );

//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	args.push_back("-C");
	args.push_back("ABCDEfghij1234567890");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-N");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA");
	args.push_back("-X");

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_22>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;
	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Option error." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	check_result = set_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<set_parameter_23>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );

	BOOST_CHECK( memcmp( cookie_name.data(),  "CookieName", strlen(cookie_name.data())) == 0 );
	BOOST_CHECK( cookie_expire == 86400 );
	BOOST_CHECK( forwarded_for == 0 );
	BOOST_CHECK( reschedule == 0 );
	BOOST_CHECK( strlen(sorry_uri.data()) == 0 );

}

void add_parameter_test()
{

	check_message_result check_result;
	std::vector<std::string> args;
	

//-------------------------------------------------------------
	args.push_back("-C");
	args.push_back("ABCDEfghij123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	args.push_back("-E");
	args.push_back("2147483647");	//INT_MAX
	args.push_back("-F");
	args.push_back("-R");
	args.push_back("-S");
	args.push_back("/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F9%Ff%FF/Aa0$-_.+!*'(),%00%0a%0A%A0%Aa%AA/zZ9$-_.+!*'(),%99%9f%9F%F90");

	check_result = add_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<add_parameter_01>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == false );
	BOOST_CHECK( check_result.message == "Cannot add option." );
//-------------------------------------------------------------
	args.erase( args.begin(), args.end());

	check_result = add_parameter(args);

	std::cout	<< std::endl;
	std::cout	<< "<add_parameter_02>" << std::endl;
	std::cout	<< "check_result.flag = [" << check_result.flag << "]" << std::endl;
	std::cout	<< "check_result.message = [" << check_result.message << "]" << std::endl;

	BOOST_CHECK( check_result.flag == true );
	BOOST_CHECK( check_result.message == "" );
}

void	handle_client_recv_test()
{

	boost::array< char, MAX_BUFFER_SIZE > recvbuffer;
	size_t recvlen;

	t_session_thread_data_map_itr	thread_data_itr;

	protocol_module_cinsert::EVENT_TAG	status;

	std::string data;

	data	=	"PUT /abc/def/ HTTP/1.0\r\n";
	data	+=	"Cookie: 10.10.10.10:11111;\r\n";
	data	+=	"X-Forwarded-For: 20.20.20.20\r\n";
	data	+=	"CONTENT-LENGTH:100\r\n\r\n";
	data	+=	"abcdefghij1234567890abcdefghij1234567890abcdefghij";
	data	+=	"1234567890abcdefghij1234567890abcdefghij1234567890";

	data	+=	"GET /abc/def/ HTTP/1.0\r\n";
	data	+=	"Cookie: 20.20.20.20:22222;\r\n";
	data	+=	"X-Forwarded-For: 20.20.20.20\r\n\r\n";

	data	+=	"HEAD /abc/def/ HTTP/1.0\r\n";
	data	+=	"Cookie: 20.20.20.20:22222;\r\n";
	data	+=	"X-Forwarded-For: 20.20.20.20\r\n";
	data	+=	"CONTENT-LENGTH:200\r\n\r\n";
	data	+=	"abcdefghij1234567890abcdefghij1234567890abcdefghij";
	data	+=	"1234567890abcdefghij1234567890abcdefghij1234567890";
	data	+=	"abcdefghij1234567890abcdefghij1234567890abcdefghij";

	recvbuffer.assign('\0');
	memcpy( recvbuffer.data(), data.c_str(), strlen(data.c_str()));

	recvlen = strlen(recvbuffer.data());

	session_thread_data_set();

	thread_data_itr = session_thread_data_map.begin();

	while( thread_data_itr != session_thread_data_map.end())
	{
		if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
		{
			break;
		}
		thread_data_itr++;
	}

	status = handle_client_recv( thread_data_itr->second->thread_id, recvbuffer, recvlen );

	session_thread_data_disp();

	session_thread_data_send(	thread_data_itr->second->thread_id,
								thread_data_itr->second->client_endpoint_tcp );

	session_thread_data_disp();

	data	=	"abcdefghij1234567890abcdefghij1234567890abcdefghij";

	data	+=	"PUT /abc/def/ HTTP/1.0\r\n";
	data	+=	"Cookie: 10.10.10.10:11111;\r\n";
	data	+=	"X-Forwarded-For: 20.20.20.20\r\n";
	data	+=	"CONTENT-LENGTH:50\r\n\r\n";
	data	+=	"abcdefghij1234567890";

	recvbuffer.assign('\0');
	memcpy( recvbuffer.data(), data.c_str(), strlen(data.c_str()));

	recvlen = strlen(recvbuffer.data());

	status = handle_client_recv( thread_data_itr->second->thread_id, recvbuffer, recvlen );

	session_thread_data_disp();

}

void	handle_client_send_test()
{

	t_session_thread_data_map_itr	thread_data_itr;

	protocol_module_cinsert::EVENT_TAG	status;

	session_thread_data_set();

	// unit_test[x4_1]  handle_client_sendのテスト（存在しないID STOP）
	boost::thread::id	thread_id;
	status = handle_client_send( thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_2]  handle_client_sendのテスト（send_status無し REALSERVER_RECV）
	thread_data_itr = session_thread_data_map.begin();

	while( thread_data_itr != session_thread_data_map.end())
	{
		if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
		{
			break;
		}
		thread_data_itr++;
	}

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, REALSERVER_RECV );

	// unit_test[x4_3]  handle_client_sendのテスト（send_status無し CLIENT_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, SORRYSERVER_RECV );

	// unit_test[x4_4]  handle_client_sendのテスト（send_status無し CLIENT_DISCONNECT）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );

	// unit_test[x4_5]  handle_client_sendのテスト（send_possible_size有り CLIENT_CONNECTION_CHECK）
	t_recive_data_map_itr				recive_data_itr;
	send_status							send_status_add;

	send_status_add.status				= SEND_NG;
	send_status_add.send_end_size		= 0;
	send_status_add.send_rest_size		= 0;
	send_status_add.send_possible_size	= 0;
	send_status_add.unsend_size			= 0;
	send_status_add.edit_division		= 0;
	send_status_add.send_offset			= 0;

	thread_data_itr->second->end_flag = END_FLAG_OFF;
	thread_data_itr->second->sorry_flag = SORRY_FLAG_OFF;

	recive_data_itr = thread_data_itr->second->recive_data_map.begin();

	if( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
	{
		thread_data_itr->second->last_endpoint_tcp = recive_data_itr->first;
//std::cout << thread_data_itr->second->last_endpoint_tcp.address() << ":" << thread_data_itr->second->last_endpoint_tcp.port() << "\n";
	
		send_status_add.status = SEND_OK;
		send_status_add.send_possible_size = 1;
		recive_data_itr->second.send_status_list.push_back( send_status_add );

		BOOST_CHECK_EQUAL( recive_data_itr->second.send_status_list.size(), ( size_t )1 );
	}

	t_send_status_list_itr				send_status_itr = recive_data_itr->second.send_status_list.begin();

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_CONNECTION_CHECK );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_6]  handle_client_sendのテスト（send_possible_size無し REALSERVER_RECV）

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, REALSERVER_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_7]  handle_client_sendのテスト（send_rest_size有り REALSERVER_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, REALSERVER_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_8]  handle_client_sendのテスト（send_possible_size無し SORRYSERVER_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, SORRYSERVER_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_9]  handle_client_sendのテスト（send_rest_size有り SORRYSERVER_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, SORRYSERVER_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_10]  handle_client_sendのテスト（send_possible_size無し CLIENT_DISCONNECT）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_11]  handle_client_sendのテスト（send_rest_size有り CLIENT_DISCONNECT）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );




	// unit_test[x4_12]  handle_client_sendのテスト（receive_data無し REALSERVER_RECV）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	thread_data_itr->second->sorry_flag = SORRY_FLAG_OFF;
	thread_data_itr->second->recive_data_map.clear();
	BOOST_CHECK_EQUAL( thread_data_itr->second->recive_data_map.size(), ( size_t )0 );

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, REALSERVER_RECV );

	// unit_test[x4_13]  handle_client_sendのテスト（receive_data無し SORRYSERVER_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, SORRYSERVER_RECV );

	// unit_test[x4_14]  handle_client_sendのテスト（receive_data無し CLIENT_DISCONNECT）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	status = handle_client_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );

//	session_thread_data_disp();
}

void	handle_realserver_send_test()
{

	t_session_thread_data_map_itr	thread_data_itr;

	protocol_module_cinsert::EVENT_TAG	status;

	session_thread_data_set();

	// unit_test[x4_15]  handle_realserver_sendのテスト（存在しないID STOP）
	boost::thread::id	thread_id;
	status = handle_realserver_send( thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_16]  handle_realserver_sendのテスト（send_status無し CLIENT_RECV）
	thread_data_itr = session_thread_data_map.begin();

	while( thread_data_itr != session_thread_data_map.end())
	{
		if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
		{
			break;
		}
		thread_data_itr++;
	}

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );

	// unit_test[x4_17]  handle_realserver_sendのテスト（send_status無し sorry_flag有り CLIENT_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );

	// unit_test[x4_18]  handle_realserver_sendのテスト（send_status無し end_flag有り CLIENT_RECEV）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );

	// unit_test[x4_19]  handle_realserver_sendのテスト（send_possible_size有り REALSERVER_SELECT）
	t_recive_data_map_itr				recive_data_itr;
	send_status							send_status_add;

	send_status_add.status				= SEND_NG;
	send_status_add.send_end_size		= 0;
	send_status_add.send_rest_size		= 0;
	send_status_add.send_possible_size	= 0;
	send_status_add.unsend_size			= 0;
	send_status_add.edit_division		= 0;
	send_status_add.send_offset			= 0;

	thread_data_itr->second->end_flag = END_FLAG_OFF;
	thread_data_itr->second->sorry_flag = SORRY_FLAG_OFF;

	recive_data_itr = thread_data_itr->second->recive_data_map.begin();

	if( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
	{
		thread_data_itr->second->last_endpoint_tcp = recive_data_itr->first;
//std::cout << thread_data_itr->second->last_endpoint_tcp.address() << ":" << thread_data_itr->second->last_endpoint_tcp.port() << "\n";
	
		send_status_add.status = SEND_OK;
		send_status_add.send_possible_size = 1;
		recive_data_itr->second.send_status_list.push_back( send_status_add );

		BOOST_CHECK_EQUAL( recive_data_itr->second.send_status_list.size(), ( size_t )1 );
	}

	t_send_status_list_itr				send_status_itr = recive_data_itr->second.send_status_list.begin();

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, REALSERVER_SELECT );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_20]  handle_realserver_sendのテスト（send_possible_size無し CLIENT_RECV）

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_21]  handle_realserver_sendのテスト（send_rest_size有り CLIENT_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_22]  handle_realserver_sendのテスト（send_possible_size無し sorry_flag有り CLIENT_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_23]  handle_realserver_sendのテスト（send_rest_size有り sorry_flag有り CLIENT_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_24]  handle_realserver_sendのテスト（send_possible_size無し end_flag有り CLIENT_RECV）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_25]  handle_realserver_sendのテスト（send_rest_size有り end_flag有り CLIENT_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );




	// unit_test[x4_26]  handle_realserver_sendのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	thread_data_itr->second->sorry_flag = SORRY_FLAG_OFF;
	thread_data_itr->second->recive_data_map.clear();
	BOOST_CHECK_EQUAL( thread_data_itr->second->recive_data_map.size(), ( size_t )0 );

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_27]  handle_realserver_sendのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_28]  handle_realserver_sendのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	status = handle_realserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

//	session_thread_data_disp();
}

void	handle_sorryserver_send_test()
{

	t_session_thread_data_map_itr	thread_data_itr;

	protocol_module_cinsert::EVENT_TAG	status;

	session_thread_data_set();

	// unit_test[x4_29]  handle_sorryserver_sendのテスト（存在しないID STOP）
	boost::thread::id	thread_id;
	status = handle_sorryserver_send( thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_30]  handle_sorryserver_sendのテスト（send_status無し CLIENT_RECV）
	thread_data_itr = session_thread_data_map.begin();

	while( thread_data_itr != session_thread_data_map.end())
	{
		if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
		{
			break;
		}
		thread_data_itr++;
	}

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );

	// unit_test[x4_31]  handle_sorryserver_sendのテスト（send_status無し sorry_flag有り CLIENT_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );

	// unit_test[x4_32]  handle_sorryserver_sendのテスト（send_status無し end_flag有り CLIENT_RECEV）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );

	// unit_test[x4_33]  handle_sorryserver_sendのテスト（send_possible_size有り SORRYSERVER_SELECT）
	t_recive_data_map_itr				recive_data_itr;
	send_status							send_status_add;

	send_status_add.status				= SEND_NG;
	send_status_add.send_end_size		= 0;
	send_status_add.send_rest_size		= 0;
	send_status_add.send_possible_size	= 0;
	send_status_add.unsend_size			= 0;
	send_status_add.edit_division		= 0;
	send_status_add.send_offset			= 0;

	thread_data_itr->second->end_flag = END_FLAG_OFF;
	thread_data_itr->second->sorry_flag = SORRY_FLAG_OFF;

	recive_data_itr = thread_data_itr->second->recive_data_map.begin();

	if( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
	{
		thread_data_itr->second->last_endpoint_tcp = recive_data_itr->first;
//std::cout << thread_data_itr->second->last_endpoint_tcp.address() << ":" << thread_data_itr->second->last_endpoint_tcp.port() << "\n";
	
		send_status_add.status = SEND_OK;
		send_status_add.send_possible_size = 1;
		recive_data_itr->second.send_status_list.push_back( send_status_add );

		BOOST_CHECK_EQUAL( recive_data_itr->second.send_status_list.size(), ( size_t )1 );
	}

	t_send_status_list_itr				send_status_itr = recive_data_itr->second.send_status_list.begin();

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, SORRYSERVER_SELECT );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_34]  handle_sorryserver_sendのテスト（send_possible_size無し CLIENT_RECV）

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_35]  handle_sorryserver_sendのテスト（send_rest_size有り CLIENT_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_36]  handle_sorryserver_sendのテスト（send_possible_size無し sorry_flag有り CLIENT_RECV）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_37]  handle_sorryserver_sendのテスト（send_rest_size有り sorry_flag有り CLIENT_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_38]  handle_sorryserver_sendのテスト（send_possible_size無し end_flag有り CLIENT_RECV）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 0;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_39]  handle_sorryserver_sendのテスト（send_rest_size有り end_flag有り CLIENT_RECV）
	send_status_itr->status = SEND_OK;
	send_status_itr->send_possible_size = 0;
	send_status_itr->send_rest_size = 1;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, CLIENT_RECV );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );




	// unit_test[x4_40]  handle_sorryserver_sendのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	thread_data_itr->second->sorry_flag = SORRY_FLAG_OFF;
	thread_data_itr->second->recive_data_map.clear();
	BOOST_CHECK_EQUAL( thread_data_itr->second->recive_data_map.size(), ( size_t )0 );

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_41]  handle_sorryserver_sendのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_42]  handle_sorryserver_sendのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->end_flag = END_FLAG_ON;

	status = handle_sorryserver_send( thread_data_itr->second->thread_id );
	BOOST_CHECK_EQUAL( status, STOP );

//	session_thread_data_disp();
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

// void	session_thread_data_set_test()
// {
// 
// 	protocol_module_cinsert_test	protocol_module_cinsert_test_1;
// 
// 	protocol_module_cinsert_test_1.session_thread_data_set();
// 
// 	protocol_module_cinsert_test_1.session_thread_data_disp();
// 
// 	protocol_module_cinsert_test_1.session_thread_data_erase();
// 
// }

void	check_parameter_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- check_parameter test start -----" );
	protocol_module_cinsert_test_1.check_parameter_test();
	BOOST_MESSAGE( "----- check_parameter test end -----" );

}
void	set_parameter_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- set_parameter test start -----" );
	protocol_module_cinsert_test_1.set_parameter_test();
	BOOST_MESSAGE( "----- set_parameter test end -----" );

}
void	add_parameter_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- add_parameter test start -----" );
	protocol_module_cinsert_test_1.add_parameter_test();
	BOOST_MESSAGE( "----- add_parameter test end -----" );

}

void	handle_client_recv_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	BOOST_MESSAGE( "----- handle_client_recv test start -----" );
	protocol_module_cinsert_test_1.handle_client_recv_test();
	BOOST_MESSAGE( "----- handle_client_recv test end -----" );

}

void	handle_client_send_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	// unit_test[x4_]  
	BOOST_MESSAGE( "----- handle_client_send test start -----" );
	protocol_module_cinsert_test_1.handle_client_send_test();
	BOOST_MESSAGE( "----- handle_client_send test end -----" );

}

void	handle_realserver_send_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	// unit_test[x4_]  
	BOOST_MESSAGE( "----- handle_realserver_send test start -----" );
	protocol_module_cinsert_test_1.handle_realserver_send_test();
	BOOST_MESSAGE( "----- handle_realserver_send test end -----" );

}

void	handle_sorryserver_send_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	// unit_test[x4_]  
	BOOST_MESSAGE( "----- handle_sorryserver_send test start -----" );
	protocol_module_cinsert_test_1.handle_sorryserver_send_test();
	BOOST_MESSAGE( "----- handle_sorryserver_send test end -----" );

}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_cinsert class test" );

	ts->add( BOOST_TEST_CASE( &handle_client_send_test ) );
	ts->add( BOOST_TEST_CASE( &handle_realserver_send_test ) );
	ts->add( BOOST_TEST_CASE( &handle_sorryserver_send_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
