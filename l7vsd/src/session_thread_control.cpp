/*!
 *    @file    session_thread_control.cpp
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

#include <sched.h>
#include "session_thread_control.h"

#include "utility.h"

namespace l7vs
{

//
//!    @brief create up down thread
//
void    session_thread_control::start_thread()
{

        int            int_val;

        upthread.reset(new boost::thread(&session_thread_control::upstream_run, this));        //! upstream thread create
        downthread.reset(new boost::thread(&session_thread_control::downstream_run, this));    //! downstream thread create

        //pthread_setschedparam
        int    retval, sched_policy;
        sched_param    scheduler_param;
        int_val    = pthread_getschedparam(upthread->native_handle(), &sched_policy, &scheduler_param);
        if (SCHED_FIFO == sched_algorithm) {
                scheduler_param.__sched_priority    = sched_priority;
                sched_policy    = SCHED_FIFO;
        } else if (SCHED_RR == sched_algorithm) {
                scheduler_param.__sched_priority    = sched_priority;
                sched_policy    = SCHED_RR;
        } else if (SCHED_BATCH == sched_algorithm) {
                sched_policy    = SCHED_BATCH;
        }
        if (0 <= sched_algorithm) {
                retval            = pthread_setschedparam(upthread->native_handle(), sched_algorithm, &scheduler_param);
                retval            = pthread_setschedparam(downthread->native_handle(), sched_algorithm, &scheduler_param);
        }

}

//
//!    @brief upstream thread bind function.
//
void    session_thread_control::upstream_run()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::upstream_run()");
                Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 1, fmt.str(), __FILE__, __LINE__);
        }

        cpu_set_t       mask;
        // when num>0, do CPU_SET times of "num_of_core_uses".
        if (0 < num_of_core_uses) {
                CPU_ZERO(&mask);
                for (int i = 0; i < num_of_core_uses; ++i) {
                        CPU_SET(i, &mask);
                }
                sched_setaffinity(0, sizeof(cpu_set_t), &mask);

                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                        boost::format formatter("void virtualservice_tcp::upstream_run() "
                                                "sched_setaffinity after num_of_core_uses = %d");
                        formatter % num_of_core_uses;
                        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 2, formatter.str(), __FILE__, __LINE__);
                }

        }
#ifdef    SCHED_SETAFFINITY
        sched_setaffinity(0, sizeof(cpu_set_t), &vsnic_cpumask);
#endif

        state_tag    state;
        upthread_running_mutex.lock();
        {
                // get first state from class upstream state.
                rw_scoped_lock    upstate_lock(upthread_state_mutex);
                state = upthread_state;    //thread local state is update.
        }
        for (;;) {  // thread loop
                if (state == WAIT) {    // after create or session end. this thread is pooling mode
                        boost::mutex::scoped_lock    lock(upthread_condition_mutex);
                        boost::xtime    wait;
#if BOOST_VERSION >= 105000
                        boost::xtime_get(&wait, boost::TIME_UTC_);
#else
                        boost::xtime_get(&wait, boost::TIME_UTC);
#endif

                        wait.sec += 1;
                        upthread_running_mutex.unlock();
                        upthread_condition.timed_wait(lock, wait);   // thread is condition wait( start at notify_all() )
                        upthread_running_mutex.lock();
                } else if (state == EXIT) { // this state is virtualservice end. thread is finishing.
                        break;
                } else { //state RUNNING
                        session->up_thread_run();    //session upstream thread looping.
                        stopupstream();
                }
                rw_scoped_lock    upstate_lock(upthread_state_mutex);
                state = upthread_state;    //thread local state is update.
        }
        upthread_running_mutex.unlock();
        boost::mutex::scoped_lock up_lk(upthread_joining_mutex);
        upthread_joining_condition.notify_all();
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::upstream_run()");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 3, fmt.str(), __FILE__, __LINE__);
        }
}
//
//! @brief    downstream thread bind function,
//
void    session_thread_control::downstream_run()
{
        if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::downstream_run()");
                Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 4, fmt.str(), __FILE__, __LINE__);
        }
        cpu_set_t       mask;
        // when num>0, do CPU_SET times of "num_of_core_uses".
        if (0 < num_of_core_uses) {
                CPU_ZERO(&mask);
                for (int i = 0; i < num_of_core_uses; ++i) {
                        CPU_SET(i, &mask);
                }
                sched_setaffinity(0, sizeof(cpu_set_t), &mask);

                if (unlikely(LOG_LV_DEBUG == Logger::getLogLevel(LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                        boost::format formatter("void virtualservice_tcp::downstream_run() "
                                                "sched_setaffinity after num_of_core_uses = %d");
                        formatter % num_of_core_uses;
                        Logger::putLogDebug(LOG_CAT_L7VSD_VIRTUALSERVICE, 5, formatter.str(), __FILE__, __LINE__);
                }

        }
#ifdef    SCHED_SETAFFINITY
        sched_setaffinity(0, sizeof(cpu_set_t), &rsnic_cpumask);
#endif

        state_tag    state;
        downthread_running_mutex.lock();
        {
                rw_scoped_lock    downstate_lock(downthread_state_mutex);
                state = downthread_state;    //thread local state is update.
        }
        for (;;) {  //thread loop
                if (state == WAIT) {    //after create or session end. this thread is pooling mode
                        boost::mutex::scoped_lock    lock(downthread_condition_mutex);
//             downthread_condition.wait( lock ); // thread is condition wait( start at notify_all() )
                        boost::xtime    wait;
#if BOOST_VERSION >= 105000
                        boost::xtime_get(&wait, boost::TIME_UTC_);
#else
                        boost::xtime_get(&wait, boost::TIME_UTC);
#endif

                        wait.sec += 1;
                        downthread_running_mutex.unlock();
                        downthread_condition.timed_wait(lock, wait);   // thread is condition wait( start at notify_all() )
                        downthread_running_mutex.lock();
                } else if (state == EXIT) { // this state is virtualservice end. thread is finishing.
                        break;
                } else { //state RUNNING
                        session->down_thread_run();//session downstream thread looping.
                        stopdownstream();
                }
                rw_scoped_lock    downstate_lock(downthread_state_mutex);
                state = downthread_state;    // thread local sate is update.
        }
        downthread_running_mutex.unlock();
        boost::mutex::scoped_lock down_lk(downthread_joining_mutex);
        downthread_joining_condition.notify_all();
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::downstream_run()");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 6, fmt.str(), __FILE__, __LINE__);
        }
}
//
//! @brief    start upstream function.
//
void    session_thread_control::startupstream()
{
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::startupstream()");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 7, fmt.str(), __FILE__, __LINE__);
        }
        rw_scoped_lock    upstate_lock(upthread_state_mutex);
        if (upthread_state != EXIT) upthread_state = RUNNING;         // upthread state update.[RUNNING] -> alive mode
        upthread_condition.notify_all();                            // conditionwait upstreamthread is run.
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::startupstream() :status = %d");
                fmt % upthread_state;
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 8, fmt.str(), __FILE__, __LINE__);
        }
}
//
//! @brief    stop upstream function
//
void    session_thread_control::stopupstream()
{
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::stopupstream()");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 9, fmt.str(), __FILE__, __LINE__);
        }
        rw_scoped_lock    upstate_lock(upthread_state_mutex);
        if (upthread_state != EXIT) upthread_state = WAIT;                 // upthread state is update [WAIT] -> pooling mode
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::stopupstream() : status = %d");
                fmt % upthread_state;
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 10, fmt.str(), __FILE__, __LINE__);
        }
}
//
//! @brief    start downstream function
//
void    session_thread_control::startdownstream()
{
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::startdownstream()");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 11, fmt.str(), __FILE__, __LINE__);
        }
        rw_scoped_lock    downstate_lock(downthread_state_mutex);
        if (downthread_state != EXIT) downthread_state = RUNNING;         // downstream state is update [RUNNING] -> alive mode
        downthread_condition.notify_all();                                // condition wait thread is run.
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::startdownstream() : status = %d");
                fmt % downthread_state;
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 12, fmt.str(), __FILE__, __LINE__);
        }
}
//
//! @brief    stop downstream function.
//
void    session_thread_control::stopdownstream()
{
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::stopdownstream()");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 13, fmt.str(), __FILE__, __LINE__);
        }
        rw_scoped_lock    downstate_lock(downthread_state_mutex);
        if (downthread_state != EXIT) downthread_state = WAIT;             // downstream state is update [WAIT] -> pooling mode
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::stopdownstream() : status = %d");
                fmt % downthread_state;
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 14, fmt.str(), __FILE__, __LINE__);
        }
}
//
//!    @brief    upstream and downstream threads finished function
//
void    session_thread_control::join()
{
        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("in_function : void session_thread_control::join() :");
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 15, fmt.str(), __FILE__, __LINE__);
        }

        boost::mutex::scoped_lock    up_lk(upthread_joining_mutex);
        boost::mutex::scoped_lock    down_lk(downthread_joining_mutex);

        {
                rw_scoped_lock    upstate_lock(upthread_state_mutex);
                upthread_state = EXIT;    //upstream state update [EXIT] -> thread exit mode
        }
        {
                boost::mutex::scoped_lock upthread_running_wait(upthread_running_mutex);
                upthread_condition.notify_all();    // conditionwait thread is run
        }

        {
                rw_scoped_lock    downstate_lock(downthread_state_mutex);
                downthread_state = EXIT;    //downstream state update [EXIT] -> thread exit mode
        }
        {
                boost::mutex::scoped_lock downthread_running_wait(downthread_running_mutex);
                downthread_condition.notify_all(); //condition wait thread is run.
        }

        upthread_joining_condition.wait(up_lk);
        downthread_joining_condition.wait(down_lk);

        if (unlikely(LOG_LV_DEBUG == l7vs::Logger::getLogLevel(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE))) {
                boost::format fmt("out_function : void session_thread_control::stopdownstream() : up_status = %d / down_status = %d");
                fmt % upthread_state % downthread_state;
                l7vs::Logger::putLogDebug(l7vs::LOG_CAT_L7VSD_VIRTUALSERVICE, 16, fmt.str(), __FILE__, __LINE__);
        }
}
//
//!    @brief    get upthread mutex object reference
//
boost::mutex    &session_thread_control::get_upthread_mutex()
{
        return    upthread_running_mutex;
}
//
//!    @brief    get downthread mutex object reference
//
boost::mutex    &session_thread_control::get_downthread_mutex()
{
        return    downthread_running_mutex;
}

}    //namespace l7vs
