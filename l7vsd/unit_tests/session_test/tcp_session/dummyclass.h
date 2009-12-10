// Dummy Class define

#define VIRTUALSERVICE_H
#define TCP_SOCKET_H
#define TCP_SSL_SOCKET_H
#define MODULE_BASE_H
#define REALSERVER_H

#include <boost/asio/ssl.hpp>

namespace l7vs{

    // Dummy module base class 
    class  module_base{
        public:
            module_base( std::string in_modulename ){};
            ~module_base(){};
    };
    
    // Dummy realserver class
    class  realserver{
        realserver(){};
        ~realserver(){};
    };
}

#include "protocol_module_base.h"
#include "tcp_socket_option.h"

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>  ssl_socket;

namespace l7vs{
    class    tcp_session;

    // test protocol module class
    class test_protocol_module : public protocol_module_base{
        public:
            test_protocol_module( std::string in_modulename ) : protocol_module_base(in_modulename ){
        
            }
            ~test_protocol_module(){};
            
            //! initialize function. called from module control. module loaded call
            //! @param[in]    realserver list iterator begin function object type
            //!    @param[in]    realserver list iterator end function object type
            //! @param[in]    realserver list iterator next function object type
            //! @param[in]    realserver list mutex lock function object type.
            //! @param[in]    realserver list mutex unlock function object type
            void    initialize(
                    rs_list_itr_func_type    inlist_begin,
                    rs_list_itr_func_type    inlist_end,
                    rs_list_itr_next_func_type    inlist_next,
                    boost::function< void( void ) >    inlist_lock,
                    boost::function< void( void ) >    inlist_unlock ){};

            //! finalize called from module control. module unloaded call.
            void    finalize(){};

            //! sorry support check
            //! @return true sorry mode is supported.
            //! @return false sorry mode is unsupported.
            bool    is_use_sorry(){
                return true;
            };

            void get_option_info(std::string&){};


            //! realserver list update event
            void    handle_rslist_update(){};

            //! module parameter check.used by l7vsadm
            //! @param[in]    module paramter string list
            //! @return    result.flag true is parameter is noproblem.
            //! @return result.flag false is paramter is problem.
            protocol_module_base::check_message_result    check_parameter( const std::vector<std::string>& args ){
                check_message_result res;
                return res;
            };
            
            //! parameter set
            //! @param[in] module paramter string list
            //! @return    result.flag true is parameter is noproblem.
            //! @return result.flag false is paramter is problem.
            protocol_module_base::check_message_result    set_parameter( const std::vector<std::string>& args ){
                check_message_result res;
                return res;
            };
            
            //! parameter add
            //! @param[in] module paramter string list
            //! @return    result.flag true is parameter is noproblem.
            //! @return result.flag false is paramter is problem.
            protocol_module_base::check_message_result    add_parameter( const std::vector<std::string>& args )
            {
                check_message_result res;
                return res;
            };

            //! TCP/IP scheduled function registation.
            //! @param[in] schedule module TCP/IP scheduled function object type
            void    register_schedule( protocol_module_base::tcp_schedule_func_type inschedule ){
            };
            
            //! UDP scheduled function registation
            //! @param[in] schedule module UDP scheduled funtion object type
            void    register_schedule( protocol_module_base::udp_schedule_func_type inschedule ){
            };

            //! called from session initialzie use in upstream_thread
            //! @param[in]    upstream thread id.
            //! @param[in]    downstream thread id
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_session_initialize(
                    const boost::thread::id up_thread_id,
            const boost::thread::id down_thread_id,
            const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
            const boost::asio::ip::udp::endpoint& client_endpoint_udp ){
                
                handle_session_initialize_in_up_thread_id = up_thread_id;
                handle_session_initialize_in_down_thread_id = down_thread_id;
                handle_session_initialize_in_client_endpoint_tcp = client_endpoint_tcp;
                handle_session_initialize_in_client_endpoint_udp = client_endpoint_udp;
                    
                return handle_session_initialize_res_tag;
                    
            };
            protocol_module_base::EVENT_TAG handle_session_initialize_res_tag;
            boost::thread::id handle_session_initialize_in_up_thread_id;
            boost::thread::id handle_session_initialize_in_down_thread_id;
            boost::asio::ip::tcp::endpoint handle_session_initialize_in_client_endpoint_tcp;
            boost::asio::ip::udp::endpoint handle_session_initialize_in_client_endpoint_udp;

