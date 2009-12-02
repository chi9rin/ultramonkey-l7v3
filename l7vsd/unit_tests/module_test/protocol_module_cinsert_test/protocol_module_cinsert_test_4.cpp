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

//--stub functions--
LOG_LEVEL_TAG	stb_getloglevel(){
//	std::cout << "getloglevel called." << std::endl;
	return LOG_LV_NONE;
}

void	stb_putLogFatal( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogFatal called." << std::endl;
}
void	stb_putLogError( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogError called." << std::endl;
}
void	stb_putLogWarn( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogWarn called." << std::endl;
}
void	stb_putLogInfo( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogInfo called." << std::endl;
}
void	stb_putLogDebug( const unsigned int, const std::string&, const char*, int ){
//	std::cout << "putLogDebug called." << std::endl;
}

void	handle_schedule1(
							boost::thread::id					thread_id,
							l7vs::protocol_module_base::rs_list_itr_func_type				inlist_begin,
							l7vs::protocol_module_base::rs_list_itr_func_type				inlist_end,
							l7vs::protocol_module_base::rs_list_itr_next_func_type			inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint ){
	outendpoint = boost::asio::ip::tcp::endpoint();
}

void	handle_schedule2(
							boost::thread::id					thread_id,
							l7vs::protocol_module_base::rs_list_itr_func_type				inlist_begin,
							l7vs::protocol_module_base::rs_list_itr_func_type				inlist_end,
							l7vs::protocol_module_base::rs_list_itr_next_func_type			inlist_next,
							boost::asio::ip::tcp::endpoint&		outendpoint ){
	outendpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 );
}

l7vs::protocol_module_base::realserverlist_type::iterator	list_begin( l7vs::protocol_module_base::realserverlist_type* list ){
	return(list->begin());
}

l7vs::protocol_module_base::realserverlist_type::iterator	list_end( l7vs::protocol_module_base::realserverlist_type* list ){
	return(list->end());
}

l7vs::protocol_module_base::realserverlist_type::iterator	list_next( l7vs::protocol_module_base::realserverlist_type::iterator itr ){
	return(++itr);
}


//--test class--
class	protocol_module_cinsert_test : public protocol_module_cinsert
{
public:
boost::mutex		sync_mutex;
boost::condition	sync_condition;
boost::condition	session_thread_data_map_condition;

protocol_module_cinsert_test() : protocol_module_cinsert()
{
	getloglevel	= &stb_getloglevel;
	putLogFatal	= &stb_putLogFatal;
	putLogError	= &stb_putLogError;
	putLogWarn	= &stb_putLogWarn;
	putLogInfo	= &stb_putLogInfo;
	putLogDebug	= &stb_putLogDebug;
}
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

void	handle_realserver_select_test()
{

	t_session_thread_data_map_itr	thread_data_itr;

	protocol_module_cinsert::EVENT_TAG	status;
	boost::asio::ip::tcp::endpoint default_endpoint;
	boost::asio::ip::tcp::endpoint rs_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;
	boost::asio::ip::tcp::endpoint rs_endpoint2 = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.87" ), 22 ) ;
	boost::asio::ip::tcp::endpoint tcp_endpoint;

	session_thread_data_set();

	// unit_test[x4_43]  handle_realserver_selectのテスト（存在しないID STOP）
	boost::thread::id	thread_id;
	status = handle_realserver_select( thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_44]  handle_realserver_selectのテスト（send_status無し CLIENT_STOP）
	thread_data_itr = session_thread_data_map.begin();

	while( thread_data_itr != session_thread_data_map.end())
	{
		if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
		{
			break;
		}
		thread_data_itr++;
	}

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_45]  handle_realserver_selectのテスト（endpoint有り REALSERVER_CONNECT）
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
		send_status_add.send_endpoint = rs_endpoint;
		recive_data_itr->second.send_status_list.push_back( send_status_add );

