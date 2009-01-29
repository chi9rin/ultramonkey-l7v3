#ifndef PROTOCOL_MODULE_SSLID_H
#define PROTOCOL_MODULE_SSLID_H

#include "ssl_protocol_module_base.h"
#include "sslid_replication_data_processor.h"
#include "sslid_session_data_processor.h"
#include <boost/thread/mutex.hpp>
#include <map>

#define MAX_SSLID_BUFFER_SIZE (MAX_BUFFER_SIZE + 76)
namespace l7vs
{

struct session_thread_data_sslid
{
        int thread_division;
        boost::thread::id pair_thread_id;
        int end_flag;
        int realserver_connect_failed_count;
        boost::asio::ip::tcp::endpoint selected_realserver;
        boost::array<char, MAX_SSLID_BUFFER_SIZE> data_buffer;
        int data_begain_offset;
        int data_size;
        int current_record_rest_size;
        bool hello_message_flag;
};

class protocol_module_sslid : public ssl_protocol_module_base
{
    public:
        protocol_module_sslid();

        virtual ~protocol_module_sslid();

        bool is_tcp();

        bool is_udp();

        void initialize( rs_list_itr_func_type	inlist_begin,
                         rs_list_itr_func_type	inlist_end,
						 rs_list_itr_func_type	inlist_next,
						 boost::function< void( void ) >	inlist_lock,
						 boost::function< void( void ) >	inlist_unlock );

        void finalize();

        bool is_use_sorry();

        check_message_result check_parameter( const std::vector<std::string>& args );

        check_message_result set_parameter( const std::vector<std::string>& args );

        check_message_result add_parameter( const std::vector<std::string>& args );

        void handle_rslist_update();

        void register_schedule( tcp_schedule_func_type inschedule );

        void register_schedule( udp_schedule_func_type inschedule );

        EVENT_TAG handle_session_initialize(
                                const boost::thread::id up_thread_id,
                                const boost::thread::id down_thread_id );

        EVENT_TAG handle_session_finalize(
                                const boost::thread::id up_thread_id,
                                const boost::thread::id down_thread_id );

        EVENT_TAG handle_accept( const boost::thread::id thread_id );

        EVENT_TAG handle_client_recv(
                                const boost::thread::id thread_id,
                                const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
                                const size_t recvlen );

        EVENT_TAG handle_realserver_select(
                                const boost::thread::id thread_id,
                                boost::asio::ip::tcp::endpoint& rs_endpoint );

        EVENT_TAG handle_realserver_select(
								const boost::thread::id thread_id,
								boost::asio::ip::udp::endpoint& rs_endpoint,
								boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
								size_t& datalen );

        EVENT_TAG handle_realserver_connect(
                                const boost::thread::id thread_id,
                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen );

        EVENT_TAG handle_realserver_connection_fail(
                                const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint& rs_endpoint );

        EVENT_TAG handle_realserver_send( const boost::thread::id thread_id );

        EVENT_TAG handle_sorryserver_select(
                                const boost::thread::id thread_id,
								boost::asio::ip::tcp::endpoint& sorry_endpoint );

        EVENT_TAG handle_sorryserver_connect(
								const boost::thread::id thread_id,
								boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
								size_t& datalen );

        EVENT_TAG handle_sorryserver_connection_fail(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint );

        EVENT_TAG handle_sorryserver_send( const boost::thread::id thread_id );

        EVENT_TAG handle_realserver_recv(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& rs_endpoint,
								const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
								const size_t recvlen );

        EVENT_TAG handle_realserver_recv(
								const boost::thread::id thread_id,
								const boost::asio::ip::udp::endpoint& rs_endpoint,
								const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
								const size_t recvlen );

        EVENT_TAG handle_sorryserver_recv(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint,
								const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
								const size_t recvlen );

        EVENT_TAG handle_response_send_inform( const boost::thread::id thread_id );

        EVENT_TAG handle_client_connection_check(
								const boost::thread::id thread_id,
								boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
								size_t& datalen );

        EVENT_TAG handle_client_select(
								const boost::thread::id thread_id,
								boost::asio::ip::udp::endpoint& cl_endpoint,
								boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
								size_t& datalen );

        EVENT_TAG handle_client_send( const boost::thread::id thread_id );

        EVENT_TAG handle_client_disconnect( const boost::thread::id thread_id );

        EVENT_TAG handle_sorry_enable( const boost::thread::id thread_id );

        EVENT_TAG handle_sorry_disable( const boost::thread::id thread_id );

        EVENT_TAG handle_realserver_disconnect(
                                const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& rs_endpoint );

        EVENT_TAG handle_sorryserver_disconnect(
								const boost::thread::id thread_id,
								const boost::asio::ip::tcp::endpoint& sorry_endpoint );

        EVENT_TAG handle_realserver_close(
								const boost::thread::id thread_id,
								const boost::asio::ip::udp::endpoint& rs_endpoint );

        void replication_interrupt();

        bool realserver_selected( const boost::asio::ip::tcp::endpoint& rs_endpoint );

    protected:
        int put_data_to_sendbuffer(
                                const boost::thread::id& thread_id,
                                boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
                                size_t& datalen);
    protected:
        int timeout;
        int maxlist;
        int reschedule;
        typedef std::map<boost::thread::id, session_thread_data_sslid*> session_thread_data_map_type;
        session_thread_data_map_type session_thread_data_map;
        boost::mutex session_thread_data_map_mutex;
        int realserver_connect_failed_max_count;
        sslid_replication_data_processor* replication_data_processor;
        sslid_session_data_processor* session_data_processor;
    private:
        static const std::string MODULE_NAME;
};

}
#endif // PROTOCOL_MODULE_SSLID_H
