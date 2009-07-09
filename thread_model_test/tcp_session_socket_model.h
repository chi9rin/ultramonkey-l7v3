
#ifndef TCP_SESSION_SOCKET_MODEL_H
#define TCP_SESSION_SOCKET_MODEL_H

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "wrlock.h"
#include "lockfree_queue.h"

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

	class tcp_session_socket_model : private boost::noncopyable{
		public:

			tcp_session_socket_model(boost::asio::io_service& io,boost::asio::ip::tcp::endpoint rs_endpoint);
			~tcp_session_socket_model();

			boost::asio::ip::tcp::socket& get_cl_socket();
                        
                        
                        void Run_cl();
                        void Run_rs();


			//! control socket
			boost::asio::ip::tcp::socket cl_socket;
			boost::asio::ip::tcp::socket rs_socket;


                        wr_mutex        threadA_state_mutex;
                        int             threadA_state;
                        wr_mutex        threadB_state_mutex;
                        int             threadB_state;


                        lockfree_queue< socket_data > cl2rs_msg;
                        lockfree_queue< socket_data > rs2cl_msg;
                        
                        boost::asio::ip::tcp::endpoint realserver_endpoint;
	};
}

#endif//TCP_SESSION_SOCKET_MODEL_H
