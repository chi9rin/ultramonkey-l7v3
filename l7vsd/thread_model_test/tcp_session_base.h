
#ifndef TCP_SESSION_BASE_H
#define TCP_SESSION_BASE_H

#include <boost/asio.hpp>


namespace l7vs{
        class virtualservice_tcp;
    class tcp_session_base : private boost::noncopyable{
        public:

            tcp_session_base(
                                virtualservice_tcp* pService,
                                boost::asio::io_service& io,
                                boost::asio::ip::tcp::endpoint rs_endpoint):
                                pVs(pService),
                                cl_socket(io),
                                rs_socket(io),
                                realserver_endpoint(rs_endpoint){
                        };
            virtual ~tcp_session_base(){
                        };

                        virtual boost::asio::ip::tcp::socket& get_cl_socket(){
                                return cl_socket;
                        };


                        virtual void Run_main(){
                        };
                        virtual void Run_sub(){
                        };

                        virtualservice_tcp* pVs;
            boost::asio::ip::tcp::socket cl_socket;
            boost::asio::ip::tcp::socket rs_socket;
                        boost::asio::ip::tcp::endpoint realserver_endpoint;

        };
} 

#endif//TCP_SESSION_BASE_H

