#define    TEST_CASE

#include <boost/test/included/unit_test.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "protocol_module_cinsert.h"

using namespace boost::unit_test_framework;
using namespace l7vs;

#define    THREAD_LOOP_NUM    (1)

//--stub functions--
void    thread_method()
{
}

LOG_LEVEL_TAG    stb_getloglevel(){
//    std::cout << "getloglevel called." << std::endl;
    return LOG_LV_NONE;
}
void    stb_putLogFatal( const unsigned int, const std::string&, const char*, int ){
//    std::cout << "putLogFatal called." << std::endl;
}
void    stb_putLogError( const unsigned int, const std::string&, const char*, int ){
//    std::cout << "putLogError called." << std::endl;
}
void    stb_putLogWarn( const unsigned int, const std::string&, const char*, int ){
//    std::cout << "putLogWarn called." << std::endl;
}
void    stb_putLogInfo( const unsigned int, const std::string&, const char*, int ){
//    std::cout << "putLogInfo called." << std::endl;
}
void    stb_putLogDebug( const unsigned int, const std::string&, const char*, int ){
//    std::cout << "putLogDebug called." << std::endl;
}
void*    stb_replication_pay_memory( const std::string&, unsigned int* ){
//    std::cout << "replication_pay_memory called." << std::endl;
    return NULL;
}
void    stb_replication_area_lock(){
//    std::cout << "replication_area_lock called." << std::endl;
}
void    stb_replication_area_unlock(){
//    std::cout << "replication_area_unlock called." << std::endl;
}

protocol_module_cinsert::realserverlist_type::iterator stb_rslist_begin() {
    return protocol_module_cinsert::realserverlist_type::iterator();
}
protocol_module_cinsert::realserverlist_type::iterator stb_rslist_end() {
    return protocol_module_cinsert::realserverlist_type::iterator();
}
protocol_module_cinsert::realserverlist_type::iterator stb_rslist_next(
        protocol_module_cinsert::realserverlist_type::iterator iter) {
    return protocol_module_cinsert::realserverlist_type::iterator();
}
void stb_rslist_lock() {
}
void stb_rslist_unlock() {
}
void stb_schedule_tcp( const boost::thread::id id,
                   protocol_module_base::rs_list_itr_func_type func_type1,
                   protocol_module_base::rs_list_itr_func_type func_type2,
                   protocol_module_base::rs_list_itr_next_func_type func_type3,
                   boost::asio::ip::tcp::endpoint& ) {
}
void stb_schedule_udp( const boost::thread::id id,
                   protocol_module_base::rs_list_itr_func_type func_type1,
                   protocol_module_base::rs_list_itr_func_type func_type2,
                   protocol_module_base::rs_list_itr_next_func_type func_type3,
                   boost::asio::ip::udp::endpoint& ) {
}

//--test class--
class    protocol_module_cinsert_test : public protocol_module_cinsert
{
public:
boost::mutex        sync_mutex;
boost::condition    sync_condition;
boost::condition    session_thread_data_map_condition;

protocol_module_cinsert_test() : protocol_module_cinsert()
{
    getloglevel    = &stb_getloglevel;
    putLogFatal    = &stb_putLogFatal;
    putLogError    = &stb_putLogError;
    putLogWarn    = &stb_putLogWarn;
    putLogInfo    = &stb_putLogInfo;
    putLogDebug    = &stb_putLogDebug;
}
~protocol_module_cinsert_test(){}
//-------------------------------------------------------------------
void    session_thread_data_map_lock()
{
    boost::mutex::scoped_lock    lk( session_thread_data_map_mutex );
    session_thread_data_map_condition.wait( lk );
}
//-------------------------------------------------------------------
void    session_thread_data_set()
{

    int    i = 0;
    int    j = 0;
    size_t    recive_data_num = 0;

    boost::thread    up_thread_1( &thread_method );
    boost::thread    up_thread_2( &thread_method );

    boost::thread    down_thread_1( &thread_method );
    boost::thread    down_thread_2( &thread_method );

    boost::asio::ip::address    address;

    boost::asio::ip::tcp::endpoint client_endpoint_tcp_1( boost::asio::ip::address::from_string("10.10.10.10"), 1111 );
    boost::asio::ip::tcp::endpoint client_endpoint_tcp_2( boost::asio::ip::address::from_string("10.10.10.10"), 2222 );
    boost::asio::ip::tcp::endpoint client_endpoint_tcp_3( boost::asio::ip::address::from_string("10.10.10.10"), 3333 );

    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_1( boost::asio::ip::address::from_string("20.20.20.20"), 1111 );
    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_2( boost::asio::ip::address::from_string("20.20.20.20"), 2222 );
    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_3( boost::asio::ip::address::from_string("20.20.20.20"), 3333 );
    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_4( boost::asio::ip::address::from_string("20.20.20.20"), 4444 );
    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_5( boost::asio::ip::address::from_string("20.20.20.20"), 5555 );
    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_6( boost::asio::ip::address::from_string("20.20.20.20"), 6666 );
    boost::asio::ip::tcp::endpoint realserver_endpoint_tcp_7( boost::asio::ip::address::from_string("20.20.20.20"), 7777 );

    t_session_thread_data_map_itr        thread_data_itr;
    t_recive_data_map_itr                recive_data_itr;

    t_session_thread_data_cinsert    up_thread_data_1( new session_thread_data_cinsert );
    t_session_thread_data_cinsert    up_thread_data_2( new session_thread_data_cinsert );
    t_session_thread_data_cinsert    down_thread_data_1( new session_thread_data_cinsert );
    t_session_thread_data_cinsert    down_thread_data_2( new session_thread_data_cinsert );

    recive_data                        client_recv_data_1;
    recive_data                        client_recv_data_2;
    recive_data                        client_recv_data_3;
    recive_data                        realserver_recv_data_1;
    recive_data                        realserver_recv_data_2;
    recive_data                        realserver_recv_data_3;
    recive_data                        realserver_recv_data_4;
    recive_data                        realserver_recv_data_5;
    recive_data                        realserver_recv_data_6;
    recive_data                        realserver_recv_data_7;
    char*                            buffer                = NULL;

    //----------------------------------------------------------------------
//    up_thread_data_1 = new session_thread_data_cinsert;

    up_thread_data_1->thread_id                = up_thread_1.get_id();
    up_thread_data_1->thread_division        = THREAD_DIVISION_UP_STREAM;
    up_thread_data_1->pair_thread_id        = down_thread_1.get_id();
    up_thread_data_1->client_endpoint_tcp    = client_endpoint_tcp_1;

    buffer = (char*)malloc( client_recv_data_1.recive_buffer_max_size );

    client_recv_data_1.recive_buffer    = buffer;
    client_recv_data_1.recive_buffer_1    = buffer;

    buffer = (char*)malloc( client_recv_data_1.recive_buffer_max_size );

    client_recv_data_1.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_1;
    send_status_1.status = SEND_OK;
    client_recv_data_1.send_status_list.push_back(send_status_1);
//////////////debug

    up_thread_data_1->recive_data_map[ client_endpoint_tcp_1 ] = client_recv_data_1;

    buffer = (char*)malloc( client_recv_data_2.recive_buffer_max_size );

    client_recv_data_2.recive_buffer    = buffer;
    client_recv_data_2.recive_buffer_1    = buffer;

    buffer = (char*)malloc( client_recv_data_2.recive_buffer_max_size );

    client_recv_data_2.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_2;
    send_status_2.status = SEND_OK;
    client_recv_data_2.send_status_list.push_back(send_status_2);
//////////////debug

    up_thread_data_1->recive_data_map[ client_endpoint_tcp_2 ] = client_recv_data_2;

    session_thread_data_map[ up_thread_1.get_id() ]    = up_thread_data_1;

    //----------------------------------------------------------------------
//    up_thread_data_2 = new session_thread_data_cinsert;

    up_thread_data_2->thread_id                = up_thread_2.get_id();
    up_thread_data_2->thread_division        = THREAD_DIVISION_UP_STREAM;
    up_thread_data_2->pair_thread_id        = down_thread_2.get_id();
    up_thread_data_2->client_endpoint_tcp    = client_endpoint_tcp_3;

    buffer = (char*)malloc( client_recv_data_3.recive_buffer_max_size );

    client_recv_data_3.recive_buffer    = buffer;
    client_recv_data_3.recive_buffer_1    = buffer;

    buffer = (char*)malloc( client_recv_data_3.recive_buffer_max_size );

    client_recv_data_3.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_3;
    send_status_3.status = SEND_OK;
    client_recv_data_3.send_status_list.push_back(send_status_3);
//////////////debug

    up_thread_data_2->recive_data_map[ client_endpoint_tcp_3 ] = client_recv_data_3;

    session_thread_data_map[ up_thread_2.get_id() ]    = up_thread_data_2;

    //----------------------------------------------------------------------
//    down_thread_data_1 = new session_thread_data_cinsert;

    down_thread_data_1->thread_id            = down_thread_1.get_id();
    down_thread_data_1->thread_division        = THREAD_DIVISION_DOWN_STREAM;
    down_thread_data_1->pair_thread_id        = up_thread_1.get_id();
#if 0
    down_thread_data_1->client_endpoint_tcp    = client_endpoint_tcp_1;
#else
    down_thread_data_1->client_endpoint_tcp    = realserver_endpoint_tcp_1;
#endif
    buffer = (char*)malloc( realserver_recv_data_1.recive_buffer_max_size );

    realserver_recv_data_1.recive_buffer    = buffer;
    realserver_recv_data_1.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_1.recive_buffer_max_size );

