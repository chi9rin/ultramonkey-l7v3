#include "sslid_to_be_test_file.h"
#include "../../include/protocol_module_base.h"
#include <string.h>

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

//log stub function
char hostname[] = "127.0.0.1";

LOG_LEVEL_TAG	stb_getloglevel(){
	//return LOG_LV_DEBUG;
	return LOG_LV_NONE;
}
void	stb_putLogFatal( const unsigned int message_id, const std::string& message, const char* file, int line){
	cout << boost::format( "%s%d%06d %s %s" )
				% "PM"
				% LOG_LV_FATAL
				% message_id
				% message.c_str()
				% hostname;
    cout << endl;
}
void	stb_putLogError( const unsigned int message_id, const std::string& message, const char* file, int line){
	cout << boost::format( "%s%d%06d %s %s" )
				% "PM"
				% LOG_LV_ERROR
				% message_id
				% message.c_str()
				% hostname;
    cout << endl;
}
void	stb_putLogWarn( const unsigned int message_id, const std::string& message, const char* file, int line){
	cout << boost::format( "%s%d%06d %s %s" )
				% "PM"
				% LOG_LV_WARN
				% message_id
				% message.c_str()
				% hostname;
    cout << endl;
}
void	stb_putLogInfo( const unsigned int message_id, const std::string& message, const char* file, int line){
	cout << boost::format( "%s%d%06d %s %s" )
				% "PM"
				% LOG_LV_INFO
				% message_id
				% message.c_str()
				% hostname;
    cout << endl;
}
void	stb_putLogDebug( const unsigned int message_id, const std::string& message, const char* file, int line){
	cout << boost::format( "%s%d%06d %s %s" )
				% "PM"
				% LOG_LV_DEBUG
				% message_id
				% message.c_str()
				% hostname;
    cout << endl;
}

//new operator function
static bool new_flg = false;
void new_install(){
	new_flg = true;
}
void new_uninstall(){
	new_flg = false;
}
void* operator new[](size_t size){
	if (new_flg)
		throw std::bad_alloc();
	else
	{
		void* p = malloc(size);
		return p;
	}
}
void operator delete[](void* p){
	free(p);
}

std::list<realserver> rs_list;
bool function_check_flag = false;
//RealServerリストの各操作関数
//RealServerリスト先頭取得関数
protocol_module_sslid::realserverlist_type::iterator rslist_begin() {
	function_check_flag = true;
	return rs_list.begin();
}
//RealServerリスト末端取得関数
protocol_module_sslid::realserverlist_type::iterator rslist_end() {
	function_check_flag = true;
	return rs_list.end();
}
//RealServerリスト次要素取得関数
protocol_module_sslid::realserverlist_type::iterator rslist_next(protocol_module_sslid::realserverlist_type::iterator itr) {
	function_check_flag = true;
	return ++rs_list.begin();
}
//RealServerリストロック関数
void rslist_lock() {
	function_check_flag = true;
}
//ealServerリストアンロック関数
void rslist_unlock() {
	function_check_flag = true;
}

void schedule_tcp_func1(const boost::thread::id thread_id, boost::function<
		std::list<realserver>::iterator(void)>, boost::function<std::list<
		realserver>::iterator(void)>, boost::function<
		std::list<realserver>::iterator(std::list<realserver>::iterator)>,
		boost::asio::ip::tcp::endpoint& rs_endpoint) {
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(
			"192.168.120.249"), 1234);
	rs_endpoint = ep;

}

void schedule_tcp_func2(const boost::thread::id thread_id, boost::function<
		std::list<realserver>::iterator(void)>, boost::function<std::list<
		realserver>::iterator(void)>, boost::function<
		std::list<realserver>::iterator(std::list<realserver>::iterator)>,
		boost::asio::ip::tcp::endpoint& rs_endpoint) {
	boost::asio::ip::tcp::endpoint ep;
	rs_endpoint = ep;
}

void* replication_pay_memory_stb(const std::string& module_name, unsigned int* data_size)
{
	*data_size = 400;
	void* data_addr = malloc( *data_size * DATA_SIZE );
	memset( data_addr, 0, *data_size * DATA_SIZE );
	sslid_replication_data_header* replication_header;
	replication_header=(sslid_replication_data_header *)data_addr;
	strcpy(replication_header->virtualserver_ip,"192.168.120.246");
	replication_header->virtualserver_port=8888;
	replication_header->offset=128*sizeof(sslid_replication_data_header);
	replication_header->size=1024*sizeof(sslid_replication_data);
	sslid_replication_data* replication_data=(sslid_replication_data*)((sslid_replication_data_header*)data_addr+128);
	strcpy(replication_data->session_id,"0123456789695442355522125555987");
	strcpy(replication_data->realserver_ip,"192.168.120.246");
	replication_data->realserver_port=8888;
	replication_data->last_time=time(0);
	replication_data->valid=1;

	return data_addr;
}

bool check_register_schedule=false;
bool check_replication_area_lock=false;
bool check_replication_area_unlock=false;

void t_schedule(const boost::thread::id w,
		protocol_module_base::rs_list_itr_func_type e,
		protocol_module_base::rs_list_itr_func_type r,
		protocol_module_base::rs_list_itr_next_func_type t,
		boost::asio::ip::tcp::endpoint& u){
	check_register_schedule=true;
}

void replication_area_lock_stb(){
	check_replication_area_lock=true;
}

void replication_area_unlock_stb(){
	check_replication_area_unlock=true;
}

//Thread function
void up_thread_func() {
}
void down_thread_func() {
}

class sslid_session_data_processor_stub : public sslid_session_data_processor
{
    public:
        sslid_session_data_processor_stub(
                            int maxlist,
                            int timeout,
                            sslid_replication_data_processor* replication_data_processor,
                            getloglevel_func_type	ingetloglevel,
							logger_func_type inputLogFatal,
							logger_func_type inputLogError,
							logger_func_type inputLogWarn,
							logger_func_type inputLogInfo,
							logger_func_type inputLogDebug ):
							sslid_session_data_processor(maxlist,
									timeout,
									replication_data_processor,
									ingetloglevel,
									inputLogFatal,
									inputLogError,
									inputLogWarn,
									inputLogInfo,
									inputLogDebug){}

        ~sslid_session_data_processor_stub(){}

		std::map<std::string, boost::asio::ip::tcp::endpoint>& get_session_endpoint_map()
		{
			return session_endpoint_map;
		}

		std::map<std::string, time_t>& get_session_lasttime_map()
		{
			return session_lasttime_map;
		}

		std::multimap<time_t, std::string>& get_lasttime_session_map()
		{
			return lasttime_session_map;
		}

};

class sslid_replication_data_processor_stub : public sslid_replication_data_processor
{
    public:
    	sslid_replication_data_processor_stub(
    			int maxlist,
    			char* sslid_replication_area_begain,
    			int sslid_replication_area_size,
    			const boost::asio::ip::tcp::endpoint& virtual_service_endpoint,
    			getloglevel_func_type	ingetloglevel,
    			logger_func_type inputLogFatal,
    			logger_func_type inputLogError,
    			logger_func_type inputLogWarn,
    			logger_func_type inputLogInfo,
    			logger_func_type inputLogDebug  ):
							sslid_replication_data_processor(maxlist,
									sslid_replication_area_begain,
									sslid_replication_area_size,
									virtual_service_endpoint,
									ingetloglevel,
									inputLogFatal,
									inputLogError,
									inputLogWarn,
									inputLogInfo,
									inputLogDebug){}

        ~sslid_replication_data_processor_stub(){}

        sslid_replication_data*& get_replication_area()
		{
			return replication_area;
		}

        std::deque<sslid_replication_temp_data>& get_temp_list()
		{
			return temp_list;
		}

};

//test class
class protocol_module_sslid_test_class: public protocol_module_sslid {
public:

void install_stb_log_func() {
    typedef boost::function<void(const unsigned int, const std::string&,
            const char*, int)> log_func_type;

    boost::function<LOG_LEVEL_TAG(void)> getloglevel = &stb_getloglevel;
    log_func_type putLogFatal = &stb_putLogFatal;
    log_func_type putLogError = &stb_putLogError;
    log_func_type putLogWarn = &stb_putLogWarn;
    log_func_type putLogInfo = &stb_putLogInfo;
    log_func_type putLogDebug = &stb_putLogDebug;
    this->init_logger_functions(getloglevel, putLogFatal, putLogError,
            putLogWarn, putLogInfo, putLogDebug);
}

void install_stb_replication_func(){
	boost::function< void* ( const std::string&, unsigned int* ) >  inreplication_pay_memory=&replication_pay_memory_stb;
	boost::function<void (void)> inlock_func=&replication_area_lock_stb;
	boost::function<void (void)> inunlock_func=&replication_area_unlock_stb;
	boost::asio::ip::tcp::endpoint tcp_ep;
	boost::asio::ip::udp::endpoint udp_ep;
	this->init_replication_functions(inreplication_pay_memory,inlock_func,inunlock_func,tcp_ep,udp_ep);
}

protocol_module_sslid_test_class() {
    install_stb_log_func();
}

//protocol_module_sslid
void protocol_module_sslid_test() {

    cout << "[1]------------------------------------------" << endl;
    //unit_test[1] モジュール名が"sslid"に設定する。
    protocol_module_sslid obj;
    BOOST_CHECK_EQUAL(obj.get_name(), "sslid");

    cout << "[2]------------------------------------------" << endl;
    //unit_test[2] sslidのインスタンスを生成する。
    protocol_module_sslid* p = new protocol_module_sslid();
    BOOST_CHECK_EQUAL(p->get_name(), "sslid");
    delete p;
}

//~protocol_module_sslid

//is_tcp
void is_tcp_test(){
    cout << "[3]------------------------------------------" << endl;
	//unit_test[3] is_tcp()メソッドのテスト,正常系で必ずTRUEを返す
    BOOST_CHECK(this->is_tcp());
}

//is_udp
void is_udp_test(){
    cout << "[4]------------------------------------------" << endl;
	//unit_test[4] is_udp()メソッドのテスト,正常系で必ずFALSEを返す
    BOOST_CHECK(!this->is_udp());
}

//get_name
void get_name_test(){
    cout << "[5]------------------------------------------" << endl;
	//unit_test[5]　get_name()メソッドのテスト,正常系で必ず"sslid"を返す
	BOOST_CHECK_EQUAL(this->get_name(), "sslid");
}

//initialize
void initialize_test() {
    boost::function<std::list<realserver>::iterator(void)> inrslist_begin =
            rslist_begin;
    boost::function<std::list<realserver>::iterator(void)> inrslist_end =
            rslist_end;
    boost::function<std::list<realserver>::iterator(std::list<realserver>::iterator)> inrslist_next =
            rslist_next;
    std::list<realserver>::iterator itr;
    boost::function<void(void)> inlist_lock = rslist_lock;
    boost::function<void(void)> inlist_unlock = rslist_unlock;

    cout << "[6]------------------------------------------" << endl;
    // unit_test[6] initialize 初期値がある場合
    this->initialize(inrslist_begin, inrslist_end, inrslist_next,
            inlist_lock, inlist_unlock);
    BOOST_CHECK_EQUAL(this->rs_list_begin, rslist_begin);
    function_check_flag = false;
    this->rs_list_begin();
    BOOST_CHECK(function_check_flag);
    BOOST_CHECK_EQUAL(this->rs_list_end, rslist_end);
    function_check_flag = false;
    this->rs_list_end();
    BOOST_CHECK(function_check_flag);
    BOOST_CHECK_EQUAL(this->rs_list_next, rslist_next);
    function_check_flag = false;
    this->rs_list_next(itr);
    BOOST_CHECK(function_check_flag);
    BOOST_CHECK_EQUAL(this->rs_list_lock, rslist_lock);
    function_check_flag = false;
    this->rs_list_lock();
    BOOST_CHECK(function_check_flag);
    BOOST_CHECK_EQUAL(this->rs_list_unlock, rslist_unlock);
    function_check_flag = false;
    this->rs_list_unlock();
    BOOST_CHECK(function_check_flag);

    cout << "[7]------------------------------------------" << endl;
    // unit_test[7] initialize 初期値が空の場合
    this->initialize(NULL, NULL, NULL, NULL, NULL);
    BOOST_CHECK(!this->rs_list_begin);
    BOOST_CHECK(!this->rs_list_end);
    BOOST_CHECK(!this->rs_list_next);
    BOOST_CHECK(!this->rs_list_lock);
    BOOST_CHECK(!this->rs_list_unlock);
}

//finalize
void finalize_test() {
    boost::function<std::list<realserver>::iterator(void)> inrslist_begin =
            rslist_begin;
    boost::function<std::list<realserver>::iterator(void)> inrslist_end =
            rslist_end;
    boost::function<std::list<realserver>::iterator(std::list<realserver>::iterator)> inrslist_next =
            rslist_next;
    boost::function<void(void)> inlist_lock = rslist_lock;
    boost::function<void(void)> inlist_unlock = rslist_unlock;
    char* sslid_replication_area_begain = new char[10];
    int sslid_replication_area_size = 0;
    boost::asio::ip::tcp::endpoint virtual_service_endpoint;
    getloglevel_func_type ingetloglevel = stb_getloglevel;
    logger_func_type inputLogFatal = stb_putLogFatal;
    logger_func_type inputLogError = stb_putLogError;
    logger_func_type inputLogWarn = stb_putLogWarn;
    logger_func_type inputLogInfo = stb_putLogInfo;
    logger_func_type inputLogDebug = stb_putLogDebug;

    cout << "[8]------------------------------------------" << endl;
    // unit_test[8] 初期値がある場合
    this->initialize(inrslist_begin, inrslist_end, inrslist_next,
            inlist_lock, inlist_unlock);
    this->finalize();
    BOOST_CHECK(!this->getloglevel);
    BOOST_CHECK(!this->putLogFatal);
    BOOST_CHECK(!this->putLogError);
    BOOST_CHECK(!this->putLogWarn);
    BOOST_CHECK(!this->putLogInfo);
    BOOST_CHECK(!this->putLogDebug);
    BOOST_CHECK(!this->rs_list_begin);
    BOOST_CHECK(!this->rs_list_end);
    BOOST_CHECK(!this->rs_list_next);
    BOOST_CHECK(!this->rs_list_lock);
    BOOST_CHECK(!this->rs_list_unlock);
    BOOST_CHECK(!this->replication_pay_memory);
    BOOST_CHECK(!this->replication_area_lock);
    BOOST_CHECK(!this->replication_area_unlock);
    BOOST_CHECK(!this->schedule_tcp);
    BOOST_CHECK(!this->schedule_udp);
    BOOST_CHECK_EQUAL(this->timeout, 0);
    BOOST_CHECK_EQUAL(this->maxlist, 0);
    BOOST_CHECK_EQUAL(this->reschedule, 0);
    BOOST_CHECK(this->session_data_processor == NULL);
    BOOST_CHECK(this->replication_data_processor == NULL);

    cout << "[9]------------------------------------------" << endl;
    // unit_test[9] 初期値が空の場合
    this->getloglevel = stb_getloglevel;
    this->putLogFatal = stb_putLogFatal;
    this->putLogError = stb_putLogError;
    this->putLogWarn = stb_putLogWarn;
    this->putLogInfo = stb_putLogInfo;
    this->putLogDebug = stb_putLogDebug;
    this->rs_list_begin = NULL;
    this->rs_list_end = NULL;
    this->rs_list_next = NULL;
    this->rs_list_lock = NULL;
    this->rs_list_unlock = NULL;
    this->replication_pay_memory = NULL;
    this->replication_area_lock = NULL;
    replication_area_unlock = NULL;
    this->schedule_tcp = NULL;
    this->schedule_udp = NULL;
    this->timeout = 10;
    this->maxlist = 10;
    this->reschedule = 10;
    this->replication_data_processor
            = new sslid_replication_data_processor(this->maxlist,
                    sslid_replication_area_begain,
                    sslid_replication_area_size, virtual_service_endpoint,
                    ingetloglevel, inputLogFatal, inputLogError,
                    inputLogWarn, inputLogInfo, inputLogDebug);
    this->session_data_processor = new sslid_session_data_processor(
            this->maxlist, this->timeout, this->replication_data_processor,
            ingetloglevel, inputLogFatal, inputLogError, inputLogWarn,
            inputLogInfo, inputLogDebug);
    this->finalize();
    BOOST_CHECK(!this->getloglevel);
    BOOST_CHECK(!this->putLogFatal);
    BOOST_CHECK(!this->putLogError);
    BOOST_CHECK(!this->putLogWarn);
    BOOST_CHECK(!this->putLogInfo);
    BOOST_CHECK(!this->putLogDebug);
    BOOST_CHECK(!this->rs_list_begin);
    BOOST_CHECK(!this->rs_list_end);
    BOOST_CHECK(!this->rs_list_next);
    BOOST_CHECK(!this->rs_list_lock);
    BOOST_CHECK(!this->rs_list_unlock);
    BOOST_CHECK(!this->replication_pay_memory);
    BOOST_CHECK(!this->replication_area_lock);
    BOOST_CHECK(!this->replication_area_unlock);
    BOOST_CHECK(!this->schedule_tcp);
    BOOST_CHECK(!this->schedule_udp);
    BOOST_CHECK_EQUAL(this->timeout, 0);
    BOOST_CHECK_EQUAL(this->maxlist, 0);
    BOOST_CHECK_EQUAL(this->reschedule, 0);
    BOOST_CHECK(this->session_data_processor == NULL);
    BOOST_CHECK(this->replication_data_processor == NULL);
    delete[] sslid_replication_area_begain;
}

//is_use_sorry
void is_use_sorry_test(){
    cout << "[10]------------------------------------------" << endl;
	//unit_test[10] is_use_sorry()メソッドのテスト,正常系で必ずFALSEを返す
	BOOST_CHECK(!this->is_use_sorry());
}

//check_parameter
void check_parameter_test(){
	std::vector<std::string> args;

    cout << "[11]------------------------------------------" << endl;
	//unit_test[11] オプション文字列が存在しない場合, チェック結果フラグにTRUEを設定する
	protocol_module_base::check_message_result check_message;
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[12]------------------------------------------" << endl;
	//unit_test[12] オプション文字列 = "-T", timeout設定フラグ = OFFの場合,チェック結果フラグにTRUEを設定する
	args.push_back("-T");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[13]------------------------------------------" << endl;
	//unit_test[13] オプション文字列 = "--T", timeout設定フラグ = OFFの場合,チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Option error."を設定する
	args.clear();
	args.push_back("--T");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"Option error.");

