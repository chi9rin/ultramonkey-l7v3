#ifndef SNMPBRIDGE_TEST
#define SNMPBRIDGE_TEST

#include <boost/thread/condition.hpp>
#include "snmpbridge.h"

class l7vs::snmpbridge;


l7vs::l7vsd::l7vsd(){
	rep = replication_ptr( new replication() );
	bridge = snmpbridge_ptr( new snmpbridge( *this, dispatcher ) );
	vslist.clear();
}

l7vs::l7vsd::~l7vsd(){}

void l7vs::l7vsd::list_virtual_service( vselist_type* out_vslist, error_code& err ){
}

void l7vs::l7vsd::list_virtual_service_verbose( l7vsd_response* response, error_code&  ){
}

void l7vs::l7vsd::add_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
	// create virtualservice
	boost::shared_ptr< virtual_service > vsptr( new virtual_service( *this, *rep, *in_vselement ) );
	vslist.push_back( vsptr );
}

void l7vs::l7vsd::del_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		vslist.erase(vsitr);
	}
}

void l7vs::l7vsd::edit_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::add_real_server( const virtualservice_element* in_vselement, error_code& err ){
	vslist_type::iterator vsitr = search_vslist( *in_vselement );
	if( vslist.end() !=  vsitr ){
		// add realserver
		(*vsitr)->add_realserver( *in_vselement, err );
		if( err )	return;
	}
}

void l7vs::l7vsd::del_real_server( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::edit_real_server( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::flush_virtual_service( error_code& err ){
}

l7vs::l7vsd::vslist_type::iterator l7vs::l7vsd::search_vslist( const virtualservice_element& in_vselement ) const {
	for( vslist_type::iterator itr = vslist.begin();
		 itr != vslist.end();
		 ++itr ){
		if( (*itr)->get_element() == in_vselement )	return itr;
	}
	return vslist.end();
}

void l7vs::l7vsd::release_virtual_service( const virtualservice_element& in_vselement ) const {
}

l7vs::l7vsd::vslist_type& l7vs::l7vsd::get_virtualservice_list(){
	return vslist;
}

boost::mutex& l7vs::l7vsd::get_virtualservice_list_mutex(){
	return vslist_mutex;
}

int	l7vs::l7vsd::run( int argc, char* argv[] ) {
	return 0;
}

bool l7vs::l7vsd::check_options( int argc, char* argv[] ){
	return true;
}

bool l7vs::l7vsd::parse_help(int& pos, int argc, char* argv[] ){
	help = true;		//help_mode flag on
	return true;
}

bool l7vs::l7vsd::parse_debug(int& pos, int argc, char* argv[] ){
	debug = true;		//debug_mode flag on
	return true;
}

std::string	l7vs::l7vsd::usage(){
	std::stringstream	stream;
	stream <<
	"Usage: l7vsd [-d] [-h]\n"
	"   -d    --debug        run in debug mode (in foreground)\n"
	"   -h    --help         print this help messages and exit\n"
	<< std::endl;
	return stream.str();
}

l7vs::virtual_service::virtual_service(	const l7vsd& invsd,
						const replication& inrep,
						const virtualservice_element& inelement){
//	if( inelement.udpmode ){
//		vs = boost::shared_ptr<l7vs::virtualservice_base>(
//				dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_tcp( invsd, inrep, inelement ) ) );
//	}else{
//		vs = boost::shared_ptr<l7vs::virtualservice_base>(
//				dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_tcp( invsd, inrep, inelement ) ) );
//	}
	vs = boost::shared_ptr<l7vs::virtualservice_base>(
			dynamic_cast<l7vs::virtualservice_base*>( new l7vs::virtualservice_tcp( invsd, inrep, inelement ) ) );
}

l7vs::virtual_service::~virtual_service(){
}

void	l7vs::virtual_service::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	vs->add_realserver( in, err );
}

l7vs::virtualservice_element&	l7vs::virtual_service::get_element(){
	return vs->get_element();
}

l7vs::virtualservice_base::virtualservice_base(	const l7vs::l7vsd& invsd,
							const l7vs::replication& inrep,
							const l7vs::virtualservice_element& inelement)
												 :	vsd( invsd ),
													rep( inrep ),
													rs_list_ref_count( 0 ),
													recvsize_up( 0 ),
													current_up_recvsize( 0 ),
													sendsize_up( 0 ),
													recvsize_down( 0 ),
													current_down_recvsize( 0 ),
													sendsize_down( 0 ),
													throughput_up( 0 ),
													throughput_down( 0 ) {
	rs_list.clear();
	rs_mutex_list.clear();
	element = inelement;
	protomod = NULL;
	schedmod = NULL;
}

l7vs::virtualservice_element&	l7vs::virtualservice_base::get_element(){
	boost::mutex::scoped_lock lk( element_mutex );
	return element;
}

unsigned long long		l7vs::virtual_service::get_throughput_upstream(){
//	return vs->get_throughput_upstream();
	return 10;
}
unsigned long long		l7vs::virtual_service::get_throughput_downstream(){
//	return vs->get_throughput_downstream();
	return 10;
}

l7vs::virtualservice_tcp::virtualservice_tcp(	const l7vsd& invsd,
												const replication& inrep,
												const virtualservice_element& inelement )
												 :	virtualservice_base( invsd, inrep, inelement ),
													acceptor_( dispatcher ) {}

l7vs::virtualservice_tcp::~virtualservice_tcp(){
}

void	l7vs::virtualservice_tcp::handle_replication_interrupt( const boost::system::error_code& in ){
}

void	l7vs::virtualservice_tcp::read_replicationdata(){
}

void	l7vs::virtualservice_tcp::handle_accept(	const l7vs::virtualservice_tcp::session_thread_control_ptr in_session,
													const boost::system::error_code& in_error ){
}

void	l7vs::virtualservice_tcp::initialize( l7vs::error_code& err ){
}

void		l7vs::virtualservice_tcp::finalize( l7vs::error_code& err ){
}

bool	l7vs::virtualservice_tcp::operator==( const l7vs::virtualservice_base& in ){
	return true;
}

bool	l7vs::virtualservice_tcp::operator!=( const l7vs::virtualservice_base& in ){
	return false;
}

void	l7vs::virtualservice_tcp::set_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
}