    realserver_recv_data_1.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_4;
    send_status_4.status = SEND_OK;
    realserver_recv_data_1.send_status_list.push_back(send_status_4);
//////////////debug

    down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_1 ] = realserver_recv_data_1;

    buffer = (char*)malloc( realserver_recv_data_2.recive_buffer_max_size );

    realserver_recv_data_2.recive_buffer    = buffer;
    realserver_recv_data_2.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_2.recive_buffer_max_size );

    realserver_recv_data_2.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_5;
    send_status_5.status = SEND_OK;
    realserver_recv_data_2.send_status_list.push_back(send_status_5);
//////////////debug

    down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_2 ] = realserver_recv_data_2;

    buffer = (char*)malloc( realserver_recv_data_3.recive_buffer_max_size );

    realserver_recv_data_3.recive_buffer    = buffer;
    realserver_recv_data_3.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_3.recive_buffer_max_size );

    realserver_recv_data_3.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_6;
    send_status_6.status = SEND_OK;
    realserver_recv_data_3.send_status_list.push_back(send_status_6);
//////////////debug

    down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_3 ] = realserver_recv_data_3;

    buffer = (char*)malloc( realserver_recv_data_4.recive_buffer_max_size );

    realserver_recv_data_4.recive_buffer    = buffer;
    realserver_recv_data_4.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_4.recive_buffer_max_size );

    realserver_recv_data_4.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_7;
    send_status_7.status = SEND_OK;
    realserver_recv_data_4.send_status_list.push_back(send_status_7);
//////////////debug

    down_thread_data_1->recive_data_map[ realserver_endpoint_tcp_4 ] = realserver_recv_data_4;

    session_thread_data_map[ down_thread_1.get_id() ]    = down_thread_data_1;

    //----------------------------------------------------------------------
//    down_thread_data_2 = new session_thread_data_cinsert;

    down_thread_data_2->thread_id            = down_thread_2.get_id();
    down_thread_data_2->thread_division        = THREAD_DIVISION_DOWN_STREAM;
    down_thread_data_2->pair_thread_id        = up_thread_2.get_id();
#if 0
    down_thread_data_2->client_endpoint_tcp    = client_endpoint_tcp_3;
#else
    down_thread_data_2->client_endpoint_tcp    = realserver_endpoint_tcp_5;
#endif
    buffer = (char*)malloc( realserver_recv_data_5.recive_buffer_max_size );

    realserver_recv_data_5.recive_buffer    = buffer;
    realserver_recv_data_5.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_5.recive_buffer_max_size );

    realserver_recv_data_5.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_8;
    send_status_8.status = SEND_OK;
    realserver_recv_data_5.send_status_list.push_back(send_status_8);
//////////////debug

    down_thread_data_2->recive_data_map[ realserver_endpoint_tcp_5 ] = realserver_recv_data_5;

    buffer = (char*)malloc( realserver_recv_data_6.recive_buffer_max_size );

    realserver_recv_data_6.recive_buffer    = buffer;
    realserver_recv_data_6.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_6.recive_buffer_max_size );

    realserver_recv_data_6.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_9;
    send_status_9.status = SEND_OK;
    realserver_recv_data_6.send_status_list.push_back(send_status_9);
//////////////debug

    down_thread_data_2->recive_data_map[ realserver_endpoint_tcp_6 ] = realserver_recv_data_6;

    buffer = (char*)malloc( realserver_recv_data_7.recive_buffer_max_size );

    realserver_recv_data_7.recive_buffer        = buffer;
    realserver_recv_data_7.recive_buffer_1    = buffer;

    buffer = (char*)malloc( realserver_recv_data_7.recive_buffer_max_size );

    realserver_recv_data_7.recive_buffer_2    = buffer;
//////////////debug
    send_status send_status_10;
    send_status_10.status = SEND_OK;
    realserver_recv_data_7.send_status_list.push_back(send_status_10);