    cout << "[14]------------------------------------------" << endl;
	//unit_test[14] オプション文字列 = "-T -T", timeout設定フラグ = OFFの場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("-T");
	args.push_back("-T");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[15]------------------------------------------" << endl;
	//unit_test[15] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する
	//unit_test[15] data test:オプション文字列 = "-T 2ewqt" timeout設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("2ewqt");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2ewqt' is not numeric character.");

    cout << "[16]------------------------------------------" << endl;
	//unit_test[16] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '$@#' is not numeric character."を設定する
	//unit_test[16] data test:オプション文字列 = "-T $@#" timeout設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("$@#");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '$@#' is not numeric character.");

    cout << "[17]------------------------------------------" << endl;
	//unit_test[17] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2148583647' is too large."を設定する
	//unit_test[17] test data:オプション文字列 = "-T 2148583647" timeout設定フラグ = OFF の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2148583647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2148583647' is too large.");

    cout << "[18]------------------------------------------" << endl;
	//unit_test[18] チェック結果フラグにTRUEを設定する
	//unit_test[18] test data:オプション文字列 = "-T 2000" timeout設定フラグ = OFF の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[19]------------------------------------------" << endl;
	//unit_test[19] チェック結果フラグにTRUEを設定する
	//unit_test[19] test data:オプション文字列 = "-T 2147483647" timeout設定フラグ = OFF の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2147483647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[20]------------------------------------------" << endl;
	//unit_test[20] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[20] test data:オプション文字列 = "-T 2000 -T" timeout設定フラグ = ON の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("-T");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);

    cout << "[21]------------------------------------------" << endl;
	//unit_test[21] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[21] test data:オプション文字列 = "-T 2000 -T 2000" timeout設定フラグ = ON の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("-T");
	args.push_back("2000");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);

    cout << "[22]------------------------------------------" << endl;
	//unit_test[22] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[22] test data:オプション文字列 = "-T -T 20 -T" timeout設定フラグ = ON の場合
	args.clear();
	args.push_back("-T");
	args.push_back("-T");
	args.push_back("20");
	args.push_back("-T");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);

    cout << "[23]------------------------------------------" << endl;
	//unit_test[23] オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素が存在しない場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("--timeout");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[24]------------------------------------------" << endl;
	//unit_test[24] オプション文字列 = "--timeout --timeout" の場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("--timeout");
	args.push_back("--timeout");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[25]------------------------------------------" << endl;
	//unit_test[25] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する
	//unit_test[25] test data:オプション文字列 = "--timeout 2ewqt" timeout設定フラグ = OFF の場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2ewqt");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2ewqt' is not numeric character.");

    cout << "[26]------------------------------------------" << endl;
	//unit_test[26] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2148583647' is too large."を設定する
	//unit_test[26] test data:オプション文字列 = "--timeout 2148583647" timeout設定フラグ = OFF の場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2148583647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2148583647' is too large.");

    cout << "[27]------------------------------------------" << endl;
	//unit_test[27] チェック結果フラグにTRUEを設定する
	//unit_test[27] test data:オプション文字列 = "--timeout 2000" timeout設定フラグ = OFF の場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[28]------------------------------------------" << endl;
	//unit_test[28] チェック結果フラグにTRUEを設定する
	//unit_test[28] test data:オプション文字列 = "--timeout 2147483647" timeout設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2147483647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[29]------------------------------------------" << endl;
	//unit_test[29] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[29] test data:オプション文字列 = "--timeout 2000 --timeout" timeout設定フラグ = ONの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("--timeout");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);

    cout << "[30]------------------------------------------" << endl;
	//unit_test[30] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[30] test data:オプション文字列 = "--timeout 2000 --timeout 2000" timeout設定フラグ = ONの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("--timeout");
	args.push_back("2000");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);

    cout << "[31]------------------------------------------" << endl;
	//unit_test[31] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[31] test data:オプション文字列 = "--timeout --timeout 2000 --timeout" timeout設定フラグ = ONの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("--timeout");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);

    cout << "[32]------------------------------------------" << endl;
	//unit_test[32] オプション文字列 = "-M" maxlist設定フラグ = OFFの場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("-M");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

    cout << "[33]------------------------------------------" << endl;
	//unit_test[33] オプション文字列 = "-M -M" maxlist設定フラグ = OFFの場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("-M");
	args.push_back("-M");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[34]------------------------------------------" << endl;
	//unit_test[34] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist' option value '1st' is not numeric character."を設定する
	//unit_test[34] test data:オプション文字列 = "-M 1st" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("-M");
	args.push_back("1st");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '1st' is not numeric character.");

    cout << "[35]------------------------------------------" << endl;
	//unit_test[35] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist' option value '2148583647' is too large."を設定する
	//unit_test[35] test data:オプション文字列 = "-M　2148583647" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("-M");
	args.push_back("2148583647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '2148583647' is too large.");

    cout << "[36]------------------------------------------" << endl;
	//unit_test[36] チェック結果フラグにTRUEを設定する
	//unit_test[36] test data:オプション文字列 = "-M 100" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("-M");
	args.push_back("100");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[37]------------------------------------------" << endl;
	//unit_test[37] チェック結果フラグにTRUEを設定する
	//unit_test[37] test data:オプション文字列 = "-M 2147483647" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("-M");
	args.push_back("2147483647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[38]------------------------------------------" << endl;
	//unit_test[38] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[38] test data:オプション文字列 = "-M 100 -M"の場合
	args.clear();
	args.push_back("-M");
	args.push_back("100");
	args.push_back("-M");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);

    cout << "[39]------------------------------------------" << endl;
	//unit_test[39] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[39] test data:オプション文字列 = "-M 100 -M 100"の場合
	args.clear();
	args.push_back("-M");
	args.push_back("100");
	args.push_back("-M");
	args.push_back("100");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);

    cout << "[40]------------------------------------------" << endl;
	//unit_test[40] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[40] test data:オプション文字列 = "-M -M 20 -M"の場合
	args.clear();
	args.push_back("-M");
	args.push_back("-M");
	args.push_back("20");
	args.push_back("-M");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);

    cout << "[41]------------------------------------------" << endl;
	//unit_test[41] チェック結果フラグにTRUEを設定する
	//unit_test[41] test data:オプション文字列 = "--maxlist" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("--maxlist");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[42]------------------------------------------" << endl;
	//unit_test[42] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist option' value '1st' is not numeric character."を設定する
	//unit_test[42] test data:オプション文字列 = "--maxlist 1st" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("1st");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '1st' is not numeric character.");

    cout << "[43]------------------------------------------" << endl;
	//unit_test[43] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist option' value '2148583647' is too large."を設定する
	//unit_test[43] test data:オプション文字列 = "--maxlist 2148583647" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("2148583647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '2148583647' is too large.");

    cout << "[44]------------------------------------------" << endl;
	//unit_test[44] チェック結果フラグにTRUEを設定する
	//unit_test[44] test data:オプション文字列 = "--maxlist 100" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("100");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[45]------------------------------------------" << endl;
	//unit_test[45] チェック結果フラグにTRUEを設定する
	//unit_test[45] test data:オプション文字列 = "--maxlist 2147483647" maxlist設定フラグ = OFFの場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("2147483647");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);


    cout << "[46]------------------------------------------" << endl;
	//unit_test[46] チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[46] test data:オプション文字列 = "--maxlist 100 --maxlist"の場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("--maxlist");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);

    cout << "[47]------------------------------------------" << endl;
	//unit_test[47] チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[47] test data:オプション文字列 = "--maxlist 100 --maxlist"の場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("--maxlist");
	args.push_back("100");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);

    cout << "[48]------------------------------------------" << endl;
	//unit_test[48] チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[48] test data:オプション文字列 = "--maxlist --maxlist 100 --maxlist"の場合
	args.clear();
	args.push_back("--maxlist");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("--maxlist");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);

    cout << "[49]------------------------------------------" << endl;
	//unit_test[49] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	//unit_test[49] test data:オプション文字列 ="-N -R"の場合
	args.clear();
	args.push_back("-N");
	args.push_back("-R");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);

    cout << "[50]------------------------------------------" << endl;
	//unit_test[50] オプション文字列 ="-N",reschedule設定フラグ = ON,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	//unit_test[50] test data:オプション文字列 ="-R -N"の場合
	args.clear();
	args.push_back("-R");
	args.push_back("-N");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);

    cout << "[51]------------------------------------------" << endl;
	//unit_test[51] オプション文字列 ="-T -M -R -N"の場合,チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	args.clear();
	args.push_back("-T");
	args.push_back("-M");
	args.push_back("-R");
	args.push_back("-N");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);

    cout << "[52]------------------------------------------" << endl;
	//unit_test[52] オプション文字列 ="-M -T -N -R"の場合,チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	args.clear();
	args.push_back("-M");
	args.push_back("-T");
	args.push_back("-N");
	args.push_back("-R");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);

    cout << "[53]------------------------------------------" << endl;
	//unit_test[53] オプション文字列 ="-T 2000 -M 100 -R"の場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("-M");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[54]------------------------------------------" << endl;
	//unit_test[54] オプション文字列 ="--timeout 2000 -M 100 -R"の場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("-M");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[55]------------------------------------------" << endl;
	//unit_test[55] オプション文字列 ="-T 2000 --maxlist 100 -R"の場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[56]------------------------------------------" << endl;
	//unit_test[56] オプション文字列 ="--timeout 2000 --maxlist 100 -R"の場合,チェック結果フラグにTRUEを設定する
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[57]------------------------------------------" << endl;
	//unit_test[57] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	//unit_test[57] test data:オプション文字列 ="-T 2000 --maxlist 100 -R -N"の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	args.push_back("-N");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);

    cout << "[58]------------------------------------------" << endl;
	//unit_test[58] オプション文字列 = 上記以外の場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error"を設定する
	args.clear();
	args.push_back("-A");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Option error.",check_message.message);
}

