/*!
 * @file  tcp_socket_option.h
 * @brief tcp session socket option class
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

#ifndef TCP_SOCKET_OPTION_H
#define TCP_SOCKET_OPTION_H

#include "config.h"
#if HAVE_DECL_IP_TRANSPARENT && !defined(IP_TRANSPARENT)
#define IP_TRANSPARENT  19
#endif

namespace l7vs
{
//! tcp_socket_option
struct tcp_socket_option_info {
        //! TCP_NODELAY (false:not set,true:set option)
        bool nodelay_opt;
        //! TCP_NODELAY option value (false:off,true:on)
        bool nodelay_val;
        //! TCP_CORK (false:not set,true:set option)
        bool cork_opt;
        //! TCP_CORK option value (false:off,true:on)
        bool cork_val;
        //! TCP_QUICKACK (false:not set,true:set option)
        bool quickack_opt;
        //! TCP_QUICKACK option value (false:off,true:on)
        bool quickack_val;
};
}
#endif //TCP_SOCKET_OPTION_H