//////////////debug

    down_thread_data_2->recive_data_map[ realserver_endpoint_tcp_7 ] = realserver_recv_data_7;

    session_thread_data_map[ down_thread_2.get_id() ]    = down_thread_data_2;

    //----------------------------------------------------------------------

    client_recv_data_1.recive_buffer_1        = NULL;
    client_recv_data_1.recive_buffer_2        = NULL;
    client_recv_data_2.recive_buffer_1        = NULL;
    client_recv_data_2.recive_buffer_2        = NULL;
    client_recv_data_3.recive_buffer_1        = NULL;
    client_recv_data_3.recive_buffer_2        = NULL;
    realserver_recv_data_1.recive_buffer_1    = NULL;
    realserver_recv_data_1.recive_buffer_2    = NULL;
    realserver_recv_data_2.recive_buffer_1    = NULL;
    realserver_recv_data_2.recive_buffer_2    = NULL;
    realserver_recv_data_3.recive_buffer_1    = NULL;
    realserver_recv_data_3.recive_buffer_2    = NULL;
    realserver_recv_data_4.recive_buffer_1    = NULL;
    realserver_recv_data_4.recive_buffer_2    = NULL;
    realserver_recv_data_5.recive_buffer_1    = NULL;
    realserver_recv_data_5.recive_buffer_2    = NULL;
    realserver_recv_data_6.recive_buffer_1    = NULL;
    realserver_recv_data_6.recive_buffer_2    = NULL;
    realserver_recv_data_7.recive_buffer_1    = NULL;
    realserver_recv_data_7.recive_buffer_2    = NULL;

#if 1
    i = 0;
    j = 0;
    recive_data_num = 0;
