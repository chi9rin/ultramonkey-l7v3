//
//!    @file    dispatcher.h
//!    @brief    snmpagent dispatch header
//
//    copyright(c) sdy corporation.2008
//    mail: h.okada at sdy.co.jp
//    Copyright (c) 2008 norihisa nakai (n dot nakai at sdy dot co do jp)
//
//    Distributed under the Boost Software License, Version 1.0. (See accompanying
//    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef L7AG_DISPATCHER_H
#define L7AG_DISPATCHER_H

#include <map>
#include <string>
#include "lockedqueue.h"
#include "QueueContainer.h"
#include "trapdata.h"
#include "parameter.h"
#include "logger.h"
#include "message.h"
#include <boost/function.hpp>
#include <boost/bind.hpp>

//! messagequeue typedef
typedef lockedqueue<QueueContainer> MessageQueue;
//! lockqueue typedef
typedef lockedqueue<trapdata>       TrapQueue;

//
//! @class    l7ag_dispatcher
//!    @brief    message dispatch class
class l7ag_dispatcher
{

private:
        bool        stop_flag;            //! stopping flag
        pthread_t   dispatch_thread;    //!    dispatch thread id

        MessageQueue   *message_que;    //! message queue object pointer
        TrapQueue      *trap_que;        //!    trap queue object pointer

        size_t  msg_size;                //! message size
        char   *msg;                    //! message pointer
        size_t  msg_pos;                //! message posision

        std::map< int, boost::function<void (void *)> > dispatch_functions;

        l7ag_dispatcher();

protected:
        static void *run(void *);

        void dispatch_message();
        void process_setting_command(void *);
        void process_trap_request(void *);
        void process_mib_collect_response_vs(void *);
        void process_mib_collect_response_rs(void *);

        void change_loglevel(unsigned long long , unsigned long long);
        void reload_configure();
        void init_dispatcher_function_map();

public:
        l7ag_dispatcher(MessageQueue *, TrapQueue *);
        ~l7ag_dispatcher();

        bool start();
        bool stop();

        MessageQueue   *get_MessageQueue() {
                return message_que;
        }
        TrapQueue      *get_TrapQueue() {
                return trap_que;
        }
};

#endif //__L7AG_DISPATCHER_H__

