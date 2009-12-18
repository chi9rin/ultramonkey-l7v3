#include <boost/asio.hpp>

#include "l7vsd.h"
#include "data_buff_base.h"
#include "tcp_thread_message.h"
//#include "tcp_thread_message_que.h"
#include "tcp_data.h"
#include "tcp_socket.h"
#include "tcp_session.h"
#include "udp_session.h"
#include "parameter.h"
#include "replication.h"
#include "protocol_module_control.h"
#include "schedule_module_control.h"
#include "error_code.h"
#include "stub.h"

// protocol_module_control　STUB code
l7vs::protocol_module_control&    l7vs::protocol_module_control::getInstance(){
    static    protocol_module_control    instance;
    return    instance;
}
void    l7vs::protocol_module_control::initialize( const std::string& infile_path ){}
void    l7vs::protocol_module_control::finalize(){}
l7vs::protocol_module_base*    l7vs::protocol_module_control::load_module( const std::string& modulename ){
    l7vs::protocol_module_base* return_value = NULL;

//     if( !pmcontrol_error_flag )
//         return_value = new protocol_module_test1;

    return return_value;
}
void    l7vs::protocol_module_control::unload_module( protocol_module_base* module_ptr ){
    delete    module_ptr;
}

l7vs::schedule_module_control&    l7vs::schedule_module_control::getInstance(){
    static    schedule_module_control    instance;
    return    instance;
}
void    l7vs::schedule_module_control::initialize( const std::string& infile_path ){}
void    l7vs::schedule_module_control::finalize(){}
l7vs::schedule_module_base*    l7vs::schedule_module_control::load_module( const std::string& modulename ){
    l7vs::schedule_module_base* return_value = NULL;

//     if( !smcontrol_error_flag )
//         return_value = new schedule_module_test1;

    return return_value;
}
void    l7vs::schedule_module_control::unload_module( schedule_module_base* module_ptr ){
    delete module_ptr;
}


// parameter stub

l7vs::Parameter::Parameter(){}

l7vs::Parameter::~Parameter(){}

bool    l7vs::Parameter::read_file( const PARAMETER_COMPONENT_TAG in,const std::string& filename ){    return true; }

int        l7vs::Parameter::get_int(    const PARAMETER_COMPONENT_TAG in_tag,
                                    const std::string& in_str,
                                    error_code& err,
					const std::string& file_name){
    int    retval = 0;
    if( debugg_flug_struct::getInstance().param_exist_flag() ){
        retval = SESSION_POOL_NUM_PARAM;
    }else{
        err.setter( true, "not exist value." );
    }
    return retval;
}

bool l7vs::Parameter::init( const PARAMETER_COMPONENT_TAG in_tag, const std::string& data){

    return(true);
}


std::string        l7vs::Parameter::get_string(const PARAMETER_COMPONENT_TAG in_tag,
                                            const std::string& in_str,
                                            error_code& err,const std::string& file_name){
    return "";
}


