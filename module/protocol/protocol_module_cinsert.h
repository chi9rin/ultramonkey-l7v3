/*
 *	@file	protocol_module_cinsert.h
 *	@brief	shared object protocol module cinsert class
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "http_protocol_module_base.h"
#include "wrlock.h"

#ifndef PROTOCOL_MODULE_CINSERT_H
#define PROTOCOL_MODULE_CINSERT_H

#define MAX_OPTION_SIZE 128

namespace l7vs
{

class protocol_module_cinsert : public http_protocol_module_base
{
public:
	static const int	THREAD_DIVISION_UP_STREAM	= 0;
	static const int	THREAD_DIVISION_DOWN_STREAM	= 1;

	static const int	END_FLAG_OFF				= 0;
	static const int	END_FLAG_ON					= 1;

	static const int	ACCEPT_END_FLAG_OFF			= 0;
	static const int	ACCEPT_END_FLAG_ON			= 1;

	static const int	SORRY_FLAG_OFF				= 0;
	static const int	SORRY_FLAG_ON				= 1;

	static const int	SORRYSERVER_SWITCH_FLAG_OFF	= 0;
	static const int	SORRYSERVER_SWITCH_FLAG_ON	= 1;

	static const int	REALSERVER_SWITCH_FLAG_OFF	= 0;
	static const int	REALSERVER_SWITCH_FLAG_ON	= 1;

	static const int	EDIT_DIVISION_NO_EDIT		= 0;
	static const int	EDIT_DIVISION_EDIT			= 1;

public:
	enum SEND_STATUS_TAG
	{
		SEND_OK= 0,
		SEND_NG,
		SEND_END,
		SEND_CONTINUE
	};

public:
	struct edit_data
	{
		std::string		data;
		size_t			data_size;
		size_t			insert_position;
		size_t			replace_size;

		bool	operator==( const edit_data& in )
				{	return	(	( data == in.data )							&&
								( data_size == in.data_size )				&&
								( insert_position == in.insert_position )	&&
								( replace_size == in.replace_size )	);
				}

		bool	operator!=( const edit_data& in )
				{	return	(	( data != in.data )							||
								( data_size != in.data_size )				||
								( insert_position != in.insert_position )	||
								( replace_size != in.replace_size )	);
				}

		edit_data() :
										data_size( 0 ),
										insert_position( 0 ),
										replace_size( 0 )	{}

		edit_data( const edit_data& in ) :
										data( in.data ),
										data_size( in.data_size ),
										insert_position( in.insert_position ),
										replace_size( in.replace_size )	{}
	} ;

    struct send_status
	{
		SEND_STATUS_TAG		status;
		size_t				send_end_size;
		size_t				send_rest_size;
		size_t				send_possible_size;
		size_t				send_offset;
		size_t				unsend_size;
		int					edit_division;
		boost::asio::ip::tcp::endpoint	send_endpoint;
		std::list< edit_data >	edit_data_list;

		bool	operator==( const send_status& in )
				{
					return	(	( status == in.status )							&&
								( send_end_size == in.send_end_size )			&&
								( send_rest_size == in.send_rest_size )			&&
								( send_possible_size == in.send_possible_size )	&&
								( send_offset == in.send_offset )				&&
								( unsend_size == in.unsend_size )				&&
								( edit_division == in.edit_division )			&&
								( send_endpoint == in.send_endpoint )	);
				}

		bool	operator!=( const send_status& in )
				{
					return	(	( status != in.status )							||
								( send_end_size != in.send_end_size )			||
								( send_rest_size != in.send_rest_size )			||
								( send_possible_size != in.send_possible_size )	||
								( send_offset != in.send_offset )				||
								( unsend_size != in.unsend_size )				||
								( edit_division != in.edit_division )			||
								( send_endpoint != in.send_endpoint )	);
				}

		send_status() :
										status( SEND_END ),
										send_end_size( 0 ),
										send_rest_size( 0 ),
										send_possible_size( 0 ),
										send_offset( 0 ),
										unsend_size( 0 ),
										edit_division( EDIT_DIVISION_NO_EDIT )	{}

		send_status( const send_status& in ) :
										status( in.status ),
										send_end_size( in.send_end_size ),
										send_rest_size( in.send_rest_size ),
										send_possible_size( in.send_possible_size ),
										send_offset( in.send_offset ),
										unsend_size( in.unsend_size ),
										edit_division( in.edit_division ),
										send_endpoint( in.send_endpoint ),
										edit_data_list( in.edit_data_list )	{}
	};

	struct recive_data
	{
		char*		recive_buffer;
		char*		recive_buffer_1;
		char*		recive_buffer_2;
		size_t		recive_buffer_max_size;
		size_t		recive_buffer_rest_size;
		std::list< send_status >	send_status_list;

		bool	operator==( const recive_data& in )
				{
					return	(	( recive_buffer == in.recive_buffer )						&&
								( recive_buffer_1 == in.recive_buffer_1 )					&&
								( recive_buffer_2 == in.recive_buffer_2 )					&&
								( recive_buffer_max_size == in.recive_buffer_max_size )		&&
								( recive_buffer_rest_size == in.recive_buffer_rest_size ));
				}

		bool	operator!=( const recive_data& in )
				{
					return	(	( recive_buffer != in.recive_buffer )						||
								( recive_buffer_1 != in.recive_buffer_1 )					||
								( recive_buffer_2 != in.recive_buffer_2 )					||
								( recive_buffer_max_size != in.recive_buffer_max_size )		||
								( recive_buffer_rest_size != in.recive_buffer_rest_size )	);
				}

		recive_data() :
										recive_buffer( NULL ),
										recive_buffer_1( NULL ),
										recive_buffer_2( NULL ),
										recive_buffer_max_size( MAX_BUFFER_SIZE ),
										recive_buffer_rest_size( recive_buffer_max_size )	{}

		recive_data( const recive_data& in ) :
										recive_buffer( in.recive_buffer ),
										recive_buffer_1( in.recive_buffer_1 ),
										recive_buffer_2( in.recive_buffer_2 ),
										recive_buffer_max_size( in.recive_buffer_max_size ),
										recive_buffer_rest_size( in.recive_buffer_rest_size ),
										send_status_list( in.send_status_list )	{}
		~recive_data()
		{
			if( recive_buffer_1 != NULL )
			{
				delete[] recive_buffer_1;
			}

			if( recive_buffer_2 != NULL )
			{
				delete[] recive_buffer_2;
			}
		}
	};

	struct session_thread_data_cinsert
	{
		boost::thread::id	thread_id;
		int					thread_division;
		boost::thread::id	pair_thread_id;
		std::map< boost::asio::ip::tcp::endpoint, recive_data >
							recive_data_map;
		int					end_flag;
		int					accept_end_flag;
		int					sorry_flag;
		int					sorryserver_switch_flag;
		int					realserver_switch_flag;
		boost::asio::ip::tcp::endpoint		client_endpoint_tcp;
		boost::asio::ip::tcp::endpoint		last_endpoint_tcp;
		EVENT_TAG			last_status;

		bool	operator==( const session_thread_data_cinsert& in )
				{	return	(	( thread_id == in.thread_id )								&&
								( thread_division == in.thread_division )					&&
								( pair_thread_id == in.pair_thread_id )						&&
								( end_flag == in.end_flag )									&&
								( accept_end_flag == in.accept_end_flag )					&&
								( sorry_flag == in.sorry_flag )								&&
								( sorryserver_switch_flag == in.sorryserver_switch_flag )	&&
								( realserver_switch_flag == in.realserver_switch_flag )		&&
								( client_endpoint_tcp == in.client_endpoint_tcp )			&&
								( last_status == in.last_status )	);
				}

		bool	operator!=( const session_thread_data_cinsert& in )
				{	
					return	(	( thread_id != in.thread_id )								||
								( thread_division != in.thread_division )					||
								( pair_thread_id != in.pair_thread_id )						||
								( end_flag != in.end_flag )									||
								( accept_end_flag != in.accept_end_flag )					||
								( sorry_flag != in.sorry_flag )								||
								( sorryserver_switch_flag != in.sorryserver_switch_flag )	||
								( realserver_switch_flag != in.realserver_switch_flag )		||
								( client_endpoint_tcp != in.client_endpoint_tcp )			||
								( last_status != in.last_status )	);
				}

		session_thread_data_cinsert() :
										end_flag( END_FLAG_OFF ),
										accept_end_flag( ACCEPT_END_FLAG_OFF ),
										sorry_flag( SORRY_FLAG_OFF ),
										sorryserver_switch_flag( SORRYSERVER_SWITCH_FLAG_OFF ),
										realserver_switch_flag( REALSERVER_SWITCH_FLAG_OFF ),
										last_status( STOP )	{}

		session_thread_data_cinsert( const session_thread_data_cinsert& in ) :
										thread_id( in.thread_id ),
										thread_division( in.thread_division ),
										pair_thread_id( in.pair_thread_id ),
										recive_data_map( in.recive_data_map ),
										end_flag( in.end_flag ),
										accept_end_flag( in.accept_end_flag ),
										sorry_flag( in.sorry_flag ),
										sorryserver_switch_flag( in.sorryserver_switch_flag ),
										realserver_switch_flag( in.realserver_switch_flag ),
										client_endpoint_tcp( in.client_endpoint_tcp ),
										last_status( in.last_status )	{}
	};

public:

	typedef		std::list< edit_data >
								t_edit_data_list;

	typedef		std::list< send_status >
								t_send_status_list;

	typedef		std::map< boost::asio::ip::tcp::endpoint, recive_data >
								t_recive_data_map;

	typedef		boost::shared_ptr<session_thread_data_cinsert>
								t_session_thread_data_cinsert;

	typedef		std::map< boost::thread::id, t_session_thread_data_cinsert>
								t_session_thread_data_map;

	typedef		t_edit_data_list::iterator
								t_edit_data_list_itr;

	typedef		t_send_status_list::iterator
								t_send_status_list_itr;

	typedef		t_recive_data_map::iterator
								t_recive_data_map_itr;

	typedef		t_session_thread_data_map::iterator
								t_session_thread_data_map_itr;

protected:
	int	cookie_expire;
	int	forwarded_for;
	int	reschedule;
	boost::array< char, MAX_OPTION_SIZE >	cookie_name;
	boost::array< char, MAX_OPTION_SIZE >	sorry_uri;
	t_session_thread_data_map	session_thread_data_map;
	wr_mutex	session_thread_data_map_mutex;

public:
	//! constractor
	protocol_module_cinsert();

	//! destractor
	~protocol_module_cinsert();

	//! tcp protocol support check
	//! @return tcp support is true
	//! @return tcp not-support is false
	bool	is_tcp();

	//! udp protocol support check
	//! @return udp support is true
	//! @return udp not-support is false
	bool	is_udp();

	//! replication interval interrrupt
	//! timer thread call this function. from virtualservice.
	void	replication_interrupt();

	//! initialize function. called from module control. module loaded call
	//! @param[in]	realserver list iterator begin function object type
	//!	@param[in]	realserver list iterator end function object type
	//! @param[in]	realserver list iterator next function object type
	//! @param[in]	realserver list mutex lock function object type.
	//! @param[in]	realserver list mutex unlock function object type
	void	initialize(
							rs_list_itr_func_type inlist_begin,
							rs_list_itr_func_type inlist_end,
							rs_list_itr_next_func_type inlist_next,
							boost::function< void( void ) > inlist_lock,
							boost::function< void( void ) > inlist_unlock );

	//! finalize called from module control. module unloaded call.
	void	finalize();

	//! sorry support check
	//! @return true sorry mode is supported.
	//! @return false sorry mode is unsupported.
	bool	is_use_sorry();

	//! module parameter check.used by l7vsadm
	//! @param[in]	module paramter string list
	//! @return	result.flag true is parameter is noproblem.
	//! @return result.flag false is paramter is problem.
	check_message_result	check_parameter( const std::vector< std::string >& args);

	//! parameter set
	//! @param[in] module paramter string list
	//! @return	result.flag true is parameter is noproblem.
	//! @return result.flag false is paramter is problem.
	check_message_result	set_parameter( const std::vector< std::string >& args);

	//! parameter add
	//! @param[in] module paramter string list
	//! @return	result.flag true is parameter is noproblem.
	//! @return result.flag false is paramter is problem.
	check_message_result	add_parameter( const std::vector< std::string >& args);

	//! realserver list update event
	void	handle_rslist_update();

	//! TCP/IP scheduled function registation.
	//! @param[in] schedule module TCP/IP scheduled function object type
	void	register_schedule( tcp_schedule_func_type inschedule );

	//! UDP scheduled function registation
	//! @param[in] schedule module UDP scheduled funtion object type
	void	register_schedule( udp_schedule_func_type inschedule );

	//! called from session initialzie use in upstream_thread
	//! @param[in]	upstream thread id.
	//! @param[in]	downstream thread id
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_session_initialize(
								const boost::thread::id up_thread_id,
								const boost::thread::id down_thread_id,
								const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
								const boost::asio::ip::udp::endpoint& client_endpoint_udp );

	//! called from session finalize use in upstream thread.
	//! @param[in]	upstream thread id.
	//! @param[in]	downstream thread id
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_session_finalize(
								const boost::thread::id up_thread_id,
								const boost::thread::id down_thread_id );

	//! called from after session accept.in client socket use in upstream thread.
	//! @param[in]	upstream thread id.
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_accept( const boost::thread::id thread_id );

	//! called from after session recv in client socket. use in upstream thread.
	//! @param[in]	upstream thread id
	//! @param[in]	recive buffer refarence.
	//! @param[in]	recive length
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_client_recv(
								const boost::thread::id thread_id,
								const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
								const size_t recvlen );

	//! called from after realserver select.use in upstream thread.
	//! @param[in]	upstream thread id
	//! @param[out]	realserver TCP endpoint
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_select(
								const boost::thread::id thread_id,
								boost::asio::ip::tcp::endpoint& rs_endpoint );

	//! called from after realserver select
	//! @param[in]	upstream thread id
	//! @param[out]	realserver UDP endpoint
	//! @param[out]	sendbudffer reference
	//! @param[out]	send data length
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_select(
								const boost::thread::id thread_id,
								boost::asio::ip::udp::endpoint& rs_endpoint,
								boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
								size_t& datalen );

	//! called from after realserver connect
	//! @param[in]	upstream thread id
	//! @param[out]	sendbuffer reference
	//! @param[out]	send data length
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_connect(
								const boost::thread::id thread_id,
								boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
								size_t& datalen );

	//! called from after realserver connection fail
	//! @param[in]	upstream thread id
	//! @param[in]	fail realserver endpoint reference
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_connection_fail(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& rs_endpoint );

	//! called from after realserver send.
	//! @param[in]	upstream thread id
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_send( const boost::thread::id thread_id );

	//! called from after sorryserver select
	//! @param[in]	upstream thread id
	//! @param[in]	sorryserver endppiont reference
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_sorryserver_select(
								const boost::thread::id thread_id,
								boost::asio::ip::tcp::endpoint& sorry_endpoint );

	//! called from after sorryserver connect
	//!	@param[in]	upstream thread id
	//! @param[out]	send buffer reference.
	//! @param[out]	send length
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_sorryserver_connect(
								const boost::thread::id thread_id,
								boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
								size_t& datalen );

	//! called from after sorryserver connection fail
	//! @param[in]	upstream thread id
	//! @param[in]	sorryserver endpoint reference.
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_sorryserver_connection_fail(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint );

	//! called from after sorryserver send
	//! @param[in]	upstream thread id
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id );

	//! called from after realserver recvive for TCP/IP
	//! @param[in]	downstream thread id
	//! @param[in]	realserver TCP/IP endpoint reference
	//! @param[in]	realserver recive buffer reference.
	//! @param[in]	recv data length
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_recv(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& rs_endpoint,
								const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
								const size_t recvlen );

	//! called from after realserver recive.for UDP
	//! @param[in]	downstream thread id
	//! @param[in]	realserver UDP endpoint reference
	//! @param[in]	recive from realserver buffer reference
	//! @param[in]	recv data length
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_recv(
								const boost::thread::id thread_id,
								const boost::asio::ip::udp::endpoint& rs_endpoint,
								const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
								const size_t recvlen );

	//! called from after sorryserver recive
	//! @param[in]	downstream thread id
	//! @param[in]	sorryserver endpoint reference
	//! @param[in]	recive from realserver buffer reference.
	//! @param[in]	recv data length
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_sorryserver_recv(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint,
								const boost::array< char, MAX_BUFFER_SIZE >& recvbuffer,
								const size_t recvlen );

	//! called from UPSTEEARM thread. make module original message.
	//! @param[in]	downstream thread id.
	//! @return 		session use EVENT mode
	EVENT_TAG	handle_response_send_inform( const boost::thread::id thread_id );

	//! called from after client connection check. use TCP/IP only. create client send message.
	//! @param[in]	downstream thread id
	//! @param[out]	send budffer reference
	//! @param[out]	send data length
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_client_connection_check(
								const boost::thread::id thread_id,
								boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
								size_t& datalen );

	//! called from after client select. use UDP only
	//! @param[in]	downstream thread id
	//!	@param[in]	client udp endpoint
	//! @param[out]	send buffer reference
	//! @param[out]	send data length
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_client_select(
								const boost::thread::id thread_id,
								boost::asio::ip::udp::endpoint& cl_endpoint,
								boost::array< char, MAX_BUFFER_SIZE >& sendbuffer,
								size_t& datalen );

	//!	called from after client send
	//!	@param[in]	downstream thread id
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_client_send( const boost::thread::id thread_id );

	//! call from client disconnect event. use upstream thread and downstream thread.
	//! @param[in]	upstream and downstream thread id( check! one thread one event! )
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_client_disconnect( const boost::thread::id thread_id );

	//! call from sorry mode event. use upstream thread and downstream thread
	//! @param[in]	upstream and downstream thread id( check! one thread one event and first time call pattern )
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_sorry_enable( const boost::thread::id thread_id );

	//! call from sorry mode disable. use upstream thread and downstream thread.
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_sorry_disable( const boost::thread::id thread_id );

	//! call from realserver disconnect. use upstream thread and downstream thread
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @param[in]	disconnected realserver endpoint.
	//! @return 	session use EVENT mode
	EVENT_TAG	handle_realserver_disconnect(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& rs_endpoint );

	//! call from sorry server disconnect. use upstraem thread and downstream thread
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @param[in]	disconnect sorryserver endpoint
	//! @return		session use EVENT mode
	EVENT_TAG	handle_sorryserver_disconnect(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint );

	//! call from realserver disconnect. use upstream thread and downstream thread.
	//! @param[in]	upstream and downstream thread id( check! one thread one event )
	//! @param[in]	disconnect realserver endpoint
	//! @return		session use EVENT mode.
	EVENT_TAG	handle_realserver_close(
								const boost::thread::id thread_id,
								const boost::asio::ip::udp::endpoint& rs_endpoint );
};

}

#endif  //PROTOCOL_MODULE_CINSERT_H

