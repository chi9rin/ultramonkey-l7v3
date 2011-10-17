/*!
 *    @file    data_buff_base.h
 *    @brief    receive and send buffer class
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

#ifndef DATA_BUFF_BASE_H
#define DATA_BUFF_BASE_H

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "logger.h"

namespace l7vs
{

//!    @class    data_buff_base
//! @brief    this class is base class of tcp_data and udp_data classes.
template< typename InternetProtocol >
class data_buff_base : private boost::noncopyable
{
public:
        //! constructor
        data_buff_base();
        //! destructor
        ~data_buff_base();
        //! initialize
        void initialize();
        //! set data
        //! @param[in]    reference to copy data
        void set_data(const boost::array< char , MAX_BUFFER_SIZE>& set_data);
        //! get data
        //! @return        reference to data buffer object
        boost::array< char , MAX_BUFFER_SIZE>& get_data();
        //! set data size
        //! @param[in]    copy size
        void set_size(const std::size_t set_size);
        //! get data size
        //! @return        data size
        std::size_t get_size();
        //! set send size
        //! @param[in]    copy size
        void set_send_size(const std::size_t set_size);
        //! get send size
        //! @return        send size
        std::size_t get_send_size();
        //! get endpoint
        //! @return        endpoint_info
        boost::asio::ip::basic_endpoint<InternetProtocol> get_endpoint();
        //! set endpoint
        //! @param[in]    copy endpoint
        void set_endpoint(const boost::asio::ip::basic_endpoint<InternetProtocol> set_endpoint);
		//! set error code
		void set_error_code( const boost::system::error_code& code );
		//! get error code
		boost::system::error_code&	get_error_code();
		//! set async_len
		void set_async_len( std::size_t	set_async_len );
		//! get async_len
		std::size_t get_async_len();

protected:
        //! tcp endpoint
        boost::asio::ip::basic_endpoint<InternetProtocol> endpoint_info;
        //! data buffer object
        boost::array< char , MAX_BUFFER_SIZE> data;
        //! data size
        std::size_t data_size;
        //! send data size
        std::size_t send_size;
		//! error code( async read/write modify)
		boost::system::error_code	error_code;
		std::size_t	async_len;
		boost::mutex	mutex_;		//use error_code and data size only. other data is no thread safe.
};// class data_buff_base



//! constructor
template< typename InternetProtocol >
data_buff_base< InternetProtocol >::data_buff_base()
{
        initialize();
}

//! destructor
template< typename InternetProtocol >
data_buff_base< InternetProtocol >::~data_buff_base()
{
}

//! initialize
template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::initialize()
{
        data_size = 0;
        send_size = 0;
		async_len = 0;
        data.assign('\0');
        endpoint_info = boost::asio::ip::basic_endpoint<InternetProtocol>();
		error_code.clear();
}

//! set data
//! @param[in]    reference to copy data
template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::set_data(const boost::array< char , MAX_BUFFER_SIZE>& set_data)
{
        data = set_data;
}

//! get data
//! @return        reference to data buffer object
template< typename InternetProtocol >
boost::array< char , MAX_BUFFER_SIZE>& data_buff_base< InternetProtocol >::get_data()
{
        return data;
}

//! set data size
//! @param[in]    copy size
template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::set_size(const std::size_t set_size)
{
        data_size = set_size;
        if (data_size > MAX_BUFFER_SIZE) {
                data_size = MAX_BUFFER_SIZE;
                //Error over MAX_BUFFER_SIZE
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] set_size over MAX_BUFFER_SIZE size : ";
                buf << set_size;
                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 0001, buf.str(), __FILE__, __LINE__);
        }
}

//! get data size
//! @return        data size
template< typename InternetProtocol >
std::size_t data_buff_base< InternetProtocol >::get_size()
{
        return data_size;
}

//! set send size
//! @param[in]    copy size
template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::set_send_size(const std::size_t set_size)
{
        send_size = set_size;
        if (send_size > MAX_BUFFER_SIZE) {
                send_size = MAX_BUFFER_SIZE;
                //Error over MAX_BUFFER_SIZE
                std::stringstream buf;
                buf << "Thread ID[";
                buf << boost::this_thread::get_id();
                buf << "] set_send_size over MAX_BUFFER_SIZE size : ";
                buf << set_size;
                Logger::putLogError(LOG_CAT_L7VSD_SESSION, 0002, buf.str(), __FILE__, __LINE__);
        }
}

//! get send size
//! @return        send size
template< typename InternetProtocol >
std::size_t data_buff_base< InternetProtocol >::get_send_size()
{
		boost::mutex::scoped_lock	lock( mutex_ );
        return send_size;
}

//! get endpoint
//! @return        endpoint_info
template< typename InternetProtocol >
boost::asio::ip::basic_endpoint<InternetProtocol> data_buff_base< InternetProtocol >::get_endpoint()
{
        return endpoint_info;
}

//! set endpoint
//! @param[in]    copy endpoint
template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::set_endpoint(const boost::asio::ip::basic_endpoint<InternetProtocol> set_endpoint)
{
        endpoint_info = set_endpoint;
}

//! set error code
//! @param[in]	error code
template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::set_error_code( const boost::system::error_code& code ){
	mutex_.lock();
	error_code.assign( code.value(), code.category() );
	mutex_.unlock();
}

//! get error code
template< typename InternetProtocol >
boost::system::error_code& data_buff_base< InternetProtocol >::get_error_code(){
	boost::mutex::scoped_lock	lock( mutex_ );
	return error_code;
}

template< typename InternetProtocol >
void data_buff_base< InternetProtocol >::set_async_len( std::size_t	set_async_len ){
	async_len = set_async_len;
}

template< typename InternetProtocol >
std::size_t data_buff_base< InternetProtocol >::get_async_len(){
	return async_len;
}
}// namespace l7vs

#endif//DATA_BUFF_BASE_H