void l7vs::Parameter::get_multistring(const PARAMETER_COMPONENT_TAG,
                         const std::string&,
                         std::vector<std::string>&,
                         error_code&,const std::string& file_name ){


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

void    l7vs::Logger::loadConf(){}

l7vs::LOG_LEVEL_TAG l7vs::Logger::getLogLevel( l7vs::LOG_CATEGORY_TAG ){
    l7vs::LOG_LEVEL_TAG        ret_tag = l7vs::LOG_LV_DEBUG;
    return ret_tag;
}

bool    l7vs::Logger::setLogLevel( l7vs::LOG_CATEGORY_TAG cat, l7vs::LOG_LEVEL_TAG level ){
    return true;
}

void    l7vs::Logger::putLogFatal( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "FATAL : " << msg << std::endl;
}

void    l7vs::Logger::putLogError( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "ERROR : " << msg << std::endl;
}

void    l7vs::Logger::putLogWarn( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "WARN  : " << msg << std::endl;
}

void    l7vs::Logger::putLogInfo( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "INFO  : " << msg << std::endl;
}

void    l7vs::Logger::putLogDebug( l7vs::LOG_CATEGORY_TAG, const unsigned int, const std::string& msg, const char*, int ){
    std::cout << "DEBUG : " << msg << std::endl;
}

l7vs::l7vsd::l7vsd(){}
l7vs::l7vsd::~l7vsd(){}
l7vs::l7vsd::vslist_type::iterator    l7vs::l7vsd::search_vslist( const virtualservice_element& )    const{
    return vslist.begin();
}

void    l7vs::l7vsd::release_virtual_service( const virtualservice_element& ) const {}


int        l7vs::replication::initialize(){
    return 0;
}
void    l7vs::replication::finalize(){}
void    l7vs::replication::switch_to_master(){}
void    l7vs::replication::switch_to_slave(){}
void*    l7vs::replication::pay_memory( const std::string& inid, unsigned int& outsize ){
    return NULL;
}
void    l7vs::replication::dump_memory(){}
void    l7vs::replication::start(){}
void    l7vs::replication::stop(){}
void    l7vs::replication::force_replicate(){}
void    l7vs::replication::reset(){}
l7vs::replication::REPLICATION_MODE_TAG    l7vs::replication::get_status(){
    l7vs::replication::REPLICATION_MODE_TAG    retmode = l7vs::replication::REPLICATION_OUT;
    return retmode;
}
int        l7vs::replication::handle_send(){
    return 0;
}
void        l7vs::replication::handle_receive( const boost::system::error_code& err, size_t size ){
}
int        l7vs::replication::lock( const std::string& inid ){
    return 0;
}
int        l7vs::replication::unlock( const std::string& inid ){
    return 0;
}
int        l7vs::replication::refer_lock_mutex( const std::string& inid, mutex_ptr& outmutex ){
    return 0;
}

l7vs::tcp_realserver_connect_socket_list::tcp_realserver_connect_socket_list(){}

l7vs::tcp_realserver_connect_socket_list::~tcp_realserver_connect_socket_list(){}

void    l7vs::tcp_realserver_connect_socket_list::push_back(list_element realserver_socket){}

l7vs::tcp_realserver_connect_socket_list::list_element    l7vs::tcp_realserver_connect_socket_list::get_socket(){
    l7vs::tcp_realserver_connect_socket_list::list_element    retval;
    return retval;
}

bool    l7vs::tcp_realserver_connect_socket_list::empty(){
    return true;
}

bool    l7vs::tcp_socket::connect(const boost::asio::ip::tcp::endpoint connect_endpoint,boost::system::error_code& ec){
    return true;
}
bool    l7vs::tcp_socket::close(boost::system::error_code& ec){
    return true;
}
bool    l7vs::tcp_socket::set_non_blocking_mode(boost::system::error_code& ec){
    return true;
}
std::size_t        l7vs::tcp_socket::write_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
    size_t    ret = 0;
    return ret;
}
std::size_t        l7vs::tcp_socket::read_some(boost::asio::mutable_buffers_1 buffers, boost::system::error_code& ec){
    size_t    ret = 0;
    return ret;
}



l7vs::tcp_session::tcp_session(virtualservice_tcp& vs,
            boost::asio::io_service& session_io,
            const tcp_socket_option_info set_option,
            const boost::asio::ip::tcp::endpoint listen_endpoint,
            const bool ssl_mode,
            boost::asio::ssl::context& set_ssl_context,
            const bool set_ssl_cache_flag,
            const int set_ssl_handshake_time_out,
            logger_implement_access* set_access_logger)
        :
        io(session_io),
        parent_service(vs),
        exit_flag(false),
        thread_state(0),
        protocol_module(NULL),
        session_pause_flag(false),
        client_socket(session_io,set_option),
        upstream_buffer_size(8192),
        downstream_buffer_size(8192),
        virtualservice_endpoint(listen_endpoint),
        access_log_flag(false),
        access_logger(set_access_logger),
        ssl_flag(ssl_mode),
        client_ssl_socket(session_io, set_ssl_context,set_option),
        ssl_context(set_ssl_context),
        ssl_cache_flag(set_ssl_cache_flag),
        ssl_handshake_timer_flag(false),
        ssl_handshake_time_out(set_ssl_handshake_time_out),
        ssl_handshake_time_out_flag(false),
        socket_opt_info(set_option){

    exit_flag = false;
    session_pause_flag = false;
    std::cout << "SESSION:CREATE" << std::endl;
}
l7vs::tcp_session::~tcp_session(void){
    std::cout << "SESSION:DESTROY" << std::endl;
}
l7vs::session_result_message l7vs::tcp_session::initialize(void){
    l7vs::session_result_message    result;
    return result;
}

