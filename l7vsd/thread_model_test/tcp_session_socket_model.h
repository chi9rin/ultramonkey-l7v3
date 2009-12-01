
#ifndef TCP_SESSION_SOCKET_MODEL_H
#define TCP_SESSION_SOCKET_MODEL_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "wrlock.h"
#include "lockfree_queue.h"
#include "tcp_session_base.h"

namespace l7vs{

        class socket_data {
		public:
                int msg_type;

                boost::array< char , 65535> buff;

                std::size_t data_size;
                socket_data():msg_type(0),data_size(0){
                };
		~socket_data(){
                };
        };

	class tcp_session_socket_model : public tcp_session_base{
		public:

			tcp_session_socket_model(virtualservice_tcp* pService,boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint);
			~tcp_session_socket_model();

                        void Run_main();
                        void Run_sub();

                        void Run_cl();
                        void Run_rs();

                        wr_mutex        threadA_state_mutex;
                        int             threadA_state;
                        wr_mutex        threadB_state_mutex;
                        int             threadB_state;


                        lockfree_queue< socket_data > cl2rs_msg;
                        lockfree_queue< socket_data > rs2cl_msg;
	};
}

#endif//TCP_SESSION_SOCKET_MODEL_H