//set_parameter
void set_parameter_test(){
    cout << "[59]------------------------------------------" << endl;
	//unit_test[59] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[59] test data:オプション文字列が存在しない,timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	protocol_module_base::check_message_result check_message;
	vector<string> args;
	install_stb_replication_func();
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[60]------------------------------------------" << endl;
	//unit_test[60] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[60] test data:オプション文字列 = "-T",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[61]------------------------------------------" << endl;
	//unit_test[61] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[61] test data:オプション文字列 = "--T",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--T");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Option error.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[62]------------------------------------------" << endl;
	//unit_test[62] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[62] test data:オプション文字列 = "-T -T",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("-T");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[63]------------------------------------------" << endl;
	//unit_test[63] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[63] test data:オプション文字列 = "-T 2ewqt",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("2ewqt");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2ewqt' is not numeric character.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[64]------------------------------------------" << endl;
	//unit_test[64] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout option' value '$@#' is not numeric character."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[64] test data:オプション文字列 = "-T $@#",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("$@#");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '$@#' is not numeric character.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[65]------------------------------------------" << endl;
	//unit_test[65] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2148583647' is too large."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[65] test data:オプション文字列 = "-T 2148583647"の場合
	args.clear();
	args.push_back("-T");
	args.push_back("2148583647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2148583647' is too large.");
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[66]------------------------------------------" << endl;
	//unit_test[66] チェック結果フラグにTRUEを設定する,timeoutに2000を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[66] test data:オプション文字列 = "-T 2000",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[67]------------------------------------------" << endl;
	//unit_test[67] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[67] test data:オプション文字列 = "-T 2147483647",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("2147483647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[68]------------------------------------------" << endl;
	//unit_test[68] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[68] test data:オプション文字列 = "-T -T 2147483647",timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("-T");
	args.push_back("2147483647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[69]------------------------------------------" << endl;
	//unit_test[69] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[69] test data:オプション文字列 = "-T 2147483647 -T",timeout設定フラグ = ON,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("2147483647");
	args.push_back("-T");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[70]------------------------------------------" << endl;
	//unit_test[70] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[70] test data:オプション文字列 = "-T 2147483647 -T 2147483647",timeout設定フラグ = ON,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("2147483647");
	args.push_back("-T");
	args.push_back("2147483647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[71]------------------------------------------" << endl;
	//unit_test[71] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[71] test data:オプション文字列 = "-T -T 2147483647 -T" timeout設定フラグ = ON,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("-T");
	args.push_back("-T");
	args.push_back("2147483647");
	args.push_back("-T");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[72]------------------------------------------" << endl;
	//unit_test[72] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[72] test data:オプション文字列 = "--timeout" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF の場合
	args.clear();
	args.push_back("--timeout");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[73]------------------------------------------" << endl;
	//unit_test[73] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[73] test data:オプション文字列 = "--timeout --timeout" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("--timeout");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[74]------------------------------------------" << endl;
	//unit_test[74] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[74] test data:オプション文字列 = "--timeout 2ewqt" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2ewqt");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2ewqt' is not numeric character.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[75]------------------------------------------" << endl;
	//unit_test[75] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2148583647' is too large."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[75] test data:オプション文字列 = "--timeout 2148583647" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2148583647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout' option value '2148583647' is too large.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[76]------------------------------------------" << endl;
	//unit_test[76] チェック結果フラグにTRUEを設定する,timeoutに2000を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[76] test data:オプション文字列 = "--timeout 2000" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[77]------------------------------------------" << endl;
	//unit_test[77] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[77] test data:オプション文字列 = "--timeout" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2147483647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[78]------------------------------------------" << endl;
	//unit_test[78] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[78] test data:オプション文字列 = "--timeout --timeout 2147483647" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("--timeout");
	args.push_back("2147483647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[79]------------------------------------------" << endl;
	//unit_test[79] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[79] test data:オプション文字列 = "--timeout 2147483647 --timeout" timeout設定フラグ = ON,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2147483647");
	args.push_back("--timeout");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[80]------------------------------------------" << endl;
	//unit_test[80] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[80] test data:オプション文字列 = "--timeout 2147483647 --timeout 2147483647" timeout設定フラグ = ON,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("2147483647");
	args.push_back("--timeout");
	args.push_back("2147483647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->timeout,INT_MAX);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[81]------------------------------------------" << endl;
	//unit_test[81] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[81] test data:オプション文字列 = "--timeout 2147483647 --timeout" timeout設定フラグ = ON,maxlist設定フラグ = OFF,reschedule設定フラグ = OFFの場合
	args.clear();
	args.push_back("--timeout");
	args.push_back("--timeout");
	args.push_back("2147483647");
	args.push_back("--timeout");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[82]------------------------------------------" << endl;
	//unit_test[82] チェック結果フラグにTRUEを設定する,timeに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[82] test data:オプション文字列 = "-M" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[83]------------------------------------------" << endl;
	//unit_test[83] チェック結果フラグにTRUEを設定する,timeに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[83] test data:オプション文字列 = "-M -M" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("-M");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[84]------------------------------------------" << endl;
	//unit_test[84] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist' option value '2ewqt' is not numeric character."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[84] test data:オプション文字列 = "-M 2ewqt" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("2ewqt");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '2ewqt' is not numeric character.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[85]------------------------------------------" << endl;
	//unit_test[85] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist' option value '2148583647' is too large."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[85] test data:オプション文字列 = "-M 2148583647" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("2148583647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '2148583647' is too large.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[86]------------------------------------------" << endl;
	//unit_test[86] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに100を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[86] test data:オプション文字列 = "-M 100" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschdule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("100");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[87]------------------------------------------" << endl;
	//unit_test[87] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに100を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[87] test data:オプション文字列 = "-M -M 100" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschdule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("-M");
	args.push_back("100");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[88]------------------------------------------" << endl;
	//unit_test[88] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに2147483674を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[88] test data:オプション文字列 = "-M 1024" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("1024");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[89]------------------------------------------" << endl;
	//unit_test[89] チェック結果フラグにFALSEを設定する,maxlistに200を設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[89] test data:オプション文字列 = "-M 200 -M" maxlist設定フラグ = ON,timeout設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("200");
	args.push_back("-M");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->maxlist,200);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[90]------------------------------------------" << endl;
	//unit_test[90] チェック結果フラグにFALSEを設定する,maxlistに200を設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[90] test data:オプション文字列 = "-M 200 -M 200" maxlist設定フラグ = ON,timeout設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("200");
	args.push_back("-M");
	args.push_back("200");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->maxlist,200);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[91]------------------------------------------" << endl;
	//unit_test[91] チェック結果フラグにFALSEを設定する,maxlistに200を設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[91] test data:オプション文字列 = "-M -M 200 -M" maxlist設定フラグ = ON,timeout設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-M");
	args.push_back("-M");
	args.push_back("200");
	args.push_back("-M");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(this->maxlist,200);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[92]------------------------------------------" << endl;
	//unit_test[92] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[92] test data:オプション文字列 = "--maxlist" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[93]------------------------------------------" << endl;
	//unit_test[93] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに1024を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[93] test data:オプション文字列 = "--maxlist --maxlist" timeout設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	args.push_back("--maxlist");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[94]------------------------------------------" << endl;
	//unit_test[94] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist' option value '2ewqt' is not numeric character."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[94] test data:オプション文字列 = "--maxlist 2ewqt" maxlist設定フラグ = OFF,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	args.push_back("2ewqt");
	check_message=this->check_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '2ewqt' is not numeric character.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[95]------------------------------------------" << endl;
	//unit_test[95] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist' option value '2148583647' is too large."を設定する,sessionデータをreplication_areaから回復する
	//unit_test[95] test data:オプション文字列 = "--maxlist 2148583647" timeout設定フラグ = OFF ,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	args.push_back("2148583647");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist' option value '2148583647' is too large.");
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[96]------------------------------------------" << endl;
	//unit_test[96] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに100を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[96] test data:オプション文字列 = "--maxlist 100" timeout設定フラグ = OFF ,maxlist設定フラグ = OFF,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	args.push_back("100");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[97]------------------------------------------" << endl;
	//unit_test[97] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,maxlistに100を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[97] test data:オプション文字列 = "--maxlist --maxlist 100" timeout設定フラグ = OFF ,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	args.push_back("--maxlist");
	args.push_back("100");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[98]------------------------------------------" << endl;
	//unit_test[98] チェック結果フラグにTRUEを設定する,maxlistにINT_MAXを設定する,timeoutに3600を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[98] test data:オプション文字列 = "--maxlist 1024" maxlist設定フラグ = OFF,timeout設定フラグ = OFF ,reschedule設定フラグ = OFF
	args.clear();
	args.push_back("--maxlist");
	args.push_back("1024");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[99]------------------------------------------" << endl;
	//unit_test[99] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[99] test data:オプション文字列 = "--maxlist 20 --maxlist"
	args.clear();
	args.push_back("--maxlist");
	args.push_back("20");
	args.push_back("--maxlist");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[100]------------------------------------------" << endl;
	//unit_test[100] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[100] test data:オプション文字列 = "--maxlist 20 --maxlist 20"
	args.clear();
	args.push_back("--maxlist");
	args.push_back("20");
	args.push_back("--maxlist");
	args.push_back("20");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[101]------------------------------------------" << endl;
	//unit_test[101] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[101] test data:オプション文字列 = "--maxlist --maxlist 20 --maxlist"
	args.clear();
	args.push_back("--maxlist");
	args.push_back("--maxlist");
	args.push_back("20");
	args.push_back("--maxlist");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/--maxlist'.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[102]------------------------------------------" << endl;
	//unit_test[102] チェック結果フラグにTRUEを設定する,rescheduleに1を設定する,timeoutに3600を設定する,maxlistに1024を設定する,sessionデータをreplication_areaから回復する
	//unit_test[102] test data:オプション文字列 = "-R" no_reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->reschedule,1);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[103]------------------------------------------" << endl;
	//unit_test[103] チェック結果フラグにFALSEを設定する,rescheduleに0を設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[103] test data:オプション文字列 ="-N -R"
	args.clear();
	args.push_back("-N");
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[104]------------------------------------------" << endl;
	//unit_test[104] チェック結果フラグにTRUEを設定する,rescheduleに0を設定する,timeoutに3600を設定する,maxlistに1024を設定する,sessionデータをreplication_areaから回復する
	//unit_test[104] test data:オプション文字列 = "-N" reschedule設定フラグ = OFF
	args.clear();
	args.push_back("-N");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->reschedule,0);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,3600);
	BOOST_CHECK_EQUAL(this->maxlist,1024);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[105]------------------------------------------" << endl;
	//unit_test[105] チェック結果フラグにFALSEを設定する,rescheduleに1を設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[105] test data:オプション文字列 = "-R -N"
	args.clear();
	args.push_back("-R");
	args.push_back("-N");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(this->reschedule,1);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[106]------------------------------------------" << endl;
	//unit_test[106] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに1を設定する,sessionデータをreplication_areaから回復する
	//unit_test[106] test data:オプション文字列 = "-T -M -R -N"
	args.clear();
	args.push_back("-T");
	args.push_back("-M");
	args.push_back("-R");
	args.push_back("-N");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[107]------------------------------------------" << endl;
	//unit_test[107] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに1を設定する,sessionデータをreplication_areaから回復する
	//unit_test[107] test data:オプション文字列 = "-M -T -N -R"
	args.clear();
	args.push_back("-M");
	args.push_back("-T");
	args.push_back("-N");
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[108]------------------------------------------" << endl;
	//unit_test[108] チェック結果フラグにTRUEを設定する,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに1を設定する,sessionデータをreplication_areaから回復する
	//unit_test[108] test data:オプション文字列 = "-T 2000 -M 100 -R"
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("-M");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->reschedule,1);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[109]------------------------------------------" << endl;
	//unit_test[109] チェック結果フラグにTRUEを設定する,reschedule設定フラグ = OFF,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに1を設定する,sessionデータをreplication_areaから回復する
	//unit_test[109] test data:オプション文字列 = "-T 2000 --maxlist 100 -R"
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->reschedule,1);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[110]------------------------------------------" << endl;
	//unit_test[110] チェック結果フラグにTRUEを設定する,reschedule設定フラグ = OFF,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに1を設定する,sessionデータをreplication_areaから回復する
	//unit_test[110] test data:オプション文字列 = "--timeout 2000 -M 100 -R"
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("-M");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->reschedule,1);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[111]------------------------------------------" << endl;
	//unit_test[111] チェック結果フラグにTRUEを設定する,reschedule設定フラグ = OFF,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに1を設定する,sessionデータをreplication_areaから回復する
	//unit_test[111] test data:オプション文字列 = "--timeout 2000 --maxlist 100 -R"
	args.clear();
	args.push_back("--timeout");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK_EQUAL(this->reschedule,1);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[112]------------------------------------------" << endl;
	//unit_test[112] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,timeoutに2000を設定する,maxlistに100を設定する,rescheduleに0を設定する,sessionデータをreplication_areaから回復する
	//unit_test[112] test data:オプション文字列 = "-T 2000 --maxlist 100 -R -N"
	args.clear();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	args.push_back("-N");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.",check_message.message);
	BOOST_CHECK_EQUAL(this->timeout,2000);
	BOOST_CHECK_EQUAL(this->maxlist,100);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;

    cout << "[113]------------------------------------------" << endl;
	//unit_test[113] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error"を設定する,sessionデータをreplication_areaから回復する
	//unit_test[113] test data:オプション文字列 = 上記以外
	args.clear();
	args.push_back("-A");
	check_message=this->set_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL("Option error.",check_message.message);
	BOOST_CHECK(this->session_data_processor!=NULL);
	BOOST_CHECK(this->replication_data_processor!=NULL);
	this->timeout=111;
	this->maxlist=111;
	this->reschedule=111;
}

//add_parameter
void add_parameter_test(){
	std::vector<std::string> args;

    cout << "[114]------------------------------------------" << endl;
	//unit_test[114] オプション文字列が存在ない場合,チェック結果フラグにTRUEを設定する
	check_message_result check_message;
	check_message=this->add_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,true);

    cout << "[115]------------------------------------------" << endl;
	//unit_test[115] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot add option."を設定する
	//unit_test[115] test data:オプション文字列が存在する場合
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("-R");
	args.push_back("1");
	check_message=this->add_parameter(args);
	BOOST_CHECK_EQUAL(check_message.flag,false);
	BOOST_CHECK_EQUAL(check_message.message,"Cannot add option.");
}

//handle_rslist_update

//register_schedule(tcp)
void register_schedule_tcp_test(){
	protocol_module_base::rs_list_itr_func_type rs_1,rs_2;
	protocol_module_base::rs_list_itr_next_func_type rs_3;
	boost::asio::ip::tcp::endpoint ep;
	protocol_module_base::tcp_schedule_func_type tcp_schedule;
    cout << "[116]------------------------------------------" << endl;
	//unit_test[116] parameterがNULLの場合,schedule_tcpにNULLを設定する
	tcp_schedule=NULL;
	this->register_schedule(tcp_schedule);
	BOOST_CHECK(this->schedule_tcp==NULL);
    cout << "[117]------------------------------------------" << endl;
	//unit_test[117] parameterがtcp_scheduleの場合,schedule_tcpにt_scheduleを設定する
    	tcp_schedule=&t_schedule;
	this->register_schedule(tcp_schedule);
	this->schedule_tcp(boost::this_thread::get_id(),rs_1,rs_2,rs_3,ep);
	BOOST_CHECK(check_register_schedule);
}

//register_schedule(udp)

//handle_session_initialize
void handle_session_initialize_test() {

    boost::thread down_thread(down_thread_func);
    boost::asio::ip::tcp::endpoint ep;
    boost::asio::ip::tcp::endpoint client_endpoint_tcp;
    boost::asio::ip::udp::endpoint client_endpoint_udp;

    cout << "[118]------------------------------------------" << endl;
    // unit_test[118] パラメータが正常に入力去れた場合、戻り値にACCEPTを設定する。
    EVENT_TAG status =
            this->handle_session_initialize(boost::this_thread::get_id(),
                    down_thread.get_id(), client_endpoint_tcp,
                    client_endpoint_udp);
    BOOST_CHECK_EQUAL(status, ACCEPT);
    std::map<const boost::thread::id, thread_data_ptr>::iterator
            itr;
    itr = this->session_thread_data_map.find(boost::this_thread::get_id());
    BOOST_CHECK(itr != this->session_thread_data_map.end());
    BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_UP_STREAM);
    BOOST_CHECK_EQUAL(itr->second->pair_thread_id, down_thread.get_id());
    BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
    BOOST_CHECK_EQUAL(itr->second->realserver_connect_failed_count, 0);
    BOOST_CHECK_EQUAL(itr->second->data_begain_offset, 0u);
    BOOST_CHECK_EQUAL(itr->second->data_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->current_record_rest_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->selected_realserver, ep);
    BOOST_CHECK(!itr->second->hello_message_flag);
    itr = this->session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(itr != this->session_thread_data_map.end());
    BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_DOWN_STREAM);
    BOOST_CHECK_EQUAL(itr->second->pair_thread_id, boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
    BOOST_CHECK_EQUAL(itr->second->realserver_connect_failed_count, 0);
    BOOST_CHECK_EQUAL(itr->second->data_begain_offset, 0u);
    BOOST_CHECK_EQUAL(itr->second->data_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->current_record_rest_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->selected_realserver, ep);
    BOOST_CHECK(!itr->second->hello_message_flag);
}

//handle_session_initialize
void handle_session_initialize_test_thread() {

    boost::thread down_thread(down_thread_func);
    boost::asio::ip::tcp::endpoint ep;
    boost::asio::ip::tcp::endpoint client_endpoint_tcp;
    boost::asio::ip::udp::endpoint client_endpoint_udp;

    cout << "[118]------------------------------------------\n";
    // unit_test[118] パラメータが正常に入力去れた場合、戻り値にACCEPTを設定する。
    EVENT_TAG status =
            this->handle_session_initialize(boost::this_thread::get_id(),
                    down_thread.get_id(), client_endpoint_tcp,
                    client_endpoint_udp);
    BOOST_CHECK_EQUAL(status, ACCEPT);
    std::map<const boost::thread::id, thread_data_ptr>::iterator itr;
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        itr = this->session_thread_data_map.find(boost::this_thread::get_id());
	BOOST_CHECK(itr != this->session_thread_data_map.end());
    }
    BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_UP_STREAM);
    BOOST_CHECK_EQUAL(itr->second->pair_thread_id, down_thread.get_id());
    BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
    BOOST_CHECK_EQUAL(itr->second->realserver_connect_failed_count, 0);
    BOOST_CHECK_EQUAL(itr->second->data_begain_offset, 0u);
    BOOST_CHECK_EQUAL(itr->second->data_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->current_record_rest_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->selected_realserver, ep);
    BOOST_CHECK(!itr->second->hello_message_flag);
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        itr = this->session_thread_data_map.find(down_thread.get_id());
	BOOST_CHECK(itr != this->session_thread_data_map.end());
    }
    BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_DOWN_STREAM);
    BOOST_CHECK_EQUAL(itr->second->pair_thread_id, boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
    BOOST_CHECK_EQUAL(itr->second->realserver_connect_failed_count, 0);
    BOOST_CHECK_EQUAL(itr->second->data_begain_offset, 0u);
    BOOST_CHECK_EQUAL(itr->second->data_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->current_record_rest_size, 0u);
    BOOST_CHECK_EQUAL(itr->second->selected_realserver, ep);
    BOOST_CHECK(!itr->second->hello_message_flag);
}

//handle_session_finalize
void handle_session_finalize_test() {
    boost::thread down_thread(down_thread_func);
    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint client_endpoint_tcp;
    boost::asio::ip::udp::endpoint client_endpoint_udp;

    cout << "[119]------------------------------------------" << endl;
    // unit_test[119] パラメータが正常に入力された場合、戻り値にSTOPを設定する。
    this->handle_session_initialize(boost::this_thread::get_id(),
            down_thread.get_id(), client_endpoint_tcp, client_endpoint_udp);
    status = this->handle_session_finalize(boost::this_thread::get_id(),
            down_thread.get_id());
    BOOST_CHECK_EQUAL(status, STOP);
    std::map<const boost::thread::id, thread_data_ptr>::iterator
            itr;
    itr = this->session_thread_data_map.find(boost::this_thread::get_id());
    BOOST_CHECK(itr == this->session_thread_data_map.end());
    itr = this->session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(itr == this->session_thread_data_map.end());

    cout << "[120]------------------------------------------" << endl;
    // unit_test[120] 初期化しなくて、直接該当関数を呼び出す場合、戻り値にSTOPを設定する。
    this->session_thread_data_map[boost::this_thread::get_id()] = thread_data_ptr(new session_thread_data_sslid);
    this->session_thread_data_map[down_thread.get_id()] = thread_data_ptr(new session_thread_data_sslid);
    status = this->handle_session_finalize(boost::this_thread::get_id(),
            down_thread.get_id());
    BOOST_CHECK_EQUAL(status, STOP);
    itr = this->session_thread_data_map.find(boost::this_thread::get_id());
    BOOST_CHECK(itr == this->session_thread_data_map.end());
    itr = this->session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(itr == this->session_thread_data_map.end());
}

