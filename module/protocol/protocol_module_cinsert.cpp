#include <vector>
#include <list>
#include <sstream>
#include <algorithm>
#include <boost/thread/pthread/mutex.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include "protocol_module_cinsert.h"

namespace l7vs
{

protocol_module_cinsert::protocol_module_cinsert() : http_protocol_module_base( "cinsert" )
{
}

protocol_module_cinsert::~protocol_module_cinsert()
{
}

bool	protocol_module_cinsert::is_tcp()
{
	return	true;
}

bool	protocol_module_cinsert::is_udp()
{
	return	false;
}

void	protocol_module_cinsert::replication_interrupt()
{
	// NOP
	return;
}

void	protocol_module_cinsert::initialize(
				rs_list_itr_func_type			inlist_begin,
				rs_list_itr_func_type			inlist_end,
				rs_list_itr_next_func_type		inlist_next,
				boost::function< void( void ) >	inlist_lock,
				boost::function< void( void ) >	inlist_unlock )
{
	rs_list_begin	= inlist_begin;
	rs_list_end		= inlist_end;
	rs_list_next	= inlist_next;
	rs_list_lock	= inlist_lock;
	rs_list_unlock	= inlist_unlock;
}

void	protocol_module_cinsert::finalize()
{
	getloglevel.clear();
	putLogFatal.clear();
	putLogError.clear();
	putLogWarn.clear();
	putLogInfo.clear();
	putLogDebug.clear();

	rs_list_begin.clear();
	rs_list_end.clear();
	rs_list_next.clear();
	rs_list_lock.clear();
	rs_list_unlock.clear();

	replication_pay_memory.clear();
	replication_area_lock.clear();
	replication_area_unlock.clear();

	schedule_tcp.clear();
	schedule_udp.clear();

	cookie_expire	= 0;
	forwarded_for	= 0;
	reschedule		= 0;
	cookie_name.assign( '\0' );
	sorry_uri.assign( '\0' );
}

bool	protocol_module_cinsert::is_use_sorry()
{
	return	true;
}

protocol_module_cinsert::check_message_result
protocol_module_cinsert::check_parameter( const std::vector< std::string >& args )
{
	check_message_result	check_result;
	return	check_result;
}


protocol_module_cinsert::check_message_result
protocol_module_cinsert::set_parameter( const std::vector< std::string >& args )
{
	check_message_result check_result;
	return check_result;
}

protocol_module_cinsert::check_message_result
protocol_module_cinsert::add_parameter( const std::vector< std::string >& args )
{
	check_message_result check_result;
	return check_result;
}

void	protocol_module_cinsert::handle_rslist_update()
{
	// NOP
	return;
}

void	protocol_module_cinsert::register_schedule( tcp_schedule_func_type inschedule )
{
	schedule_tcp = inschedule;
}

void	protocol_module_cinsert::register_schedule( udp_schedule_func_type inschedule )
{
	// NOP
	return;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_session_initialize(
				const boost::thread::id up_thread_id,
				const boost::thread::id down_thread_id,
				const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
				const boost::asio::ip::udp::endpoint& client_endpoint_udp )
{

	session_thread_data_cinsert*	up_thread_data		= NULL;
	session_thread_data_cinsert*	down_thread_data	= NULL;
	recive_data						client_recv_data;
	char*							buffer				= NULL;

	try
	{
		up_thread_data = new session_thread_data_cinsert;

		up_thread_data->thread_id				= up_thread_id;
		up_thread_data->thread_division			= THREAD_DIVISION_UP_STREAM;
		up_thread_data->pair_thread_id			= down_thread_id;
// 		up_thread_data->end_flag				= END_FLAG_OFF;
// 		up_thread_data->accept_end_flag			= ACCEPT_END_FLAG_OFF;
// 		up_thread_data->sorry_flag				= SORRY_FLAG_OFF;
// 		up_thread_data->sorryserver_switch_flag	= SORRYSERVER_SWITCH_FLAG_OFF;
// 		up_thread_data->realserver_switch_flag	= REALSERVER_SWITCH_FLAG_OFF;
		up_thread_data->client_endpoint_tcp		= client_endpoint_tcp;

// 		client_recv_data.recive_buffer_max_size		= MAX_BUFFER_SIZE;
// 		client_recv_data.recive_buffer_rest_size	= MAX_BUFFER_SIZE;
//		client_recv_data = new recive_data;
//		client_recv_data->recive_buffer_max_size	= MAX_BUFFER_SIZE;
//		client_recv_data->recive_buffer_rest_size	= MAX_BUFFER_SIZE;

		buffer = (char*)malloc( client_recv_data.recive_buffer_max_size );

		client_recv_data.recive_buffer		= buffer;
		client_recv_data.recive_buffer_1	= buffer;
//		client_recv_data->recive_buffer		= buffer;
//		client_recv_data->recive_buffer_1	= buffer;

		buffer = (char*)malloc( client_recv_data.recive_buffer_max_size );

		client_recv_data.recive_buffer_2	= buffer;
//		client_recv_data->recive_buffer_2	= buffer;

		up_thread_data->recive_data_map[ client_endpoint_tcp ] = client_recv_data;

		down_thread_data = new session_thread_data_cinsert;

		down_thread_data->thread_id					= down_thread_id;
		down_thread_data->thread_division			= THREAD_DIVISION_DOWN_STREAM;
		down_thread_data->pair_thread_id			= up_thread_id;
// 		down_thread_data->end_flag					= END_FLAG_OFF;
// 		down_thread_data->accept_end_flag			= ACCEPT_END_FLAG_OFF;
// 		down_thread_data->sorry_flag				= SORRY_FLAG_OFF;
// 		down_thread_data->sorryserver_switch_flag	= SORRYSERVER_SWITCH_FLAG_OFF;
// 		down_thread_data->realserver_switch_flag	= REALSERVER_SWITCH_FLAG_OFF;
		down_thread_data->client_endpoint_tcp		= client_endpoint_tcp;

		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );
			session_thread_data_map[ up_thread_id ]		= up_thread_data;
			session_thread_data_map[ down_thread_id ]	= down_thread_data;
		}

	} catch (const std::exception& ex)
	{
		return FINALIZE;
	} catch (...)
	{
		return FINALIZE;
	}