		BOOST_CHECK_EQUAL( recive_data_itr->second.send_status_list.size(), ( size_t )1 );
	}

	t_send_status_list_itr				send_status_itr = recive_data_itr->second.send_status_list.begin();

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, REALSERVER_CONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, rs_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_46]  handle_realserver_selectのテスト（SEND_OK無し=SEND_NG STOP）
	send_status_itr->status = SEND_NG;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_NG );

	// unit_test[x4_47]  handle_realserver_selectのテスト（SEND_OK無し=SEND_CONTINUE STOP）
	send_status_itr->status = SEND_CONTINUE;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_48]  handle_realserver_selectのテスト（SEND_OK無し=SEND_END STOP）
	send_status_itr->status = SEND_END;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_49]  handle_realserver_selectのテスト（endpoint無しCookie無しschedule結果無し CLIENT_DISCONNECT）
	schedule_tcp.clear();
	schedule_tcp = handle_schedule1;
	send_status_itr->send_endpoint = default_endpoint;
	thread_data_itr->second->last_endpoint_tcp = default_endpoint;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
	BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_50]  handle_realserver_selectのテスト（endpoint無しCookie無しschedule結果有り REALSERVER_CONNECT）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule2;
	send_status_itr->send_endpoint = default_endpoint;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, REALSERVER_CONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, rs_endpoint2 );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_51]  handle_realserver_selectのテスト（endpoint無しCookie有り該当結果有りrealserver_list無し FINALIZE）
	std::string	data;

	data =	"HTTP/1.0 100 abcd\r\n";
	data +=	"Cookie: CookieName=10.10.10.10:11111;\r\n";
	data +=	"X-Forwarded-For: 20.20.20.20\r\n";
	data +=	"CONTENT-LENGTH:100\r\n\r\n";
	data +=	"abcdefghij1234567890abcdefghij1234567890abcdefghij";
	data +=	"1234567890abcdefghij1234567890abcdefghij1234567890";

	strcpy( recive_data_itr->second.recive_buffer, data.c_str() );
	recive_data_itr->second.recive_buffer_rest_size -= data.size();
	send_status_itr->send_possible_size = data.size();
	strcpy( cookie_name.data(), "CookieName" );

	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule1;
	send_status_itr->send_endpoint = default_endpoint;
	thread_data_itr->second->last_endpoint_tcp = default_endpoint;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, FINALIZE );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_52]  handle_realserver_selectのテスト（endpoint無しCookie有り該当結果有りrealserver無し CLIENT_DISCONNECT）
	l7vs::protocol_module_base::realserverlist_type		rs_list;

	rs_list_begin = boost::bind( &list_begin, &rs_list );
	rs_list_end = boost::bind( &list_end, &rs_list );
	rs_list_next = boost::bind( &list_next, _1 );

	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule1;
	send_status_itr->send_endpoint = default_endpoint;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_53]  handle_realserver_selectのテスト（endpoint無しCookie有り該当結果有りrealserver無し強制再設定schedule結果有り REALSERVER_CONNECT）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule2;
	send_status_itr->send_endpoint = default_endpoint;
	reschedule = 1;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, REALSERVER_CONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, rs_endpoint2 );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_54]  handle_realserver_selectのテスト（endpoint無しCookie有り該当結果有りrealserver無し強制再設定schedule結果無し CLIENT_DISCONNECT）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule1;
	send_status_itr->send_endpoint = default_endpoint;
	thread_data_itr->second->last_endpoint_tcp = default_endpoint;
	reschedule = 1;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_55]  handle_realserver_selectのテスト（endpoint無しCookie有り該当結果有りrealserver有り REALSERVER_CONNECT）
	reschedule = 0;
	l7vs::realserver	server1, server2, server3, server4;

//	l7vs::protocol_module_base::rs_list_itr_func_type		rslist_begin;
//	l7vs::protocol_module_base::rs_list_itr_func_type		rslist_end;
//	l7vs::protocol_module_base::rs_list_itr_next_func_type	rslist_next;

	server1.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.10.10.10" ), 11111  ) ;
	server1.weight = 1;
	rs_list.push_back( server1 );
	server2.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "20.20.20.20" ), 22222  ) ;
	server2.weight = 2;
	rs_list.push_back( server2 );
	server3.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "30.30.30.30" ), 33333  ) ;
	server3.weight = 3;
	rs_list.push_back( server3 );
	server4.tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "40.40.40.40" ), 44444  ) ;
	server4.weight = 4;
	rs_list.push_back( server4 );

	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule1;
	send_status_itr->send_endpoint = default_endpoint;
	thread_data_itr->second->last_endpoint_tcp = default_endpoint;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, REALSERVER_CONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, server1.tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_56]  handle_realserver_selectのテスト（endpoint無しCookie有り該当結果有りrealserver有り強制再設定無視 REALSERVER_CONNECT）
	thread_data_itr->second->end_flag = END_FLAG_OFF;
	schedule_tcp.clear();
	schedule_tcp = handle_schedule2;
	send_status_itr->send_endpoint = default_endpoint;
	thread_data_itr->second->last_endpoint_tcp = default_endpoint;
	reschedule = 1;

	send_status_itr->status = SEND_OK;

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, REALSERVER_CONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, server1.tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );






	// unit_test[x4_57]  handle_realserver_selectのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->recive_data_map.clear();
	BOOST_CHECK_EQUAL( thread_data_itr->second->recive_data_map.size(), ( size_t )0 );

	status = handle_realserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );

