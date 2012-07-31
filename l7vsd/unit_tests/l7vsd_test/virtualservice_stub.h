#ifndef    VIRTUALSERVICE_H
#define    VIRTUALSERVICE_H

#include <iostream>
#include "virtualservice_element.h"
#include "replication_stub.h"
#include "boost/thread.hpp"

namespace    l7vs
{
class    l7vsd;

class    virtual_service
{
public:

//variable
        const    l7vs::l7vsd        &vsd;
        virtualservice_element        element;

        //for stub
        bool    initialize_called;
        static    bool    initialize_fail;
        bool    set_virtualservice_called;
        static    bool    set_virtualservice_fail;
        bool    edit_virtualservice_called;
        static    bool    edit_virtualservice_fail;

        bool    add_realserver_called;
        static    bool    add_realserver_fail;
        bool    edit_realserver_called;
        static    bool    edit_realserver_fail;
        bool    del_realserver_called;
        static    bool    del_realserver_fail;

        static    bool    finalize_called;
        static    bool    finalize_fail;
        static    bool    stop_called;

        bool    run_called;




//function
        virtual_service(const l7vs::l7vsd &in_vsd,
                        const l7vs::replication & ,
                        const virtualservice_element &)
                :    vsd(in_vsd),
                     initialize_called(false),
                     set_virtualservice_called(false),
                     edit_virtualservice_called(false),
                     add_realserver_called(false),
                     edit_realserver_called(false),
                     del_realserver_called(false),
                     run_called(false)
        {}

        ~virtual_service()
        {}

        void                        initialize(error_code &err) {
                initialize_called = true;
                if (initialize_fail)    err.setter(true, "initialize_fail!");
        }
        void                        finalize(error_code &err);

//    bool                        operator==( const virtualservice_base& );
//    bool                        operator!=( const virtualservice_base& );


        void                        set_virtualservice(const virtualservice_element &in, error_code &err) {
                element = in;
                set_virtualservice_called = true;
                if (set_virtualservice_fail)    err.setter(true, "set_virtualservice_fail!");
        }

        void                        edit_virtualservice(const virtualservice_element &in, error_code &err) {
                element = in;
                edit_virtualservice_called = true;
                if (edit_virtualservice_fail)    err.setter(true, "edit_virtualservice_fail!");
        }

        void                        add_realserver(const virtualservice_element &in, error_code &err) {
                element = in;
                add_realserver_called = true;
                if (add_realserver_fail)    err.setter(true, "add_realserver_fail!");
        }

        void                        edit_realserver(const virtualservice_element &in, error_code &err) {
                element = in;
                edit_realserver_called = true;
                if (edit_realserver_fail)    err.setter(true, "edit_realserver_fail!");
        }

        void                        del_realserver(const virtualservice_element &in, error_code &err) {
                element = in;
                del_realserver_called = true;
                if (del_realserver_fail)    err.setter(true, "del_realserver_fail!");
        }

        virtualservice_element        &get_element() {
                return element;
        }

        void                        run() {
                run_called = true;
        }
        void                        stop() {
                stop_called = true;
        }

//     void        connection_active( const boost::asio::ip::tcp::endpoint& in );
//     void        connection_inactive( const boost::asio::ip::tcp::endpoint& in );
//     void        release_session( const boost::thread::id thread_id );
//
//     void        connection_active( const boost::asio::ip::tcp::endpoint&  );
//     void        connection_inactive( const boost::asio::ip::tcp::endpoint&  );
//     void        release_session( const boost::thread::id );

        unsigned long long            get_qos_upstream();
        unsigned long long            get_qos_downstream();
        unsigned long long            get_throughput_upstream();
        unsigned long long            get_throughput_downstream();

        void                        update_up_recv_size(unsigned long long);
        void                        update_up_send_size(unsigned long long);
        void                        update_down_recv_size(unsigned long long);
        void                        update_down_send_size(unsigned long long);

        unsigned long long            get_up_recv_size() {
                return 12345ULL;
        }
        unsigned long long            get_up_send_size() {
                return 23456ULL;
        }
        unsigned long long            get_down_recv_size() {
                return 34567ULL;
        }
        unsigned long long            get_down_send_size() {
                return 45678ULL;
        }

//     boost::shared_ptr<protocol_module_base>
//                                 get_protocol_module();
//     boost::shared_ptr<schedule_module_base>
//                                 get_schedule_module();

};


}    //namespace    l7vs
#endif    //VIRTUALSERVICE_H
