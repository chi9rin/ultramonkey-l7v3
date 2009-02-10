#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "data_buff_base.h"
#include "tcp_thread_message.h"
#include "tcp_thread_message_que.h"
#include "tcp_data.h"
#include "tcp_socket.h"
#include "tcp_session.h"
#include "udp_session.h"
#include "parameter.h"
#include "replication.h"
#include "stub.h"

#include "protocol_module_test1.h"
#include "schedule_module_test1.h"

// protocol_module_control　STUB code
l7vs::protocol_module_control&	l7vs::protocol_module_control::getInstance(){
	static	protocol_module_control	instance;
	return	instance;
}
void	l7vs::protocol_module_control::initialize( const std::string& infile_path ){}
void	l7vs::protocol_module_control::finalize(){}
l7vs::protocol_module_base*	l7vs::protocol_module_control::load_module( const std::string& modulename ){
	l7vs::protocol_module_base* return_value = NULL;

	if( !debugg_flug_struct::getInstance().pmcontrol_err_flag() )
		return_value = new protocol_module_test1;

	return return_value;
}
void	l7vs::protocol_module_control::unload_module( protocol_module_base* module_ptr ){
	delete	module_ptr;
}

l7vs::schedule_module_control&	l7vs::schedule_module_control::getInstance(){
	static	schedule_module_control	instance;
	return	instance;
}
void	l7vs::schedule_module_control::initialize( const std::string& infile_path ){}
void	l7vs::schedule_module_control::finalize(){}
l7vs::schedule_module_base*	l7vs::schedule_module_control::load_module( const std::string& modulename ){
	l7vs::schedule_module_base* return_value = NULL;

	if( !debugg_flug_struct::getInstance().smcontrol_err_flag() )
		return_value = new schedule_module_test1;

	return return_value;
}

void	l7vs::schedule_module_control::unload_module( schedule_module_base* module_ptr ){
	delete module_ptr;
}



