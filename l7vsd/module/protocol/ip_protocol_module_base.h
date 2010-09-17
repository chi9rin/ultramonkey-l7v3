/*
 *    @file    ip_protocol_module_base.h
 *    @brief    shared object http protocol module abstract class
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
#ifndef    IP_PROTOCOL_MODULE_BASE_H
#define    IP_PROTOCOL_MODULE_BASE_H

#include "protocol_module_base.h"
#include "http_utility.h"

namespace l7vs
{

        class ip_protocol_module_base : public protocol_module_base
        {
        protected:
                //! http statistic infomation
                http_stats http_stats_info;

        public:

                //! constractor
                ip_protocol_module_base(std::string in_modulename) : protocol_module_base(in_modulename) {};

                //! destractor
                virtual    ~ip_protocol_module_base() {};


                //! get base statistic object.
                //! @return                        base statistic object.
                stats_base& get_stats() {
                        return http_stats_info;
                }

                //! increment http statistics
                //! @param  const char*            buffer
                void    increment_stats(const char *buffer) {
                        if (buffer != NULL && statistic != 0) {

                                if (http_utility::is_get_request(buffer)) {
                                        http_stats_info.http_get_requests++;
                                } else if (http_utility::is_post_request(buffer)) {
                                        http_stats_info.http_post_requests++;
                                }

                                http_stats_info.http_requests++;
                        }
                }
        };

} // namespace l7vsd

#endif    //IP_PROTOCOL_MODULE_BASE_H