#else
    thread_data_itr = session_thread_data_map.begin();

    std::cout    << "session_thread_data_map entry = ["
                << session_thread_data_map.size() << "]" << std::endl;

    std::cout << std::endl;

    i = 0;
    while( thread_data_itr != session_thread_data_map.end() )
    {
        i++;

        std::cout    << "<< thread_data[" << i << "] >>" << std::endl;
        std::cout    << "key                     = ["
                    << thread_data_itr->first << "]" << std::endl;
        std::cout    << "thread_id               = ["
                    << thread_data_itr->second->thread_id << "]" << std::endl;
        std::cout    << "thread_division         = ["
                    << thread_data_itr->second->thread_division << "]" << std::endl;
        std::cout    << "pair_thread_id          = ["
                    << thread_data_itr->second->pair_thread_id << "]" << std::endl;
        std::cout    << "end_flag                = ["
                    << thread_data_itr->second->end_flag << "]" << std::endl;
        std::cout    << "accept_end_flag         = ["
                    << thread_data_itr->second->accept_end_flag << "]" << std::endl;
        std::cout    << "sorry_flag              = ["
                    << thread_data_itr->second->sorry_flag << "]" << std::endl;
        std::cout    << "sorryserver_switch_flag = ["
                    << thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
        std::cout    << "realserver_switch_flag  = ["
                    << thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
        std::cout    << "client_endpoint_tcp     = ["
                    << thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
        std::cout    << "recive_data_map entry = ["
                    << thread_data_itr->second->recive_data_map.size() << "]" << std::endl;
        std::cout << std::endl;

        recive_data_num = 0;

        if( thread_data_itr->first == up_thread_1.get_id() )
        {
            recive_data_num = 2;
        }
        else if( thread_data_itr->first == up_thread_2.get_id() )
        {
            recive_data_num = 1;
        }
        else if( thread_data_itr->first == down_thread_1.get_id() )
        {
            recive_data_num = 4;
        }
        else if( thread_data_itr->first == down_thread_2.get_id() )
        {
            recive_data_num = 3;
        }

        recive_data_itr = thread_data_itr->second->recive_data_map.begin();

        j = 0;

        while( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
        {
            j++;

            std::cout    << "< recive_data[" << j << "] >" << std::endl;
            std::cout    << "key                     = ["
                        << recive_data_itr->first << "]" << std::endl;
            std::cout    << "recive_buffer_max_size  = ["
                        << recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
            std::cout    << "recive_buffer_rest_size = ["
                        << recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
            std::cout    << "recive_buffer           = ["
                        << (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
            std::cout    << "recive_buffer_1         = ["
                        << (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
            std::cout    << "recive_buffer_2         = ["
                        << (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
            std::cout << std::endl;

            recive_data_itr++;

        }

        thread_data_itr++;

    }

    std::cout << std::endl;
#endif

}

void    session_thread_data_disp()
{

    int    i = 0;
    int    j = 0;
    int    k = 0;
    int    l = 0;

    t_session_thread_data_map_itr        thread_data_itr;
    t_recive_data_map_itr                recive_data_itr;
    t_send_status_list_itr                send_status_itr;
    t_edit_data_list_itr                edit_data_itr;

std::cout    << "session_thread_data_disp" << std::endl;

    thread_data_itr = session_thread_data_map.begin();

    std::cout    << "session_thread_data_map entry = ["
                << session_thread_data_map.size() << "]" << std::endl;

    std::cout << std::endl;

    i = 0;

    while( thread_data_itr != session_thread_data_map.end() )
    {
        i++;

        std::cout    << "<< thread_data[" << i << "] >>" << std::endl;
        std::cout    << "key                     = ["
                    << thread_data_itr->first << "]" << std::endl;
        std::cout    << "thread_id               = ["
                    << thread_data_itr->second->thread_id << "]" << std::endl;
        std::cout    << "thread_division         = ["
                    << thread_data_itr->second->thread_division << "]" << std::endl;
        std::cout    << "pair_thread_id          = ["
                    << thread_data_itr->second->pair_thread_id << "]" << std::endl;
        std::cout    << "end_flag                = ["
                    << thread_data_itr->second->end_flag << "]" << std::endl;
        std::cout    << "accept_end_flag         = ["
                    << thread_data_itr->second->accept_end_flag << "]" << std::endl;
        std::cout    << "sorry_flag              = ["
                    << thread_data_itr->second->sorry_flag << "]" << std::endl;
        std::cout    << "sorryserver_switch_flag = ["
                    << thread_data_itr->second->sorryserver_switch_flag << "]" << std::endl;
        std::cout    << "realserver_switch_flag  = ["
                    << thread_data_itr->second->realserver_switch_flag << "]" << std::endl;
        std::cout    << "client_endpoint_tcp     = ["
                    << thread_data_itr->second->client_endpoint_tcp << "]" << std::endl;
        std::cout    << "recive_data_map entry = ["
                    << thread_data_itr->second->recive_data_map.size() << "]" << std::endl;
        std::cout << std::endl;

        recive_data_itr = thread_data_itr->second->recive_data_map.begin();

        j = 0;

        while( recive_data_itr != thread_data_itr->second->recive_data_map.end() )
        {
            j++;

            std::cout    << "< recive_data[" << j << "] >" << std::endl;
            std::cout    << "key                     = ["
                        << recive_data_itr->first << "]" << std::endl;
            std::cout    << "recive_buffer_max_size  = ["
                        << recive_data_itr->second.recive_buffer_max_size << "]" << std::endl;
            std::cout    << "recive_buffer_rest_size = ["
                        << recive_data_itr->second.recive_buffer_rest_size << "]" << std::endl;
            std::cout    << "recive_buffer           = ["
                        << (void*)recive_data_itr->second.recive_buffer << "]" << std::endl;
            std::cout    << "recive_buffer_1         = ["
                        << (void*)recive_data_itr->second.recive_buffer_1 << "]" << std::endl;
            std::cout    << "recive_buffer_2         = ["
                        << (void*)recive_data_itr->second.recive_buffer_2 << "]" << std::endl;
            std::cout    << "send_status_list entry = ["
                        << recive_data_itr->second.send_status_list.size() << "]" << std::endl;
            std::cout << std::endl;

            send_status_itr = recive_data_itr->second.send_status_list.begin();
    
            k = 0;
    
            while( send_status_itr != recive_data_itr->second.send_status_list.end() )
            {
                k++;

                std::cout    << "< send_status[" << k << "] >" << std::endl;
                std::cout    << "status  = ["
                            << send_status_itr->status << "]" << std::endl;
                std::cout    << "send_end_size = ["
                            << send_status_itr->send_end_size << "]" << std::endl;
                std::cout    << "send_rest_size = ["
                            << send_status_itr->send_rest_size << "]" << std::endl;
                std::cout    << "send_possible_size = ["
                            << send_status_itr->send_possible_size << "]" << std::endl;
                std::cout    << "send_offset = ["
                            << send_status_itr->send_offset << "]" << std::endl;
                std::cout    << "unsend_size = ["
                            << send_status_itr->unsend_size << "]" << std::endl;
                std::cout    << "edit_division = ["
                            << send_status_itr->edit_division << "]" << std::endl;
                std::cout    << "send_endpoint = ["
                            << send_status_itr->send_endpoint << "]" << std::endl;
                std::cout    << "edit_data_list entry = ["
                            << send_status_itr->edit_data_list.size() << "]" << std::endl;
                std::cout << std::endl;

                edit_data_itr = send_status_itr->edit_data_list.begin();
        
                l = 0;
        
                while( edit_data_itr != send_status_itr->edit_data_list.end() )
                {
                    l++;
    
                    std::cout    << "< edit_data[" << l << "] >" << std::endl;
                    std::cout    << "data  = ["
                                << edit_data_itr->data << "]" << std::endl;
                    std::cout    << "data_size  = ["
                                << edit_data_itr->data_size << "]" << std::endl;
                    std::cout    << "insert_position  = ["
                                << edit_data_itr->insert_position << "]" << std::endl;
                    std::cout    << "replace_size  = ["
                                << edit_data_itr->replace_size << "]" << std::endl;
                    std::cout << std::endl;

                    edit_data_itr++;

                }

                send_status_itr++;

            }

            recive_data_itr++;

        }

        thread_data_itr++;

    }
    std::cout << std::endl;

}

void session_thread_data_erase()
{

    t_session_thread_data_map_itr        thread_data_itr;

    thread_data_itr = session_thread_data_map.begin();

std::cout    << "session_thread_data_erase" << std::endl;

session_thread_data_cinsert* thread_data;

thread_data = new session_thread_data_cinsert;

delete thread_data;

    while( thread_data_itr != session_thread_data_map.end() )
    {
        session_thread_data_map.erase(thread_data_itr);
        thread_data_itr = session_thread_data_map.begin();
    }

}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void    handle_client_disconnect_test()
{
    session_thread_data_set();

    protocol_module_cinsert::EVENT_TAG status;
    t_session_thread_data_map_itr    thread_data_itr;

////////////up thread case///////////////
    thread_data_itr = session_thread_data_map.begin();
    while( thread_data_itr != session_thread_data_map.end())
    {
        if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
        {
            break;
        }
        thread_data_itr++;
    }

    // up thread case 1
    status = handle_client_disconnect( thread_data_itr->second->thread_id );
    // unit_test[1] handle_client_disconnect check return value = FINALIZE(up thread case 1)
    BOOST_CHECK_EQUAL( status, FINALIZE );

////////////down thread case///////////////
    thread_data_itr = session_thread_data_map.begin();
    while( thread_data_itr != session_thread_data_map.end())
    {
        if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
        {
            break;
        }
        thread_data_itr++;
    }
    // down thread case 1
    status = handle_client_disconnect( thread_data_itr->second->thread_id );
    // unit_test[2] handle_client_disconnect check return value = FINALIZE(down thread case 1)
    BOOST_CHECK_EQUAL( status, FINALIZE );

}

void    handle_realserver_disconnect_test()
{
    session_thread_data_set();

    protocol_module_cinsert::EVENT_TAG status;
    t_session_thread_data_map_itr    thread_data_itr;
    t_recive_data_map_itr            recive_data_itr;
    t_send_status_list_itr            send_status_itr;

////////////up thread case///////////////
    thread_data_itr = session_thread_data_map.begin();
    while( thread_data_itr != session_thread_data_map.end())
    {
        if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
        {
            break;
        }
        thread_data_itr++;
    }
    // up thread case 1
    thread_data_itr->second->end_flag = END_FLAG_ON;
    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[3] handle_realserver_disconnect check return value = CLIENT_RECV(up thread case 1)
    BOOST_CHECK_EQUAL( status, CLIENT_RECV );

    // up thread case 2
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[4] handle_realserver_disconnect check return value = CLIENT_RECV(up thread case 2)
    BOOST_CHECK_EQUAL( status, CLIENT_RECV );
    // unit_test[5] handle_realserver_disconnect check end_flag = END_FLAG_ON(up thread case 2)
    BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );

    // up thread case 3
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[6] handle_realserver_disconnect check return value = SORRYSERVER_SELECT(up thread case 3)
    BOOST_CHECK_EQUAL( status, SORRYSERVER_SELECT );

    // up thread case 4
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
    recive_data_itr = thread_data_itr->second->recive_data_map.find( thread_data_itr->second->client_endpoint_tcp );
    if( recive_data_itr != thread_data_itr->second->recive_data_map.end() ){
        recive_data_itr->second.send_status_list.clear();
    }

    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[7] handle_realserver_disconnect check return value = CLIENT_RECV(up thread case 4)
    BOOST_CHECK_EQUAL( status, CLIENT_RECV );

    // up thread error case 1
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
    thread_data_itr->second->recive_data_map.clear();
    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[8] handle_realserver_disconnect check return value = STOP(up thread error case 1)
    BOOST_CHECK_EQUAL( status, STOP );

////////////down thread case///////////////
    thread_data_itr = session_thread_data_map.begin();
    while( thread_data_itr != session_thread_data_map.end())
    {
        if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
        {
            break;
        }
        thread_data_itr++;
    }
    // down thread case 1
    thread_data_itr->second->end_flag = END_FLAG_OFF;

    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[9] handle_realserver_disconnect check return value = CLIENT_CONNECTION_CHECK(down thread case 1)
    BOOST_CHECK_EQUAL( status, CLIENT_CONNECTION_CHECK );
    // unit_test[10] handle_realserver_disconnect check end_flag = END_FLAG_ON(down thread case 1)
    BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );

    // down thread case 2
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    recive_data_itr = thread_data_itr->second->recive_data_map.find( thread_data_itr->second->client_endpoint_tcp );
    while( recive_data_itr != thread_data_itr->second->recive_data_map.end() ){
        recive_data_itr->second.send_status_list.clear();
        recive_data_itr++;
    }

    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[11] handle_realserver_disconnect check return value = CLIENT_DISCONNECT(down thread case 2)
    BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
    // unit_test[12] handle_realserver_disconnect check end_flag = END_FLAG_ON(down thread case 2)
    BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );

}

