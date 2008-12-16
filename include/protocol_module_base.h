#ifndef	PROTOCOL_MODULE_BASE_H
#define	PROTOCOL_MODULE_BASE_H

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <module_base.h>

class protocol_module_base : public module_base
{
protected:

public:
	struct check_message_result
	{
		bool	flag;
		string	message;
	
		bool	operator==( const check_message& in ){ return flag == in.flag; }
		bool	operator!=( const check_message& in ){ return flag != in.flag; }
	};

	enum	HANDLE_STATUS_TAG
	{
		//use in upstream_thread
		CLIENT_RECV,
		CLIENT_RESPOND_SEND,
		REALSERVER_CONNECT,
		SORRYSERVER_CONNECT,
		//use in downstream_thread
		REALSERVER_RECV,
		SORRYSERVER_RECV,
		CLIENT_RESPOND_SEND,
		CLIENT_SEND,
		//COMMON Status
		REALSERVER_DISCONNECT,
		SORRYSERVER_DISCONNECT,
		CLIENT_DISCONNECT,
		FINALIZE,
		STOP
	}

	protocol_module_base();

	virtual	bool	is_use_sorry() = 0;
	virtual	const check_message_result&	check_parameter( const std::vector<std::string> args ) =0;

//moduleがロードされた後で、VSからRS_List操作関数オブジェクトをもらう（it->begin(),it->end(),it->next()）
//RS_Listは変更時にロックされるので、その時はbegin(),end(),next()しないような仕組みが必要
	virtual boost::function< void ( void ) > rs_list_begin;
	virtual boost::function< void ( void ) > rs_list_end;
	virtual boost::function< void ( void ) > rs_list_next;

//RS_List変更時、モジュールに対して変更の通知をする
//変更開始と変更終了の通知をすれば、ロックタイミングをVS側からコントロールできる
	virtual	handle_rs_list_edit_start();
	virtual	handle_rs_list_edit_end();

	virtual	boost::function< void ( void ) > rs_list_lock;
	virtual	boost::function< void ( void ) > rs_list_unlock;

	virtual	void	set_parameter( const std::vector<std::string>& args ) = 0;
	virtual	void	add_parameter( const std::vector<std::string>& args ) = 0;

	virtual	void	handle_sorry_enable() = 0;
	virtual	void	handle_sorry_disable() = 0;

	//use in upstream_thread
	virtual	HANDLE_STATUS_TAG	handle_accept( const pthread_t up_thread_id, const pthread_t down_thread_id ) = 0;

	virtual	HANDLE_STATUS_TAG	handle_client_recv( const pthread_t thread_id, const boost::array<char,n>& buffer, const int recv_len, boost::asio::ip::basic_endpoint & rs_endpoint ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_realserver_connect( const pthread_t thread_id, boost::array<char,n>& buffer ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_realserver_connection_fail( const pthread_t thread_id ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_realserver_send( const pthread_t thread_id) = 0;
	virtual	HANDLE_STATUS_TAG	handle_sorryserver_connect( const pthread_t thread_id, boost::array<char,n>& buffer ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_sorryserver_connection_fail( const pthread_t thread_id ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_sorryserver_send( const pthread_t thread_id ) = 0;

	//use in downstream_thread
	virtual	HANDLE_STATUS_TAG	handle_realserver_recv( const pthread_t thread_id, const boost::asio::ip::basic_endpoint & rs_endpoint, const boost::array<char,n>& buffer, const int recv_len ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_sorryserver_recv( const pthread_t thread_id, const boost::array<char,n>& buffer, const int recv_len ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_clientrespondmessage_get( const pthread_t thread_id, boost::array<char,n>& buffer ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_client_send( const pthread_t thread_id, const boost::array<char,n>& buffer ) = 0;

	//use in upstream/downstream thread
	virtual	HANDLE_STATUS_TAG	handle_client_disconnect( const pthread_t thread_id ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_realserver_disconnect( const pthread_t thread_id, const boost::asio::ip::basic_endpoint & rs_endpoint ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_sorryserver_disconnect( const pthread_t thread_id ) = 0;
	virtual	HANDLE_STATUS_TAG	handle_finalize( const pthread_t thread_id ) = 0;

//	virtual	HANDLE_STATUS_TAG	handle_event( const pthread_t thread_id, HANDLE_STATUS_TAG prev_status,  ) = 0;
};

#endif	//PROTOCOL_MODULE_BASE_H