void	l7vs::virtualservice_tcp::edit_virtualservice( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
}

void	l7vs::virtualservice_tcp::add_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
	l7vs::virtualservice_element&	in_element = const_cast<l7vs::virtualservice_element&>( in );
	for( std::vector<l7vs::realserver_element>::iterator itr = in_element.realserver_vector.begin();
		 itr != in_element.realserver_vector.end();
		 ++itr ){
		l7vs::realserver	rs;
		rs.tcp_endpoint		= itr->tcp_endpoint;
		rs.udp_endpoint		= itr->udp_endpoint;
		rs.weight			= itr->weight;
		rs_list.push_back( rs );
//		rs_mutex_list.insert( std::pair<tcp_endpoint_type,mutex_ptr>( rs.tcp_endpoint,
//																		mutex_ptr( new boost::mutex ) ) );
	}
}

void	l7vs::virtualservice_tcp::edit_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
}

void	l7vs::virtualservice_tcp::del_realserver( const l7vs::virtualservice_element& in, l7vs::error_code& err ){
}

void	l7vs::virtualservice_tcp::run(){
}

void	l7vs::virtualservice_tcp::stop(){
}

void	l7vs::virtualservice_tcp::connection_active( const boost::asio::ip::tcp::endpoint& in ){
}

void	l7vs::virtualservice_tcp::connection_inactive( const boost::asio::ip::tcp::endpoint& in ){
}

void	l7vs::virtualservice_tcp::release_session( const boost::thread::id thread_id ){}




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

bool get_int_errorflag = false;
bool get_string_errorflag = false;
std::string nic = NIC_DEFAULT;
std::string ip_addr = ADDR_DEFAULT;
int portno = PORT_DEFAULT;
int interval = INTERVAL_DEFAULT;
int status = 0;
std::string snmpagent_start_stop = "info";
std::string snmpagent_manager_receive = "info";
std::string snmpagent_manager_send = "info";
std::string snmpagent_l7vsd_receive = "info";
std::string snmpagent_l7vsd_send = "info";
std::string snmpagent_logger = "info";
std::string snmpagent_parameter = "info";
std::string snmpagent_system = "info";
std::string snmpagent_system_memory = "info";
std::string snmpagent_system_endpoint = "info";
std::string snmpagent_system_signal = "info";
std::string snmpagent_system_environment = "info";

