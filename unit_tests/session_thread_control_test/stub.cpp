#include <boost/asio.hpp>

#include "data_buff_base.h"
#include "tcp_thread_message.h"
#include "tcp_thread_message_que.h"
#include "tcp_data.h"
#include "tcp_socket.h"
#include "tcp_session.h"
#include "udp_session.h"


l7vs::l7vsd::l7vsd(){}
l7vs::l7vsd::~l7vsd(){}
l7vs::l7vsd::vslist_type::iterator	l7vs::l7vsd::search_vslist( const virtualservice_element& )	const{
}

int		l7vs::replication::initialize(){
	return 0;
}
void	l7vs::replication::finalize(){}
void	l7vs::replication::switch_to_master(){}
void	l7vs::replication::switch_to_slave(){}
void*	l7vs::replication::pay_memory( std::string& inid, unsigned int& outsize ){
	return NULL;
}
void	l7vs::replication::dump_memory(){}
void	l7vs::replication::start(){}
void	l7vs::replication::stop(){}
void	l7vs::replication::force_replicate(){}
void	l7vs::replication::reset(){}
l7vs::replication::REPLICATION_MODE_TAG	l7vs::replication::get_status(){
	l7vs::replication::REPLICATION_MODE_TAG	retmode = l7vs::replication::REPLICATION_OUT;
	return retmode;
}
int		l7vs::replication::check_interval(){
	return 0;
}
int		l7vs::replication::handle_send(){
	return 0;
}
int		l7vs::replication::handle_receive(){
	return 0;
}
int		l7vs::replication::lock( std::string& inid ){
	return 0;
}
int		l7vs::replication::unlock( std::string& inid ){
	return 0;
}
int		l7vs::replication::refer_lock_mutex( std::string& inid, boost::mutex& outmutex ){
	return 0;
}


l7vs::data_buff_base::data_buff_base(){}
l7vs::data_buff_base::~data_buff_base(){}
void	l7vs::data_buff_base::initialize(){}
std::size_t	l7vs::data_buff_base::get_size(){
	return send_size;
}
void	l7vs::data_buff_base::set_size(const std::size_t set_size){}
boost::array< char , MAX_BUFFER_SIZE>&	l7vs::data_buff_base::get_data(){
	return data;
}
void	l7vs::data_buff_base::set_data(const boost::array< char , MAX_BUFFER_SIZE>& data){}
void	l7vs::data_buff_base::set_send_size(const std::size_t set_size){}
std::size_t	l7vs::data_buff_base::get_send_size(){
	return send_size;
}

l7vs::tcp_realserver_connect_socket_list::tcp_realserver_connect_socket_list(){}
l7vs::tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list(){}
void	l7vs::tcp_realserver_connect_socket_list::push_back(list_element realserver_socket){}
l7vs::tcp_realserver_connect_socket_list::list_element	l7vs::tcp_realserver_connect_socket_list::get_socket(){
}
bool	l7vs::tcp_realserver_connect_socket_list::empty(){}

l7vs::tcp_thread_message::tcp_thread_message(){}
l7vs::tcp_thread_message::~tcp_thread_message(){}

l7vs::tcp_thread_message_que::tcp_thread_message_que(){}
l7vs::tcp_thread_message_que::~tcp_thread_message_que(){}
void	l7vs::tcp_thread_message_que::push(tcp_thread_message_ptr message){}
l7vs::tcp_thread_message_que::tcp_thread_message_ptr	l7vs::tcp_thread_message_que::front(){
}
bool	l7vs::tcp_thread_message_que::empty(){}
void 	l7vs::tcp_thread_message_que::clear(){}


l7vs::tcp_data::tcp_data(){}
l7vs::tcp_data::~tcp_data(){}
void	l7vs::tcp_data::initialize(){}
void	l7vs::tcp_data::set_endpoint(const boost::asio::ip::tcp::endpoint set_endpoint){}
boost::asio::ip::tcp::endpoint	l7vs::tcp_data::get_endpoint(){
	return endpoint_info;
}