//handle_session_finalize
void handle_session_finalize_test_thread() {
    boost::thread down_thread(down_thread_func);
    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint client_endpoint_tcp;
    boost::asio::ip::udp::endpoint client_endpoint_udp;

    cout << "[119]------------------------------------------\n";
    // unit_test[119] パラメータが正常に入力された場合、戻り値にSTOPを設定する。
    this->handle_session_initialize(boost::this_thread::get_id(),
            down_thread.get_id(), client_endpoint_tcp, client_endpoint_udp);
    status = this->handle_session_finalize(boost::this_thread::get_id(),
            down_thread.get_id());
    BOOST_CHECK_EQUAL(status, STOP);
    std::map<const boost::thread::id, thread_data_ptr>::iterator itr;
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        itr = this->session_thread_data_map.find(boost::this_thread::get_id());
	BOOST_CHECK(itr == this->session_thread_data_map.end());
    }
    
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        itr = this->session_thread_data_map.find(down_thread.get_id());
	BOOST_CHECK(itr == this->session_thread_data_map.end());
    }
    

    cout << "[120]------------------------------------------" << endl;
}

//handle_accept
void handle_accept_test(){
    cout << "[121]------------------------------------------" << endl;
	//unit_test[121] handle_accpet()メソッドのテスト,正常系で必ずCLIENT_RECVを返す

	boost::asio::ip::tcp::endpoint ep;
	session_thread_data_sslid* thread_up_data_value = new session_thread_data_sslid;
	thread_up_data_value->thread_division=THREAD_DIVISION_UP_STREAM;
	thread_up_data_value->realserver_connect_failed_count=0;
	thread_up_data_value->data_begain_offset=0;
	thread_up_data_value->data_size=0;
	thread_up_data_value->current_record_rest_size=0;
	thread_up_data_value->hello_message_flag=false;
	thread_data_ptr thread_data(thread_up_data_value);
	this->session_thread_data_map.insert(std::pair<const boost::thread::id,thread_data_ptr>(boost::this_thread::get_id(),thread_data));
	BOOST_CHECK_EQUAL(this->handle_accept(boost::this_thread::get_id()), protocol_module_base::CLIENT_RECV);
	this->session_thread_data_map.clear();
}

void handle_accept_test_thread(){
    cout << "[122]------------------------------------------\n";
	//unit_test[121] handle_accpet()メソッドのテスト,正常系で必ずCLIENT_RECVを返す

	boost::asio::ip::tcp::endpoint ep;
	session_thread_data_sslid* thread_up_data_value = new session_thread_data_sslid;
	thread_up_data_value->thread_division=THREAD_DIVISION_UP_STREAM;
	thread_up_data_value->realserver_connect_failed_count=0;
	thread_up_data_value->data_begain_offset=0;
	thread_up_data_value->data_size=0;
	thread_up_data_value->current_record_rest_size=0;
	thread_up_data_value->hello_message_flag=false;
	thread_data_ptr thread_data(thread_up_data_value);
	{
	    boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map.insert(std::pair<const boost::thread::id,thread_data_ptr>(boost::this_thread::get_id(),thread_data));
	}
	BOOST_CHECK_EQUAL(this->handle_accept(boost::this_thread::get_id()), protocol_module_base::CLIENT_RECV);
}

//handle_client_recv
void handle_client_recv_test() {

    boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
    int recvlen = 0;
    EVENT_TAG status;
	char* mem_cmp_buffer;
    int mem_cmp_result;
	int mem_cmp_length;

    cout << "[122]------------------------------------------" << endl;
    // unit_test[122] 終了フラグがONの場合、戻り値にCLIENT_RECVを設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_ON;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[123]------------------------------------------" << endl;
    // unit_test[123] 終了フラグがOFFで、且つ データサイズが0で、且つ 新SSLレコードでなくて、
    // unit_test[123] 且つ受信データサイズ > 0、且つdata_begain_offset = 0の場合、戻り値をREALSERVER_SELECTに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 10u;
        recvlen = 3;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 3u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        this->session_thread_data_map.clear();
    }

    cout << "[124]------------------------------------------" << endl;
    // unit_test[124] 終了フラグがOFFで、且つ データサイズが0で、且つ 新SSLレコードでなくて、
    // unit_test[124] 且つ受信データサイズ > 0、且つdata_begain_offset > 0の場合、戻り値をREALSERVER_SELECTに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->current_record_rest_size = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 3;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x02;
        }
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 3u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        this->session_thread_data_map.clear();
    }

    cout << "[125]------------------------------------------" << endl;
    // unit_test[125] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[125] 且つdata_begain_offset が 0で,且つcheck_ssl_record_sendable()の戻り値が-1 (異常)の場合、戻り値をFINALIZEに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvbuffer[0] = 0x00;
        recvlen = 6;
        for(int i=1; i<recvlen; i++)
        {
            recvbuffer[i] = 0x03;
        }
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 6u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[126]------------------------------------------" << endl;
    // unit_test[126] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[126] 且つdata_begain_offset > 0で,且つcheck_ssl_record_sendable()の戻り値が-1 (異常)の場合、戻り値をFINALIZEに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvbuffer[0] = 0x00;
        recvlen = 6;
        for(int i=1; i<recvlen; i++)
        {
            recvbuffer[i] = 0x04;
        }
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 6u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[127]------------------------------------------" << endl;
    // unit_test[127] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[127] 且つdata_begain_offset = 0で,且つcheck_ssl_record_sendable()の戻り値が1(送信不可)の場合、戻り値をCLIENT_RECVに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 1;
        recvbuffer[0] = 0x05;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 1u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[128]------------------------------------------" << endl;
    // unit_test[128] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[128] 且つdata_begain_offset > 0で、且つcheck_ssl_record_sendable()の戻り値が1(送信不可)の場合、戻り値をCLIENT_RECVに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 1;
        recvbuffer[0] = 0x06;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 1u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[129]------------------------------------------" << endl;
    // unit_test[129] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[129] 且つdata_begain_offset = 0で,且つcheck_ssl_record_sendable()の戻り値が0(送信可能)の場合、戻り値をREALSERVER_SELECTに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 76;
        recvbuffer[0] = 0x16;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x01;
        recvbuffer[6] = 0x07;
        recvbuffer[7] = 0x07;
        recvbuffer[8] = 0x07;
        recvbuffer[9] = 0x03;
        recvbuffer[10] = 0x01;
        for(int i=11;i<recvlen;i++)
        {
            recvbuffer[i] = 0x07;
        }
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        this->session_thread_data_map.clear();
    }

    cout << "[130]------------------------------------------" << endl;
    // unit_test[130] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[130] 且つ、data_begain_offset > 0で、且つcheck_ssl_record_sendable()の戻り値が0(送信可能)の場合、戻り値にREALSERVER_SELECTを設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 76;
        recvbuffer[0] = 0x16;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x01;
        recvbuffer[6] = 0x08;
        recvbuffer[7] = 0x08;
        recvbuffer[8] = 0x08;
        recvbuffer[9] = 0x03;
        recvbuffer[10] = 0x01;
        for(int i=11; i< recvlen; i++)
        {
            recvbuffer[i] = 0x08;
        }
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        this->session_thread_data_map.clear();
    }

    cout << "[131]------------------------------------------" << endl;
    // unit_test[131] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[131] 且つ data_begain_offset = 0で、且つ且つcheck_ssl_record_sendable()の戻り値が0(送信可能)の場合、戻り値をREALSERVER_SELECTに設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 6;
        recvbuffer[0] = 0x17;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x09;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 6u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        this->session_thread_data_map.clear();
    }

    cout << "[132]------------------------------------------" << endl;
    // unit_test[132] 終了フラグがOFFで、 且つ データサイズが0で、 且つ 新SSLレコードで、
    // unit_test[132] 且つdata_begain_offset > 0で、且つcheck_ssl_record_sendable()の戻り値が0(送信可能)の場合、戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 6;
        recvbuffer[0] = 0x17;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x10;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), recvbuffer.c_array(), recvlen);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 6u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        this->session_thread_data_map.clear();
    }

    cout << "[133]------------------------------------------" << endl;
    // unit_test[133] 終了フラグがOFFで、 且つ データサイズ > 0、且つ新SSLレコードでなくて、且つdata_begain_offset > 0の場合
    // unit_test[133] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 36u;
        up_thread_data->current_record_rest_size = 15u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 5;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x11;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 15u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[134]------------------------------------------" << endl;
    // unit_test[134] 終了フラグがOFFで、且つ データサイズ > 0、且つ新SSLレコードでなくて、且つdata_begain_offset = 0の場合
    // unit_test[134] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 15u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 5;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x12;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 15u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[135]------------------------------------------" << endl;
    // unit_test[135] 終了フラグがOFFで、且つ データサイズ > 0、且つ新SSLレコードで,且つdata_begain_offset > 0
    // unit_test[135] 且つcheck_ssl_record_sendable()の戻り値が-1(異常)の場合
    // unit_test[135] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 10u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_begain_offset = 12u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 5;
        recvbuffer[0] = 0x00;
        for(int i=1; i< recvlen; i++)
        {
            recvbuffer[i] = 0x13;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 15u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[136]------------------------------------------" << endl;
    // unit_test[136] 終了フラグがOFFで、 且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset = 0
    // unit_test[136] 且つcheck_ssl_record_sendable()の戻り値が-1 (異常)の場合
    // unit_test[136] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 10u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 5;
        recvbuffer[0] = 0x00;
        for(int i=1; i< recvlen; i++)
        {
            recvbuffer[i] = 0x13;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 15u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[137]------------------------------------------" << endl;
    // unit_test[137] 終了フラグがOFFで、 且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset > 0
    // unit_test[137] 且つcheck_ssl_record_sendable()の戻り値が1 (送信不可)の場合
    // unit_test[137] 戻り値が CLIENT_RECVで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 1u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_begain_offset = 13u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 2;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x14;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 3u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[138]------------------------------------------" << endl;
    // unit_test[138] 終了フラグがOFFで、 且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset = 0
    // unit_test[138] 且つcheck_ssl_record_sendable()の戻り値が1 (送信不可)の場合
    // unit_test[138] 戻り値が CLIENT_RECVで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 1u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 2;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x15;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 3u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[139]------------------------------------------" << endl;
    // unit_test[139] 終了フラグがOFFで、 且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset > 0
    // unit_test[139] 且つcheck_ssl_record_sendable()の戻り値が０（送信可能）の場合
    // unit_test[139] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 3u;
        up_thread_data->data_begain_offset = 12u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[12] = 0x16;
        up_thread_data->data_buffer[13] = 0x03;
        up_thread_data->data_buffer[14] = 0x01;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 76;
        recvbuffer[0] = 0x00;
        recvbuffer[1] = 0x9e;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x00;
        recvbuffer[6] = 0x03;
        recvbuffer[7] = 0x01;
        for(int i=8; i<recvlen; i++)
        {
            recvbuffer[i] = 0x16;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 79u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[140]------------------------------------------" << endl;
    // unit_test[140] 終了フラグがOFFで、 且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset = 0
    // unit_test[140] 且つheck_ssl_record_sendable()の戻り値が０（送信可能）の場合
    // unit_test[140] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 3u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[12] = 0x16;
        up_thread_data->data_buffer[13] = 0x03;
        up_thread_data->data_buffer[14] = 0x01;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 76;
        recvbuffer[0] = 0x00;
        recvbuffer[1] = 0x9e;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x00;
        recvbuffer[6] = 0x03;
        recvbuffer[7] = 0x01;
        for(int i=8; i<recvlen; i++)
        {
            recvbuffer[i] = 0x17;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 79u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[141]------------------------------------------" << endl;
    // unit_test[141] 終了フラグがOFFで、且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset > 0
    // unit_test[141] 且つheck_ssl_record_sendable()の戻り値が０(送信可能)の場合
    // unit_test[141] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 3u;
        up_thread_data->data_begain_offset = 12u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[12] = 0x17;
        up_thread_data->data_buffer[13] = 0x03;
        up_thread_data->data_buffer[14] = 0x01;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 6;
        recvbuffer[0] = 0x00;
        recvbuffer[1] = 0x9e;
        for(int i=2; i<recvlen; i++)
        {
            recvbuffer[i] = 0x18;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 9u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[142]------------------------------------------" << endl;
    // unit_test[142] 終了フラグがOFFで、且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset = 0
    // unit_test[142] 且つcheck_ssl_record_sendable()の戻り値が０(送信可能)の場合
    // unit_test[142] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 3u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[0] = 0x17;
        up_thread_data->data_buffer[1] = 0x03;
        up_thread_data->data_buffer[2] = 0x01;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 6;
        recvbuffer[0] = 0x00;
        recvbuffer[1] = 0x9e;
        for(int i=2; i<recvlen; i++)
        {
            recvbuffer[i] = 0x19;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 9u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[143]------------------------------------------" << endl;
    // unit_test[143] 受信データサイズ > 受信バッファサイズの場合
    // unit_test[143] 戻り値がFINALIZEで設定する。
    recvlen = recvbuffer.size() + 1;
    status = this->handle_client_recv(boost::this_thread::get_id(),
            recvbuffer, recvlen);
    BOOST_CHECK_EQUAL(status, FINALIZE);

    cout << "[144]------------------------------------------" << endl;
    // unit_test[144] スレッドIDがmapに存在しない場合
    // unit_test[144] 戻り値がFINALIZEで設定する。
    status = this->handle_client_recv(boost::this_thread::get_id(),
            recvbuffer, recvlen);
    BOOST_CHECK_EQUAL(status, FINALIZE);

    cout << "[145]------------------------------------------" << endl;
    // unit_test[145] 終了フラグがOFFで,且つ data_size+recvlen > MAX_SSLID_BUFFER_SIZEの場合
    // unit_test[145] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 76u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        recvlen = 10;
        status = this->handle_client_recv(boost::this_thread::get_id(),
                recvbuffer, recvlen);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }


}

//handle_client_recv
void handle_client_recv_test_thread() {

    boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
    int recvlen = 0;
    EVENT_TAG status;
	char* mem_cmp_buffer;
    int mem_cmp_result;
	int mem_cmp_length;

    cout << "[141]------------------------------------------\n";
    // unit_test[141] 終了フラグがOFFで、且つ データサイズ > 0、且つ新SSLレコードで、且つdata_begain_offset > 0
    // unit_test[141] 且つheck_ssl_record_sendable()の戻り値が０(送信可能)の場合
    // unit_test[141] 戻り値がREALSERVER_SELECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_size = 3u;
        up_thread_data->data_begain_offset = 12u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[12] = 0x17;
        up_thread_data->data_buffer[13] = 0x03;
        up_thread_data->data_buffer[14] = 0x01;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
        }
        recvlen = 6;
        recvbuffer[0] = 0x00;
        recvbuffer[1] = 0x9e;
        for(int i=2; i<recvlen; i++)
        {
            recvbuffer[i] = 0x18;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array() + up_thread_data->data_begain_offset, up_thread_data->data_size);
        memcpy(mem_cmp_buffer + up_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = up_thread_data->data_size + recvlen;
        status = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
        mem_cmp_result = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 9u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
        delete[] mem_cmp_buffer;
    }
}

//handle_realserver_select(tcp)
void handle_realserver_select_tcp_test() {

    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint rs_endpoint;
    boost::asio::ip::tcp::endpoint comp_endpoint;
    boost::asio::ip::tcp::endpoint
            ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                    12345);
    boost::asio::ip::tcp::endpoint ep2;
    int maxlist = 1;
    char* sslid_replication_area_begain = new char[10];
    int sslid_replication_area_size = 0;
    boost::asio::ip::tcp::endpoint virtual_service_endpoint;
    std::string session_id;
    boost::function<LOG_LEVEL_TAG(void)> ingetloglevel = stb_getloglevel;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogFatal = stb_putLogFatal;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogError = stb_putLogError;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogWarn = stb_putLogWarn;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogInfo = stb_putLogInfo;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogDebug = stb_putLogDebug;

    cout << "[146]------------------------------------------" << endl;
    // unit_test[146] realserver接続回数が最大回数の場合、戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count
                = this->realserver_connect_failed_max_count;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
         this->session_thread_data_map.clear();
    }

    cout << "[147]------------------------------------------" << endl;
    // unit_test[147] realserver接続回数が最大回数を越える場合、戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count
                = this->realserver_connect_failed_max_count + 1;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[148]------------------------------------------" << endl;
    // unit_test[148] realserver接続回数が最大回数に未満で, reschedule が 1(ON)、endpointが決定の場合
    // unit_test[148] 戻り値が REALSERVER_CONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func1;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count
                = this->realserver_connect_failed_max_count - 1;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 1;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->selected_realserver != comp_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[149]------------------------------------------" << endl;
    // unit_test[149] realserver接続回数が最大回数に未満で, reschedule が 1(ON)、endpointが未決定の場合
    // unit_test[149] 戻り値が CLIENT_DISCONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func2;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count
                = this->realserver_connect_failed_max_count - 1;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 1;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[150]------------------------------------------" << endl;
    // unit_test[150] realserver接続回数が最大回数に未満で, 且つreschedule が 0 (OFF)の場合
    // unit_test[150] 戻り値が CLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count
                = this->realserver_connect_failed_max_count - 1;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 0;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[151]------------------------------------------" << endl;
    // unit_test[151] realserverの接続失敗回数が0, 且つselected_realserver が NULLでない場合
    // unit_test[151] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep1;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[152]------------------------------------------" << endl;
    // unit_test[152] realserverの接続失敗回数が0, 且つselected_realserver が NULLで、 且つhello_message_flagがfalseの場合
    // unit_test[152] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = false;
        up_thread_data->end_flag = END_FLAG_OFF;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[153]------------------------------------------" << endl;
    // unit_test[153] realserverの接続失敗回数が0, 且つselected_realserver が NULLで、 且つhello_message_flagがtrueで
    // unit_test[153] 且つendpointが決定の場合
    // unit_test[153] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x00;
        up_thread_data->data_size = 76u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->schedule_tcp = schedule_tcp_func1;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->selected_realserver != comp_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[154]------------------------------------------" << endl;
    // unit_test[154] realserver接続失敗回数が0で, 且つselected_realserverがNULLで、且つhello_message_flagがtrueで
    // unit_test[154] 且つセッションIDがなくで、且つendpointが未決定の場合
    // unit_test[154] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x00;
        up_thread_data->data_size = 76u;
        up_thread_data->end_flag = END_FLAG_OFF;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->schedule_tcp = schedule_tcp_func2;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[155]------------------------------------------" << endl;
    // unit_test[155] realserver接続失敗回数が0で, 且つselected_realserverがNULLで、且つhello_message_flagがtrueで
    // unit_test[155] 且つセッションIDがあるで, 且つendpointが決定の場合
    // unit_test[155] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_size = 76u;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        char* temp_data_buffer = up_thread_data->data_buffer.c_array();
        session_id.assign(temp_data_buffer + 44, temp_data_buffer + 76);
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        (dynamic_cast<sslid_session_data_processor_stub*> (session_data_processor))->get_session_endpoint_map()[session_id]
                = ep1;
        time_t last_time = time(0);
        (dynamic_cast<sslid_session_data_processor_stub*> (session_data_processor))->get_session_lasttime_map()[session_id]
                = last_time;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        delete this->session_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[156]------------------------------------------" << endl;
    // unit_test[156] realserver接続失敗回数が0で, 且つselected_realserverがNULLで、且つhello_message_flagがtrueで
    // unit_test[156] 且つセッションIDがあり, endpointが未決定で、reschedule後、endpointが決定の場合
    // unit_test[156] 戻り値がREALSERVER_CONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func1;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 1;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        delete this->session_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[157]------------------------------------------" << endl;
    // unit_test[157] realserver接続失敗回数が0で, 且つselected_realserverがNULLで、且つhello_message_flagがtrueで
    // unit_test[157] 且つセッションIDがあり, endpointが未決定で、reschedule後、endpointも未決定の場合
    // unit_test[157] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func2;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 1;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        delete this->session_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[158]------------------------------------------" << endl;
    // unit_test[158] realserver接続失敗回数が0で, 且つselected_realserverがNULLで, 且つhello_message_flagがtrueで
    // unit_test[158] 且つセッションIDがあり, endpointが未決定で、且つ no rescheduleの場合
    // unit_test[158] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = 0;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 0;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[159]------------------------------------------" << endl;
    // unit_test[159] realserver接続失敗回数が0で, 且つselected_realserver が NULLないの場合
    // unit_test[159] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep1;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[160]------------------------------------------" << endl;
    // unit_test[160] realserver接続失敗回数が0で, 且つselected_realserver が　NULLで, 且つhello_message_flag が falseの場合
    // unit_test[160] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = false;
        up_thread_data->end_flag = END_FLAG_OFF;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[161]------------------------------------------" << endl;
    // unit_test[161] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[161] 且つセッションIDがなくで、且つendpointが決定の場合
    // unit_test[161] 戻り値がREALSERVER_CONNECT設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x00;
        up_thread_data->data_size = 76u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->schedule_tcp = schedule_tcp_func1;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->selected_realserver != comp_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[162]------------------------------------------" << endl;
    // unit_test[162] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[162] 且つセッションIDがなくで、且つendpointが未決定の場合
    // unit_test[162] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x00;
        up_thread_data->data_size = 76u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->end_flag = END_FLAG_OFF;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->schedule_tcp = schedule_tcp_func2;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[163]------------------------------------------" << endl;
    // unit_test[163] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[163] 且つセッションIDがあるで, 且つendpointが決定の場合
    // unit_test[163] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_size = 76u;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        char* temp_data_buffer2 = up_thread_data->data_buffer.c_array();
        session_id.assign(temp_data_buffer2 + 44, temp_data_buffer2 + 76);
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        (dynamic_cast<sslid_session_data_processor_stub*> (session_data_processor))->get_session_endpoint_map()[session_id]
                = ep1;
        time_t last_time2 = time(0);
        (dynamic_cast<sslid_session_data_processor_stub*> (session_data_processor))->get_session_lasttime_map()[session_id]
                = last_time2;
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        delete this->session_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[164]------------------------------------------" << endl;
    // unit_test[164] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[164] 且つセッションIDがあるで, endpointが未決定で、reschedule後、endpointが決定の場合
    // unit_test[164] 戻り値がREALSERVER_CONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func1;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 1;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        delete this->session_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[165]------------------------------------------" << endl;
    // unit_test[165] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[165] 且つセッションIDがあるで, endpointが未決定で、reschedule後、endpointも未決定の場合
    // unit_test[165] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func2;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 1;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        delete this->session_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[166]------------------------------------------" << endl;
    // unit_test[166] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[166] セッションIDがあり、endpointが未決定でrescheduleしない場合
    // unit_test[166] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        this->reschedule = 0;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                1024, 3600, this->replication_data_processor, ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug);
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

}

//handle_realserver_select(tcp)
void handle_realserver_select_tcp_test_thread() {

    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint rs_endpoint;
    boost::asio::ip::tcp::endpoint comp_endpoint;
    boost::asio::ip::tcp::endpoint
            ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                    12345);
    int maxlist = 1;
    char* sslid_replication_area_begain = new char[10];
    int sslid_replication_area_size = 0;
    boost::asio::ip::tcp::endpoint ep2;
    boost::asio::ip::tcp::endpoint virtual_service_endpoint;
    std::string session_id;
    boost::function<LOG_LEVEL_TAG(void)> ingetloglevel = stb_getloglevel;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogFatal = stb_putLogFatal;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogError = stb_putLogError;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogWarn = stb_putLogWarn;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogInfo = stb_putLogInfo;
    boost::function<void(const unsigned int, const std::string&,
            const char*, int)> inputLogDebug = stb_putLogDebug;

    cout << "[164]------------------------------------------\n";
    // unit_test[164] realserver接続失敗回数 < 0で, 且つselected_realserver が NULLで, 且つhello_message_flag が trueで
    // unit_test[164] 且つセッションIDがあるで, endpointが未決定で、reschedule後、endpointが決定の場合
    // unit_test[164] 戻り値がREALSERVER_CONNECTで設定する。
    {
        this->schedule_tcp = schedule_tcp_func1;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->realserver_connect_failed_count = -1;
        up_thread_data->selected_realserver = ep2;
        up_thread_data->hello_message_flag = true;
        up_thread_data->data_buffer[43] = 0x20;
        up_thread_data->data_begain_offset = 0u;
        for (int i = 0; i < 32; i++) {
            up_thread_data->data_buffer[44 + i] = 0x01;
        }
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
            this->reschedule = 1;
            this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                    sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                    inputLogWarn, inputLogInfo, inputLogDebug);
            this->session_data_processor = new sslid_session_data_processor_stub(
                    1024, 3600, this->replication_data_processor, ingetloglevel,
                    inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                    inputLogDebug);
        }
        status = this->handle_realserver_select(boost::this_thread::get_id(),
                rs_endpoint);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
    }
}