            //! called from session finalize use in upstream thread.
            //! @param[in]    upstream thread id.
            //! @param[in]    downstream thread id
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_session_finalize(
                    const boost::thread::id up_thread_id,
            const boost::thread::id down_thread_id ){
                
                handle_session_finalize_in_up_thread_id = up_thread_id;
                handle_session_finalize_in_down_thread_id = down_thread_id;
                return     handle_session_finalize_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_session_finalize_res_tag;
            boost::thread::id handle_session_finalize_in_up_thread_id;
            boost::thread::id handle_session_finalize_in_down_thread_id;

            //! called from after session accept.in client socket use in upstream thread.
            //! @param[in]    upstream thread id.
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_accept( const boost::thread::id thread_id ){
                handle_accept_in_thread_id = thread_id;
                return handle_accept_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_accept_res_tag;
            boost::thread::id handle_accept_in_thread_id;

            //! called from after session recv in client socket. use in upstream thread.
            //! @param[in]    upstream thread id
            //! @param[in]    recive buffer refarence.
            //! @param[in]    recive length
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_client_recv(
                    const boost::thread::id thread_id,
            const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
            const size_t recvlen ){
                
                handle_client_recv_in_thread_id = thread_id;
                handle_client_recv_in_recvbuffer = recvbuffer;
                handle_client_recv_in_recvlen = recvlen;
                return handle_client_recv_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_client_recv_res_tag;
            boost::thread::id handle_client_recv_in_thread_id;
            boost::array<char,MAX_BUFFER_SIZE> handle_client_recv_in_recvbuffer;
            size_t handle_client_recv_in_recvlen;

            //! called from after realserver select.use in upstream thread. 
            //! @param[in]    upstream thread id
            //! @param[out]    realserver TCP endpoint
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_select(
                    const boost::thread::id thread_id,
            boost::asio::ip::tcp::endpoint& rs_endpoint ){
                
                handle_realserver_select_in_thread_id = thread_id;
                rs_endpoint = handle_realserver_select_out_rs_endpoint;
                return handle_realserver_select_tcp_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_select_tcp_res_tag;
            boost::thread::id handle_realserver_select_in_thread_id;
            boost::asio::ip::tcp::endpoint handle_realserver_select_out_rs_endpoint;

            //! called from after realserver select
            //! @param[in]    upstream thread id
            //! @param[out]    realserver UDP endpoint
            //! @param[out]    sendbudffer reference
            //! @param[out]    send data length
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_select(
                const boost::thread::id thread_id,
                boost::asio::ip::udp::endpoint& rs_endpoint,
                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                size_t& datalen ){
                
                handle_realserver_select_udp_thread_id = thread_id;
                rs_endpoint = handle_realserver_select_udp_rs_endpoint;
                sendbuffer = handle_realserver_select_udp_sendbuffer;
                datalen = handle_realserver_select_udp_datalen;
                return handle_realserver_select_udp_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_select_udp_res_tag;
            boost::thread::id handle_realserver_select_udp_thread_id;
            boost::asio::ip::udp::endpoint handle_realserver_select_udp_rs_endpoint;
            boost::array<char,MAX_BUFFER_SIZE> handle_realserver_select_udp_sendbuffer;
            size_t  handle_realserver_select_udp_datalen;

            //! called from after realserver connect
            //! @param[in]    upstream thread id
            //! @param[out]    sendbuffer reference
            //! @param[out]    send data length
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_connect(
                const boost::thread::id thread_id,
                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                size_t& datalen ){
                
                handle_realserver_connect_thread_id = thread_id;
                sendbuffer = handle_realserver_connect_sendbuffer;
                datalen = handle_realserver_connect_datalen;
                return handle_realserver_connect_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_connect_res_tag;
            boost::thread::id handle_realserver_connect_thread_id;
            boost::array<char,MAX_BUFFER_SIZE> handle_realserver_connect_sendbuffer;
            size_t  handle_realserver_connect_datalen;

            //! called from after realserver connection fail
            //! @param[in]    upstream thread id
            //! @param[in]    fail realserver endpoint reference
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_connection_fail(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint& rs_endpoint ){
                
                handle_realserver_connection_fail_thread_id = thread_id;
                handle_realserver_connection_fail_rs_endpoint = rs_endpoint;
                return    handle_realserver_connection_fail_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_connection_fail_res_tag;
            boost::thread::id handle_realserver_connection_fail_thread_id;
            boost::asio::ip::tcp::endpoint handle_realserver_connection_fail_rs_endpoint;
            
            //! called from after realserver send.
            //! @param[in]    upstream thread id
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_send(
                const boost::thread::id thread_id ){
                
                handle_realserver_send_in_thread_id = thread_id;
                return handle_realserver_send_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_send_res_tag;
            boost::thread::id handle_realserver_send_in_thread_id;

            //! called from after sorryserver select
            //! @param[in]    upstream thread id
            //! @param[in]    sorryserver endppiont reference
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_sorryserver_select(
                const boost::thread::id thread_id,
                boost::asio::ip::tcp::endpoint& sorry_endpoint ){
                
                handle_sorryserver_select_in_thread_id = thread_id;
                handle_sorryserver_select_in_sorry_endpoint = sorry_endpoint;
                sorry_endpoint = handle_sorryserver_select_out_sorry_endpoint;
                return handle_sorryserver_select_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorryserver_select_res_tag;
            boost::thread::id handle_sorryserver_select_in_thread_id;
            boost::asio::ip::tcp::endpoint handle_sorryserver_select_out_sorry_endpoint;
            boost::asio::ip::tcp::endpoint handle_sorryserver_select_in_sorry_endpoint;
            
            //! called from after sorryserver connect
            //!    @param[in]    upstream thread id
            //! @param[out]    send buffer reference.
            //! @param[out]    send length
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_sorryserver_connect(
                const boost::thread::id thread_id,
                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                size_t& datalen ){
                
                handle_sorryserver_connect_thread_id = thread_id;
                sendbuffer = handle_sorryserver_connect_sendbuffer;
                datalen = handle_sorryserver_connect_datalen;
                return handle_sorryserver_connect_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorryserver_connect_res_tag;
            boost::thread::id handle_sorryserver_connect_thread_id;
            boost::array<char,MAX_BUFFER_SIZE> handle_sorryserver_connect_sendbuffer;
            size_t  handle_sorryserver_connect_datalen;

            //! called from after sorryserver connection fail
            //! @param[in]    upstream thread id
            //! @param[in]    sorryserver endpoint reference.
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_sorryserver_connection_fail(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint& sorry_endpoint ){
                    
                handle_sorryserver_connection_fail_thread_id = thread_id;
                handle_sorryserver_connection_fail_sorry_endpoint = sorry_endpoint;
                return handle_sorryserver_connection_fail_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorryserver_connection_fail_res_tag;
            boost::thread::id handle_sorryserver_connection_fail_thread_id;
            boost::asio::ip::tcp::endpoint handle_sorryserver_connection_fail_sorry_endpoint;

            //! called from after sorryserver send
            //! @param[in]    upstream thread id
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_sorryserver_send( const boost::thread::id thread_id ){
                
                handle_sorryserver_send_in_thread_id = thread_id;
                return handle_sorryserver_send_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorryserver_send_res_tag;
            boost::thread::id handle_sorryserver_send_in_thread_id;

            //! called from after realserver recvive for TCP/IP
            //! @param[in]    downstream thread id
            //! @param[in]    realserver TCP/IP endpoint reference
            //! @param[in]    realserver recive buffer reference.
            //! @param[in]    recv data length
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_recv(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint& rs_endpoint,
                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen ){
                
                handle_realserver_recv_tcp_in_thread_id = thread_id;
                handle_realserver_recv_tcp_in_rs_endpoint = rs_endpoint;
                handle_realserver_recv_tcp_in_recvbuffer = recvbuffer;
                handle_realserver_recv_tcp_in_recvlen = recvlen;
                return handle_realserver_recv_tcp_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_recv_tcp_res_tag;
            boost::thread::id handle_realserver_recv_tcp_in_thread_id;
            boost::asio::ip::tcp::endpoint handle_realserver_recv_tcp_in_rs_endpoint;
            boost::array<char,MAX_BUFFER_SIZE> handle_realserver_recv_tcp_in_recvbuffer;
            size_t  handle_realserver_recv_tcp_in_recvlen;
            
            //! called from after realserver recive.for UDP
            //! @param[in]    downstream thread id
            //! @param[in]    realserver UDP endpoint reference
            //! @param[in]    recive from realserver buffer reference
            //! @param[in]    recv data length
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_recv(
                const boost::thread::id thread_id,
                const boost::asio::ip::udp::endpoint& rs_endpoint,
                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen ){
                
                handle_realserver_recv_udp_thread_id = thread_id;
                handle_realserver_recv_udp_rs_endpoint = rs_endpoint;
                handle_realserver_recv_udp_recvbuffer = recvbuffer;
                handle_realserver_recv_udp_recvlen = recvlen;
                return     handle_realserver_recv_udp_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_recv_udp_res_tag;
            boost::thread::id handle_realserver_recv_udp_thread_id;
            boost::asio::ip::udp::endpoint handle_realserver_recv_udp_rs_endpoint;
            boost::array<char,MAX_BUFFER_SIZE> handle_realserver_recv_udp_recvbuffer;
            size_t  handle_realserver_recv_udp_recvlen;

            //! called from after sorryserver recive
            //! @param[in]    downstream thread id
            //! @param[in]    sorryserver endpoint reference
            //! @param[in]    recive from realserver buffer reference.
            //! @param[in]    recv data length
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_sorryserver_recv(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint& sorry_endpoint,
                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                const size_t recvlen ){
                    
                handle_sorryserver_recv_in_thread_id = thread_id;
                handle_sorryserver_recv_in_sorry_endpoint = sorry_endpoint;
                handle_sorryserver_recv_in_recvbuffer = recvbuffer;
                handle_sorryserver_recv_in_recvlen = recvlen;
                return handle_sorryserver_recv_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorryserver_recv_res_tag;
            boost::thread::id handle_sorryserver_recv_in_thread_id;
            boost::asio::ip::tcp::endpoint handle_sorryserver_recv_in_sorry_endpoint;
            boost::array<char,MAX_BUFFER_SIZE> handle_sorryserver_recv_in_recvbuffer;
            size_t  handle_sorryserver_recv_in_recvlen;

            //! called from UPSTEEARM thread. make module original message.
            //! @param[in]    downstream thread id.
            //! @return         session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_response_send_inform(
                const boost::thread::id thread_id ){
                
                handle_response_send_inform_thread_id = thread_id;
                return handle_response_send_inform_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_response_send_inform_res_tag;
            boost::thread::id handle_response_send_inform_thread_id;

            //! called from after client connection check. use TCP/IP only. create client send message.
            //! @param[in]    downstream thread id
            //! @param[out]    send budffer reference
            //! @param[out]    send data length
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_client_connection_check(
                const boost::thread::id thread_id,
                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                size_t& datalen ){
                    
                handle_client_connection_check_thread_id = thread_id;
                sendbuffer = handle_client_connection_check_sendbuffer;
                datalen = handle_client_connection_check_datalen;
                return handle_client_connection_check_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_client_connection_check_res_tag;
            boost::thread::id handle_client_connection_check_thread_id;
            boost::array<char,MAX_BUFFER_SIZE> handle_client_connection_check_sendbuffer;
            size_t  handle_client_connection_check_datalen;

            //! called from after client select. use UDP only
            //! @param[in]    downstream thread id
            //!    @param[in]    client udp endpoint
            //! @param[out]    send buffer reference
            //! @param[out]    send data length
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_client_select(
                const boost::thread::id thread_id,
                boost::asio::ip::udp::endpoint& cl_endpoint,
                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                size_t& datalen ){
                    
                handle_client_select_thread_id = thread_id;
                handle_client_select_cl_endpoint = cl_endpoint;
                sendbuffer = handle_client_select_sendbuffer;
                datalen = handle_client_select_datalen;
                return handle_client_select_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_client_select_res_tag;
            boost::thread::id handle_client_select_thread_id;
            boost::asio::ip::udp::endpoint handle_client_select_cl_endpoint;
            boost::array<char,MAX_BUFFER_SIZE> handle_client_select_sendbuffer;
            size_t  handle_client_select_datalen;
            //!    called from after client send
            //!    @param[in]    downstream thread id
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_client_send( const boost::thread::id thread_id ){
                
                handle_client_send_in_thread_id = thread_id;
                return handle_client_send_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_client_send_res_tag;
            boost::thread::id handle_client_send_in_thread_id;

            //! call from client disconnect event. use upstream thread and downstream thread.
            //! @param[in]    upstream and downstream thread id( check! one thread one event! )
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_client_disconnect(const boost::thread::id thread_id ){
                
                handle_client_disconnect_thread_id = thread_id;
                return handle_client_disconnect_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_client_disconnect_res_tag;
            boost::thread::id handle_client_disconnect_thread_id;

            //! call from sorry mode event. use upstream thread and downstream thread
            //! @param[in]    upstream and downstream thread id( check! one thread one event and first time call pattern )    
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_sorry_enable( const boost::thread::id thread_id ){
                
                handle_sorry_enable_thread_id = thread_id;
                return handle_sorry_enable_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorry_enable_res_tag;
            boost::thread::id handle_sorry_enable_thread_id;

            //! call from sorry mode disable. use upstream thread and downstream thread.
            //! @param[in]    upstream and downstream thread id( check! one thread one event )
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_sorry_disable( const boost::thread::id thread_id ){
                
                handle_sorry_disable_thread_id = thread_id;
                return handle_sorry_disable_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_sorry_disable_res_tag;
            boost::thread::id handle_sorry_disable_thread_id;
            
            //! call from realserver disconnect. use upstream thread and downstream thread
            //! @param[in]    upstream and downstream thread id( check! one thread one event )
            //! @param[in]    disconnected realserver endpoint.
            //! @return     session use EVENT mode
            protocol_module_base::EVENT_TAG    handle_realserver_disconnect(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint& rs_endpoint ){
                
                handle_realserver_disconnect_thread_id = thread_id;
                handle_realserver_disconnect_rs_endpoint = rs_endpoint;
                handle_realserver_disconnect_rs_endpoint_list.push_back(rs_endpoint);
                return handle_realserver_disconnect_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_disconnect_res_tag;
            boost::thread::id handle_realserver_disconnect_thread_id;
            boost::asio::ip::tcp::endpoint handle_realserver_disconnect_rs_endpoint;
            std::list< boost::asio::ip::tcp::endpoint > handle_realserver_disconnect_rs_endpoint_list;
                    
            //! call from sorry server disconnect. use upstraem thread and downstream thread
            //! @param[in]    upstream and downstream thread id( check! one thread one event )
            //! @param[in]    disconnect sorryserver endpoint
            //! @return        session use EVENT mode    
            protocol_module_base::EVENT_TAG    handle_sorryserver_disconnect(
                const boost::thread::id thread_id,
                const boost::asio::ip::tcp::endpoint& sorry_endpoint ){
                    
                handle_sorryserver_disconnect_thread_id = thread_id;
                handle_sorryserver_disconnect_rs_endpoint = sorry_endpoint;
                return handle_sorryserver_disconnect_res_tag;
            };
            protocol_module_base::EVENT_TAG  handle_sorryserver_disconnect_res_tag;
            boost::thread::id handle_sorryserver_disconnect_thread_id;
            boost::asio::ip::tcp::endpoint handle_sorryserver_disconnect_rs_endpoint;

            //! call from realserver disconnect. use upstream thread and downstream thread.
            //! @param[in]    upstream and downstream thread id( check! one thread one event )
            //! @param[in]    disconnect realserver endpoint
            //! @return        session use EVENT mode.
            protocol_module_base::EVENT_TAG    handle_realserver_close(
                const boost::thread::id thread_id,
                const boost::asio::ip::udp::endpoint& rs_endpoint ){
                    
                handle_realserver_close_thread_id = thread_id;
                handle_realserver_close_rs_endpoint = rs_endpoint;
                return    handle_realserver_close_res_tag;
            };
            protocol_module_base::EVENT_TAG handle_realserver_close_res_tag;
            boost::thread::id handle_realserver_close_thread_id;
            boost::asio::ip::udp::endpoint handle_realserver_close_rs_endpoint;
            
    };

    class     virtualservice_element{
        public:
            virtualservice_element(){};
            ~virtualservice_element(){};
            boost::asio::ip::tcp::endpoint
                            sorry_endpoint;
    };
    
    // Dummy virtualservice_tcp Class
    class    virtualservice_tcp{
        public:
            virtualservice_tcp(){
                get_protocol_module_res = NULL;
            };
            ~virtualservice_tcp(){};
            
            virtualservice_element my_element;

            virtualservice_element& get_element(){
                return my_element;
            };
            
            unsigned long long            get_qos_upstream(){
                return get_qos_upstream_res;
            };
            unsigned long long get_qos_upstream_res;
            unsigned long long            get_qos_downstream(){
                return get_qos_downstream_res;
            };
            unsigned long long get_qos_downstream_res;
            unsigned long long            get_throughput_upstream(){
                return get_throughput_upstream_res;
            };
            unsigned long long get_throughput_upstream_res;
            unsigned long long            get_throughput_downstream(){
                return get_throughput_downstream_res;
            };
            unsigned long long get_throughput_downstream_res;
            void update_up_recv_size( unsigned long long size){
                update_up_recv_size_in = size;
            };
            unsigned long long update_up_recv_size_in;
            void update_up_send_size( unsigned long long size){
                update_up_send_size_in = size;
            };
            unsigned long long update_up_send_size_in;
            void update_down_recv_size( unsigned long long size){
                update_down_recv_size_in = size;
            };
            unsigned long long update_down_recv_size_in;
            void update_down_send_size( unsigned long long size){
                update_down_send_size_in = size;
            };
            unsigned long long update_down_send_size_in;
            
            protocol_module_base* get_protocol_module(){
                return get_protocol_module_res;
            };
            protocol_module_base* get_protocol_module_res;
            
            void    release_session( const tcp_session* session_ptr ){
                release_session_ptr = const_cast<tcp_session*>(session_ptr);
            };
            tcp_session*        release_session_ptr;
            
            void connection_active( const boost::asio::ip::tcp::endpoint& active_endpoint){
                connection_active_list.push_back(active_endpoint);
            };
            std::list<boost::asio::ip::tcp::endpoint> connection_active_list;
            
            void connection_inactive( const boost::asio::ip::tcp::endpoint& inactive_endpoint){
                connection_inactive_list.push_back(inactive_endpoint);
            };
            std::list<boost::asio::ip::tcp::endpoint> connection_inactive_list;

            unsigned long long get_wait_upstream(){
                return get_wait_upstream_res;
            }
            unsigned long long get_wait_upstream_res;

            unsigned long long get_wait_downstream(){
                return get_wait_downstream_res;
            }
            unsigned long long get_wait_downstream_res;

    };
    
    // Dummy tcp_socket Class
    class    tcp_socket{
        public:
            static bool connect_res;
            static boost::asio::ip::tcp::endpoint connect_connect_endpoint;
            static boost::system::error_code* connect_ec;
            static bool connect_call_check;
            static bool set_non_blocking_mode_res;
            static boost::system::error_code set_non_blocking_mode_ec;
            static bool is_connect;
            
            
//            //! tcp_socket_option
//            struct tcp_socket_option_info{
//                //! TCP_NODELAY   (false:not set,true:set option)
//                bool nodelay_opt;
//                //! TCP_NODELAY option value  (false:off,true:on)
//                bool nodelay_val;
//                //! TCP_CORK      (false:not set,true:set option)
//                bool cork_opt;
//                //! TCP_CORK option value     (false:off,true:on)
//                bool cork_val;
//                //! TCP_QUICKACK  (false:not set,true:set option)
//                bool quickack_opt;
//                //! TCP_QUICKACK option value (false:off,true:on)
//                bool quickack_val;
//            };
            
            tcp_socket(boost::asio::io_service& io) : 
                    my_socket(io){
                opt_info.nodelay_opt = false;
                opt_info.cork_opt = false;
                opt_info.quickack_opt = false;
            };
            
            
            tcp_socket(boost::asio::io_service& io,const tcp_socket_option_info set_option) : 
                my_socket(io),
                opt_info(set_option){
            };
            ~tcp_socket(){};
            
            boost::asio::ip::tcp::socket& get_socket(){
                return my_socket;
            };
            
            boost::asio::ip::tcp::socket my_socket;
            tcp_socket_option_info opt_info;
            
            //! accept
            void accept(){};
            
            //! @param[in]        connect_endpoint is connection endpoint
            //! @param[out]        ec is reference error code object
            bool connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec){
                tcp_socket::connect_call_check = true;
                tcp_socket::connect_connect_endpoint = connect_endpoint;
                tcp_socket::connect_ec = &ec;
                if(tcp_socket::is_connect){
                    my_socket.connect(connect_endpoint,ec);
                }
                return tcp_socket::connect_res;
            };
            


            //! close socket
            //! @param[out]        ec is reference error code object
            //! @return         true is socket close
            //! @return         false is not open socket
            bool close(boost::system::error_code& ec){
                close_call_check = true;
                ec = close_out_ec;
                return close_res;
            };
            
            bool close_res;
            boost::system::error_code close_out_ec;
            bool close_call_check;
            
            //! set non blocking mode of the socket 
            //! @return            ec is reference error code object
            bool set_non_blocking_mode(boost::system::error_code& ec){
                ec = tcp_socket::set_non_blocking_mode_ec;
                return tcp_socket::set_non_blocking_mode_res;
            };
            
            //! write socket
            //! @param[in]        buffers is wite data buffer
            //! @param[out]        ec is reference error code object
            //! @return            write data size
            std::size_t write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
                write_some_call_check = true;
                write_some_buffers_size_in = boost::asio::buffer_size(*buffers.begin());
                write_some_buffers_in = boost::asio::detail::buffer_cast_helper(*buffers.begin());
                ec = write_some_ec;
                return write_some_res;
            };
            
            std::size_t write_some_res;
            boost::system::error_code write_some_ec;
            void* write_some_buffers_in;
            std::size_t write_some_buffers_size_in;
            bool write_some_call_check;
            
            //! read socket
            //! @param[out]        buffers is read data buffer
            //! @param[out]        ec is reference error code object
            //! @return            read data size
            std::size_t read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
                read_some_call_check = true;
                read_some_buffers_size_in = boost::asio::buffer_size(*buffers.begin());
                boost::array<char,MAX_BUFFER_SIZE>* pBuf = (boost::array<char,MAX_BUFFER_SIZE>*)boost::asio::detail::buffer_cast_helper(*buffers.begin());
                for(int i = 0 ; i < (int)read_some_buffers_size_in ;i++){
                    if(i >= MAX_BUFFER_SIZE) break;
                    (*pBuf)[i] = read_some_buffers_out[i];
                }
                ec = read_some_ec;
                return read_some_res;
            };
            
            std::size_t read_some_res;
            boost::system::error_code read_some_ec;
            boost::array<char,MAX_BUFFER_SIZE> read_some_buffers_out;
            std::size_t read_some_buffers_size_in;
            bool read_some_call_check;

            bool is_open(){
                return is_open_res;
            };
            bool is_open_res;
    };
    
    bool tcp_socket::connect_res;
    boost::asio::ip::tcp::endpoint tcp_socket::connect_connect_endpoint;
    boost::system::error_code* tcp_socket::connect_ec;
    bool tcp_socket::connect_call_check;
    bool tcp_socket::set_non_blocking_mode_res = true;;
    boost::system::error_code tcp_socket::set_non_blocking_mode_ec;
    bool tcp_socket::is_connect = false;

    // Dummy tcp_ssl_socket Class
    class    tcp_ssl_socket{
        public:
            static bool set_non_blocking_mode_res;
            static boost::system::error_code set_non_blocking_mode_ec;
            static bool is_connect;

            boost::asio::ssl::stream<boost::asio::ip::tcp::socket> my_socket;
            tcp_socket_option_info opt_info;

            tcp_ssl_socket(boost::asio::io_service& io,boost::asio::ssl::context& context,const tcp_socket_option_info set_option) :
                my_socket(io,context),
                opt_info(set_option){
            };
            ~tcp_ssl_socket(){};

            boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& get_socket(){
                return my_socket;
            };

            //! close socket
            //! @param[out]        ec is reference error code object
            //! @return         true is socket close
            //! @return         false is not open socket
            bool close(boost::system::error_code& ec){
                close_call_check = true;
                ec = close_out_ec;
                return close_res;
            };

            bool close_res;
            boost::system::error_code close_out_ec;
            bool close_call_check;

            //! set non blocking mode of the socket
            //! @return            ec is reference error code object
            bool set_non_blocking_mode(boost::system::error_code& ec){
                ec = tcp_socket::set_non_blocking_mode_ec;
                return tcp_socket::set_non_blocking_mode_res;
            };


            //! accept
            bool accept(){
                tcp_ssl_socket::accept_call_check = true;
                return tcp_ssl_socket::accept_res;
            };
            bool accept_res;
            bool accept_call_check;
        

            //! write socket
            //! @param[in]        buffers is wite data buffer
            //! @param[out]        ec is reference error code object
            //! @return            write data size
            std::size_t write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
                write_some_call_check = true;
                write_some_buffers_size_in = boost::asio::buffer_size(*buffers.begin());
                write_some_buffers_in = boost::asio::detail::buffer_cast_helper(*buffers.begin());
                ec = write_some_ec;
                return write_some_res;
            };

            std::size_t write_some_res;
            boost::system::error_code write_some_ec;
            void* write_some_buffers_in;
            std::size_t write_some_buffers_size_in;
            bool write_some_call_check;

            //! read socket
            //! @param[out]        buffers is read data buffer
            //! @param[out]        ec is reference error code object
            //! @return            read data size
            std::size_t read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
                read_some_call_check = true;
                read_some_buffers_size_in = boost::asio::buffer_size(*buffers.begin());
                boost::array<char,MAX_BUFFER_SIZE>* pBuf = (boost::array<char,MAX_BUFFER_SIZE>*)boost::asio::detail::buffer_cast_helper(*buffers.begin());
                for(int i = 0 ; i < (int)read_some_buffers_size_in ;i++){
                    if(i >= MAX_BUFFER_SIZE) break;
                    (*pBuf)[i] = read_some_buffers_out[i];
                }
                ec = read_some_ec;
                return read_some_res;
            };

            std::size_t read_some_res;
            boost::system::error_code read_some_ec;
            boost::array<char,MAX_BUFFER_SIZE> read_some_buffers_out;
            std::size_t read_some_buffers_size_in;
            bool read_some_call_check;

            bool is_open(){
                return is_open_res;
            };
            bool is_open_res;


        //! handshake socket
        //! @param[in]        handshake_type is handshaking as a server or client
        //! @return         true is handshaked
        //! @return         false is handshake failure
        bool handshake(boost::asio::ssl::stream_base::handshake_type type){
		return true;
        }
    };

    bool tcp_ssl_socket::set_non_blocking_mode_res = true;
    boost::system::error_code tcp_ssl_socket::set_non_blocking_mode_ec;
    bool tcp_ssl_socket::is_connect = false;

}