l7vs::tcp_socket::tcp_socket(boost::asio::io_service& io) : my_socket( io ){}
l7vs::tcp_socket::~tcp_socket(){}
boost::asio::ip::tcp::socket&	l7vs::tcp_socket::get_socket(){
	return my_socket;
}
bool	l7vs::tcp_socket::connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec){
	return true;
}
bool	l7vs::tcp_socket::close(boost::system::error_code& ec){
	return true;
}
bool	l7vs::tcp_socket::set_non_blocking_mode(boost::system::error_code& ec){
	return true;
}
std::size_t		l7vs::tcp_socket::write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
	size_t	ret = 0;
	return ret;
}
std::size_t		l7vs::tcp_socket::read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
	size_t	ret = 0;
	return ret;
}




l7vs::tcp_session::tcp_session(l7vs::virtualservice_tcp& vs, boost::asio::io_service& session_io) : io( session_io ),
																									parent_service( vs ),
																									client_socket( io ) {
	exit_flag = false;
	session_pause_flag = false;
	std::cout << "SESSION:CREATE" << std::endl;
}
l7vs::tcp_session::~tcp_session(void){
	std::cout << "SESSION:DESTROY" << std::endl;
}
l7vs::session_result_message l7vs::tcp_session::initialize(void){
	l7vs::session_result_message	result;
	return result;
}

boost::asio::ip::tcp::socket& l7vs::tcp_session::get_client_socket(void){
}

bool	l7vs::tcp_session::is_thread_wait(void){
}

void	l7vs::tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message){
	switch( message ){
	case SORRY_STATE_DISABLE:
		{
			boost::mutex::scoped_lock( exit_flag_update_mutex );
			exit_flag = false;
		}
		break;
	case SESSION_END:
		{
			boost::mutex::scoped_lock( exit_flag_update_mutex );
			exit_flag = true;
		}
		break;
	case SESSION_PAUSE_ON:
		{
			boost::mutex::scoped_lock( module_function_sorry_disable_mutex );
			session_pause_flag = true;
		}
		break;
	case SESSION_PAUSE_OFF:
		{
			boost::mutex::scoped_lock( module_function_sorry_disable_mutex );
			session_pause_flag = false;
		}
		break;
	}
}

void	l7vs::tcp_session::up_thread_run(void){
	unsigned int i = 0;
	std::cout << "UP:RUN" << std::endl;
	for( i = 0; i < (INT_MAX/12); ++i ){
		{
			boost::mutex::scoped_lock( exit_flag_update_mutex );
			if( exit_flag )break;
		}
	}
	std::cout << "UP:STOP(" << i << ")" << std::endl;
}

void	l7vs::tcp_session::down_thread_run(void){
	unsigned int i = 0;
	std::cout << "DOWN:RUN" << std::endl;
	for( i = 0; i < (INT_MAX/12); ++i ){
		{
			boost::mutex::scoped_lock( module_function_sorry_disable_mutex );
			if( session_pause_flag )break;
		}
	}
	std::cout << "DOWN:STOP(" << i << ")" << std::endl;
}

// void	thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist);
// 
// void	up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_get_distination_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type);
// void	up_thread_all_socket_close(void);
// 
// void	down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type);
// void	down_thread_all_socket_close(void);


l7vs::udp_session::udp_session(virtualservice_udp& vs, boost::asio::io_service& session_io) :	io( session_io ),
																								parent_service( vs ),
																								client_side_socket( io ) {}
l7vs::udp_session::~udp_session(){}
l7vs::session_result_message	l7vs::udp_session::initialize(const udp_endpoint listen_end){
	l7vs::session_result_message	result;
	return result;
}

void	l7vs::udp_session::set_virtual_service_message(const UDP_VIRTUAL_SERVICE_MESSAGE_TAG message){}
void	l7vs::udp_session::run(void){}
bool	l7vs::udp_session::client_send(const udp_endpoint client_endpoint, const udp_session_buff& data_buff,const std::size_t data_size,boost::system::error_code& ec){
	return true;
}
void	l7vs::udp_session::release_request(const boost::thread::id release_thread_id){}

void	l7vs::udp_session::make_request_thread(const int max_count){}
void	l7vs::udp_session::active_request_thread(const udp_endpoint client_endpoint , const udp_session_buff receive_data, const std::size_t receive_size){}
void	l7vs::udp_session::all_active_request_stop(void){}