//handle_realserver_select(udp)
void handle_realserver_select_udp_test(){
    cout << "[167]------------------------------------------" << endl;
    // unit_test[167] 戻り値が STOPで設定する。
    EVENT_TAG status;
    boost::array<char,MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen;
    boost::asio::ip::udp::endpoint rs_endpoint_udp;
    status = this->handle_realserver_select(boost::this_thread::get_id(),
            rs_endpoint_udp, sendbuffer, datalen);
    BOOST_CHECK_EQUAL(status, STOP);
}

//handle_realserver_connect
void handle_realserver_connect_test() {

    boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen = 0;
    EVENT_TAG status;
	char* mem_cmp_buffer;
	int mem_cmp_length;
	int mem_cmp_result;

    cout << "[168]------------------------------------------" << endl;
    // unit_test[168] current_record_rest_size>data_sizeで、且つdata_size<送信バッファサイズの場合
    // unit_test[168] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = 20u;
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x01;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, 10u);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[169]------------------------------------------" << endl;
    // unit_test[169] current_record_rest_size>data_sizeで, 且つdata_sizeが送信バッファサイズの場合
    // unit_test[169] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 1u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x02;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 1u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[170]------------------------------------------" << endl;
    // unit_test[170] current_record_rest_sizeがdata_sizeで, 且つdata_size<送信バッファサイズの場合
    // unit_test[170] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = 10u;
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x03;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, 10u);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[171]------------------------------------------" << endl;
    // unit_test[171] current_record_rest_sizeがdata_sizeで, 且つdata_sizeが送信バッファサイズの場合
    // unit_test[171] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x04;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[172]------------------------------------------" << endl;
    // unit_test[172] current_record_rest_size>data_sizeで,且つdata_size＞送信バッファサイズの場合
    // unit_test[172] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 5u;
        up_thread_data->data_begain_offset = 10u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x05;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 5u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[173]------------------------------------------" << endl;
    // unit_test[173] current_record_rest_sizeがdata_sizeで,且つdata_size＞送信バッファサイズの場合
    // unit_test[173] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_begain_offset = 10u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x06;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[174]------------------------------------------" << endl;
    // unit_test[174] current_record_rest_size＜data_sizeで, 且つcurrent_record_rest_size<送信バッファサイズの場合
    // unit_test[174] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = 10u;
        up_thread_data->data_size = 20u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x07;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->current_record_rest_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 15u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, 10u);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[175]------------------------------------------" << endl;
    // unit_test[175] current_record_rest_size<data_sizeで, 且つcurrent_record_rest_sizeが送信バッファサイズの場合
    // unit_test[175] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x08;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->current_record_rest_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 5u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[176]------------------------------------------" << endl;
    // unit_test[176] current_record_rest_size＜data_sizeで,且つcurrent_record_rest_size＞送信バッファサイズの場合
    // unit_test[176] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size =static_cast<size_t>(MAX_BUFFER_SIZE) + 20u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x09;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 5u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 20u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

}

