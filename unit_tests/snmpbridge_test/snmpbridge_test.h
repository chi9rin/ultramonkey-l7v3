#ifndef SNMPBRIDGE_TEST
#define SNMPBRIDGE_TEST

l7vs::l7vsd::l7vsd(){}

l7vs::l7vsd::~l7vsd(){}

void l7vs::l7vsd::list_virtual_service( vselist_type* out_vslist, error_code& err ){
}

void l7vs::l7vsd::list_virtual_service_verbose( l7vsd_response* response, error_code&  ){
}

void l7vs::l7vsd::add_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
//temp
	// create virtualservice
	boost::shared_ptr< virtual_service >
		vsptr( new virtual_service( *this, *rep, *in_vselement ) );
	// vs initialize
//	vsptr->initialize( err );
//	vsptr->set_virtualservice( *in_vselement, err );

	vslist.push_back( vsptr );
//temp
}

void l7vs::l7vsd::del_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::edit_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::add_real_server( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::del_real_server( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::edit_real_server( const virtualservice_element* in_vselement, error_code& err ){
}

void l7vs::l7vsd::flush_virtual_service( error_code& err ){
}

l7vs::l7vsd::vslist_type::iterator l7vs::l7vsd::search_vslist( const virtualservice_element& in_vselement ) const {
	vslist_type::iterator itr = vslist.begin();
	return itr;
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

l7vs::virtual_service::virtual_service(	const l7vsd& ,
						const replication& ,
						const virtualservice_element& ){
}

l7vs::virtual_service::~virtual_service(){
}


l7vs::virtualservice_element&	l7vs::virtual_service::get_element(){
	return vs->get_element();
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
	}
	return buf;
}

#endif // SNMPBRIDGE_TEST