void    handle_realserver_disconnect_test_thread( int thread_no, bool* ret )
{
    t_session_thread_data_map_itr    thread_data_itr;

    {
        boost::mutex::scoped_lock    lk( sync_mutex );
        std::cout << "handle_realserver_disconnect_test_thread start:" << thread_no << std::endl;//debug
        thread_data_itr = session_thread_data_map.begin();
        if( thread_no == 1 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr->second->end_flag = END_FLAG_OFF;
            thread_data_itr->second->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
        }
        else if( thread_no == 2 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr++;
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr->second->end_flag = END_FLAG_OFF;
            thread_data_itr->second->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
        }
        else if( thread_no == 3 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
        }
        else if( thread_no == 4 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr++;
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
        }

        sync_condition.wait( lk );
    }
    protocol_module_cinsert::EVENT_TAG status;
    status = handle_realserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );

    if( thread_no == 1 ){
        if( status == SORRYSERVER_SELECT )
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    else if( thread_no == 2 ){
        if( status == SORRYSERVER_SELECT )
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    else if( thread_no == 3 ){
        if( status == CLIENT_CONNECTION_CHECK)
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    else if( thread_no == 4 ){
        if( status == CLIENT_CONNECTION_CHECK)
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    std::cout << "handle_realserver_disconnect_test_thread end:" << thread_no << std::endl;//debug
}

void    handle_sorryserver_disconnect_test()
{
    session_thread_data_set();

    protocol_module_cinsert::EVENT_TAG status;
    t_session_thread_data_map_itr    thread_data_itr;
    t_recive_data_map_itr            recive_data_itr;
    t_send_status_list_itr            send_status_itr;

////////////up thread case///////////////
    thread_data_itr = session_thread_data_map.begin();
    while( thread_data_itr != session_thread_data_map.end())
    {
        if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
        {
            break;
        }
        thread_data_itr++;
    }
    // up thread case 1
    thread_data_itr->second->end_flag = END_FLAG_ON;
    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[13] handle_sorryserver_disconnect check return value = CLIENT_RECV(up thread case 1)
    BOOST_CHECK_EQUAL( status, CLIENT_RECV );

    // up thread case 2
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[14] handle_sorryserver_disconnect check return value = CLIENT_RECV(up thread case 2)
    BOOST_CHECK_EQUAL( status, CLIENT_RECV );
    // unit_test[15] handle_sorryserver_disconnect check end_flag = END_FLAG_ON(up thread case 2)
    BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );

    // up thread case 3
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[16] handle_sorryserver_disconnect check return value = REALSERVER_SELECT(up thread case 3)
    BOOST_CHECK_EQUAL( status, REALSERVER_SELECT );

    // up thread case 4
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
    recive_data_itr = thread_data_itr->second->recive_data_map.find( thread_data_itr->second->client_endpoint_tcp );
    if( recive_data_itr != thread_data_itr->second->recive_data_map.end() ){
        recive_data_itr->second.send_status_list.clear();
    }

    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[17] handle_sorryserver_disconnect check return value = CLIENT_RECV(up thread case 4)
    BOOST_CHECK_EQUAL( status, CLIENT_RECV );

    // up thread error case 1
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    thread_data_itr->second->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
    thread_data_itr->second->recive_data_map.clear();
    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[18] handle_sorryserver_disconnect check return value = STOP(up thread error case 1)
    BOOST_CHECK_EQUAL( status, STOP );

////////////down thread case///////////////
    thread_data_itr = session_thread_data_map.begin();
    while( thread_data_itr != session_thread_data_map.end())
    {
        if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
        {
            break;
        }
        thread_data_itr++;
    }
    // down thread case 1
    thread_data_itr->second->end_flag = END_FLAG_OFF;

    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[19] handle_sorryserver_disconnect check return value = CLIENT_CONNECTION_CHECK(down thread case 1)
    BOOST_CHECK_EQUAL( status, CLIENT_CONNECTION_CHECK );
    // unit_test[20] handle_sorryserver_disconnect check end_flag = END_FLAG_ON(down thread case 1)
    BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );

    // down thread case 2
    thread_data_itr->second->end_flag = END_FLAG_OFF;
    recive_data_itr = thread_data_itr->second->recive_data_map.find( thread_data_itr->second->client_endpoint_tcp );
    while( recive_data_itr != thread_data_itr->second->recive_data_map.end() ){
        recive_data_itr->second.send_status_list.clear();
        recive_data_itr++;
    }

    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    // unit_test[21] handle_sorryserver_disconnect check return value = CLIENT_DISCONNECT(down thread case 2)
    BOOST_CHECK_EQUAL( status, CLIENT_DISCONNECT );
    // unit_test[22] handle_sorryserver_disconnect check end_flag = END_FLAG_ON(down thread case 2)
    BOOST_CHECK( thread_data_itr->second->end_flag == END_FLAG_ON );
}

void    handle_sorryserver_disconnect_test_thread( int thread_no, bool* ret )
{
    t_session_thread_data_map_itr    thread_data_itr;

    {
        boost::mutex::scoped_lock    lk( sync_mutex );
        std::cout << "handle_sorryserver_disconnect_test_thread start:" << thread_no << std::endl;//debug
        thread_data_itr = session_thread_data_map.begin();
        if( thread_no == 1 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr->second->end_flag = END_FLAG_OFF;
            thread_data_itr->second->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
        }
        else if( thread_no == 2 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr++;
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_UP_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr->second->end_flag = END_FLAG_OFF;
            thread_data_itr->second->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
        }
        else if( thread_no == 3 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
        }
        else if( thread_no == 4 ){
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
            thread_data_itr++;
            while( thread_data_itr != session_thread_data_map.end())
            {
                if( thread_data_itr->second->thread_division == THREAD_DIVISION_DOWN_STREAM )
                {
                    break;
                }
                thread_data_itr++;
            }
        }

        sync_condition.wait( lk );
    }
    protocol_module_cinsert::EVENT_TAG status;
    status = handle_sorryserver_disconnect( thread_data_itr->second->thread_id, thread_data_itr->second->client_endpoint_tcp );
    if( thread_no == 1 ){
        if( status == REALSERVER_SELECT )
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    else if( thread_no == 2 ){
        if( status == REALSERVER_SELECT )
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    else if( thread_no == 3 ){
        if( status == CLIENT_CONNECTION_CHECK)
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    else if( thread_no == 4 ){
        if( status == CLIENT_CONNECTION_CHECK)
        {
            *ret = true;
        }
        else
        {
            *ret = false;
        }
    }
    std::cout << "handle_sorryserver_disconnect_test_thread end:" << thread_no << std::endl;//debug
}

// void    init_logger_functions_test()
// {
//     boost::function< LOG_LEVEL_TAG(void) >    ingetloglevel = &stb_getloglevel;
//     boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogFatal    = &stb_putLogFatal;
//     boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogError    = &stb_putLogError;
//     boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogWarn    = &stb_putLogWarn;
//     boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogInfo    = &stb_putLogInfo;
//     boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogDebug    = &stb_putLogDebug;
// 
//     BOOST_CHECK( getloglevel    == NULL );
//     BOOST_CHECK( putLogFatal    == NULL );
//     BOOST_CHECK( putLogError    == NULL );
//     BOOST_CHECK( putLogWarn        == NULL );
//     BOOST_CHECK( putLogInfo        == NULL );
//     BOOST_CHECK( putLogDebug    == NULL );
// 
//     init_logger_functions(    ingetloglevel,
//                             inputLogFatal,
//                             inputLogError,
//                             inputLogWarn,
//                             inputLogInfo,
//                             inputLogDebug);
// 
//     // unit_test[23] init_logger_functions check value
//     BOOST_CHECK( getloglevel    != NULL );
//     BOOST_CHECK( putLogFatal    != NULL );
//     BOOST_CHECK( putLogError    != NULL );
//     BOOST_CHECK( putLogWarn        != NULL );
//     BOOST_CHECK( putLogInfo        != NULL );
//     BOOST_CHECK( putLogDebug    != NULL );
// }

void    init_replication_functions_test()
{
    boost::function< void* ( const std::string&, unsigned int* ) >    inreplication_pay_memory    =    &stb_replication_pay_memory;
    boost::function< void( void ) >    inreplication_area_lock    =    &stb_replication_area_lock;
    boost::function< void( void ) >    inreplication_area_unlock    =    &stb_replication_area_unlock;
    boost::asio::ip::address    address_1;
    unsigned short                port_1 = 1111;
    boost::asio::ip::address    address_2;
    unsigned short                port_2 = 2222;
    boost::asio::ip::tcp::endpoint invirtual_service_endpoint_tcp( address_1, port_1 );
    boost::asio::ip::udp::endpoint invirtual_service_endpoint_udp( address_2, port_2 );

    BOOST_CHECK( replication_pay_memory        == NULL );
    BOOST_CHECK( replication_area_lock        == NULL );
    BOOST_CHECK( replication_area_unlock    == NULL );

    init_replication_functions(
        inreplication_pay_memory,
        inreplication_area_lock,
        inreplication_area_unlock,
        invirtual_service_endpoint_tcp,
        invirtual_service_endpoint_udp );

    // unit_test[24] init_replication_functions check value
    BOOST_CHECK( replication_pay_memory        != NULL );
    BOOST_CHECK( replication_area_lock        != NULL );
    BOOST_CHECK( replication_area_unlock    != NULL );
    BOOST_CHECK_EQUAL( virtual_service_endpoint_tcp, invirtual_service_endpoint_tcp );
    BOOST_CHECK_EQUAL( virtual_service_endpoint_udp, invirtual_service_endpoint_udp );
}

void    replication_interrupt_test()
{
    // unit_test[25] replication_interrupt call check
    replication_interrupt();
}

void    initialize_test()
{
    rs_list_itr_func_type            inlist_begin = &stb_rslist_begin;
    rs_list_itr_func_type            inlist_end = &stb_rslist_end;
    rs_list_itr_next_func_type        inlist_next = &stb_rslist_next;
    boost::function< void( void ) >    inlist_lock = &stb_rslist_lock;
    boost::function< void( void ) >    inlist_unlock = &stb_rslist_unlock;

    BOOST_CHECK( rs_list_begin    == NULL );
    BOOST_CHECK( rs_list_end    == NULL );
    BOOST_CHECK( rs_list_next    == NULL );
    BOOST_CHECK( rs_list_lock    == NULL );
    BOOST_CHECK( rs_list_unlock    == NULL );

    initialize(
        inlist_begin,
        inlist_end,
        inlist_next,
        inlist_lock,
        inlist_unlock );

    // unit_test[26] initialize check value
    BOOST_CHECK( rs_list_begin    != NULL );
    BOOST_CHECK( rs_list_end    != NULL );
    BOOST_CHECK( rs_list_next    != NULL );
    BOOST_CHECK( rs_list_lock    != NULL );
    BOOST_CHECK( rs_list_unlock    != NULL );
}

void    finalize_test()
{
    rs_list_itr_func_type            inlist_begin = &stb_rslist_begin;
    rs_list_itr_func_type            inlist_end = &stb_rslist_end;
    rs_list_itr_next_func_type        inlist_next = &stb_rslist_next;
    boost::function< void( void ) >    inlist_lock = &stb_rslist_lock;
    boost::function< void( void ) >    inlist_unlock = &stb_rslist_unlock;
    boost::function< void* ( const std::string&, unsigned int* ) >    inreplication_pay_memory    =    &stb_replication_pay_memory;
    boost::function< void( void ) >    inreplication_area_lock    =    &stb_replication_area_lock;
    boost::function< void( void ) >    inreplication_area_unlock    =    &stb_replication_area_unlock;
    tcp_schedule_func_type inschedule = &stb_schedule_tcp;


    rs_list_begin    = inlist_begin;
    rs_list_end        = inlist_end;
    rs_list_next    = inlist_next;
    rs_list_lock    = inlist_lock;
    rs_list_unlock    = inlist_unlock;

    replication_pay_memory = inreplication_pay_memory;
    replication_area_lock = inreplication_area_lock;
    replication_area_unlock = inreplication_area_unlock;
    schedule_tcp = inschedule;
    cookie_expire = 86400;
    forwarded_for = 1;
    reschedule = 1;
    cookie_name.assign( 'a' );
    sorry_uri.assign( 'a' );

    finalize();

    // unit_test[27] finalize check value
    BOOST_CHECK_EQUAL( getloglevel.empty() , true );
    BOOST_CHECK_EQUAL( putLogFatal.empty(), true );
    BOOST_CHECK_EQUAL( putLogError.empty(), true );
    BOOST_CHECK_EQUAL( putLogWarn.empty(), true );
    BOOST_CHECK_EQUAL( putLogInfo.empty(), true );
    BOOST_CHECK_EQUAL( putLogDebug.empty(), true );

    BOOST_CHECK_EQUAL( rs_list_begin.empty(), true );
    BOOST_CHECK_EQUAL( rs_list_end.empty(), true );
    BOOST_CHECK_EQUAL( rs_list_next.empty(), true );
    BOOST_CHECK_EQUAL( rs_list_lock.empty(), true );
    BOOST_CHECK_EQUAL( rs_list_unlock.empty(), true );

    BOOST_CHECK_EQUAL( replication_pay_memory.empty(), true );
    BOOST_CHECK_EQUAL( replication_area_lock.empty(), true );
    BOOST_CHECK_EQUAL( replication_area_unlock.empty(), true );

    BOOST_CHECK_EQUAL( schedule_tcp.empty(), true );
    BOOST_CHECK_EQUAL( schedule_udp.empty(), true );

    BOOST_CHECK_EQUAL( cookie_expire, 0 );
    BOOST_CHECK_EQUAL( forwarded_for, 0 );
    BOOST_CHECK_EQUAL( reschedule, 0 );
    BOOST_CHECK_EQUAL( cookie_name[0],'\0' );
    BOOST_CHECK_EQUAL( sorry_uri[0],'\0' );
}

void    handle_rslist_update_test()
{
    // unit_test[28] handle_rslist_update call check
    handle_rslist_update();
}

void    register_schedule_tcp_test()
{
    tcp_schedule_func_type inschedule = &stb_schedule_tcp;
    BOOST_CHECK( schedule_tcp == NULL );
    register_schedule( inschedule );

    // unit_test[29] register_schedule TCP check value
    BOOST_CHECK( schedule_tcp != NULL );
}

void    register_schedule_udp_test()
{
    udp_schedule_func_type inschedule = &stb_schedule_udp;

    // unit_test[30] register_schedule UDP call check
    register_schedule( inschedule );
}

void    handle_realserver_select_udp_test()
{
    protocol_module_cinsert::EVENT_TAG status;

    boost::thread::id thread_id;
    boost::asio::ip::udp::endpoint rs_endpoint;
    boost::array< char, MAX_BUFFER_SIZE > sendbuffer;
    size_t datalen;

    status = handle_realserver_select(
        thread_id,
        rs_endpoint,
        sendbuffer,
        datalen );

    // unit_test[31] handle_realserver_select UDP check return value = STOP
    BOOST_CHECK_EQUAL( status, STOP );
}

void    handle_realserver_recv_udp_test()
{
    protocol_module_cinsert::EVENT_TAG status;

    boost::thread::id thread_id;
    boost::asio::ip::udp::endpoint rs_endpoint;
    boost::array< char, MAX_BUFFER_SIZE > recvbuffer;
    size_t recvlen;

    status = handle_realserver_recv(
        thread_id,
        rs_endpoint,
        recvbuffer,
        recvlen );

    // unit_test[32] handle_realserver_recv UDP check return value = STOP
    BOOST_CHECK_EQUAL( status, STOP );
}

void    handle_response_send_inform_test()
{
    protocol_module_cinsert::EVENT_TAG status;

    boost::thread::id thread_id;

    status = handle_response_send_inform( thread_id );

    // unit_test[33] handle_response_send_inform check return value = STOP
    BOOST_CHECK_EQUAL( status, STOP );
}

void    handle_client_select_test()
{
    protocol_module_cinsert::EVENT_TAG status;

    boost::thread::id thread_id;
    boost::asio::ip::udp::endpoint cl_endpoint;
    boost::array< char, MAX_BUFFER_SIZE > sendbuffer;
    size_t datalen;

    status = handle_client_select(
        thread_id,
        cl_endpoint,
        sendbuffer,
        datalen );

    // unit_test[34] handle_client_select check return value = STOP
    BOOST_CHECK_EQUAL( status, STOP );
}

void    handle_realserver_close_test()
{
    protocol_module_cinsert::EVENT_TAG status;

    boost::thread::id thread_id;
    boost::asio::ip::udp::endpoint rs_endpoint;

    status = protocol_module_cinsert::handle_realserver_close(
        thread_id,
        rs_endpoint );

    // unit_test[35] handle_realserver_close check return value = STOP
    BOOST_CHECK_EQUAL( status, STOP );
}

}; // protocol_module_cinsert_test

//--test class--
class    protocol_module_cinsert_test_2 : public protocol_module_cinsert
{
public:
boost::mutex        sync_mutex;
boost::condition    sync_condition;
boost::condition    session_thread_data_map_condition;

protocol_module_cinsert_test_2() : protocol_module_cinsert() {}
~protocol_module_cinsert_test_2(){}

void    init_logger_functions_test()
{
    boost::function< LOG_LEVEL_TAG(void) >    ingetloglevel = &stb_getloglevel;
    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogFatal    = &stb_putLogFatal;
    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogError    = &stb_putLogError;
    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogWarn    = &stb_putLogWarn;
    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogInfo    = &stb_putLogInfo;
    boost::function< void ( const unsigned int, const std::string&, const char*, int ) >    inputLogDebug    = &stb_putLogDebug;

    BOOST_CHECK( getloglevel    == NULL );
    BOOST_CHECK( putLogFatal    == NULL );
    BOOST_CHECK( putLogError    == NULL );
    BOOST_CHECK( putLogWarn        == NULL );
    BOOST_CHECK( putLogInfo        == NULL );
    BOOST_CHECK( putLogDebug    == NULL );

    init_logger_functions(    ingetloglevel,
                            inputLogFatal,
                            inputLogError,
                            inputLogWarn,
                            inputLogInfo,
                            inputLogDebug);

    // unit_test[23] init_logger_functions check value
    BOOST_CHECK( getloglevel    != NULL );
    BOOST_CHECK( putLogFatal    != NULL );
    BOOST_CHECK( putLogError    != NULL );
    BOOST_CHECK( putLogWarn        != NULL );
    BOOST_CHECK( putLogInfo        != NULL );
    BOOST_CHECK( putLogDebug    != NULL );
}

}; // protocol_module_cinsert_test_2


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//--test functions--
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
void    session_thread_data_map_lock(
                protocol_module_cinsert_test* protocol_module_cinsert_test_1 )
{
    protocol_module_cinsert_test_1->session_thread_data_map_lock();
}

//-------------------------------------------------------------------

// void    session_thread_data_set_test()
// {
// 
//     protocol_module_cinsert_test    protocol_module_cinsert_test_1;
// 
//     protocol_module_cinsert_test_1.session_thread_data_set();
// 
//     protocol_module_cinsert_test_1.session_thread_data_disp();
// 
//     protocol_module_cinsert_test_1.session_thread_data_erase();
// 
// }
void    handle_client_disconnect_test()
{

    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_client_disconnect test start -----" );
    protocol_module_cinsert_test_1.handle_client_disconnect_test();
    BOOST_MESSAGE( "----- handle_client_disconnect test end -----" );

}

void    handle_realserver_disconnect_test_thread(
                protocol_module_cinsert_test* protocol_module_cinsert_test_1,
                int thread_no,
                bool* ret )
{
    for( int i = 0; i < THREAD_LOOP_NUM; i++ )
    {
        protocol_module_cinsert_test_1->handle_realserver_disconnect_test_thread( thread_no, ret );
    }
}

void    handle_realserver_disconnect_test1()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_realserver_disconnect test start -----" );
    protocol_module_cinsert_test_1.handle_realserver_disconnect_test();
    BOOST_MESSAGE( "----- handle_realserver_disconnect test end -----" );
}

void    handle_realserver_disconnect_test2()
{
    bool    thread_1;
    bool    thread_2;
    bool    thread_3;
    bool    thread_4;

    protocol_module_cinsert_test    protocol_module_cinsert_test_1;
    protocol_module_cinsert_test_1.session_thread_data_set();

    BOOST_MESSAGE( "----- handle_realserver_disconnect test multi thread start -----" );
    boost::thread    t1(    boost::bind(    &handle_realserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 1, &thread_1));
    boost::thread    t2(    boost::bind(    &handle_realserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 2, &thread_2));
    boost::thread    t3(    boost::bind(    &handle_realserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 3, &thread_3));
    boost::thread    t4(    boost::bind(    &handle_realserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 4, &thread_4));

    sleep(1);

    protocol_module_cinsert_test_1.sync_condition.notify_all();

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // unit_test[36] handle_realserver_disconnect check return value multi thread
    BOOST_CHECK( thread_1 == true );
    BOOST_CHECK( thread_2 == true );
    BOOST_CHECK( thread_3 == true );
    BOOST_CHECK( thread_4 == true );

    BOOST_MESSAGE( "----- handle_realserver_disconnect test multi thread end -----" );
}

void    handle_sorryserver_disconnect_test_thread(
                protocol_module_cinsert_test* protocol_module_cinsert_test_1,
                int thread_no,
                bool* ret )
{
    for( int i = 0; i < THREAD_LOOP_NUM; i++ )
    {
        protocol_module_cinsert_test_1->handle_sorryserver_disconnect_test_thread( thread_no, ret );
    }
}

void    handle_sorryserver_disconnect_test1()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_sorryserver_disconnect test start -----" );
    protocol_module_cinsert_test_1.handle_sorryserver_disconnect_test();
    BOOST_MESSAGE( "----- handle_sorryserver_disconnect test end -----" );
}

void    handle_sorryserver_disconnect_test2()
{
    bool    thread_1;
    bool    thread_2;
    bool    thread_3;
    bool    thread_4;

    protocol_module_cinsert_test    protocol_module_cinsert_test_1;
    protocol_module_cinsert_test_1.session_thread_data_set();

    BOOST_MESSAGE( "----- handle_sorryserver_disconnect test multi thread start -----" );
    boost::thread    t1(    boost::bind(    &handle_sorryserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 1, &thread_1));
    boost::thread    t2(    boost::bind(    &handle_sorryserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 2, &thread_2));
    boost::thread    t3(    boost::bind(    &handle_sorryserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 3, &thread_3));
    boost::thread    t4(    boost::bind(    &handle_sorryserver_disconnect_test_thread,
                                        &protocol_module_cinsert_test_1, 4, &thread_4));

    sleep(1);

    protocol_module_cinsert_test_1.sync_condition.notify_all();

    t1.join();
    t2.join();
    t3.join();
    t4.join();

    // unit_test[37] handle_sorryserver_disconnect check return value multi thread
    BOOST_CHECK( thread_1 == true );
    BOOST_CHECK( thread_2 == true );
    BOOST_CHECK( thread_3 == true );
    BOOST_CHECK( thread_4 == true );

    BOOST_MESSAGE( "----- handle_sorryserver_disconnect test multi thread end -----" );
}

void    init_logger_functions_test()
{
    protocol_module_cinsert_test_2    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- init_logger_functions test start -----" );
    protocol_module_cinsert_test_1.init_logger_functions_test();
    BOOST_MESSAGE( "----- init_logger_functions test end -----" );
}

void    init_replication_functions_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- init_replication_functions test start -----" );
    protocol_module_cinsert_test_1.init_replication_functions_test();
    BOOST_MESSAGE( "----- init_replication_functions test end -----" );
}