//	session_thread_data_disp();
}

void	handle_sorryserver_select_test()
{

	t_session_thread_data_map_itr	thread_data_itr;

	protocol_module_cinsert::EVENT_TAG	status;
	boost::asio::ip::tcp::endpoint tcp_endpoint = boost::asio::ip::tcp::endpoint ( boost::asio::ip::address::from_string( "10.144.169.86" ), 21 ) ;

	session_thread_data_set();

	// unit_test[x4_58]  handle_sorryserver_selectのテスト（存在しないID STOP）
	boost::thread::id	thread_id;
	status = handle_sorryserver_select( thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );

	// unit_test[x4_59]  handle_sorryserver_selectのテスト（send_status無し STOP）
	thread_data_itr = session_thread_data_map.begin();

	while( thread_data_itr != session_thread_data_map.end())
	{
		if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
		{
			break;
		}
		thread_data_itr++;
	}

	status = handle_sorryserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );

//	thread_data_itr->second->sorry_flag = SORRY_FLAG_ON;

	// unit_test[x4_60]  handle_sorryserver_selectのテスト（SORRYSERVER_CONNECT）
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
		recive_data_itr->second.send_status_list.push_back( send_status_add );

		BOOST_CHECK_EQUAL( recive_data_itr->second.send_status_list.size(), ( size_t )1 );
	}

	t_send_status_list_itr				send_status_itr = recive_data_itr->second.send_status_list.begin();

	status = handle_sorryserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, SORRYSERVER_CONNECT );
	BOOST_CHECK_EQUAL( send_status_itr->send_endpoint, tcp_endpoint );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_OK );

	// unit_test[x4_61]  handle_sorryserver_selectのテスト（SEND_OK無し=SEND_NG STOP）
	send_status_itr->status = SEND_NG;

	status = handle_sorryserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_NG );

	// unit_test[x4_62]  handle_sorryserver_selectのテスト（SEND_OK無し=SEND_CONTINUE STOP）
	send_status_itr->status = SEND_CONTINUE;

	status = handle_sorryserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_CONTINUE );

	// unit_test[x4_63]  handle_sorryserver_selectのテスト（SEND_OK無し=SEND_END STOP）
	send_status_itr->status = SEND_END;

	status = handle_sorryserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
	BOOST_CHECK_EQUAL( status, STOP );
	BOOST_CHECK_EQUAL( send_status_itr->status, SEND_END );

	// unit_test[x4_64]  handle_sorryserver_selectのテスト（receive_data無し あり得ないけどやられたらSTOP）
	thread_data_itr->second->recive_data_map.clear();
	BOOST_CHECK_EQUAL( thread_data_itr->second->recive_data_map.size(), ( size_t )0 );

	status = handle_sorryserver_select( thread_data_itr->second->thread_id, tcp_endpoint );
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


void	handle_realserver_select_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	// unit_test[x4_]  
	BOOST_MESSAGE( "----- handle_realserver_select test start -----" );
	protocol_module_cinsert_test_1.handle_realserver_select_test();
	BOOST_MESSAGE( "----- handle_realserver_select test end -----" );

}

void	handle_sorryserver_select_test()
{

	protocol_module_cinsert_test	protocol_module_cinsert_test_1;

	// unit_test[x4_]  
	BOOST_MESSAGE( "----- handle_sorryserver_select test start -----" );
	protocol_module_cinsert_test_1.handle_sorryserver_select_test();
	BOOST_MESSAGE( "----- handle_sorryserver_select test end -----" );

}


//-------------------------------------------------------------------
//-------------------------------------------------------------------
test_suite*	init_unit_test_suite( int argc, char* argv[] ){

	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_cinsert class test" );

	ts->add( BOOST_TEST_CASE( &handle_client_send_test ) );
	ts->add( BOOST_TEST_CASE( &handle_realserver_send_test ) );
	ts->add( BOOST_TEST_CASE( &handle_sorryserver_send_test ) );

	ts->add( BOOST_TEST_CASE( &handle_realserver_select_test ) );
	ts->add( BOOST_TEST_CASE( &handle_sorryserver_select_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
