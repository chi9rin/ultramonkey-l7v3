/*!
 *	@file	udp_session.h
 *	@brief	
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

#ifndef UDP_SESSION_H
#define UDP_SESSION_H

#include <bitset>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "protocol_module_base.h"
#include "session_result_message.h"

#define UDP_SESSION_THREAD_STATE_BIT 8

namespace l7vs{
	class virtualservice_udp;
	class udp_request_thread_control;
	
	class udp_session : private boost::noncopyable{
		protected:
			typedef boost::shared_ptr< l7vs::udp_request_thread_control > udp_request_thread_control_ptr;
			typedef std::map<boost::thread::id,udp_request_thread_control_ptr> udp_request_thread_control_map;
			typedef boost::asio::ip::udp::endpoint udp_endpoint;
			typedef boost::array<char,MAX_BUFFER_SIZE> udp_session_buff;
			typedef boost::asio::ip::udp::socket udp_socket;
			
			enum UDP_VIRTUAL_SERVICE_MESSAGE_TAG{
				SESSION_END = 0
			};

			boost::asio::io_service& io;
			virtualservice_udp& parent_service;
			bool exit_flag;
			boost::mutex exit_flag_update_mutex;
			std::bitset<UDP_SESSION_THREAD_STATE_BIT> thread_state;
			protocol_module_base* protocol_module;
			udp_socket client_side_socket;
			boost::mutex client_send_mutex;
			udp_endpoint listen_endpoiunt;
			udp_request_thread_control_map pool_request_map;
			udp_request_thread_control_map active_request_map;
			boost::mutex request_map_mutex;
		public:
			udp_session(virtualservice_udp& vs, boost::asio::io_service& session_io);
			~udp_session();
			session_result_message initialize(const udp_endpoint listen_end);
			void set_virtual_service_message(const UDP_VIRTUAL_SERVICE_MESSAGE_TAG message);
			void run(void);
			bool client_send(const udp_endpoint client_endpoint, const udp_session_buff& data_buff,const std::size_t data_size,boost::system::error_code& ec);
			void release_request(const boost::thread::id release_thread_id);
		protected:
			void make_request_thread(const int max_count);
			void active_request_thread(const udp_endpoint client_endpoint , const udp_session_buff receive_data, const std::size_t receive_size);
			void all_active_request_stop(void);

	};// class udp_session
}// namespace l7vs

#endif//UDP_SESSION_H
