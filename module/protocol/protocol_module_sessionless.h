#include <boost/thread/mutex.hpp>
#include "http_protocol_module_base.h"

#ifndef PROTOCOL_MODULE_SESSIONLESS_H
#define PROTOCOL_MODULE_SESSIONLESS_H

#define MAX_OPTION_SIZE 128

namespace l7vs
{

class protocol_module_sessionless : public http_protocol_module_base
{
public:
	enum SEND_STATUS_TAG
    {
		SEND_OK= 0,
		SEND_NG,
		SEND_END,
		SEND_CONTINUE
    };
	struct edit_data {
        std::string data;
        size_t data_size;
        size_t insert_posission;
        size_t replace_size;
    } ;

    struct send_status {
        SEND_STATUS_TAG status;
        size_t send_end_size;
        size_t send_rest_size;
        size_t send_possible_size;
        size_t send_offset;
        size_t unsend_size;
        int edit_division;
        boost::asio::ip::tcp::endpoint send_endpoint;
        std::list<edit_data> edit_data_list;
    };

    struct recive_data {
        char* recive_buffer;
        char* recive_buffer1;
        char* recive_buffer2;
        size_t recive_buffer_max_size;
        size_t recive_buffer_rest_size;
        std::list<send_status>  send_status_list;
	};


    struct session_thread_data_sessionless {
        boost::thread::id thread_id;
        int thread_division;
        boost::thread::id pair_thread_id;
        std::map<boost::asio::ip::tcp::endpoint, recive_data> recive_data_map;
        int end_flag;
        int accept_end_flag;
        int sorry_flag;
        int sorryserver_switch_flag;
        int realserver_switch_flag;
        boost::asio::ip::tcp::endpoint target_endpoint;
        boost::asio::ip::tcp::endpoint client_endpoint_tcp;
        EVENT_TAG last_status;
    };

   typedef std::list<send_status>::iterator send_status_it;
   typedef std::map<boost::thread::id, session_thread_data_sessionless*>::iterator session_thread_data_map_it;
   typedef std::map<boost::asio::ip::tcp::endpoint, recive_data>::iterator recive_data_map_it;
protected:
	int forwarded_for;
	boost:: array<char,MAX_OPTION_SIZE> sorry_uri ;
	std::map<boost::thread::id, session_thread_data_sessionless*> session_thread_data_map;
	boost::mutex session_thread_data_map_mutex;

public:
	static const std::string MODULE_NAME;

	static const int THREAD_DIVISION_UP_STREAM = 0; //上りスレッド
	static const int THREAD_DIVISION_DOWN_STREAM = 1;  //下りスレッド

	static const int END_FLAG_OFF = 0; //終了フラグOFF
	static const int END_FLAG_ON = 1; //終了フラグON

	static const int ACCEPT_END_FLAG_OFF = 0; //ACCEPT完了フラグOFF
	static const int ACCEPT_END_FLAG_ON = 1; //ACCEPT完了フラグON

	static const int SORRY_FLAG_ON = 1; //SORRY状態
	static const int SORRY_FLAG_OFF = 0; //SORRY状態以外

	static const int SORRYSERVER_SWITCH_FLAG_OFF = 0; //sorryserver切替中以外
	static const int SORRYSERVER_SWITCH_FLAG_ON = 1; //sorryserver切替中

	static const int REALSERVER_SWITCH_FLAG_OFF = 0; //realserver切替中以外
	static const int REALSERVER_SWITCH_FLAG_ON = 1; //realserver切替中

	static const int EDIT_DIVISION_NO_EDIT = 0; //編集無し
	static const int EDIT_DIVISION_EDIT = 1; //編集あり

public:
	protocol_module_sessionless();
	~protocol_module_sessionless();

