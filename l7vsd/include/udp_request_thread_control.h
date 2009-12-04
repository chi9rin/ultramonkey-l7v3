/*!
 *    @file    udp_request_thread_control.h
 *    @brief    request used 1 threads. thread pooling unit is 1 thread control.
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

#ifndef UDP_REQUEST_THREAD_CONTROL_H
#define UDP_REQUEST_THREAD_CONTROL_H

#include <boost/shared_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

#include "udp_request.h"

namespace l7vs{

    class udp_request_thread_control : private boost::noncopyable{
        protected:
            typedef boost::shared_ptr< boost::thread > thread_ptr;
            typedef boost::shared_ptr< l7vs::udp_request > request_ptr;

            enum UDP_REQUEST_THREAD_STATE_TAG{
                WAIT = 0,
                RUNNING,
                EXIT
            };

            thread_ptr request_thread;
            UDP_REQUEST_THREAD_STATE_TAG request_thread_state;
            boost::condition thread_condition;
            boost::mutex condition_mutex;
            request_ptr my_request;

        public:
            udp_request_thread_control(request_ptr request);
            ~udp_request_thread_control(void);
            void start(void);
            void stop(void);
            void join(void);
            boost::thread::id get_thread_id();
            boost::shared_ptr< l7vs::udp_request > get_request();
            
        protected:
            void run(void);

    };// class udp_request_thread_control
}// namespace l7vs

#endif//UDP_REQUEST_THREAD_CONTROL_H
