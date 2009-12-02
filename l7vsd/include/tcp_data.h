/*!
 *	@file	tcp_data.h
 *	@brief	tcp session processing data class
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

#ifndef TCP_DATA_H
#define TCP_DATA_H

#include "data_buff_base.h"

namespace l7vs{

//!	@class	tcp_data
//! @brief	this class is tcp session object use data for processing.
	typedef data_buff_base<boost::asio::ip::tcp> tcp_data;

}// namespace l7vs

#endif//TCP_DATA_H