int		l7vs::replication::initialize(){
	return 0;
}
void	l7vs::replication::finalize(){}
void	l7vs::replication::switch_to_master(){}
void	l7vs::replication::switch_to_slave(){}
void*	l7vs::replication::pay_memory( const std::string& inid, unsigned int& outsize ){
	void* retval = debugg_flug_struct::getInstance().get_rep_area();
	outsize = l7vs::virtualservice_base::MAX_REPLICATION_DATA_NUM;
	return retval;
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
int		l7vs::replication::handle_send(){
	return 0;
}
void		l7vs::replication::handle_receive( const boost::system::error_code& err, size_t size ){
}
int		l7vs::replication::lock( const std::string& inid ){
	return 0;
}
int		l7vs::replication::unlock( const std::string& inid ){
	return 0;
}
int		l7vs::replication::refer_lock_mutex( const std::string& inid, mutex_ptr outmutex ){
	return 0;
}


// parameter stub
l7vs::Parameter::Parameter(){}

l7vs::Parameter::~Parameter(){}

bool	l7vs::Parameter::read_file( const PARAMETER_COMPONENT_TAG in ){	return true; }

int		l7vs::Parameter::get_int(	const PARAMETER_COMPONENT_TAG in_tag,
									const std::string& in_str,
									error_code& err ){
	int	retval = 0;
	if( debugg_flug_struct::getInstance().param_exist_flag() ){
		retval = SESSION_POOL_NUM_PARAM;
	}else{
		err.setter( true, "not exist value." );
	}
	return retval;
}

std::string		l7vs::Parameter::get_string(const PARAMETER_COMPONENT_TAG in_tag,
											const std::string& in_str,
											error_code& err){
	return "";
}

void	l7vs::session_thread_control::upstream_run(){
	state_tag	state;
	{	// get first state from class upstream state.
		boost::mutex::scoped_lock upcond_lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
	boost::mutex::scoped_lock up_exit_lock( upthread_exit_mutex );
	for(;;){	// thread loop
		if( state == WAIT ){	// after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( upthread_condition_mutex );
			upthread_condition.wait( lock );	// thread is condition wait.( start at notify_all() )
		}
		else if( state == EXIT ){ // this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session->up_thread_run();	//session upstream thread looping.
			stopupstream();
		}
		boost::mutex::scoped_lock	upcond_lock( upthread_condition_mutex );	// upstream state lock
		state = upthread_state;	//thread local state is update.
	}
}

void	l7vs::session_thread_control::downstream_run(){
	state_tag	state;
	{
		boost::mutex::scoped_lock downcond_lock( downthread_condition_mutex );	//downstream state is lock
		state = downthread_state;	//thread local state is update.
	}
	boost::mutex::scoped_lock down_exit_lock( downthread_exit_mutex );
	for(;;){	//thread loop
		if( state == WAIT ){	//after create or session end. this thread is pooling mode
			boost::mutex::scoped_lock	lock( downthread_condition_mutex );
			downthread_condition.wait( lock ); // thread is condition wait( start at notify_all() )
		}
		else if( state == EXIT ){// this state is vitrualservice end. thread is finishing.
			break;
		}
		else{	//state RUNNING
			session->down_thread_run();//session downstream thread looping.
			stopdownstream();
		}
		boost::mutex::scoped_lock	downcond_lock( downthread_condition_mutex );	//downstream state lock
		state = downthread_state;	// thread local sate is update.
	}
}

void	l7vs::session_thread_control::startupstream(){
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	//upstream state lock
	if( upthread_state != EXIT ) upthread_state = RUNNING;		// upthread state update.[RUNNING] -> alive mode
	upthread_condition.notify_all();							// conditionwait upstreamthread is run.
}

void	l7vs::session_thread_control::stopupstream(){
	boost::mutex::scoped_lock	lock( upthread_condition_mutex );	// upstream state lock
	if( upthread_state != EXIT ) upthread_state = WAIT;				// upthread state is update [WAIT] -> pooling mode
}

void	l7vs::session_thread_control::startdownstream(){
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );		// downstream state lock
	if( downthread_state != EXIT ) downthread_state = RUNNING;		// downstream state is update [RUNNING] -> alive mode
	downthread_condition.notify_all();								// condition wait thread is run.
}

void	l7vs::session_thread_control::stopdownstream(){
	boost::mutex::scoped_lock	lock( downthread_condition_mutex );	// downstream state lock
	if( downthread_state != EXIT ) downthread_state = WAIT;			// downstream state is update [WAIT] -> pooling mode
}

void	l7vs::session_thread_control::join(){
	boost::mutex::scoped_lock	uplock( upthread_condition_mutex );	//upstream state lock
	upthread_state = EXIT;	//upstream state update [EXIT] -> thread exit mode
	upthread_condition.notify_all();	// conditionwait thread is run
	boost::mutex::scoped_lock( upthread_exit_mutex );
	boost::mutex::scoped_lock	downlock( downthread_condition_mutex );//downstream state is lock
	downthread_state = EXIT;	//downstream state update [EXIT] -> thread exit mode
	downthread_condition.notify_all(); //condition wait thread is run.
	boost::mutex::scoped_lock( downthread_exit_mutex );
}


l7vs::Logger::Logger() :
	scopedLogCategory(LOG_CAT_L7VSD_LOGGER),
	scopedLogId(0),
	scopedLogMessage("Logger Constructor"),
	scopedLogFile(__FILE__),
	scopedLogLine(__LINE__) {}

l7vs::Logger::Logger( LOG_CATEGORY_TAG cat, const unsigned int id, const std::string& msg, const char* file, int line) :
	scopedLogCategory(cat),
	scopedLogId(id),
	scopedLogMessage(msg),
	scopedLogFile(file),
	scopedLogLine(line) {}

l7vs::Logger::~Logger(){}

void	l7vs::Logger::loadConf(){}

l7vs::LOG_LEVEL_TAG l7vs::Logger::getLogLevel( l7vs::LOG_CATEGORY_TAG ){
	l7vs::LOG_LEVEL_TAG		ret_tag = l7vs::LOG_LV_DEBUG;
	return ret_tag;
}

bool	l7vs::Logger::setLogLevel( l7vs::LOG_CATEGORY_TAG cat, l7vs::LOG_LEVEL_TAG level ){
	return true;
}

void	l7vs::Logger::putLogFatal( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
	std::cout << "FATAL : " << msg << std::endl;
}

void	l7vs::Logger::putLogError( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
	std::cout << "ERROR : " << msg << std::endl;
}

void	l7vs::Logger::putLogWarn( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
	std::cout << "WARN  : " << msg << std::endl;
}

void	l7vs::Logger::putLogInfo( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
	std::cout << "INFO  : " << msg << std::endl;
}

void	l7vs::Logger::putLogDebug( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
	std::cout << "DEBUG : " << msg << std::endl;
}


l7vs::l7vsd::l7vsd(){}
l7vs::l7vsd::~l7vsd(){}
l7vs::l7vsd::vslist_type::iterator	l7vs::l7vsd::search_vslist( const virtualservice_element& )	const{
	return vslist.begin();
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
	l7vs::tcp_realserver_connect_socket_list::list_element	retval;
	return retval;
}
bool	l7vs::tcp_realserver_connect_socket_list::empty(){
	return true;
}

l7vs::tcp_thread_message::tcp_thread_message(){}
l7vs::tcp_thread_message::~tcp_thread_message(){}

l7vs::tcp_thread_message_que::tcp_thread_message_que(){}
l7vs::tcp_thread_message_que::~tcp_thread_message_que(){}
void	l7vs::tcp_thread_message_que::push(tcp_thread_message_ptr message){}
l7vs::tcp_thread_message_que::tcp_thread_message_ptr	l7vs::tcp_thread_message_que::front(){
	return message_que.front();
}
bool	l7vs::tcp_thread_message_que::empty(){
	return true;
}
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
																									client_socket( io ) {}
l7vs::tcp_session::~tcp_session(void){}
l7vs::session_result_message l7vs::tcp_session::initialize(void){
	l7vs::session_result_message	result;
	return result;
}

boost::asio::ip::tcp::socket& l7vs::tcp_session::get_client_socket(void){
	return client_socket.get_socket();
}

bool	l7vs::tcp_session::is_thread_wait(void){
	return true;
}

void	l7vs::tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message){
	if( message == SESSION_END ){
		exit_flag = true;
	}
}

void	l7vs::tcp_session::up_thread_run(void){
	exit_flag = false;
	while(!exit_flag){
		
	}
	parent_service.release_session( boost::this_thread::get_id() );
}

void	l7vs::tcp_session::down_thread_run(void){}

void	l7vs::tcp_session::thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist){}
void	l7vs::tcp_session::up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::up_thread_all_socket_close(void){}

void	l7vs::tcp_session::down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){}
void	l7vs::tcp_session::down_thread_all_socket_close(){}


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

