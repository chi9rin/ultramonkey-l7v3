#ifndef SNMPBRIDGE_TEST
#define SNMPBRIDGE_TEST

l7vs::l7vsd::l7vsd(){}

l7vs::l7vsd::~l7vsd(){}

void l7vs::l7vsd::list_virtual_service( vselist_type* out_vslist, error_code& err ){
}

void l7vs::l7vsd::list_virtual_service_verbose(	vselist_type* out_vslist, 
												replication::REPLICATION_MODE_TAG* repmode,
												logstatus_list_type* logstatus,
												bool*	snmpstatus,
												logstatus_list_type* snmplogstatus,
												error_code& err ){
}

void l7vs::l7vsd::add_virtual_service( const virtualservice_element* in_vselement, error_code& err ){
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

int portno = 60162;//temp
int interval = 1000;//temp

void set_portno(int in){
	portno = in;
}

void set_interval(int in){
	interval = in;
}

l7vs::Parameter::Parameter(){}

l7vs::Parameter::~Parameter(){}

bool l7vs::Parameter::read_file(PARAMETER_COMPONENT_TAG comp){
	return true;
}

int l7vs::Parameter::get_int(	const l7vs::PARAMETER_COMPONENT_TAG comp,
								const std::string& key,
								l7vs::error_code& err ){
	int ret = 0;
	if(key == "port"){
		ret = portno;
	}
	else if(key == "interval"){
		ret = interval;
	}
	return ret;
}

std::string l7vs::Parameter::get_string(		const l7vs::PARAMETER_COMPONENT_TAG comp,
												const std::string& key,
												l7vs::error_code& err ){
	std::string buf("parameter");
	return buf;
}

#endif // SNMPBRIDGE_TEST
