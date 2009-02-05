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

bool	is_tcp()
{
	return	true;
}

bool	is_udp()
{
	return	false;
}

void	replication_interrupt()
{
}

void	initialize(
				rs_list_itr_func_type	inlist_begin,
				rs_list_itr_func_type	inlist_end,
				rs_list_itr_next_func_type	inlist_next,
				boost::function< void( void ) >	inlist_lock,
				boost::function< void( void ) >	inlist_unlock )
{
}

void	finalize()
{
}

bool	is_use_sorry()
{
	return	true;
}

check_message_result	check_parameter( const std::vector< std::string >& args )
{
	check_message_result	check_result;
	return	check_result;
}


check_message_result	set_parameter( const std::vector< std::string >& args )
{
	check_message_result check_result;
	return check_result;
}

check_message_result	add_parameter( const std::vector< std::string >& args )
{
	check_message_result check_result;
	return check_result;
}

void	handle_rslist_update()
{
}

void	register_schedule( tcp_schedule_func_type inschedule )
{
}

void	register_schedule( udp_schedule_func_type inschedule )
{
}

EVENT_TAG	handle_session_initialize(
				const boost::thread::id up_thread_id,
				const boost::thread::id down_thread_id,
				const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
				const boost::asio::ip::udp::endpoint& client_endpoint_udp )
{
	return	STOP;
}
EVENT_TAG	handle_session_finalize(
				const boost::thread::id up_thread_id,
				const boost::thread::id down_thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_accept( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_client_recv(
				const boost::thread::id thread_id,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::udp::endpoint& rs_endpoint,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_connect(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_connection_fail(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_send( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_sorryserver_select(
				const boost::thread::id thread_id,
				boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return	STOP;
}

EVENT_TAG	handle_sorryserver_connect(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

EVENT_TAG	handle_sorryserver_connection_fail(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return	STOP;
}

EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint& rs_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::udp::endpoint& rs_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

EVENT_TAG	handle_sorryserver_recv(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint& sorry_endpoint,
				const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
				const size_t recvlen )
{
	return	STOP;
}

EVENT_TAG	handle_response_send_inform( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_client_connection_check(
				const boost::thread::id thread_id,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

EVENT_TAG	handle_client_select(
				const boost::thread::id thread_id,
				boost::asio::ip::udp::endpoint& cl_endpoint,
				boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
				size_t& datalen )
{
	return	STOP;
}

EVENT_TAG	handle_client_send( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_client_disconnect( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_sorry_enable( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_sorry_disable( const boost::thread::id thread_id )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_disconnect(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return	STOP;
}

EVENT_TAG	handle_sorryserver_disconnect(
				const boost::thread::id thread_id,
				const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return	STOP;
}

EVENT_TAG	handle_realserver_close(
				const boost::thread::id thread_id,
				const boost::asio::ip::udp::endpoint & rs_endpoint )
{

	return	STOP;
}

}
