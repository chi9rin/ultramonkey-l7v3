
#ifndef TCP_SESSION_BASE_H
#define TCP_SESSION_BASE_H

#include <boost/asio.hpp>

#include "virtual_service.h"


namespace l7vs{
	class tcp_session_base : private boost::noncopyable{
		public:

			tcp_session_base(
                                virtual_service* pService,
                                boost::asio::io_service& io,
                                boost::asio::ip::tcp::endpoint rs_endpoint):
                                pVs(pService),
                                cl_socket(io),
                                rs_socket(io),
                                realserver_endpoint(rs_endpoint){
                        };
			virtual ~tcp_session_base(){
                        };

                        virtual void Run_main(){
                        };
                        virtual void Run_sub(){
                        };

                        virtual_service* pVs;
			boost::asio::ip::tcp::socket cl_socket;
			boost::asio::ip::tcp::socket rs_socket;
                        boost::asio::ip::tcp::endpoint realserver_endpoint;

        };
} 

#endif//TCP_SESSION_BASE_H

