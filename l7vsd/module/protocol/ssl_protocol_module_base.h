/*
 * @file  ssl_protocol_module_base.h
 * @brief ssl protocol module base header file.
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

#ifndef SSL_PROTOCOL_MODULE_BASE_H
#define SSL_PROTOCOL_MODULE_BASE_H

#include "protocol_module_base.h"
#include <string>

namespace l7vs
{

class ssl_protocol_module_base : public protocol_module_base
{
public:
        ssl_protocol_module_base(std::string in_modulename);
        virtual ~ssl_protocol_module_base();
protected:
        int get_ssl_session_id(const char *record_data, size_t recv_length, std::string &session_id);

        int check_ssl_record_sendable(bool is_message_form_client,
                                      const char *record_data,
                                      size_t recv_length,
                                      size_t &all_length,
                                      bool &is_hello_message);
protected:
        const static size_t HELLO_MSG_HEADER_LENGTH = 76;
        const static size_t SESSION_ID_BEGAIN_OFFSET = 44;
        const static size_t SSL_RECORD_MIN_SIZE = 6;
public:
        bool is_exec_OK(unsigned int vs_attr);

public:
        //! format dump data.
        //! @param[in]    data want to format
        //! @param[in]    data size
        //! @param[out]    format string
        //! @return
        static void  dump_session_id(
                const char *data,
                const size_t data_size,
                std::string &data_dump) {
                if (data == NULL || data_size == 0) {
                        return;
                }

                boost::format formatter("%02X");
                for (size_t i = 0; i < data_size; i++) {
                        formatter % static_cast<unsigned short>(static_cast<unsigned char>(data[i]));
                        data_dump += formatter.str();
                }
        }
};

}   // namespace l7vsd

#endif // SSL_PROTOCOL_MODULE_BASE_H
