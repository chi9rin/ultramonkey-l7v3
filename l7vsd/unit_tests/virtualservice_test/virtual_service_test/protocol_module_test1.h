#ifndef    PROTOCOL_MODULE_TEST1
#define    PROTOCOL_MODULE_TEST1

#include    "protocol_module_base.h"

#include "stub.h"

namespace l7vs{

class    protocol_module_test1 : public protocol_module_base{
public:
    protocol_module_test1() : protocol_module_base( "PMtest1" ) {
        std::cout << "PMtest1 create" << std::endl;
    }
    ~protocol_module_test1(){
        std::cout << "PMtest1 destroy" << std::endl;
    }

    void    initialize(
                            rs_list_itr_func_type        inlist_begin,
                            rs_list_itr_func_type        inlist_end,
                            rs_list_itr_next_func_type    inlist_next,
                            boost::function< void( void ) >    inlist_lock,
                            boost::function< void( void ) >    inlist_unlock ){}

    void    finalize(){}

    bool    is_tcp(){ return true; }
    bool    is_udp(){ return false; }
    bool    is_use_sorry(){ return true; }
    check_message_result    check_parameter( const std::vector<std::string>& args ){
        protocol_module_base::check_message_result    result;
        result.flag = true;
        std::cout << "ProtocolModule check parameter called." << std::endl;
        return result;
    }

    void    handle_rslist_update(){}

    check_message_result    set_parameter( const std::vector<std::string>& args ){
        protocol_module_base::check_message_result    result;
        result.flag = true;
        std::cout << "ProtocolModule set parameter called." << std::endl;
        return result;
    }
    check_message_result    add_parameter( const std::vector<std::string>& args ){
        protocol_module_base::check_message_result    result;
        result.flag = false;
        std::cout << "ProtocolModule add parameter called." << std::endl;
        return result;
    }

    void    register_schedule( tcp_schedule_func_type inschedule ){}

    void    register_replication_area_lock(
                                    boost::function< void( void ) > inlock_func ){}

    void    register_replication_area_unlock(
                                    boost::function< void( void ) > inunlock_func ){}

    void    replication_interrupt(){
        debugg_flug_struct::getInstance().pm_rep_count_inc();
        std::cout << "Protocol Module Replication." << std::endl;
    }

    //use in upstream_thread
    EVENT_TAG    handle_session_initialize(
                                    const boost::thread::id up_thread_id,
                                    const boost::thread::id down_thread_id,
                                    const boost::asio::ip::tcp::endpoint& client_endpoint_tcp,
                                    const boost::asio::ip::udp::endpoint& client_endpoint_udp ){return STOP;}

    EVENT_TAG    handle_session_finalize(
                                    const boost::thread::id up_thread_id,
                                    const boost::thread::id down_thread_id ){return STOP;}

    EVENT_TAG    handle_accept( const boost::thread::id thread_id ){return STOP;}

    EVENT_TAG    handle_client_recv(
                                    const boost::thread::id thread_id,
                                    const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                    const size_t recvlen ){return STOP;}

    EVENT_TAG    handle_realserver_select(
                                    const boost::thread::id thread_id,
                                    boost::asio::ip::tcp::endpoint& rs_endpoint ){return STOP;}

    EVENT_TAG    handle_realserver_connect(
                                    const boost::thread::id thread_id,
                                    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                    size_t& datalen ){return STOP;}
    
    EVENT_TAG    handle_realserver_connection_fail(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint& rs_endpoint ){return STOP;}
    
    EVENT_TAG    handle_realserver_send( const boost::thread::id thread_id ){return STOP;}
    
    EVENT_TAG    handle_sorryserver_select(
                                    const boost::thread::id thread_id,
                                    boost::asio::ip::tcp::endpoint & sorry_endpoint ){return STOP;}

    EVENT_TAG    handle_sorryserver_connect(
                                    const boost::thread::id thread_id,
                                    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                    size_t& datalen ){return STOP;}

    EVENT_TAG    handle_sorryserver_connection_fail(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint& sorry_endpoint ){return STOP;}
    
    EVENT_TAG    handle_sorryserver_send( const boost::thread::id thread_id ){return STOP;}

    //use in downstream_thread
    EVENT_TAG    handle_realserver_recv(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint & rs_endpoint,
                                    const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                    const size_t recvlen ){return STOP;}
    
    EVENT_TAG    handle_sorryserver_recv(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint & sorry_endpoint,
                                    const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
                                    const size_t recvlen ){return STOP;}
    
    EVENT_TAG    handle_response_send_inform( const boost::thread::id thread_id ){return STOP;}

    EVENT_TAG    handle_client_connection_check(
                                    const boost::thread::id thread_id,
                                    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                    size_t& datalen ){return STOP;}

    EVENT_TAG    handle_client_send( const boost::thread::id thread_id ){return STOP;}

    //use in upstream/downstream thread
    EVENT_TAG    handle_client_disconnect( const boost::thread::id thread_id ){return STOP;}
    
    EVENT_TAG    handle_sorry_enable( const boost::thread::id upthread_id ){return STOP;}

    EVENT_TAG    handle_sorry_disable( const boost::thread::id upthread_id ){return STOP;}

    EVENT_TAG    handle_realserver_disconnect(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint & rs_endpoint ){return STOP;}
    
    EVENT_TAG    handle_sorryserver_disconnect(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::tcp::endpoint & sorry_endpoint ){return STOP;}

    //unuse method
    void    register_schedule( udp_schedule_func_type inschedule ){}
    EVENT_TAG    handle_realserver_select(
                                    const boost::thread::id thread_id,
                                    boost::asio::ip::udp::endpoint& rs_endpoint,
                                    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                    size_t& datalen ){return STOP;}
    EVENT_TAG    handle_realserver_recv(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::udp::endpoint & rs_endpoint,
                                    const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
                                    const size_t recvlen ){return STOP;}
    EVENT_TAG    handle_client_select(
                                    const boost::thread::id thread_id,
                                    boost::asio::ip::udp::endpoint& rs_endpoint,
                                    boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                    size_t& datalen ){return STOP;}
    EVENT_TAG    handle_realserver_close(
                                    const boost::thread::id thread_id,
                                    const boost::asio::ip::udp::endpoint & rs_endpoint ){return STOP;};
};

}

#endif