//handle_realserver_connect
void handle_realserver_connect_test_thread() {

    boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen = 0;
    EVENT_TAG status;
	char* mem_cmp_buffer;
	int mem_cmp_length;
	int mem_cmp_result;

    cout << "[175]------------------------------------------\n";
    // unit_test[175] current_record_rest_size<data_sizeで, 且つcurrent_record_rest_sizeが送信バッファサイズの場合
    // unit_test[175] 戻り値がREALSERVER_SENDで設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset;i<up_thread_data->data_begain_offset+up_thread_data->data_size;i++)
        {
            up_thread_data->data_buffer[i] = 0x08;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->current_record_rest_size;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
        }
        status = this->handle_realserver_connect(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(sendbuffer.c_array(), mem_cmp_buffer, mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 5u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_connect_failed_count, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
    }
}

//handle_realserver_connection_fail
void handle_realserver_connection_fail_test(){
	boost::asio::ip::tcp::endpoint ep;
	session_thread_data_sslid* thread_up_data_value = new session_thread_data_sslid;
	thread_up_data_value->thread_division=THREAD_DIVISION_UP_STREAM;
	thread_up_data_value->realserver_connect_failed_count=0;
	thread_up_data_value->data_begain_offset=0;
	thread_up_data_value->data_size=0;
	thread_up_data_value->current_record_rest_size=0;
	thread_up_data_value->hello_message_flag=false;
	thread_data_ptr thread_data(thread_up_data_value);
	this->session_thread_data_map[boost::this_thread::get_id()]=thread_data;
	std::map<const boost::thread::id,thread_data_ptr>::iterator iter;
	iter=this->session_thread_data_map.find(boost::this_thread::get_id());
	thread_data_ptr data=iter->second;

    cout << "[177]------------------------------------------" << endl;
	//unit_test[177] 終了フラグをON,遷移先ステータスを設定する,status = CLIENT_DISCONNECT
	//unit_test[177] test data:no rescheduleモード、初めて失敗するの場合
	this->reschedule=0;
	EVENT_TAG schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	BOOST_CHECK_EQUAL(data->end_flag,END_FLAG_ON);
	BOOST_CHECK_EQUAL(schedule, protocol_module_base::CLIENT_DISCONNECT);

    cout << "[178]------------------------------------------" << endl;
	//unit_test[178] realserver_connect_failed_count で1を加算する,遷移先ステータスを設定する,status = REALSERVER_SELECT
	//unit_test[178] test data:rescheduleモード、初めて失敗するの場合
	this->reschedule=1;
	schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	BOOST_CHECK_EQUAL(data->realserver_connect_failed_count,1);
	BOOST_CHECK_EQUAL(schedule, protocol_module_base::REALSERVER_SELECT);

    cout << "[179]------------------------------------------" << endl;
	//unit_test[179] realserver_connect_failed_count で1を加算する,遷移先ステータスにREALSERVER_SELECTを設定する
	//unit_test[179] test data:rescheduleモード、３目失敗するの場合
	this->reschedule=1;
	schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	BOOST_CHECK_EQUAL(data->realserver_connect_failed_count,3);
	BOOST_CHECK_EQUAL(schedule, protocol_module_base::REALSERVER_SELECT);

    cout << "[180]------------------------------------------" << endl;
	//unit_test[180] 終了フラグをON,遷移先ステータスにCLIENT_DISCONNECTを設定する
	//unit_test[180] test data:no rescheduleモード、４目失敗するの場合
	this->reschedule=0;
	schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	BOOST_CHECK_EQUAL(data->end_flag,END_FLAG_ON);
	BOOST_CHECK_EQUAL(schedule, protocol_module_base::CLIENT_DISCONNECT);

	this->session_thread_data_map.clear();
}

void handle_realserver_connection_fail_test_thread_reschedule(){
	boost::asio::ip::tcp::endpoint ep;
	session_thread_data_sslid* thread_up_data_value = new session_thread_data_sslid;
	thread_up_data_value->thread_division=THREAD_DIVISION_UP_STREAM;
	thread_up_data_value->realserver_connect_failed_count=0;
	thread_up_data_value->data_begain_offset=0;
	thread_up_data_value->data_size=0;
	thread_up_data_value->current_record_rest_size=0;
	thread_up_data_value->hello_message_flag=false;
	thread_data_ptr thread_data(thread_up_data_value);
	thread_data_ptr data;
	{
		boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
		this->session_thread_data_map[boost::this_thread::get_id()]=thread_data;
		std::map<const boost::thread::id,thread_data_ptr>::iterator iter;
		iter=this->session_thread_data_map.find(boost::this_thread::get_id());
		data=iter->second;
		this->reschedule=1;
	}

    cout << "[179]------------------------------------------\n";
	//unit_test[179] realserver_connect_failed_count で1を加算する,遷移先ステータスにREALSERVER_SELECTを設定する
	//unit_test[179] test data:rescheduleモード、３目失敗するの場合
	EVENT_TAG schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	BOOST_CHECK_EQUAL(data->realserver_connect_failed_count,3);
	BOOST_CHECK_EQUAL(schedule, protocol_module_base::REALSERVER_SELECT);
}

void handle_realserver_connection_fail_test_thread_noreschedule(){
	boost::asio::ip::tcp::endpoint ep;
	session_thread_data_sslid* thread_up_data_value = new session_thread_data_sslid;
	thread_up_data_value->thread_division=THREAD_DIVISION_UP_STREAM;
	thread_up_data_value->realserver_connect_failed_count=0;
	thread_up_data_value->data_begain_offset=0;
	thread_up_data_value->data_size=0;
	thread_up_data_value->current_record_rest_size=0;
	thread_up_data_value->hello_message_flag=false;
	thread_data_ptr thread_data(thread_up_data_value);
	thread_data_ptr data;
	{
		boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
		this->session_thread_data_map[boost::this_thread::get_id()]=thread_data;
		std::map<const boost::thread::id,thread_data_ptr>::iterator iter;
		iter=this->session_thread_data_map.find(boost::this_thread::get_id());
		data=iter->second;
		this->reschedule=0;
	}

    cout << "[177]------------------------------------------\n";
	//unit_test[177] 終了フラグをON,遷移先ステータスを設定する,status = CLIENT_DISCONNECT
	//unit_test[177] test data:no rescheduleモード、初めて失敗するの場合
	EVENT_TAG schedule=this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
	BOOST_CHECK_EQUAL(data->end_flag,END_FLAG_ON);
	BOOST_CHECK_EQUAL(schedule, protocol_module_base::CLIENT_DISCONNECT);
}

//handle_realserver_send
void handle_realserver_send_test() {

    EVENT_TAG status;

    cout << "[181]------------------------------------------" << endl;
    // unit_test[181] データサイズが0で、且つdata_begain_offsetが0の場合
    // unit_test[181] 戻り値がCLIENT_RECVで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[182]------------------------------------------" << endl;
    // unit_test[182] データサイズが0で,且つdata_begain_offset >0の場合
    // unit_test[182] 戻り値がCLIENT_RECVで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[183]------------------------------------------" << endl;
    // unit_test[183] データサイズ > 0で, 且つcurrent_record_rest_size > 0で,且つdata_begain_offset=0の場合
    // unit_test[183] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 10u;
        up_thread_data->current_record_rest_size = 10u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[184]------------------------------------------" << endl;
    // unit_test[184] データサイズ > 0で, 且つcurrent_record_rest_size > 0で,且つdata_begain_offset>0の場合
    // unit_test[184] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 10u;
        up_thread_data->current_record_rest_size = 10u;
        up_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[185]------------------------------------------" << endl;
    // unit_test[185] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が-1(異常)で,且つdata_begain_offsetが0の場合
    // unit_test[185] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[0] = 0x00;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[186]------------------------------------------" << endl;
    // unit_test[186] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が-1(異常)で,且つdata_begain_offset>0の場合
    // unit_test[186] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[0] = 0x00;
        up_thread_data->data_begain_offset =10u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[187]------------------------------------------" << endl;
    // unit_test[187] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が1(送信不可)で,且つdata_begain_offsetが0の場合
    // unit_test[187] 戻り値がCLIENT_RECVで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 1u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[188]------------------------------------------" << endl;
    // unit_test[188] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が1(送信不可)で,且つdata_begain_offset>0の場合
    // unit_test[188] 戻り値がCLIENT_RECVで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 1u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[189]------------------------------------------" << endl;
    // unit_test[189] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_send()の戻り値が０(送信可能)で,且つdata_begain_offset が0の場合
    // unit_test[189] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 76u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[0] = 0x16;
        up_thread_data->data_buffer[1] = 0x03;
        up_thread_data->data_buffer[2] = 0x01;
        up_thread_data->data_buffer[3] = 0x00;
        up_thread_data->data_buffer[4] = 0x9e;
        up_thread_data->data_buffer[5] = 0x01;
        up_thread_data->data_buffer[9] = 0x03;
        up_thread_data->data_buffer[10] = 0x01;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->hello_message_flag = false;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[190]------------------------------------------" << endl;
    // unit_test[190] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_send()の戻り値が0(送信可能)で,且つdata_begain_offset > 0の場合
    // unit_test[190] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 76u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[10] = 0x16;
        up_thread_data->data_buffer[11] = 0x03;
        up_thread_data->data_buffer[12] = 0x01;
        up_thread_data->data_buffer[13] = 0x00;
        up_thread_data->data_buffer[14] = 0x9e;
        up_thread_data->data_buffer[15] = 0x01;
        up_thread_data->data_buffer[19] = 0x03;
        up_thread_data->data_buffer[20] = 0x01;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->hello_message_flag = false;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[191]------------------------------------------" << endl;
    // unit_test[191] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_send()の戻り値が0(送信可能)で,且つdata_begain_offset が0の場合
    // unit_test[191] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 10u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[0] = 0x17;
        up_thread_data->data_buffer[1] = 0x03;
        up_thread_data->data_buffer[2] = 0x01;
        up_thread_data->data_buffer[3] = 0x00;
        up_thread_data->data_buffer[4] = 0x9e;
        up_thread_data->data_begain_offset = 0u;
        up_thread_data->hello_message_flag = true;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[192]------------------------------------------" << endl;
    // unit_test[192] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_send()の戻り値が0(送信可能)で,且つdata_begain_offset > 0の場合
    // unit_test[192] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 10u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[10] = 0x17;
        up_thread_data->data_buffer[11] = 0x03;
        up_thread_data->data_buffer[12] = 0x01;
        up_thread_data->data_buffer[13] = 0x00;
        up_thread_data->data_buffer[14] = 0x9e;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->hello_message_flag = true;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
        this->session_thread_data_map.clear();
    }

}

//handle_realserver_send
void handle_realserver_send_test_thread() {

    EVENT_TAG status;

    cout << "[190]------------------------------------------\n";
    // unit_test[190] データサイズ > 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_send()の戻り値が0(送信可能)で,且つdata_begain_offset > 0の場合
    // unit_test[190] 戻り値がREALSERVER_CONNECTで設定する。
    {
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->data_size = 76u;
        up_thread_data->current_record_rest_size = 0u;
        up_thread_data->data_buffer[10] = 0x16;
        up_thread_data->data_buffer[11] = 0x03;
        up_thread_data->data_buffer[12] = 0x01;
        up_thread_data->data_buffer[13] = 0x00;
        up_thread_data->data_buffer[14] = 0x9e;
        up_thread_data->data_buffer[15] = 0x01;
        up_thread_data->data_buffer[19] = 0x03;
        up_thread_data->data_buffer[20] = 0x01;
        up_thread_data->data_begain_offset = 10u;
        up_thread_data->hello_message_flag = false;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
        }
        status = this->handle_realserver_send(boost::this_thread::get_id());
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
    }
}

//handle_realserver_recv(tcp)
void handle_realserver_recv_tcp_test() {

    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint rs_endpoint;
    boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
    int recvlen;
	int mem_cmp_result;
	int mem_cmp_length;
	char* mem_cmp_buffer;

    cout << "[193]------------------------------------------" << endl;
    // unit_test[193] データサイズ が 0で, 且つcurrent_record_rest_size > 0の場合
    // unit_test[193] 戻り値がCLIENT_CONNECTION_CHECK設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->current_record_rest_size = 10u;
        recvlen = 0;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }

    cout << "[194]------------------------------------------" << endl;
    // unit_test[194] データサイズ が 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が-1(異常)の場合
    // unit_test[194] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        recvbuffer[0] = 0x00;
        recvlen = 6;
        for(int i=1; i<recvlen; i++)
        {
            recvbuffer[i] = 0x01;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, down_thread_data->data_buffer.c_array()+down_thread_data->data_begain_offset, down_thread_data->data_size);
        memcpy(mem_cmp_buffer+down_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = down_thread_data->data_size + recvlen;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        mem_cmp_result = memcmp(mem_cmp_buffer, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[195]------------------------------------------" << endl;
    // unit_test[195] データサイズ が 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が1(送信不可)の場合
    // unit_test[195] 戻り値がREALSERVER_RECVで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->data_begain_offset = 0u;
        down_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        recvlen = 3;
        for(int i=0; i<recvlen; i++)
        {
            recvbuffer[i] = 0x02;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, down_thread_data->data_buffer.c_array()+down_thread_data->data_begain_offset, down_thread_data->data_size);
        memcpy(mem_cmp_buffer+down_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = down_thread_data->data_size + recvlen;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        mem_cmp_result = memcmp(mem_cmp_buffer, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[196]------------------------------------------" << endl;
    // unit_test[196] データサイズ が 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が0(送信可能)の場合
    // unit_test[196] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->data_begain_offset = 0u;
        down_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        recvbuffer[0] = 0x16;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x02;
        recvbuffer[6] = 0x03;
        recvbuffer[7] = 0x03;
        recvbuffer[8] = 0x03;
        recvbuffer[9] = 0x03;
        recvbuffer[10] = 0x01;
        recvlen = 76;
        for(int i=11; i<recvlen; i++)
        {
            recvbuffer[i] = 0x03;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, down_thread_data->data_buffer.c_array()+down_thread_data->data_begain_offset, down_thread_data->data_size);
        memcpy(mem_cmp_buffer+down_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = down_thread_data->data_size + recvlen;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        mem_cmp_result = memcmp(mem_cmp_buffer, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[197]------------------------------------------" << endl;
    // unit_test[197] データサイズ が 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値0(送信可能)の場合
    // unit_test[197] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->data_begain_offset = 0u;
        down_thread_data->current_record_rest_size = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        recvbuffer[0] = 0x17;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvlen = 10;
        for(int i=5; i<recvlen; i++)
        {
            recvbuffer[i] = 0x04;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, down_thread_data->data_buffer.c_array()+down_thread_data->data_begain_offset, down_thread_data->data_size);
        memcpy(mem_cmp_buffer+down_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = down_thread_data->data_size + recvlen;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        mem_cmp_result = memcmp(mem_cmp_buffer, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[198]------------------------------------------" << endl;
    // unit_test[198] データサイズ > 0で, 且つcurrent_record_rest_size > 0で, 且つdata_begain_offset>0の場合
    // unit_test[198] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10u;
        down_thread_data->data_begain_offset = 5u;
        down_thread_data->current_record_rest_size = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        recvlen = 15;
        for(int i=5; i<recvlen; i++)
        {
            recvbuffer[i] = 0x05;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, down_thread_data->data_buffer.c_array()+down_thread_data->data_begain_offset, down_thread_data->data_size);
        memcpy(mem_cmp_buffer+down_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = down_thread_data->data_size + recvlen;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        mem_cmp_result = memcmp(mem_cmp_buffer, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 25u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[199]------------------------------------------" << endl;
    // unit_test[199] データサイズ + 受信データサイズ > MAX_SSLID_BUFFER_SIZEの場合
    // unit_test[199] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 76u + 1u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        recvlen = MAX_BUFFER_SIZE;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

}

//handle_realserver_recv(tcp)
void handle_realserver_recv_tcp_test_thread() {

    EVENT_TAG status;
    boost::asio::ip::tcp::endpoint rs_endpoint;
    boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
    int recvlen;
	int mem_cmp_result;
	int mem_cmp_length;
	char* mem_cmp_buffer;

    cout << "[196]------------------------------------------\n";
    // unit_test[196] データサイズ が 0で, 且つcurrent_record_rest_size が 0で, 且つcheck_ssl_record_sendable()の戻り値が0(送信可能)の場合
    // unit_test[196] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->data_begain_offset = 0u;
        down_thread_data->current_record_rest_size = 0u;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        }
        recvbuffer[0] = 0x16;
        recvbuffer[1] = 0x03;
        recvbuffer[2] = 0x01;
        recvbuffer[3] = 0x00;
        recvbuffer[4] = 0x9e;
        recvbuffer[5] = 0x02;
        recvbuffer[6] = 0x03;
        recvbuffer[7] = 0x03;
        recvbuffer[8] = 0x03;
        recvbuffer[9] = 0x03;
        recvbuffer[10] = 0x01;
        recvlen = 76;
        for(int i=11; i<recvlen; i++)
        {
            recvbuffer[i] = 0x03;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, down_thread_data->data_buffer.c_array()+down_thread_data->data_begain_offset, down_thread_data->data_size);
        memcpy(mem_cmp_buffer+down_thread_data->data_size, recvbuffer.c_array(), recvlen);
        mem_cmp_length = down_thread_data->data_size + recvlen;
        status = this->handle_realserver_recv(boost::this_thread::get_id(),
                rs_endpoint, recvbuffer, recvlen);
        mem_cmp_result = memcmp(mem_cmp_buffer, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    }
}

//handle_realserver_recv(udp)
void handle_realserver_recv_udp_test(){
    cout << "[200]------------------------------------------" << endl;
    // unit_test[200] 戻り値が STOPで設定する。
    boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
    size_t recvlen;
    boost::asio::ip::udp::endpoint rs_endpoint_udp;
    EVENT_TAG status = this->handle_realserver_recv(boost::this_thread::get_id(),
            rs_endpoint_udp, recvbuffer, recvlen);
    BOOST_CHECK_EQUAL(status, STOP);
}

//handle_client_connection_check
void handle_client_connection_check_test() {

    boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen = 0;
    EVENT_TAG status;
    int maxlist = 0;
    int timeout = 0;
    char* sslid_replication_area_begain;
    int sslid_replication_area_size = 0;
    std::string session_id;
    boost::asio::ip::tcp::endpoint virtual_service_endpoint;
    sslid_replication_data_processor * replication_data_processor = NULL;
    getloglevel_func_type ingetloglevel = stb_getloglevel;
    logger_func_type inputLogFatal = stb_putLogFatal;
    logger_func_type inputLogError = stb_putLogError;
    logger_func_type inputLogWarn = stb_putLogWarn;
    logger_func_type inputLogInfo = stb_putLogInfo;
    logger_func_type inputLogDebug = stb_putLogDebug;
	boost::asio::ip::tcp::endpoint rs_endpoint(boost::asio::ip::address::from_string("192.168.120.249"), 12345);

    cout << "[201]------------------------------------------" << endl;
    // unit_test[201] mapに下りスレッドIDが存在しない場合
    // unit_test[201] 戻り値がFINALIZEで設定する。
    status = this->handle_client_connection_check(boost::this_thread::get_id(),
            sendbuffer, datalen);
    BOOST_CHECK_EQUAL(status, FINALIZE);

    cout << "[202]------------------------------------------" << endl;
    // unit_test[202] current_record_rest_size>0で,且つdata_begain_offset が 0の場合
    // unit_test[202] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 10u;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 66u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        this->session_thread_data_map.clear();
    }

    cout << "[203]------------------------------------------" << endl;
    // unit_test[203] current_record_rest_size>0で,且つdata_begain_offset >0の場合
    // unit_test[203] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 10u;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 66u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 20u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        this->session_thread_data_map.clear();
    }


    cout << "[204]------------------------------------------" << endl;
    // unit_test[204] current_record_rest_sizeが0で、且つhello_message_flagがtrueで、且つdata_size ≧ 76で、且つdata_begain_offsetが0で
    // unit_test[204] 且つget_ssl_session_id() の戻り値が 0 の場合
    // unit_test[204] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = true;
        down_thread_data->data_buffer[43] = 0x20;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 0u;
        for(size_t i=44; i<down_thread_data->data_size; i++)
        {
            down_thread_data->data_buffer[i] = 0x01;
        }
        session_id.assign(down_thread_data->data_buffer.c_array()+44,down_thread_data->data_buffer.c_array()+76);
        down_thread_data->selected_realserver = rs_endpoint;
        maxlist = 3;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                maxlist, timeout, this->replication_data_processor, stb_getloglevel,
                stb_putLogFatal, stb_putLogError, stb_putLogWarn,
                stb_putLogInfo, stb_putLogDebug);
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL((dynamic_cast<sslid_session_data_processor_stub*>(this->session_data_processor))->get_session_endpoint_map()[session_id], rs_endpoint);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        delete replication_data_processor;
        this->session_thread_data_map.clear();
    }

    cout << "[205]------------------------------------------" << endl;
    // unit_test[205] current_record_rest_sizeが0で,且つhello_message_flagがtrueで,且つdata_size ≧ 76で,且つdata_begain_offset>0で
    // unit_test[205] 且つget_ssl_session_id()の戻り値が0の場合
    // unit_test[205] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = true;
        down_thread_data->data_buffer[53] = 0x20;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 10u;
        for(size_t i=54; i<down_thread_data->data_size; i++)
        {
            down_thread_data->data_buffer[i] = 0x01;
        }
        session_id.assign(down_thread_data->data_buffer.c_array()+54,down_thread_data->data_buffer.c_array()+86);
        down_thread_data->selected_realserver = rs_endpoint;
        maxlist = 3;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        this->replication_data_processor = new sslid_replication_data_processor(maxlist, sslid_replication_area_begain,
                sslid_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
        this->session_data_processor = new sslid_session_data_processor_stub(
                maxlist, timeout, this->replication_data_processor, stb_getloglevel,
                stb_putLogFatal, stb_putLogError, stb_putLogWarn,
                stb_putLogInfo, stb_putLogDebug);
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL((dynamic_cast<sslid_session_data_processor_stub*>(this->session_data_processor))->get_session_endpoint_map()[session_id], rs_endpoint);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        delete replication_data_processor;
        this->session_thread_data_map.clear();
    }


    cout << "[206]------------------------------------------" << endl;
    // unit_test[206] current_record_rest_sizeが0で,且つhello_message_flagがtrueで,且つdata_begain_offsetが0で
    // unit_test[206] 且つget_ssl_session_id()の戻り値が1の場合
    // unit_test[206] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = true;
        down_thread_data->data_buffer[43] = 0x00;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        this->session_thread_data_map.clear();
    }

    cout << "[207]------------------------------------------" << endl;
    // unit_test[207] current_record_rest_sizeが0で,且つhello_message_flagがtrueで,且つdata_begain_offset>0で
    // unit_test[207] 且つget_ssl_session_id()の戻り値が1の場合
    // unit_test[207] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = true;
        down_thread_data->data_buffer[53] = 0x00;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        this->session_thread_data_map.clear();
    }

    cout << "[208]------------------------------------------" << endl;
    // unit_test[208] current_record_rest_sizeが0で,且つhello_message_flagがfalseで,且つdata_begain_offsetが0の場合
    // unit_test[208] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = false;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        this->session_thread_data_map.clear();
    }

    cout << "[209]------------------------------------------" << endl;
    // unit_test[209] current_record_rest_sizeが0で,且つhello_message_flagがfalseで,且つdata_begain_offset>0の場合
    // unit_test[209] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = false;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        this->session_thread_data_map.clear();
    }

}

