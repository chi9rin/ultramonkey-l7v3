
#ifndef TCP_SESSION_STREAM_MODEL_H
#define TCP_SESSION_STREAM_MODEL_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "wrlock.h"
#include "lockfree_queue.h"

namespace l7vs{
        class tcp_session_stream_model : private boost::noncopyable{
                public:
                        tcp_session_stream_model(boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint);
                        ~tcp_session_stream_model();

                        boost::asio::ip::tcp::socket& get_cl_socket();

                        void Run_up();
                        void Run_dw();

                        boost::asio::ip::tcp::socket cl_socket;
                        boost::asio::ip::tcp::socket rs_socket;

                        wr_mutex        threadA_state_mutex;
                        int             threadA_state;
                        wr_mutex        threadB_state_mutex;
                        int             threadB_state;

                        boost::asio::ip::tcp::endpoint realserver_endpoint;
                        bool exit_flag;
                        wr_mutex exit_flag_update_mutex;
                        wr_mutex cl_close_mutex;
                        wr_mutex rs_close_mutex;
        };
}

#endif//TCP_SESSION_STREAM_MODEL_H