void    replication_interrupt_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- replication_interrupt test start -----" );
    protocol_module_cinsert_test_1.replication_interrupt_test();
    BOOST_MESSAGE( "----- replication_interrupt test end -----" );
}

void    initialize_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- initialize test start -----" );
    protocol_module_cinsert_test_1.initialize_test();
    BOOST_MESSAGE( "----- initialize test end -----" );
}

void    finalize_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- finalize test start -----" );
    protocol_module_cinsert_test_1.finalize_test();
    BOOST_MESSAGE( "----- finalize test end -----" );
}

void    handle_rslist_update_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_rslist_update test start -----" );
    protocol_module_cinsert_test_1.handle_rslist_update_test();
    BOOST_MESSAGE( "----- handle_rslist_update test end -----" );
}

void    register_schedule_tcp_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- register_schedule_tcp test start -----" );
    protocol_module_cinsert_test_1.register_schedule_tcp_test();
    BOOST_MESSAGE( "----- register_schedule_tcp test end -----" );
}

void    register_schedule_udp_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- register_schedule_udp test start -----" );
    protocol_module_cinsert_test_1.register_schedule_udp_test();
    BOOST_MESSAGE( "----- register_schedule_udp test end -----" );
}

void    handle_realserver_select_udp_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_realserver_select_udp test start -----" );
    protocol_module_cinsert_test_1.handle_realserver_select_udp_test();
    BOOST_MESSAGE( "----- handle_realserver_select_udp test end -----" );
}