//handle_client_connection_check
void handle_client_connection_check_test_thread() {

    boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen = 0;
    EVENT_TAG status;
    std::string session_id;
    boost::asio::ip::tcp::endpoint virtual_service_endpoint;
    getloglevel_func_type ingetloglevel = stb_getloglevel;
    logger_func_type inputLogFatal = stb_putLogFatal;
    logger_func_type inputLogError = stb_putLogError;
    logger_func_type inputLogWarn = stb_putLogWarn;
    logger_func_type inputLogInfo = stb_putLogInfo;
    logger_func_type inputLogDebug = stb_putLogDebug;
	boost::asio::ip::tcp::endpoint rs_endpoint(boost::asio::ip::address::from_string("192.168.120.249"), 12345);

    cout << "[208]------------------------------------------\n";
    // unit_test[208] current_record_rest_sizeが0で,且つhello_message_flagがfalseで,且つdata_begain_offsetが0の場合
    // unit_test[208] 戻り値がCLIENT_SENDで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->hello_message_flag = false;
        down_thread_data->data_size = 76u;
        down_thread_data->data_begain_offset = 0u;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        }
        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                sendbuffer, datalen);
        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 76u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
    }
}

//handle_client_select
void handle_client_select_test(){
    cout << "[210]------------------------------------------" << endl;
	// unit_test[210] 戻り値がSTOPで設定する。
	boost::asio::ip::udp::endpoint cl_endpoint;
	boost::array<char,MAX_BUFFER_SIZE> sendbuffer;
	size_t datalen;
	EVENT_TAG status;
	status = this->handle_client_select(boost::this_thread::get_id(), cl_endpoint, sendbuffer, datalen);
	BOOST_CHECK_EQUAL(status, STOP);
}

//handle_client_send
void handle_client_send_test() {

    EVENT_TAG status;

    cout << "[211]------------------------------------------" << endl;
    // unit_test[211] 下りスレッドIDがmapに存在しない場合
    // unit_test[211] 戻り値がFINALIZEで設定する。
    this->session_thread_data_map.clear();
    status = this->handle_client_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);

    cout << "[212]------------------------------------------" << endl;
    // unit_test[212] データサイズが0で,且つ終了フラグがONで,且つdata_begain_offsetが0の場合
    // unit_test[212] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->end_flag = END_FLAG_ON;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[213]------------------------------------------" << endl;
    // unit_test[213] データサイズが0で,且つ終了フラグがONで,且つdata_begain_offset>0の場合
    // unit_test[213] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->end_flag = END_FLAG_ON;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[214]------------------------------------------" << endl;
    // unit_test[214] データサイズが0で,且つ終了フラグがOFFで,且つdata_begain_offset が 0の場合
    // unit_test[214] 戻り値がREALSERVER_RECVで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->end_flag = END_FLAG_OFF;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[215]------------------------------------------" << endl;
    // unit_test[215] データサイズが0で,且つ終了フラグがOFFで,且つdata_begain_offset > 0の場合
    // unit_test[215] 戻り値がREALSERVER_RECVで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 0u;
        down_thread_data->end_flag = END_FLAG_OFF;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[216]------------------------------------------" << endl;
    // unit_test[216] データサイズ>0で,且つcurrent_record_rest_size>0で,且つdata_begain_offsetが0の場合
    // unit_test[216] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10u;
        down_thread_data->current_record_rest_size = 8u;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }

    cout << "[217]------------------------------------------" << endl;
    // unit_test[217] データサイズ>0で,且つcurrent_record_rest_size>0で,且つdata_begain_offsetが0で,且つdata_begain_offset>0の場合
    // unit_test[217] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10u;
        down_thread_data->current_record_rest_size = 8u;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }

    cout << "[218]------------------------------------------" << endl;
    // unit_test[218] データサイズ>0で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が-1(異常)で,且つdata_begain_offsetが0の場合
    // unit_test[218] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->data_buffer[0] = 0x00;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[219]------------------------------------------" << endl;
    // unit_test[219] データサイズ>0で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が-1(異常)で,且つdata_begain_offset>0の場合
    // unit_test[219] 戻り値がFINALIZEで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->data_buffer[10] = 0x00;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, FINALIZE);
        this->session_thread_data_map.clear();
    }

    cout << "[220]------------------------------------------" << endl;
    // unit_test[220] データサイズ>0で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が1(送信不可)で,且つdata_begain_offsetが0で
    // unit_test[220] 且つ終了フラグがONの場合
    // unit_test[220] 戻る値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 3u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->end_flag = END_FLAG_ON;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[221]------------------------------------------" << endl;
    // unit_test[221] データサイズ>0で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が1(送信不可)で,且つdata_begain_offset>0で
    // unit_test[221] 且つ終了フラグがONの場合
    // unit_test[221] 戻り値がCLIENT_DISCONNECTで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 3u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->end_flag = END_FLAG_ON;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
        this->session_thread_data_map.clear();
    }

    cout << "[222]------------------------------------------" << endl;
    // unit_test[222] データサイズ>0で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が1(送信不可)で,且つdata_begain_offsetが0で
    // unit_test[222] 且つ終了フラグがOFFの場合
    // unit_test[222] 戻り値がREALSERVER_RECVで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 3u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->end_flag = END_FLAG_OFF;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[223]------------------------------------------" << endl;
    // unit_test[223] データサイズ>0で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が1(送信不可)で,且つdata_begain_offset>0で
    // unit_test[223] 且つ終了フラグがOFFの場合
    // unit_test[223] 戻り値がREALSERVER_RECVで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 3u;
        down_thread_data->current_record_rest_size = 0u;
        down_thread_data->end_flag = END_FLAG_OFF;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
        this->session_thread_data_map.clear();
    }

    cout << "[224]------------------------------------------" << endl;
    // unit_test[224] データサイズ>=76で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が0(送信可能)で,且つdata_begain_offsetが0の場合
    // unit_test[224] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 76;
        down_thread_data->current_record_rest_size = 0;
        down_thread_data->data_buffer[0] = 0x16;
        down_thread_data->data_buffer[1] = 0x03;
        down_thread_data->data_buffer[2] = 0x01;
        down_thread_data->data_buffer[3] = 0x00;
        down_thread_data->data_buffer[4] = 0x9e;
        down_thread_data->data_buffer[5] = 0x02;
        down_thread_data->data_buffer[9] = 0x03;
        down_thread_data->data_buffer[10] = 0x01;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }

    cout << "[225]------------------------------------------" << endl;
    // unit_test[225] データサイズ>=76で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が0(送信可能)で,且つdata_begain_offset>0が０の場合
    // unit_test[225] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 86;
        down_thread_data->current_record_rest_size = 0;
        down_thread_data->data_buffer[10] = 0x16;
        down_thread_data->data_buffer[11] = 0x03;
        down_thread_data->data_buffer[12] = 0x01;
        down_thread_data->data_buffer[13] = 0x00;
        down_thread_data->data_buffer[14] = 0x9e;
        down_thread_data->data_buffer[15] = 0x02;
        down_thread_data->data_buffer[19] = 0x03;
        down_thread_data->data_buffer[20] = 0x01;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }

    cout << "[226]------------------------------------------" << endl;
    // unit_test[226] データサイズ>=6で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が0(送信可能)で,且つdata_begain_offsetが0の場合
    // unit_test[226] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10;
        down_thread_data->current_record_rest_size = 0;
        down_thread_data->data_buffer[0] = 0x17;
        down_thread_data->data_buffer[1] = 0x03;
        down_thread_data->data_buffer[2] = 0x01;
        down_thread_data->data_buffer[3] = 0x00;
        down_thread_data->data_buffer[4] = 0x9e;
        down_thread_data->data_begain_offset = 0u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }

    cout << "[227]------------------------------------------" << endl;
    // unit_test[227] データサイズ>=6で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が0(送信可能)で,且つdata_begain_offset>0の場合
    // unit_test[227] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 10;
        down_thread_data->current_record_rest_size = 0;
        down_thread_data->data_buffer[10] = 0x17;
        down_thread_data->data_buffer[11] = 0x03;
        down_thread_data->data_buffer[12] = 0x01;
        down_thread_data->data_buffer[13] = 0x00;
        down_thread_data->data_buffer[14] = 0x9e;
        down_thread_data->data_begain_offset = 10u;
        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(!this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
        this->session_thread_data_map.clear();
    }
}

//handle_client_send
void handle_client_send_test_thread() {

    EVENT_TAG status;

    cout << "[225]------------------------------------------\n";
    // unit_test[225] データサイズ>=76で,且つcurrent_record_rest_sizeが0で,且つcheck_ssl_record_sendable()の戻り値が0(送信可能)で,且つdata_begain_offset>0が０の場合
    // unit_test[225] 戻り値がCLIENT_CONNECTION_CHECKで設定する。
    {
        thread_data_ptr down_thread_data(new session_thread_data_sslid);
        down_thread_data->data_size = 86;
        down_thread_data->current_record_rest_size = 0;
        down_thread_data->data_buffer[10] = 0x16;
        down_thread_data->data_buffer[11] = 0x03;
        down_thread_data->data_buffer[12] = 0x01;
        down_thread_data->data_buffer[13] = 0x00;
        down_thread_data->data_buffer[14] = 0x9e;
        down_thread_data->data_buffer[15] = 0x02;
        down_thread_data->data_buffer[19] = 0x03;
        down_thread_data->data_buffer[20] = 0x01;
        down_thread_data->data_begain_offset = 10u;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
        }
        status = this->handle_client_send(boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 163u);
        BOOST_CHECK(this->session_thread_data_map[boost::this_thread::get_id()]->hello_message_flag);
        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    }
}

//handle_client_disconnect
void handle_client_disconnect_test(){
    cout << "[228]------------------------------------------" << endl;
	//unit_test[228] handle_client_disconnect()メソッドのテスト,up and down threads,正常系で必ずFINALIZEを返す
	boost::thread tdown_for_get_id(down_thread_func);
	boost::thread_group threads;
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_client_disconnect_test_thread_func,
                                   this,
                                   boost::this_thread::get_id()));
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_client_disconnect_test_thread_func,
                                   this,
                                   tdown_for_get_id.get_id()));
    	threads.join_all();

    cout << "[229]------------------------------------------" << endl;
	//unit_test[229] one thread,return FINALIZE
	BOOST_CHECK_EQUAL(this->handle_client_disconnect(boost::this_thread::get_id()),FINALIZE);
}
void handle_client_disconnect_test_thread_func(const boost::thread::id thread_id){
    EVENT_TAG ret = this->handle_client_disconnect(thread_id);
    BOOST_CHECK_EQUAL(ret, FINALIZE);
}

//handle_realserver_disconnect
void handle_realserver_disconnect_test(){
    cout << "[230]------------------------------------------" << endl;
	//unit_test[230] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_realserver_disconnect(boost::this_thread::get_id(),ep), STOP);
}

//handle_sorryserver_select
void handle_sorryserver_select_test(){
    cout << "[231]------------------------------------------" << endl;
	//unit_test[231] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_sorryserver_select(boost::this_thread::get_id(),ep), STOP);
}

//handle_sorryserver_connect
void handle_sorryserver_connect_test(){
    cout << "[232]------------------------------------------" << endl;
	//unit_test[232] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	boost::array<char,MAX_BUFFER_SIZE> buffer;
	size_t datalen;
	BOOST_CHECK_EQUAL(this->handle_sorryserver_connect(boost::this_thread::get_id(),buffer,datalen), STOP);
}

//handle_sorryserver_connection_fail
void handle_sorryserver_connection_fail_test(){
    cout << "[233]------------------------------------------" << endl;
	//unit_test[233] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_sorryserver_connection_fail(boost::this_thread::get_id(),ep), STOP);
}

//handle_sorryserver_send
void handle_sorryserver_send_test(){
    cout << "[234]------------------------------------------" << endl;
	//unit_test[234] 正常系で必ずSTOPを返す
	BOOST_CHECK_EQUAL(this->handle_sorryserver_send(boost::this_thread::get_id()), STOP);
}

//handle_sorryserver_recv
void handle_sorryserver_recv_test(){
    cout << "[235]------------------------------------------" << endl;
	//unit_test[235] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	boost::array<char,MAX_BUFFER_SIZE> buffer;
	size_t datalen;
	BOOST_CHECK_EQUAL(this->handle_sorryserver_recv(boost::this_thread::get_id(),ep,buffer,datalen), STOP);
}

//handle_response_send_inform
void handle_response_send_inform_test(){
    cout << "[236]------------------------------------------" << endl;
	//unit_test[236] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_response_send_inform(boost::this_thread::get_id()), STOP);
}

//handle_sorry_enable
void handle_sorry_enable_test(){
    cout << "[237]------------------------------------------" << endl;
	//unit_test[237] 全部の場合,遷移先ステータスを設定する,status = STOP
	//unit_test[237] test data:NULL
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_sorry_enable(boost::this_thread::get_id()), STOP);
}

//handle_sorry_disable
void handle_sorry_disable_test(){
    cout << "[238]------------------------------------------" << endl;
	//unit_test[238] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_sorry_disable(boost::this_thread::get_id()), STOP);
}

//handle_sorryserver_disconnect
void handle_sorryserver_disconnect_test(){
    cout << "[239]------------------------------------------" << endl;
	//unit_test[239] 正常系で必ずSTOPを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->handle_sorryserver_disconnect(boost::this_thread::get_id(),ep), STOP);
}