	bool is_tcp();
	bool is_udp();
	void replication_interrupt();
	void initialize(rs_list_itr_func_type	inlist_begin,
			rs_list_itr_func_type	inlist_end,
			rs_list_itr_next_func_type	inlist_next,
			boost::function< void( void ) >	inlist_lock,
			boost::function< void( void ) >	inlist_unlock);
	void finalize();
	bool is_use_sorry();
	check_message_result check_parameter(const std::vector<std::string>& args);
	check_message_result set_parameter(const std::vector<std::string>& args);
	check_message_result add_parameter(const std::vector<std::string>& args);
	void handle_rslist_update();
	void register_schedule(tcp_schedule_func_type inschedule);
	void register_schedule(udp_schedule_func_type inschedule);
	EVENT_TAG handle_session_initialize(const boost::thread::id up_thread_id, const boost::thread::id down_thread_id, const boost::asio::ip::tcp::endpoint& client_endpoint_tcp, const boost::asio::ip::udp::endpoint& client_endpoint_udp);
	EVENT_TAG handle_session_finalize(const boost::thread::id up_thread_id, const boost::thread::id down_thread_id);
	EVENT_TAG handle_accept(const boost::thread::id thread_id);
	EVENT_TAG handle_client_recv(const boost::thread::id thread_id, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	EVENT_TAG handle_realserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& rs_endpoint);
	EVENT_TAG handle_realserver_select(const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& rs_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	EVENT_TAG handle_realserver_connect(const boost::thread::id thread_id, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	EVENT_TAG handle_realserver_connection_fail(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint);
	EVENT_TAG handle_realserver_send(const boost::thread::id thread_id);
	EVENT_TAG handle_sorryserver_select(const boost::thread::id thread_id, boost::asio::ip::tcp::endpoint& sorry_endpoint);
	EVENT_TAG handle_sorryserver_connect(const boost::thread::id thread_id, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	EVENT_TAG handle_sorryserver_connection_fail(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint);
	EVENT_TAG handle_sorryserver_send(const boost::thread::id thread_id);
	EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	EVENT_TAG handle_realserver_recv(const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint, const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	EVENT_TAG handle_sorryserver_recv(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint, const boost::array<char, MAX_BUFFER_SIZE>& recvbuffer, const size_t recvlen);
	EVENT_TAG handle_response_send_inform(const boost::thread::id thread_id);
	EVENT_TAG handle_client_connection_check(const boost::thread::id thread_id, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	EVENT_TAG handle_client_select(const boost::thread::id thread_id, boost::asio::ip::udp::endpoint& cl_endpoint, boost::array<char,MAX_BUFFER_SIZE>& sendbuffer, size_t& datalen);
	EVENT_TAG handle_client_send(const boost::thread::id thread_id);
	EVENT_TAG handle_client_disconnect(const boost::thread::id thread_id);
	EVENT_TAG handle_sorry_enable(const boost::thread::id thread_id);
	EVENT_TAG handle_sorry_disable(const boost::thread::id thread_id);
	EVENT_TAG handle_realserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& rs_endpoint);
	EVENT_TAG handle_sorryserver_disconnect(const boost::thread::id thread_id, const boost::asio::ip::tcp::endpoint& sorry_endpoint);
	EVENT_TAG handle_realserver_close(const boost::thread::id thread_id, const boost::asio::ip::udp::endpoint& rs_endpoint);
};

inline bool operator < (const protocol_module_sessionless::edit_data& lref, const protocol_module_sessionless::edit_data& rref)
{
	return lref.insert_posission < rref.insert_posission;
}

class data_send_possible
{
public:
	inline bool operator () (const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_OK
				&&  send_status.send_possible_size > 0;
	}
};

class data_send_disable
{
public:
	inline bool operator () (const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_NG
		|| send_status.send_rest_size > 0 ;
	}
};

class data_send_continue
{
public:
	inline bool operator () (const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_CONTINUE;
	}
};

class data_send_ng
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_NG;
	}
};

class data_send_ok
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status)
	{
		return send_status.status == protocol_module_sessionless::SEND_OK;
	}
};

class data_send_repeated
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status_first,
	        const protocol_module_sessionless::send_status& send_status_second)
	{
		return send_status_first.status == protocol_module_sessionless::SEND_NG
		|| send_status_first.status == protocol_module_sessionless::SEND_CONTINUE;
	}
};

class data_send_ok_incorrect
{
public:
	inline bool operator ()(const protocol_module_sessionless::send_status& send_status_first,
	        const protocol_module_sessionless::send_status& send_status_second)
	{
		return send_status_first.status == protocol_module_sessionless::SEND_OK
		&& send_status_first.send_rest_size > 0;
	}
};

}

#endif  //PROTOCOL_MODULE_SESSIONLESS_H