void set_nic(std::string in){
	nic = in;
}
void set_ip_addr(std::string in){
	ip_addr = in;
}
void set_portno(int in){
	portno = in;
}
void set_interval(int in){
	interval = in;
}
void set_status(int in){
	status = in;
}
void set_snmpagent_start_stop(std::string in){
	snmpagent_start_stop = in;
}
void set_snmpagent_manager_receive(std::string in){
	snmpagent_manager_receive = in;
}
void set_snmpagent_manager_send(std::string in){
	snmpagent_manager_send = in;
}
void set_snmpagent_l7vsd_receive(std::string in){
	snmpagent_l7vsd_receive = in;
}
void set_snmpagent_l7vsd_send(std::string in){
	snmpagent_l7vsd_send = in;
}
void set_snmpagent_logger(std::string in){
	snmpagent_logger = in;
}
void set_snmpagent_parameter(std::string in){
	snmpagent_parameter = in;
}
void set_snmpagent_system(std::string in){
	snmpagent_system = in;
}
void set_snmpagent_system_memory(std::string in){
	snmpagent_system_memory = in;
}
void set_snmpagent_system_endpoint(std::string in){
	snmpagent_system_endpoint = in;
}
void set_snmpagent_system_signal(std::string in){
	snmpagent_system_signal = in;
}
void set_snmpagent_system_environment(std::string in){
	snmpagent_system_environment = in;
}

std::map<std::string, l7vs::LOG_LEVEL_TAG> levelstring_map;

l7vs::Parameter::Parameter(){}

l7vs::Parameter::~Parameter(){}

bool l7vs::Parameter::read_file(PARAMETER_COMPONENT_TAG comp){
	return true;
}

int l7vs::Parameter::get_int(	const l7vs::PARAMETER_COMPONENT_TAG comp,
								const std::string& key,
								l7vs::error_code& err ){
	int ret = 0;
	if( get_int_errorflag ){
		err.setter( true, "" );
		return ret;
	}
	if( comp == l7vs::PARAM_COMP_SNMPAGENT ){
		if( key == "port" ){
			ret = portno;
		}
		else if( key == "interval" ){
			ret = interval;
		}
		else if( key == "status" ){
			ret = status;
		}
	}
	return ret;
}

std::string l7vs::Parameter::get_string(		const l7vs::PARAMETER_COMPONENT_TAG comp,
												const std::string& key,
												l7vs::error_code& err ){
	std::string buf( "" );
	if( get_string_errorflag ){
		err.setter( true, "" );
		return buf;
	}
	if( comp == l7vs::PARAM_COMP_SNMPAGENT ){
		if( key == "nic" ){
			buf = nic;
		}
		else if( key == "ip_addr" ){
			buf = ip_addr;
		}
	}
	else if( l7vs::PARAM_COMP_LOGGER ){
		if( key == "snmpagent_start_stop" ){
			buf = snmpagent_start_stop;
		}
		else if( key == "snmpagent_manager_receive" ){
			buf = snmpagent_manager_receive;
		}
		else if( key == "snmpagent_manager_send" ){
			buf = snmpagent_manager_send;
		}
		else if( key == "snmpagent_l7vsd_receive" ){
			buf = snmpagent_l7vsd_receive;
		}
		else if( key == "snmpagent_l7vsd_send" ){
			buf = snmpagent_l7vsd_send;
		}
		else if( key == "snmpagent_logger" ){
			buf = snmpagent_logger;
		}
		else if( key == "snmpagent_parameter" ){
			buf = snmpagent_parameter;
		}
		else if( key == "snmpagent_system" ){
			buf = snmpagent_system;
		}
		else if( key == "snmpagent_system_memory" ){
			buf = snmpagent_system_memory;
		}
		else if( key == "snmpagent_system_endpoint" ){
			buf = snmpagent_system_endpoint;
		}
		else if( key == "snmpagent_system_signal" ){
			buf = snmpagent_system_signal;
		}
		else if( key == "snmpagent_system_environment" ){
			buf = snmpagent_system_environment;
		}
	}
	return buf;
}


#endif // SNMPBRIDGE_TEST
