#include	"protocol_module_base.h"

namespace l7vs{

class	protocol_module_test2 : public protocol_module_base{
public:
	protocol_module_test2();
	~protocol_module_test2();

	void	initialize(
							rs_list_itr_func_type	inlist_begin,
							rs_list_itr_func_type	inlist_end,
							rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock );

	void	finalize();

	bool	is_tcp();
	bool	is_udp();
	bool	is_use_sorry();
	check_message_result	check_parameter( const std::vector<std::string>& args );

	void	handle_rslist_update();

	check_message_result	set_parameter( const std::vector<std::string>& args );
	check_message_result	add_parameter( const std::vector<std::string>& args );

	void	register_schedule( udp_schedule_func_type inschedule );

	void	register_replication_area_lock(
									boost::function< void( void ) > inlock_func );

	void	register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func );

	void	replication_interrupt(){}

	//use in upstream_thread
	EVENT_TAG	handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id );

	EVENT_TAG	handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id );

	EVENT_TAG	handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const size_t recvlen );

	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen );

	
	EVENT_TAG	handle_realserver_send( const boost::thread::id thread_id );
	

	//use in downstream_thread
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const size_t recvlen );
	EVENT_TAG	handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen );
	EVENT_TAG	handle_client_send( const boost::thread::id thread_id );

	//use in upstream/downstream thread
	EVENT_TAG	handle_client_disconnect( const boost::thread::id thread_id );

	EVENT_TAG	handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint );

	//unuse method
	void	register_schedule( tcp_schedule_func_type inschedule ){}
	EVENT_TAG	handle_accept( const boost::thread::id thread_id ){return STOP;}
	EVENT_TAG	handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint& rs_endpoint ){return STOP;}
	EVENT_TAG	handle_realserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}
	EVENT_TAG	handle_realserver_connection_fail(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint ){return STOP;}
	EVENT_TAG	handle_response_send_inform( const boost::thread::id thread_id ){return STOP;}
	EVENT_TAG	handle_sorryserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::tcp::endpoint & sorry_endpoint ){return STOP;}
	EVENT_TAG	handle_sorryserver_connect(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen ){return STOP;}
	EVENT_TAG	handle_sorryserver_connection_fail(
									const boost::thread::id thread_id ){return STOP;}
	EVENT_TAG	handle_sorryserver_send( const boost::thread::id thread_id ){return STOP;}
	EVENT_TAG	handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const size_t recvlen ){return STOP;}
	EVENT_TAG	handle_sorryserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const size_t recvlen ){return STOP;}
	EVENT_TAG	handle_client_connection_check(
									const boost::thread::id thread_id,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									int& datalen ){return STOP;}
	EVENT_TAG	handle_sorry_enable( const boost::thread::id upthread_id ){return STOP;}
	EVENT_TAG	handle_sorry_disable( const boost::thread::id upthread_id ){return STOP;}
	EVENT_TAG	handle_realserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & rs_endpoint ){return STOP;}
	
	EVENT_TAG	handle_sorryserver_disconnect(
									const boost::thread::id thread_id,
									const boost::asio::ip::tcp::endpoint & sorry_endpoint ){return STOP;}
};

protocol_module_test2::protocol_module_test2() : protocol_module_base()
{
	name = "test1";
}

protocol_module_test2::~protocol_module_test2(){}

void
protocol_module_test2::initialize(
							protocol_module_base::rs_list_itr_func_type	inlist_begin,
							protocol_module_base::rs_list_itr_func_type	inlist_end,
							protocol_module_base::rs_list_itr_func_type	inlist_next,
							boost::function< void( void ) >	inlist_lock,
							boost::function< void( void ) >	inlist_unlock ){}

void
protocol_module_test2::finalize(){}

bool
protocol_module_test2::is_tcp(){ return false; }
bool
protocol_module_test2::is_udp(){ return true; }

bool
protocol_module_test2::is_use_sorry(){ return false; }

protocol_module_base::check_message_result
protocol_module_test2::check_parameter( const std::vector<std::string>& args ){
	protocol_module_base::check_message_result	result;
	result.flag = true;
	return result;
}

void
protocol_module_test2::handle_rslist_update(){}

protocol_module_base::check_message_result
protocol_module_test2::set_parameter( const std::vector<std::string>& args ){
	protocol_module_base::check_message_result	result;
	result.flag = true;
	return result;
}

protocol_module_base::check_message_result
protocol_module_test2::add_parameter( const std::vector<std::string>& args ){
	protocol_module_base::check_message_result	result;
	result.flag = true;
	return result;
}

void
protocol_module_test2::register_schedule( protocol_module_base::udp_schedule_func_type inschedule ){}

void
protocol_module_test2::register_replication_area_lock(
									boost::function< void( void ) > inlock_func ){}

void
protocol_module_test2::register_replication_area_unlock(
									boost::function< void( void ) > inunlock_func ){}

	//use in upstream_thread
protocol_module_base::EVENT_TAG
protocol_module_test2::handle_session_initialize(
									const boost::thread::id upthread_id,
									const boost::thread::id down_thread_id )
{
	return STOP;
}


protocol_module_base::EVENT_TAG
protocol_module_test2::handle_session_finalize(
									const boost::thread::id up_thread_id,
									const boost::thread::id down_thread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test2::handle_client_recv(
									const boost::thread::id thread_id,
									const boost::array<char,MAX_BUFFER_SIZE>& recvbuffer,
									const int recvlen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test2::handle_realserver_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test2::handle_realserver_send( const boost::thread::id thread_id )
{
	return STOP;
}
	

	//use in downstream_thread
protocol_module_base::EVENT_TAG
protocol_module_test2::handle_realserver_recv(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint,
									const boost::array<char,MAX_BUFFER_SIZE>& readbuffer,
									const size_t recvlen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test2::handle_client_select(
									const boost::thread::id thread_id,
									boost::asio::ip::udp::endpoint& rs_endpoint,
									boost::array<char,MAX_BUFFER_SIZE>& sendbuffer,
									size_t& datalen )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test2::handle_client_send( const boost::thread::id thread_id )
{
	return STOP;
}

	//use in upstream/downstream thread
protocol_module_base::EVENT_TAG
protocol_module_test2::handle_client_disconnect( const boost::thread::id thread_id )
{
	return STOP;
}

protocol_module_base::EVENT_TAG
protocol_module_test2::handle_realserver_close(
									const boost::thread::id thread_id,
									const boost::asio::ip::udp::endpoint & rs_endpoint )
{
	return STOP;
}

}

extern "C" l7vs::protocol_module_base*
create_module(){
	return dynamic_cast<l7vs::protocol_module_base*>(new l7vs::protocol_module_test2());
}

extern "C" void
destroy_module( l7vs::protocol_module_base* in ){
	delete in;
}
