/*
 * @file  ssl_protocol_module_base.cpp
 * @brief protocol module base of protocol module sslid.
 * @brief this module base provide get session id and check record
 * @brief data for sslid.
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

#include "ssl_protocol_module_base.h"
#include <arpa/inet.h>

namespace l7vs
{
ssl_protocol_module_base::ssl_protocol_module_base(std::string in_modulename)
        : protocol_module_base(in_modulename)
{
        //ctor
}

ssl_protocol_module_base::~ssl_protocol_module_base()
{
        //dtor
}


//! get ssl session id
//! @param[in] ssl record data pointer
//! @param[in] ssl record data length
//! @param[out] ssl session id
//! @return 0  get session id
//! @return 1  no session id
//! @return -1 recv data error
int ssl_protocol_module_base::get_ssl_session_id(const char *record_data,
                size_t recv_length,
                std::string &session_id)
{
        //check the ssl record data length
        if (record_data != NULL) {
                //check the length of ssl session id (session id length is 32 bytes)
                if (record_data[SESSION_ID_BEGAIN_OFFSET - 1] == 0x20) {
                        if (recv_length >= HELLO_MSG_HEADER_LENGTH) {
                                //get session id
                                session_id.assign(record_data + SESSION_ID_BEGAIN_OFFSET,
                                                  record_data + HELLO_MSG_HEADER_LENGTH);
                                return 0;
                        } else {
                                //no session id
                                return 1;
                        }
                } else {
                        //the length of ssl session id is 0, the ssl record data dose not have a session id.
                        return 1;
                }
        } else {
                //the ssl record data length < HELLO_MSG_HEADER_LENGTH, error!
                return -1;
        }
}


//! check the ssl record data whether is able to send, or not.
//! @param[in] is message from client
//! @param[in] ssl record data pointer
//! @param[in] receive data length
//! @param[out] ssl record data length
//! @param[out] is ssl client/server hello record data
//! @return 0 send ok
//! @return 1 can't sendable
//! @return -1 error
int ssl_protocol_module_base::check_ssl_record_sendable(bool is_message_form_client,
                const char *record_data,
                size_t recv_length,
                size_t &all_length,
                bool &is_hello_message)
{
        //check parameter
        if (record_data == NULL) {
                return -1;
        }
        is_hello_message = false;
        //check receive length
        if (recv_length >= SSL_RECORD_MIN_SIZE) {
                //check record
                if ((record_data[0] == 0x14 ||
                     record_data[0] == 0x15 ||
                     record_data[0] == 0x16 ||
                     record_data[0] == 0x17) && (
                            record_data[1] == 0x03) && (
                            record_data[2] == 0x00 || record_data[2] == 0x01)) {
                        //is ssl record,check handshake hello
                        if (record_data[0] == 0x16 && (record_data[5] == 0x01 || record_data[5] == 0x02)) {
                                //is handshake hello,check the length
                                if (recv_length >= SESSION_ID_BEGAIN_OFFSET) {
                                        //check session id length
                                        if (record_data[SESSION_ID_BEGAIN_OFFSET - 1] == 0x20) {
                                                //session id length is 32,check the record length
                                                if (recv_length >= HELLO_MSG_HEADER_LENGTH) {
                                                        //check the hello message
                                                        if (record_data[9] == 0x03 &&
                                                            (record_data[10] == 0x00 || record_data[10] == 0x01) &&
                                                            ((is_message_form_client && record_data[5] == 0x01) ||
                                                             (!is_message_form_client && record_data[5] == 0x02))) {
                                                                //is hello message,set the record length
                                                                is_hello_message = true;
                                                                unsigned short *message_size = reinterpret_cast<unsigned short *>(const_cast<char *>(record_data) + 3);
                                                                all_length = ntohs(*message_size) + 5;
                                                                return 0;
                                                        } else {
                                                                //data error
                                                                all_length = recv_length;
                                                                return 0;
                                                        }
                                                } else {
                                                        //session id length is 32,but record length is short
                                                        return 1;
                                                }
                                        } else {
                                                //session id length is not 32,and is a handshake hello message
                                                is_hello_message = true;
                                                unsigned short *message_size = reinterpret_cast<unsigned short *>(const_cast<char *>(record_data) + 3);
                                                all_length = ntohs(*message_size) + 5;
                                                return 0;
                                        }
                                } else {
                                        //record length is short
                                        return 1;
                                }
                        } else {
                                //is not handshake hello message
                                unsigned short *message_size = reinterpret_cast<unsigned short *>(const_cast<char *>(record_data) + 3);
                                all_length = ntohs(*message_size) + 5;
                                return 0;
                        }
                } else {
                        //is not sslv3 record
                        all_length = recv_length;
                        return 0;
                }
        } else {
                //record length is short
                return 1;
        }
}
bool ssl_protocol_module_base::is_exec_OK(unsigned int vs_attr)
{
        return !(vs_attr & VS_CONTACT_CLASS_SSL);
}
}
