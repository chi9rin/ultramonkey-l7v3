/*!
 *	@file	endpoint.h
 *	@brief	endpoint serialize helper class
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

#ifndef	ENDPOINT_H
#define	ENDPOINT_H

#include <boost/serialization/nvp.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
namespace serialization {

template<class Archive, class T>
inline void serialize(
	Archive & ar,
	boost::asio::ip::basic_endpoint<T> & t,
	const unsigned int file_version 
){
	boost::serialization::split_free( ar, t, file_version );
}

template<class Archive, class T>
inline void save(
	Archive & ar,
	boost::asio::ip::basic_endpoint<T> const& t,
	const unsigned int /* file_version */
){
	std::string		addr	= t.address().to_string();
	unsigned short	port	= t.port();
	ar << boost::serialization::make_nvp( "addr", addr );
	ar << boost::serialization::make_nvp( "port", port );
}

template<class Archive, class T>
inline void load(
	Archive & ar,
	boost::asio::ip::basic_endpoint<T>& t,
	const unsigned int /* file_version */ 
){
	std::string		addr;
	unsigned short	port;
	ar >> boost::serialization::make_nvp( "addr", addr );
	ar >> boost::serialization::make_nvp( "port", port );
	t = boost::asio::ip::basic_endpoint<T>( boost::asio::ip::address::from_string( addr ), port );
}

}	// namespace serialization
}	// namespace boost

#endif	// ENDPOINT_H