boost::asio::ip::tcp::socket& l7vs::tcp_session::get_client_socket(void){
    return client_socket.get_socket();
}

ssl_socket& l7vs::tcp_session::get_client_ssl_socket()
{
    return client_ssl_socket.get_socket();
}

bool    l7vs::tcp_session::is_thread_wait(void){
    return true;
}

//! handshake timer handler
void l7vs::tcp_session::handle_ssl_handshake_timer(const boost::system::error_code& error){};
//! reset ssl object for reuse
bool l7vs::tcp_session::ssl_clear_keep_cache(SSL *clear_ssl){
    return true;
}
std::string l7vs::tcp_session::endpoint_to_string( const boost::asio::ip::tcp::endpoint& target_endpoint){
    return "";
}; 

void    l7vs::tcp_session::set_virtual_service_message(const TCP_VIRTUAL_SERVICE_MESSAGE_TAG  message){
    switch( message ){
    case SORRY_STATE_ENABLE:
        {
            boost::mutex::scoped_lock( exit_flag_update_mutex );
            exit_flag = true;
        }
        break;
    case SORRY_STATE_DISABLE:
        {
            boost::mutex::scoped_lock( exit_flag_update_mutex );
            exit_flag = false;
        }
        break;
    case SESSION_END:
        {
            boost::mutex::scoped_lock( exit_flag_update_mutex );
            boost::mutex::scoped_lock( module_function_sorry_disable_mutex );
            exit_flag = true;
            session_pause_flag = true;
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
    case    ACCESS_LOG_ON:{
        std::cout<< "set_virtual_service_message called : ACCESS_LOG_ON" << std::endl;
        break;
    }
    case    ACCESS_LOG_OFF:{
        std::cout<< "set_virtual_service_message called : ACCESS_LOG_OFF" << std::endl;
        break;
    }


    }
}

void    l7vs::tcp_session::up_thread_run(void){
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

void    l7vs::tcp_session::down_thread_run(void){
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

void    l7vs::tcp_session::thread_state_update(const std::bitset<TCP_SESSION_THREAD_STATE_BIT> thread_flag,const bool regist){}
void    l7vs::tcp_session::up_thread_client_accept_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_client_receive(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_client_respond(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_send(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_connect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_send(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_get_destination_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_connect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_connect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_connection_fail_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::up_thread_all_socket_close(void){}

void    l7vs::tcp_session::down_thread_realserver_receive(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_realserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_all_realserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_client_connection_chk_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_client_respond_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_client_send(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_client_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_client_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_sorryserver_receive(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_sorryserver_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_sorryserver_disconnect_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_sorry_enable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_sorryserver_mod_disconnect(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_sorry_disable_event(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_exit(const TCP_PROCESS_TYPE_TAG process_type){}
void    l7vs::tcp_session::down_thread_all_socket_close(){}

void    l7vs::tcp_session::up_thread_client_accept(const TCP_PROCESS_TYPE_TAG process_type){}

l7vs::udp_session::udp_session(virtualservice_udp& vs, boost::asio::io_service& session_io) :    io( session_io ),
                                                                                                parent_service( vs ),
                                                                                                client_side_socket( io ) {}
l7vs::udp_session::~udp_session(){}
l7vs::session_result_message    l7vs::udp_session::initialize(const udp_endpoint listen_end){
    l7vs::session_result_message    result;
    return result;
}

void    l7vs::udp_session::set_virtual_service_message(const UDP_VIRTUAL_SERVICE_MESSAGE_TAG message){}
void    l7vs::udp_session::run(void){}
bool    l7vs::udp_session::client_send(const udp_endpoint client_endpoint, const udp_session_buff& data_buff,const std::size_t data_size,boost::system::error_code& ec){
    return true;
}
void    l7vs::udp_session::release_request(const boost::thread::id release_thread_id){}

void    l7vs::udp_session::make_request_thread(const int max_count){}
void    l7vs::udp_session::active_request_thread(const udp_endpoint client_endpoint , const udp_session_buff receive_data, const std::size_t receive_size){}
void    l7vs::udp_session::all_active_request_stop(void){}