void    handle_realserver_recv_udp_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_realserver_recv_udp test start -----" );
    protocol_module_cinsert_test_1.handle_realserver_recv_udp_test();
    BOOST_MESSAGE( "----- handle_realserver_recv_udp test end -----" );
}

void    handle_response_send_inform_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_response_send_inform test start -----" );
    protocol_module_cinsert_test_1.handle_response_send_inform_test();
    BOOST_MESSAGE( "----- handle_response_send_inform test end -----" );
}

void    handle_client_select_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_client_select test start -----" );
    protocol_module_cinsert_test_1.handle_client_select_test();
    BOOST_MESSAGE( "----- handle_client_select test end -----" );
}

void    handle_realserver_close_test()
{
    protocol_module_cinsert_test    protocol_module_cinsert_test_1;

    BOOST_MESSAGE( "----- handle_realserver_close test start -----" );
    protocol_module_cinsert_test_1.handle_realserver_close_test();
    BOOST_MESSAGE( "----- handle_realserver_close test end -----" );
}

//-------------------------------------------------------------------
//-------------------------------------------------------------------
test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    test_suite* ts = BOOST_TEST_SUITE( "protocol_module_cinsert class test" );

    ts->add( BOOST_TEST_CASE( &handle_client_disconnect_test ) );
    ts->add( BOOST_TEST_CASE( &handle_realserver_disconnect_test1 ) );
    ts->add( BOOST_TEST_CASE( &handle_realserver_disconnect_test2 ) );
    ts->add( BOOST_TEST_CASE( &handle_sorryserver_disconnect_test1 ) );
    ts->add( BOOST_TEST_CASE( &handle_sorryserver_disconnect_test2 ) );
    ts->add( BOOST_TEST_CASE( &init_logger_functions_test ) );
    ts->add( BOOST_TEST_CASE( &init_replication_functions_test ) );
    ts->add( BOOST_TEST_CASE( &replication_interrupt_test ) );
    ts->add( BOOST_TEST_CASE( &initialize_test ) );
    ts->add( BOOST_TEST_CASE( &finalize_test ) );
    ts->add( BOOST_TEST_CASE( &handle_rslist_update_test ) );
    ts->add( BOOST_TEST_CASE( &register_schedule_tcp_test ) );
    ts->add( BOOST_TEST_CASE( &register_schedule_udp_test ) );
    ts->add( BOOST_TEST_CASE( &handle_realserver_select_udp_test ) );
    ts->add( BOOST_TEST_CASE( &handle_realserver_recv_udp_test ) );
    ts->add( BOOST_TEST_CASE( &handle_response_send_inform_test ) );
    ts->add( BOOST_TEST_CASE( &handle_client_select_test ) );
    ts->add( BOOST_TEST_CASE( &handle_realserver_close_test ) );

    framework::master_test_suite().add( ts );

    return 0;
}