	return	ACCEPT;

}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_session_finalize(
				const boost::thread::id up_thread_id,
				const boost::thread::id down_thread_id )
{

	session_thread_data_cinsert*	up_thread_data		= NULL;
	session_thread_data_cinsert*	down_thread_data	= NULL;
	session_thread_data_map_itr		thread_data_itr;
	recive_data_map_itr				recive_data_itr;
std::cout	<< "check_point_1" << std::endl;
	try
	{
		{
			boost::mutex::scoped_lock	lock( session_thread_data_map_mutex );

			thread_data_itr = session_thread_data_map.find( up_thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				up_thread_data = thread_data_itr->second;
				session_thread_data_map.erase( up_thread_id );
std::cout	<< "check_point_2" << std::endl;
			}

			thread_data_itr = session_thread_data_map.find( down_thread_id );

			if( thread_data_itr != session_thread_data_map.end() )
			{
				down_thread_data = thread_data_itr->second;
				session_thread_data_map.erase( down_thread_id );
std::cout	<< "check_point_3" << std::endl;
			}
		}

std::cout	<< "check_point_4" << std::endl;




		if( up_thread_data != NULL )
		{
std::cout	<< "check_point_5" << std::endl;
			recive_data_itr = up_thread_data->recive_data_map.begin();

			while( !up_thread_data->recive_data_map.empty() )
			{
std::cout	<< "check_point_6" << std::endl;
				if( recive_data_itr->second.recive_buffer_1 != NULL )
				{
					free( recive_data_itr->second.recive_buffer_1 );
					recive_data_itr->second.recive_buffer_1 = NULL;
std::cout	<< "check_point_7" << std::endl;
				}

				if( recive_data_itr->second.recive_buffer_2 != NULL )
				{
					free( recive_data_itr->second.recive_buffer_2 );
					recive_data_itr->second.recive_buffer_2 = NULL;
std::cout	<< "check_point_8" << std::endl;
				}

				recive_data_itr->second.recive_buffer = NULL;

				up_thread_data->recive_data_map.erase( recive_data_itr );

				recive_data_itr++;

			}

			delete up_thread_data;

		}
std::cout	<< "check_point_9" << std::endl;
		if( down_thread_data != NULL )
		{
std::cout	<< "check_point_10" << std::endl;
			recive_data_itr = down_thread_data->recive_data_map.begin();

			while( !down_thread_data->recive_data_map.empty() )
			{
std::cout	<< "check_point_11" << std::endl;
				if( recive_data_itr->second.recive_buffer_1 != NULL )
				{
					free( recive_data_itr->second.recive_buffer_1 );
					recive_data_itr->second.recive_buffer_1 = NULL;
std::cout	<< "check_point_12" << std::endl;
				}

				if( recive_data_itr->second.recive_buffer_2 != NULL )
				{
					free( recive_data_itr->second.recive_buffer_2 );
					recive_data_itr->second.recive_buffer_2 = NULL;
std::cout	<< "check_point_13" << std::endl;
				}

				recive_data_itr->second.recive_buffer = NULL;

				down_thread_data->recive_data_map.erase( recive_data_itr );

				recive_data_itr++;

			}

std::cout	<< "check_point_14" << std::endl;
			delete down_thread_data;

		}

	} catch (const std::exception& ex)
	{
	} catch (...)
	{
	}
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_accept( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_recv(
				const boost::thread::id thread_id,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::udp::endpoint& rs_endpoint,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	// NOP
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_connect(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_connection_fail(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_send( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_connect(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_connection_fail(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_send( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint& rs_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::udp::endpoint& rs_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint& sorry_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_response_send_inform( const boost::thread::id thread_id )
{
	// NOP
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_connection_check(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_select(
				const boost::thread::id thread_id,
				boost::asio::ip::udp::endpoint& cl_endpoint,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	// NOP
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_send( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_client_disconnect( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorry_enable( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorry_disable( const boost::thread::id thread_id )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_disconnect(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_sorryserver_disconnect(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return	STOP;
}

protocol_module_cinsert::EVENT_TAG
protocol_module_cinsert::handle_realserver_close(
				const boost::thread::id thread_id,
				const boost::asio::ip::udp::endpoint & rs_endpoint )
{
	// NOP
	return	STOP;
}

}
