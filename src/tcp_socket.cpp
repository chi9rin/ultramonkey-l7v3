//
//!	@file	tcp_socket.cpp
//!	@brief	tcp session socket class
//
//	copyright (c) 2008 TOKYO COMPUTER SERVICE CO.,LTD.
//	mail: 
//
//	Distributed under the Boost Software License, Version 1.0.(See accompanying
//	file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/thread/thread.hpp>

#include "tcp_socket.h"
#include "logger.h"

namespace l7vs{

	//! construcor
	tcp_socket::tcp_socket(boost::asio::io_service& io):
		my_socket(io),
		open_flag(false){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::tcp_socket", __FILE__, __LINE__ );

	}

	//! destructor
	tcp_socket::~tcp_socket(){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::~tcp_socket", __FILE__, __LINE__ );
	}

	//! get reference control socket
	//! @return			reference control socket
	boost::asio::ip::tcp::socket& tcp_socket::get_socket(){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::get_socket", __FILE__, __LINE__ );
		return my_socket;
	}

	//! connect socket
	//! @param[in]		connect_endpoint is connection endpoint
	//! @param[out]		ec is reference error code object
	//! @return 		true is connect
	//! @return 		false is connect failure 
	bool tcp_socket::connect(boost::asio::ip::tcp::endpoint connect_endpoint,
		boost::system::error_code& ec){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::connect", __FILE__, __LINE__ );
		
		boost::mutex::scoped_lock scope_lock(socket_mutex);
		
		if(!open_flag){
			my_socket.connect(connect_endpoint,ec);
			if(!ec){
				open_flag = true;
				//----Debug log----------------------------------------------------------------------
				if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION)){
					std::stringstream buf;
					buf << "Thread ID[";
					buf << boost::this_thread::get_id();
					buf << "] tcp_socket::connect [";
					buf << connect_endpoint;
					buf << "]";
					Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
				}
				//----Debug log----------------------------------------------------------------------
			}else{
				open_flag = false;
			}
		}
		return open_flag;
	}
	
	//! accept socket
	void tcp_socket::accept(){
		boost::mutex::scoped_lock scope_lock(socket_mutex);
		open_flag = true;
		//----Debug log----------------------------------------------------------------------
		if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION)){
			boost::system::error_code ec;
			std::stringstream buf;
			buf << "Thread ID[";
			buf << boost::this_thread::get_id();
			buf << "] tcp_socket::accept [";
			buf << my_socket.remote_endpoint(ec);
			buf << "]";
			Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
		}
		//----Debug log----------------------------------------------------------------------
	}

	//! close socket
	//! @param[out]		ec is reference error code object
	//! @return 		true is socket close
	//! @return 		false is not open socket
	bool tcp_socket::close(boost::system::error_code& ec){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::close", __FILE__, __LINE__ );
		
		boost::mutex::scoped_lock scope_lock(socket_mutex);
		//----Debug log----------------------------------------------------------------------
		if (LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_SESSION)){
			if(open_flag){
				boost::system::error_code ec;
				std::stringstream buf;
				buf << "Thread ID[";
				buf << boost::this_thread::get_id();
				buf << "] tcp_socket::close [";
				buf << my_socket.remote_endpoint(ec);
				buf << "]";
				Logger::putLogDebug( LOG_CAT_L7VSD_SESSION, 9999, buf.str(), __FILE__, __LINE__ );
			}
		}
		//----Debug log----------------------------------------------------------------------
		bool bres = false;
		if(open_flag){
			open_flag = false;
			bres = true;
		}
		my_socket.close(ec);
		
		return bres;
	}

	//! set non blocking mode of the socket 
	//! @return			ec is reference error code object
	bool tcp_socket::set_non_blocking_mode(boost::system::error_code& ec){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::set_non_blocking_mode", __FILE__, __LINE__ );
		
		boost::asio::socket_base::non_blocking_io cmd(true);
		my_socket.io_control(cmd,ec);
		
		return true;
	}

	//! write socket
	//! @param[in]		buffers is wite data buffer
	//! @param[out]		ec is reference error code object
	//! @return			write data size	
	std::size_t tcp_socket::write_some(boost::asio::mutable_buffers_1 buffers,
		boost::system::error_code& ec){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::write_some", __FILE__, __LINE__ );
		
		boost::mutex::scoped_lock scope_lock(socket_mutex);
		std::size_t res_size = 0;
		res_size = my_socket.write_some(buffers,ec);
		if(ec){
			if (!open_flag) {
				res_size = 0;
				ec.clear();
			}
		}
		
		return res_size;
	}

	//! read socket
	//! @param[out]		buffers is read data buffer
	//! @param[out]		ec is reference error code object
	//! @return			read data size
	std::size_t tcp_socket::read_some(boost::asio::mutable_buffers_1 buffers,
		boost::system::error_code& ec){
		Logger	logger( LOG_CAT_L7VSD_SESSION, 9999, "tcp_socket::read_some", __FILE__, __LINE__ );
		
		boost::mutex::scoped_lock scope_lock(socket_mutex);
		std::size_t res_size = 0;
		res_size = my_socket.read_some(buffers,ec);
		if(ec){
			if (!open_flag) {
				res_size = 0;
				ec.clear();
			}
		}
		
		return res_size;
	}

	//! is open
	//! @return 		true is open
	//! @return 		false is close
	bool tcp_socket::is_open(){
		return open_flag;
	}

}// namespace l7vs
 
