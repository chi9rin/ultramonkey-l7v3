#include	"protocol_module_base.h"

namespace l7vs{

class	protocol_module_test1 : public protocol_module_base{
public:
	protocol_module_test1();
	~protocol_module_test1();

	void	initialize(
							rs_list_itr_func_type	inlist_begin,
							rs_list_itr_func_type	inlist_end,
							rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock );

	void	init_logger_functions(
							getloglevel_func_type	ingetloglevel,
							logger_func_type		inputLogFatal,
							logger_func_type		inputLogError,
							logger_func_type		inputLogWarn,
							logger_func_type		inputLogInfo,
							logger_func_type		inputLogDebug );

	void	init_replication_functions(
							replicationpaymemory_func_type  inreplication_pay_memory,
							boost::function< void( void ) > inlock_func,
							boost::function< void( void ) > inunlock_func );

	void	finalize();

	bool	is_tcp();
	bool	is_udp();
	bool	is_use_sorry();
	check_message_result	check_parameter( const std::vector<std::string>& args );

	void	handle_rslist_update();

	check_message_result	set_parameter( const std::vector<std::string>& args );
	check_message_result	add_parameter( const std::vector<std::string>& args );

	void	register_schedule( tcp_schedule_func_type inschedule );

	void	register_replication_area_lock(
									boost::function< void( void ) > inlock_func );

	void	register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func );

	//use in upstream_thread
	EVENT_TAG	handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id );

	EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id );

	EVENT_TAG	handle_accept( const boost::thread::id thread_id );

	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen );

	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint );

	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen );
	
	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint );
	
	EVENT_TAG	handle_realserver_send( const boost::thread::id thread_id );
	
	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & sorry_endpoint );

	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen );

	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id );
	
	EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id );

	//use in downstream_thread
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen );
	
	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen );
	
	EVENT_TAG	handle_response_send_inform( const boost::thread::id thread_id );

	EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & recv_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen );

	EVENT_TAG	handle_client_send( const boost::thread::id thread_id );

	//use in upstream/downstream thread
	EVENT_TAG	handle_client_disconnect( const boost::thread::id thread_id );
	
	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id );

	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id );

	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint );
	
	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint );

	//unuse method
	void	register_schedule( udp_schedule_func_type inschedule ){}
	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ){return STOP;}
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen ){return STOP;}
	EVENT_TAG	handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ){return STOP;}
	EVENT_TAG	handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint ){return STOP;};
};

protocol_module_test1::protocol_module_test1() : protocol_module_base()
{
	name = "test1";
}

protocol_module_test1::~protocol_module_test1(){}

void
protocol_module_test1::initialize(
							protocol_module_base::rs_list_itr_func_type	inlist_begin,
							protocol_module_base::rs_list_itr_func_type	inlist_end,
							protocol_module_base::rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock ){}

void
protocol_module_test1::init_logger_functions(
							getloglevel_func_type	ingetloglevel,
							logger_func_type		inputLogFatal,
							logger_func_type		inputLogError,
							logger_func_type		inputLogWarn,
							logger_func_type		inputLogInfo,
							logger_func_type		inputLogDebug ){}

void
protocol_module_test1::init_replication_functions(
							replicationpaymemory_func_type  inreplication_pay_memory,
							boost::function< void( void ) > inlock_func,
							boost::function< void( void ) > inunlock_func ){}

void	finalize(){}

bool	is_tcp(){ return true; }
bool	is_udp(){ return false; }
bool
protocol_module_test1::is_use_sorry(){ return true; }

protocol_module_base::check_message_result
protocol_module_test1::check_parameter( const std::vector<std::string>& args ){
	protocol_module_base::check_message_result	result;
	result.flag = true;
	return result;
}

void
protocol_module_test1::handle_rslist_update(){}

protocol_module_base::check_message_result
protocol_module_test1::set_parameter( const std::vector<std::string>& args ){
	protocol_module_base::check_message_result	result;
	result.flag = true;
	return result;
}

protocol_module_base::check_message_result
protocol_module_test1::add_parameter( const std::vector<std::string>& args ){
	protocol_module_base::check_message_result	result;
	result.flag = true;
	return result;
}

void
protocol_module_test1::register_schedule( protocol_module_base::tcp_schedule_func_type inschedule ){}

void
protocol_module_test1::register_replication_area_lock(
									boost::function< void( void ) > inlock_func ){}

void
protocol_module_test1::register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func ){}

	//use in upstream_thread
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_accept( const boost::thread::id thread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_realserver_send( const boost::thread::id thread_id )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorryserver_connection_fail(
									const boost::thread::id thread_id )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorryserver_send( const boost::thread::id thread_id )
{
	return STOP;
}

	//use in downstream_thread
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const int recvlen )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_response_send_inform( const boost::thread::id thread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & recv_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_client_send( const boost::thread::id thread_id )
{
	return STOP;
}

	//use in upstream/downstream thread
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_client_disconnect( const boost::thread::id thread_id )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorry_enable( const boost::thread::id upthread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorry_disable( const boost::thread::id upthread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test1::handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint )
{
	return STOP;
}
	
protocol_module_base::EVENT_TAG
protocol_module_test1::handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint )
{
	return STOP;
}

}

extern "C" l7vs::protocol_module_base*
create_module(){
	return (new l7vs::protocol_module_test1());
}

extern "C" void
destroy_module( l7vs::protocol_module_base* in ){
	delete in;
}