//replication_interrupt
void replication_interrupt_test(){
	install_stb_replication_func();
	unsigned int data_size = 0;
	void* data_addr = NULL;
	const boost::asio::ip::tcp::endpoint ep;
	data_addr = replication_pay_memory( get_name(), &data_size );
	this->replication_data_processor = new sslid_replication_data_processor_stub(
					1024, (char*)data_addr, data_size, ep, stb_getloglevel,
					stb_putLogFatal, stb_putLogError, stb_putLogWarn, stb_putLogInfo,
					stb_putLogDebug);

    cout << "[240]------------------------------------------" << endl;
	//unit_test[240] op_codeが「A」の場合、replication_dataをreplication_areaに追加する。
	time_t last_time_A = time(0);
	boost::asio::ip::tcp::endpoint real_ep_A;
	struct sslid_replication_temp_data replication_temp_data_add;
	replication_temp_data_add.op_code='A';
	replication_temp_data_add.session_id="11111111111111111111111111111111";
	replication_temp_data_add.last_time=last_time_A;
	replication_temp_data_add.realserver_addr=real_ep_A;
	sslid_replication_data_processor_stub* pstub;
	try
	{
		pstub = dynamic_cast<sslid_replication_data_processor_stub*> (replication_data_processor);
	}
	catch(std::bad_cast& )
	{
		std::cout<<"bad_cast:::"<<std::endl;
	}
	pstub->get_replication_area()=new sslid_replication_data;
	pstub->register_replication_area_lock(replication_area_lock_stb);
	pstub->register_replication_area_unlock(replication_area_unlock_stb);
	pstub->get_temp_list().push_back(replication_temp_data_add);
	pstub->get_replication_area()->valid=0;

	boost::thread test_thread1(boost::bind(&protocol_module_sslid_test_class::replication_interrupt,this));
	sleep(5);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->valid,1);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->last_time,last_time_A);
	BOOST_CHECK_EQUAL(memcmp(pstub->get_replication_area()->session_id,"11111111111111111111111111111111",SSLID_LENGTH),0);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->realserver_ip,real_ep_A.address().to_string().c_str());
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->realserver_port,real_ep_A.port());
	test_thread1.interrupt();

    cout << "[241]------------------------------------------" << endl;
	//unit_test[241] op_codeが「U」、validが１且つセッションIDが一致の場合、replication_area中の該当データを更新する。
	time_t last_time_U = time(0);
	boost::asio::ip::tcp::endpoint real_ep_U;
	struct sslid_replication_temp_data replication_temp_data_update;
	replication_temp_data_update.op_code='U';
	replication_temp_data_update.session_id="11111111111111111111111111111111";
	replication_temp_data_update.last_time=last_time_U;
	replication_temp_data_update.realserver_addr=real_ep_U;
	pstub->get_temp_list().push_back(replication_temp_data_update);
	pstub->get_replication_area()->valid=1;
	strcpy(pstub->get_replication_area()->session_id,"11111111111111111111111111111111");
	boost::thread test_thread2(boost::bind(&protocol_module_sslid_test_class::replication_interrupt,this));
	sleep(5);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->valid,1);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->last_time,last_time_U);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->realserver_ip,real_ep_U.address().to_string().c_str());
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->realserver_port,real_ep_U.port());
	test_thread2.interrupt();

    cout << "[242]------------------------------------------" << endl;
	//unit_test[242] op_codeが「D」、validが１且つセッションIDが存在する場合、replication_areaから該当データを削除する。
	time_t last_time_D = time(0);
	boost::asio::ip::tcp::endpoint real_ep_D;
	struct sslid_replication_temp_data replication_temp_data_delete;
	replication_temp_data_delete.op_code='D';
	replication_temp_data_delete.session_id="11111111111111111111111111111111";
	replication_temp_data_delete.last_time=last_time_D;
	replication_temp_data_delete.realserver_addr=real_ep_D;
	pstub->get_temp_list().push_back(replication_temp_data_delete);
	pstub->get_replication_area()->valid=1;
	strcpy(pstub->get_replication_area()->session_id,"11111111111111111111111111111111");
	boost::thread test_thread3(boost::bind(&protocol_module_sslid_test_class::replication_interrupt,this));
	sleep(5);
	BOOST_CHECK_EQUAL(pstub->get_replication_area()->valid,0);
	test_thread3.interrupt();
	sleep(5);
	delete this->replication_data_processor;
}

//put_data_to_sendbuffer
void put_data_to_sendbuffer_test(){
    boost::array<char,MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen = 0u;
    int ret = 0;
    int mem_cmp_result;
    int mem_cmp_length;
    char* mem_cmp_buffer;

    cout << "[243]------------------------------------------" << endl;
    // unit_test[243] スレッドIDがmapに存在していない場合
    // unit_test[243] 戻り値が -1で設定する。
    ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(), sendbuffer, datalen);
    BOOST_CHECK_EQUAL(ret, -1);

    cout << "[244]------------------------------------------" << endl;
    // unit_test[244] current_record_rest_size>data_sizeで, 且つdata_size<送信バッファサイズの場合
    // unit_test[244] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = 20u;
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x01;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, 10u);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[245]------------------------------------------" << endl;
    // unit_test[245] current_record_rest_size>data_sizeで, 且つdata_size=送信バッファサイズの場合
    // unit_test[245] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 1u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x02;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 1u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[246]------------------------------------------" << endl;
    // unit_test[246] current_record_rest_sizeがdata_sizeで, 且つdata_size<送信バッファサイズの場合
    // unit_test[246] 戻り値が0設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = 10u;
        up_thread_data->data_size = 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x03;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, 10u);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[247]------------------------------------------" << endl;
    // unit_test[247] current_record_rest_sizeがdata_sizeで, 且つdata_sizeが送信バッファサイズの場合
    // unit_test[247] 戻り値が0で設定する
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x04;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->data_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 0u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[248]------------------------------------------" << endl;
    // unit_test[248] current_record_rest_size>data_sizeで,且つdata_size＞送信バッファサイズの場合
    // unit_test[248] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 5u;
        up_thread_data->data_begain_offset = 10u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x05;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 5u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[249]------------------------------------------" << endl;
    // unit_test[249] current_record_rest_sizeがdata_sizeで,且つdata_size＞送信バッファサイズの場合
    // unit_test[249] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_begain_offset = 10u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x06;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[250]------------------------------------------" << endl;
    // unit_test[250] current_record_rest_size＜data_sizeで, 且つcurrent_record_rest_size<送信バッファサイズの場合
    // unit_test[250] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = 10u;
        up_thread_data->data_size = 20u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x07;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->current_record_rest_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 15u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, 10u);
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[251]------------------------------------------" << endl;
    // unit_test[251] current_record_rest_size<data_sizeで, 且つcurrent_record_rest_sizeが送信バッファサイズの場合
    // unit_test[251] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE);
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x08;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = up_thread_data->current_record_rest_size;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 5u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 0u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

    cout << "[252]------------------------------------------" << endl;
    // unit_test[252] current_record_rest_size＜data_sizeで,且つcurrent_record_rest_size＞送信バッファサイズの場合
    // unit_test[252] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 20u;
        up_thread_data->data_begain_offset = 5u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x08;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        this->session_thread_data_map[boost::this_thread::get_id()]
                = up_thread_data;
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 5u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 20u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
        delete[] mem_cmp_buffer;
        this->session_thread_data_map.clear();
    }

}

//put_data_to_sendbuffer
void put_data_to_sendbuffer_test_thread(){
    boost::array<char,MAX_BUFFER_SIZE> sendbuffer;
    size_t datalen = 0u;
    int ret = 0;
    int mem_cmp_result;
    int mem_cmp_length;
    char* mem_cmp_buffer;

    cout << "[249]------------------------------------------\n";
    // unit_test[249] current_record_rest_sizeがdata_sizeで,且つdata_size＞送信バッファサイズの場合
    // unit_test[249] 戻り値が0で設定する。
    {
        datalen = 0;
        thread_data_ptr up_thread_data(new session_thread_data_sslid);
        up_thread_data->current_record_rest_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_size = static_cast<size_t>(MAX_BUFFER_SIZE) + 10u;
        up_thread_data->data_begain_offset = 10u;
        for(size_t i=up_thread_data->data_begain_offset; i<up_thread_data->data_begain_offset+up_thread_data->data_size; i++)
        {
            up_thread_data->data_buffer[i] = 0x06;
        }
        mem_cmp_buffer = new char[MAX_SSLID_BUFFER_SIZE];
        memcpy(mem_cmp_buffer, up_thread_data->data_buffer.c_array()+up_thread_data->data_begain_offset, up_thread_data->data_size);
        mem_cmp_length = MAX_BUFFER_SIZE;
        {
            boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
            this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
        }
        ret = this->put_data_to_sendbuffer(boost::this_thread::get_id(),
                sendbuffer, datalen);
        mem_cmp_result = memcmp(mem_cmp_buffer, sendbuffer.c_array(), mem_cmp_length);
        BOOST_CHECK_EQUAL(mem_cmp_result, 0);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_begain_offset, 10u+static_cast<size_t>(MAX_BUFFER_SIZE));
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_record_rest_size, 10u);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_size, 10u);
        BOOST_CHECK_EQUAL(ret, 0);
        BOOST_CHECK_EQUAL(datalen, static_cast<size_t>(MAX_BUFFER_SIZE));
    }
}

//realserver_selected
void realserver_selected_test(){
    cout << "[253]------------------------------------------" << endl;
	//unit_test[253] 正常系で必ずFALSEを返す
	boost::asio::ip::tcp::endpoint ep;
	BOOST_CHECK_EQUAL(this->realserver_selected(ep), false);
}


};


/***********************************************************************************
 *以下はBOOST_TEST_SUITEに入れる関数
 ***********************************************************************************/

void protocol_module_sslid_test(){
    protocol_module_sslid_test_class obj;
    obj.protocol_module_sslid_test();
}
void is_tcp_test(){
    protocol_module_sslid_test_class obj;
    obj.is_tcp_test();
}
void is_udp_test(){
    protocol_module_sslid_test_class obj;
    obj.is_udp_test();
}
void get_name_test(){
    protocol_module_sslid_test_class obj;
    obj.get_name_test();
}
void initialize_test(){
    protocol_module_sslid_test_class obj;
    obj.initialize_test();
}
void finalize_test(){
    protocol_module_sslid_test_class obj;
    obj.finalize_test();
}
void is_use_sorry_test(){
    protocol_module_sslid_test_class obj;
    obj.is_use_sorry_test();
}
void check_parameter_test(){
    protocol_module_sslid_test_class obj;
    obj.check_parameter_test();
}
void set_parameter_test(){
    protocol_module_sslid_test_class obj;
    obj.set_parameter_test();
}
void add_parameter_test(){
    protocol_module_sslid_test_class obj;
    obj.add_parameter_test();
}
//handle_rslist_update_test
void register_schedule_tcp_test(){
    protocol_module_sslid_test_class obj;
    obj.register_schedule_tcp_test();
}
//register_schedule_udp_test
void handle_session_initialize_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_session_initialize_test();
}
void handle_session_initialize_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_session_initialize_test_thread,&obj));
    }
    threads.join_all();
}
void handle_session_finalize_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_session_finalize_test();
}
void handle_session_finalize_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_session_finalize_test_thread,&obj));
    }
    threads.join_all();
}
void handle_accept_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_accept_test();
}
void handle_accept_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for(int i=0; i<THREAD_COUNT; i++)
    {
        threads.create_thread(bind(&protocol_module_sslid_test_class::handle_accept_test_thread,&obj));
    }
    threads.join_all();
}
void handle_client_recv_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_client_recv_test();
}
void handle_client_recv_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_client_recv_test_thread,&obj));
    }
    threads.join_all();
}
void handle_realserver_select_tcp_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_select_tcp_test();
}
void handle_realserver_select_tcp_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_select_tcp_test_thread,&obj));
    }
    threads.join_all();
}
void handle_realserver_select_udp_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_select_udp_test();
}
void handle_realserver_connect_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_connect_test();
}
void handle_realserver_connect_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_connect_test_thread,&obj));
    }
    threads.join_all();
}
void handle_realserver_connection_fail_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_connection_fail_test();
}
void handle_realserver_connection_fail_test_thread_reschedule()
{
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for(int i=0; i<THREAD_COUNT; i++)
    {
        threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_connection_fail_test_thread_reschedule,&obj));
    }
    threads.join_all();
}
void handle_realserver_connection_fail_test_thread_noreschedule()
{
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for(int i=0; i<THREAD_COUNT; i++)
    {
        threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_connection_fail_test_thread_noreschedule,&obj));
    }
    threads.join_all();
}
void handle_realserver_send_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_send_test();
}
void handle_realserver_send_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_send_test_thread,&obj));
    }
    threads.join_all();
}
void handle_realserver_recv_tcp_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_recv_tcp_test();
}
void handle_realserver_recv_tcp_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_recv_tcp_test_thread,&obj));
    }
    threads.join_all();
}
void handle_realserver_recv_udp_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_recv_udp_test();
}
void handle_client_connection_check_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_client_connection_check_test();
}
void handle_client_connection_check_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_client_connection_check_test_thread,&obj));
    }
    threads.join_all();
}
void handle_client_select_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_client_select_test();
}
void handle_client_send_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_client_send_test();
}
void handle_client_send_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::handle_realserver_select_tcp_test_thread,&obj));
    }
    threads.join_all();
}
void handle_client_disconnect_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_client_disconnect_test();
}
void handle_realserver_disconnect_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_realserver_disconnect_test();
}
void handle_sorryserver_select_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorryserver_select_test();
}
void handle_sorryserver_connect_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorryserver_connect_test();
}
void handle_sorryserver_connection_fail_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorryserver_connection_fail_test();
}
void handle_sorryserver_send_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorryserver_send_test();
}
void handle_sorryserver_recv_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorryserver_recv_test();
}
void handle_response_send_inform_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_response_send_inform_test();
}
void handle_sorry_enable_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorry_enable_test();
}
void handle_sorry_disable_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorry_disable_test();
}
void handle_sorryserver_disconnect_test(){
    protocol_module_sslid_test_class obj;
    obj.handle_sorryserver_disconnect_test();
}
void replication_interrupt_test(){
    protocol_module_sslid_test_class obj;
    obj.replication_interrupt_test();
}
void put_data_to_sendbuffer_test(){
    protocol_module_sslid_test_class obj;
    obj.put_data_to_sendbuffer_test();
}
void put_data_to_sendbuffer_test_thread(){
    protocol_module_sslid_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
    	threads.create_thread(bind(&protocol_module_sslid_test_class::put_data_to_sendbuffer_test_thread,&obj));
    }
    threads.join_all();
}
void realserver_selected_test(){
    protocol_module_sslid_test_class obj;
    obj.realserver_selected_test();
}


void protocol_module_sslid_test_main() {
	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "sslid_ut" );
	ts->add(BOOST_TEST_CASE( &protocol_module_sslid_test ));
	ts->add(BOOST_TEST_CASE( &is_tcp_test ));
	ts->add(BOOST_TEST_CASE( &is_udp_test ));
	ts->add(BOOST_TEST_CASE( &get_name_test ));
	ts->add(BOOST_TEST_CASE( &initialize_test ));
	ts->add(BOOST_TEST_CASE( &finalize_test ));
	ts->add(BOOST_TEST_CASE( &is_use_sorry_test ));
	ts->add(BOOST_TEST_CASE( &check_parameter_test ));
	ts->add(BOOST_TEST_CASE( &set_parameter_test ));
	ts->add(BOOST_TEST_CASE( &add_parameter_test ));
//	ts->add(BOOST_TEST_CASE( &handle_rslist_update_test ));
	ts->add(BOOST_TEST_CASE( &register_schedule_tcp_test ));
//	ts->add(BOOST_TEST_CASE( &register_schedule_udp_test ));
	ts->add(BOOST_TEST_CASE( &handle_session_initialize_test ));
	ts->add(BOOST_TEST_CASE( &handle_session_initialize_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_session_finalize_test ));
	ts->add(BOOST_TEST_CASE( &handle_session_finalize_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_accept_test ));
	ts->add(BOOST_TEST_CASE( &handle_accept_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_client_recv_test ));
	ts->add(BOOST_TEST_CASE( &handle_client_recv_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_select_tcp_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_select_tcp_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_select_udp_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_connect_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_connect_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_connection_fail_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_connection_fail_test_thread_reschedule ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_connection_fail_test_thread_noreschedule ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_send_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_send_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_recv_tcp_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_recv_tcp_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_recv_udp_test ));
	ts->add(BOOST_TEST_CASE( &handle_client_connection_check_test ));
	ts->add(BOOST_TEST_CASE( &handle_client_connection_check_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_client_select_test ));
	ts->add(BOOST_TEST_CASE( &handle_client_send_test ));
	ts->add(BOOST_TEST_CASE( &handle_client_send_test_thread ));
	ts->add(BOOST_TEST_CASE( &handle_client_disconnect_test ));
	ts->add(BOOST_TEST_CASE( &handle_realserver_disconnect_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorryserver_select_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorryserver_connect_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorryserver_connection_fail_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorryserver_send_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorryserver_recv_test ));
	ts->add(BOOST_TEST_CASE( &handle_response_send_inform_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorry_enable_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorry_disable_test ));
	ts->add(BOOST_TEST_CASE( &handle_sorryserver_disconnect_test ));
	ts->add(BOOST_TEST_CASE( &replication_interrupt_test ));
	ts->add(BOOST_TEST_CASE( &put_data_to_sendbuffer_test ));
	ts->add(BOOST_TEST_CASE( &put_data_to_sendbuffer_test_thread ));
	ts->add(BOOST_TEST_CASE( &realserver_selected_test ));
	framework::master_test_suite().add(ts);
}

