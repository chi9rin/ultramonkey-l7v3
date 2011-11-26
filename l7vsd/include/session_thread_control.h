/*!
 *    @file    session_thread_control.h
 *    @brief    session used 2 threads. thread pooling unit is 2 thread control.
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

#ifndef    SESSION_THREAD_CONTROL_H
#define    SESSION_THREAD_CONTROL_H
#include <boost/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "tcp_session.h"

#include "wrlock.h"
#include "parameter.h"
#include "error_code.h"

#define PARAM_SCHED_PRIORITY    "task_scheduler_priority"
#define    NUM_OF_CORE_USES    "num_of_core_uses"

namespace l7vs
{

class    tcp_session;

//!
//!    @brief    session thread pooling utility class.
//! @class    session_thread_control is session thread pool utility.
class    session_thread_control : private boost::noncopyable
{
public:
        typedef    boost::shared_ptr< boost::thread >    thread_ptr;        //! shared_ptr thread typedef
        typedef    boost::shared_ptr< tcp_session >    session_ptr;    //! shared_ptr session typedef
        typedef    boost::thread::id                    thread_id_type;    //! thread id typedef
        enum    state_tag {   //! @enum state_tag upthread and down thread state enum
                WAIT    = 0,    //! thread pooling mode
                RUNNING,        //! thread running mode
                EXIT            //! thread exit mode
        };
protected:
        thread_ptr            upthread;                    //! upstream thread
        wr_mutex            upthread_state_mutex;        //! mutex for upstream thread status
        state_tag            upthread_state;                //! upstream thread state
        boost::mutex        upthread_condition_mutex;    //! upthread condition use mutex
        boost::condition    upthread_condition;            //! upthread condition
        boost::mutex        upthread_running_mutex;
        boost::mutex        upthread_joining_mutex;
        boost::condition    upthread_joining_condition;
        thread_ptr            downthread;                    //! downstream thread
        wr_mutex            downthread_state_mutex;        //! mutex for downstream thread status
        state_tag            downthread_state;            //! downstream thread state
        boost::mutex        downthread_condition_mutex;    //! downstream condition use mutex
        boost::condition    downthread_condition;        //! downstream condition
        boost::mutex        downthread_running_mutex;
        boost::mutex        downthread_joining_mutex;
        boost::condition    downthread_joining_condition;
        session_ptr            session;                    //! session class shared pointer
        void                upstream_run();                //! upstream thread bind function
        void                downstream_run();            //! downstream thread bind function

        cpu_set_t            vsnic_cpumask;
        cpu_set_t            rsnic_cpumask;

        int                    sched_algorithm;
        int                    sched_priority;
        int                    num_of_core_uses;
public:
        //! constructor.
        //! @param session_ptr    session class shared ptr
        session_thread_control(tcp_session *ptr, cpu_set_t in_upcpu, cpu_set_t in_downcpu, int    in_sched_algorithm) :
                upthread_state(WAIT),
                downthread_state(WAIT),
                vsnic_cpumask(in_upcpu),
                rsnic_cpumask(in_downcpu),
                sched_algorithm(in_sched_algorithm) {
                int            int_val;
                l7vs::error_code    err;
                Parameter        param;
                //get scheduler priority parameter
                int_val    = param.get_int(l7vs::PARAM_COMP_L7VSD, PARAM_SCHED_PRIORITY, err);
                if (!err)
                        if ((int_val > sched_get_priority_min(in_sched_algorithm)) || (int_val < sched_get_priority_max(in_sched_algorithm)))
                                sched_priority = int_val;
                        else
                                sched_priority = sched_get_priority_min(in_sched_algorithm);
                else
                        sched_priority = 10;
                //get number of use cpu_cores
                int_val = param.get_int(l7vs::PARAM_COMP_VIRTUALSERVICE, NUM_OF_CORE_USES, err);
                if (!err)
                        num_of_core_uses = int_val;
                else
                        num_of_core_uses = 0;

                session.reset(ptr);

        }
        //! destructor
        ~session_thread_control() {
        }

        //! create up down thread
        void start_thread();

        //! session shared ptr getter
        //! @return session shared ptr
        session_ptr        get_session() {
                return session;
        }
        //! upstream thread start function
        void            startupstream();
        //! downstream thread start function
        void            startdownstream();
        //! upstream thread stop function
        void            stopupstream();
        //! downstream thread stop function
        void            stopdownstream();
        //! all thread destroy function.
        void            join();
        //! upstream-thread id getter
        //! @return thread_id_type    upstream thread id
        thread_id_type    get_upthread_id() {
                return upthread->get_id();
        }
        //! downstream-thread id getter
        //! @return thread_id_type    downstream thread id
        thread_id_type    get_downthread_id() {
                return downthread->get_id();
        }
        //! upthread mutex reference getter
        //! @return reference of boost::mutex
        boost::mutex    &get_upthread_mutex();
        //! downthread mutex reference getter
        //! @return reference of boost::mutex
        boost::mutex    &get_downthread_mutex();

        void            session_stop() {
                session->set_virtual_service_message(tcp_session::SESSION_END, boost::asio::ip::tcp::endpoint());
        }
        void            session_sorry_mode_change(int sorry_flag) {
                if (INT_MAX == sorry_flag)
                        session->set_virtual_service_message(tcp_session::SORRY_STATE_DISABLE, boost::asio::ip::tcp::endpoint());
                else if (0 != sorry_flag)
                        session->set_virtual_service_message(tcp_session::SORRY_STATE_ENABLE, boost::asio::ip::tcp::endpoint());
        }
        void            session_sorry_enable() {
                session->set_virtual_service_message(tcp_session::SORRY_STATE_ENABLE, boost::asio::ip::tcp::endpoint());
        }
        void            session_sorry_disable() {
                session->set_virtual_service_message(tcp_session::SORRY_STATE_DISABLE, boost::asio::ip::tcp::endpoint());
        }
        void            session_pause_on() {
                session->set_virtual_service_message(tcp_session::SESSION_PAUSE_ON, boost::asio::ip::tcp::endpoint());
        }
        void            session_pause_off() {
                session->set_virtual_service_message(tcp_session::SESSION_PAUSE_OFF, boost::asio::ip::tcp::endpoint());
        }

        void session_accesslog_output_mode_on() {
                session->set_virtual_service_message(tcp_session::ACCESS_LOG_ON, boost::asio::ip::tcp::endpoint());
        }
        void session_accesslog_output_mode_off() {
                session->set_virtual_service_message(tcp_session::ACCESS_LOG_OFF, boost::asio::ip::tcp::endpoint());
        }
        void            session_access_log_output_mode_change(int accesslog_flag) {
                if (accesslog_flag == true)
                        session->set_virtual_service_message(tcp_session::ACCESS_LOG_ON, boost::asio::ip::tcp::endpoint());
                else if (accesslog_flag == false)
                        session->set_virtual_service_message(tcp_session::ACCESS_LOG_OFF, boost::asio::ip::tcp::endpoint());
        }

        void            session_realserver_remove(const boost::asio::ip::tcp::endpoint &in) {
                session->set_virtual_service_message(tcp_session::REALSERVER_REMOVE, in);
        }


};

}//    namespace l7vs
#endif    //SESSION_THREAD_CONTROL_H
