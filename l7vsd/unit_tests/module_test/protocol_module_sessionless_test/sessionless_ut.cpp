/*
 * @file  sessionless_ut.cpp
 * @brief protocol module sessionless test file.
 *
 * L7VSD: Linux Virtual Server for Layer7 Load Balancing
 * Copyright (C) 2009  NTT COMWARE Corporation.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 **********************************************************************/

#include <boost/test/included/unit_test.hpp>
#include "../../../module/protocol/http_protocol_module_base.cpp"
#include "../../../module/protocol/protocol_module_sessionless.cpp"

#define REQUEST_BUFFER_SIZE 50u  //REQUEST_BUFFER_SIZE
#define RESPONSE_BUFFER_SIZE 50u  //RESPONSE_BUFFER_SIZE
#define USE_BUFFER_SIZE 90u //USE_BUFFER_SIZE
#define THREAD_COUNT 10

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

//log stub function
char hostname[] = "127.0.0.1";

LOG_LEVEL_TAG    stb_getloglevel(){
    return LOG_LV_DEBUG;
}
LOG_LEVEL_TAG    stb_getloglevel_is_none(){
    return LOG_LV_NONE;
}

void    stb_putLogFatal( const unsigned int message_id, const std::string& message, const char* file, int line){
//    cout << boost::format( "%s%d%06d %s %s" )
//                % "PM"
//                % LOG_LV_FATAL
//                % message_id
//                % message.c_str()
//                % hostname;
//    cout << endl;
}
void    stb_putLogError( const unsigned int message_id, const std::string& message, const char* file, int line){
//    cout << boost::format( "%s%d%06d %s %s" )
//                % "PM"
//                % LOG_LV_ERROR
//                % message_id
//                % message.c_str()
//                % hostname;
//    cout << endl;
}
void    stb_putLogWarn( const unsigned int message_id, const std::string& message, const char* file, int line){
//    cout << boost::format( "%s%d%06d %s %s" )
//                % "PM"
//                % LOG_LV_WARN
//                % message_id
//                % message.c_str()
//                % hostname;
//    cout << endl;
}
void    stb_putLogInfo( const unsigned int message_id, const std::string& message, const char* file, int line){
//    cout << boost::format( "%s%d%06d %s %s" )
//                % "PM"
//                % LOG_LV_INFO
//                % message_id
//                % message.c_str()
//                % hostname;
//    cout << endl;
}
void    stb_putLogDebug( const unsigned int message_id, const std::string& message, const char* file, int line){
//    cout << boost::format( "%s%d%06d %s %s" )
//                % "PM"
//                % LOG_LV_DEBUG
//                % message_id
//                % message.c_str()
//                % hostname;
//    cout << endl;
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

//RealServerリストの各操作関数
//RealServerリスト先頭取得関数
protocol_module_sessionless::realserverlist_type::iterator rslist_begin() {
    return protocol_module_sessionless::realserverlist_type::iterator();
}
//RealServerリスト末端取得関数
protocol_module_sessionless::realserverlist_type::iterator rslist_end() {
    return protocol_module_sessionless::realserverlist_type::iterator();
}
//RealServerリスト次要素取得関数
protocol_module_sessionless::realserverlist_type::iterator rslist_next(
        protocol_module_sessionless::realserverlist_type::iterator iter) {
    return protocol_module_sessionless::realserverlist_type::iterator();
}
//RealServerリストロック関数
void rslist_lock() {
}
//RealServerリストアンロック関数
void rslist_unlock() {
}
//コンポーネント領域割り当て関数
void* pay_memory(const std::string& str, unsigned int* pro) {
    return 0 ;
}
//コンポーネント領域ロック関数
void area_lock() {}
//コンポーネント領域アンロック関数
void area_unlock() {}
//Client振り分け処理関数
void schedule_tcp_stb( const boost::thread::id id,
                   protocol_module_base::rs_list_itr_func_type func_type1,
                   protocol_module_base::rs_list_itr_func_type func_type2,
                   protocol_module_base::rs_list_itr_next_func_type func_type3,
                   boost::asio::ip::tcp::endpoint& ) {
}

//Thread function
void up_thread_func(){}
void down_thread_func(){}

// using string to endpoint template function
template<typename InternetProtocol>
boost::asio::ip::basic_endpoint<InternetProtocol> string_to_endpoint(std::string str){
    std::string::size_type pos = str.find(":");
    std::string hostname = str.substr(0, pos);
    std::string portname = str.substr(pos + 1, str.length());
    boost::asio::io_service io_service;
    typename InternetProtocol::resolver resolver(io_service);
    typename InternetProtocol::resolver::query query(hostname, portname);
    typename InternetProtocol::resolver::iterator end;
    typename InternetProtocol::resolver::iterator itr = resolver.resolve(query);
    if (itr == end)
        return typename InternetProtocol::endpoint();
    return *itr;
}

//schedule function
void schedule_tcp_determinate(//endpoint = 決定
        const boost::thread::id thread_id,
        protocol_module_base::rs_list_itr_func_type rs_list_begin,
        protocol_module_base::rs_list_itr_func_type rs_list_end,
        protocol_module_base::rs_list_itr_next_func_type rs_list_next,
        boost::asio::ip::tcp::endpoint &rs_endpoint){
    rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888");
}

void schedule_tcp_nodeterminate(//endpoint = 未決定
        const boost::thread::id thread_id,
        protocol_module_base::rs_list_itr_func_type rs_list_begin,
        protocol_module_base::rs_list_itr_func_type rs_list_end,
        protocol_module_base::rs_list_itr_next_func_type rs_list_next,
        boost::asio::ip::tcp::endpoint &rs_endpoint){
}

//test class
class protocol_module_sessionless_test_class : public protocol_module_sessionless
{
public:
boost::mutex check_mutex;

//stub log function install
void install_stb_log_func(){
    typedef boost::function< void ( const unsigned int, const std::string&, const char*, int ) > log_func_type;

    boost::function< LOG_LEVEL_TAG(void) >    getloglevel = &stb_getloglevel;
    log_func_type    putLogFatal    = &stb_putLogFatal;
    log_func_type    putLogError    = &stb_putLogError;
    log_func_type    putLogWarn    = &stb_putLogWarn;
    log_func_type    putLogInfo    = &stb_putLogInfo;
    log_func_type    putLogDebug    = &stb_putLogDebug;
    this->init_logger_functions(getloglevel, putLogFatal, putLogError,
        putLogWarn, putLogInfo, putLogDebug);

}

//initinalize session_thread_data_sessionless
void init_session_thread_data_sessionless(thread_data_ptr data,
                                            boost::thread::id thread_id){
    data->thread_id = boost::this_thread::get_id();
    data->thread_division = THREAD_DIVISION_UP_STREAM;
    data->pair_thread_id = thread_id;
    data->accept_end_flag = ACCEPT_END_FLAG_OFF;
    data->end_flag = END_FLAG_OFF;
    data->sorry_flag = SORRY_FLAG_OFF;
    data->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
    data->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
    data->last_status = INITIALIZE;
    data->receive_data_map.clear();
}

//initinalize receive_data
 void init_receive_data(receive_data& data){
    data.receive_buffer_max_size = 0;
    data.receive_buffer_rest_size = 0;
    data.send_status_list.clear();
}
//initinalize send_status
void init_send_status(send_status& data){
    data.status = SEND_END;
    data.send_end_size = 0;
    data.send_rest_size = 0;
    data.send_possible_size = 0;
    data.send_offset = 0;
    data.unsend_size = 0;
    data.edit_division = EDIT_DIVISION_NO_EDIT;
    data.edit_data_list.clear();
}
//initinalize edit_data
void init_edit_data(edit_data& data)
{
    data.data = "";
    data.data_size = 0;
    data.insert_posission = 0;
    data.replace_size = 0;
}

protocol_module_sessionless_test_class()
{
    install_stb_log_func();
}

//protocol_module_sessionless 馮家純
void protocol_module_sessionless_test(){
    cout << "[1]--------------------------------------------- " << endl;
    //unit_test[1] モジュール名に（"sessionless"）を設定する,forwarded_forにFORWARDED_FOR_OFFを設定する,sorry_uriに'\0'を設定する
     //モジュール名に（"sessionless"）を設定する
    BOOST_CHECK_EQUAL(this->name, "sessionless");
    //forwarded_forにFORWARDED_FOR_OFFを設定する
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_OFF);
    char chk[MAX_OPTION_SIZE];
    memset(chk, '\0', MAX_OPTION_SIZE);
	chk[0] = '/';
    //sorry_uriに'\0'を設定する
    BOOST_CHECK_EQUAL(memcmp(this->sorry_uri.data(), chk, MAX_OPTION_SIZE), 0);
}

//is_tcp 馮家純
void is_tcp_test(){
    cout << "[2]--------------------------------------------- " << endl;
    //unit_test[2] is_tcp TRUEを返却する
    BOOST_CHECK(this->is_tcp());
}

//is_udp 馮家純
void is_udp_test(){
    cout << "[3]--------------------------------------------- " << endl;
    //unit_test[3] is_udp FALSEを返却する
    BOOST_CHECK_EQUAL(this->is_udp(), false);
}

//get_name 馮家純
void get_name_test(){
    cout << "[4]--------------------------------------------- " << endl;
    //unit_test[4] モジュール名に（"sessionless"）を返却する
    BOOST_CHECK_EQUAL(this->get_name(), "sessionless");
}

//initialize 董作方
void initialize_test() {
    cout << "[5]--------------------------------------------- " << endl;
    //unit_test[5] RealServerリストの各操作関数メソッドinitializeのテスト
    rs_list_itr_func_type inrslist_begin = rslist_begin;
    rs_list_itr_func_type inrslist_end = rslist_end;
    rs_list_itr_next_func_type inrslist_next = rslist_next;
    boost::function<void(void)> inlist_lock = rslist_lock;
    boost::function<void(void)> inlist_unlock = rslist_unlock;

    this->initialize(inrslist_begin, inrslist_end, inrslist_next, inlist_lock, inlist_unlock);

    BOOST_CHECK_EQUAL(this->rs_list_begin, rslist_begin);
    BOOST_CHECK_EQUAL(this->rs_list_end, rslist_end);
    BOOST_CHECK_EQUAL(this->rs_list_next, rslist_next);
    BOOST_CHECK_EQUAL(this->rs_list_lock, rslist_lock);
    BOOST_CHECK_EQUAL(this->rs_list_unlock, rslist_unlock);

    cout << "[6]--------------------------------------------- " << endl;
    //unit_test[6] RealServerリストの各操作関数空のテスト
    inrslist_begin.clear();
    inrslist_end.clear();
    inrslist_next.clear();
    inlist_lock.clear();
    inlist_unlock.clear();
    this->initialize(inrslist_begin,inrslist_end,inrslist_next,inlist_lock,inlist_unlock);

    BOOST_CHECK_EQUAL(this->rs_list_begin.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_end.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_next.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_lock.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_unlock.empty(), true);
}

//finalize 董作方
void finalize_test() {
    cout << "[7]--------------------------------------------- " << endl;
    //unit_test[7] 各操作関数を初期化する
    //RealServerリストの各操作関数
    this->rs_list_begin = rslist_begin;
    this->rs_list_end = rslist_end;
    this->rs_list_next = rslist_next;
    this->rs_list_lock = rslist_lock;
    this->rs_list_unlock = rslist_unlock;
    //Replicationの各操作関数
    this->replication_pay_memory = pay_memory;
    this->replication_area_lock = area_lock;
    this->replication_area_unlock = area_unlock;
    //ScheduleModuleの振分関数
    this->schedule_tcp = schedule_tcp_stb;
    //各モジュールオプション
    this->forwarded_for = FORWARDED_FOR_ON;
    this->sorry_uri.assign('a');

    this->finalize();

    BOOST_CHECK_EQUAL(this->getloglevel.empty() , true);
    BOOST_CHECK_EQUAL(this->putLogFatal.empty(), true);
    BOOST_CHECK_EQUAL(this->putLogError.empty(), true);
    BOOST_CHECK_EQUAL(this->putLogWarn.empty(), true);
    BOOST_CHECK_EQUAL(this->putLogInfo.empty(), true);
    BOOST_CHECK_EQUAL(this->putLogDebug.empty(), true);

    BOOST_CHECK_EQUAL(this->rs_list_begin.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_end.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_next.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_lock.empty(), true);
    BOOST_CHECK_EQUAL(this->rs_list_unlock.empty(), true);

    BOOST_CHECK_EQUAL(this->replication_pay_memory.empty(), true);
    BOOST_CHECK_EQUAL(this->replication_area_lock.empty(), true);
    BOOST_CHECK_EQUAL(this->replication_area_unlock.empty(), true);

    BOOST_CHECK_EQUAL(this->schedule_tcp.empty(), true);
    BOOST_CHECK_EQUAL(this->schedule_udp.empty(), true);

    BOOST_CHECK_EQUAL(this->forwarded_for,FORWARDED_FOR_OFF);
    BOOST_CHECK_EQUAL(this->sorry_uri[0],'\0');
}

//is_use_sorry 馬翠翠
void is_use_sorry_test() {
    cout << "[8]--------------------------------------------- " << endl;
    // unit_test[8] 正常系で必ずtrueを返す
    BOOST_CHECK_EQUAL(this->is_use_sorry(), true);
}

//check_parameter 馬翠翠
void check_parameter_test() {
    check_message_result result;
    vector<string> args;

    cout << "[9]--------------------------------------------- " << endl;
    // unit_test[9] オプション文字列にデータなし場合、チェック結果フラグにTRUEを設定する
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[10]--------------------------------------------- " << endl;
    // unit_test[10] オプション文字列 = "-F"の場合、チェック結果フラグにTRUEを設定する
    args.push_back("-F");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[11]--------------------------------------------- " << endl;
    // unit_test[11] オプション文字列 = "--forwarded-for"の場合、チェック結果フラグにTRUEを設定する
    args.clear();
    args.push_back("--forwarded-for");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[12]--------------------------------------------- " << endl;
    // unit_test[12] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is too long."（%sは次要素）を設定する
    // unit_test[12] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF 次要素の文字列長 > 127の場合
    args.clear();
    args.push_back("-S");
    args.push_back("/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "'-S/--sorry-uri' option value '/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.");

    cout << "[13]--------------------------------------------- " << endl;
    // unit_test[13] sorry-uri設定フラグをON、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
    // unit_test[13] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 = 127 チェックOKの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
    args.push_back("--sorry-uri");
    args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.message, "Cannot set multiple option '-S/--sorry-uri'.");
    BOOST_CHECK_EQUAL(result.flag, false);

    cout << "[14]--------------------------------------------- " << endl;
    // unit_test[14] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is not a valid URI."（%sは次要素）を設定する
    // unit_test[14] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 = 127 チェックNGの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "'-S/--sorry-uri' option value '/a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/' is not a valid URI.");

    cout << "[15]--------------------------------------------- " << endl;
    // unit_test[15] sorry-uri設定フラグをON、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
    // unit_test[15] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 < 127 チェックOKの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/abc123");
    args.push_back("--sorry-uri");
    args.push_back("/abc123");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.message, "Cannot set multiple option '-S/--sorry-uri'.");
    BOOST_CHECK_EQUAL(result.flag, false);

    cout << "[16]--------------------------------------------- " << endl;
    // unit_test[16] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is not a valid URI."（%sは次要素）を設定する
    // unit_test[16] test data:オプション文字列 = "-S"、sorryURI設定フラグ = OFF、次要素の文字列長 < 127 チェックNGの場合
    args.clear();
    args.push_back("-S");
    args.push_back("123\r\n");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "'-S/--sorry-uri' option value '123\r\n' is not a valid URI.");

    cout << "[17]--------------------------------------------- " << endl;
    // unit_test[17] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set  option value '-S/--sorry-uri'."を設定する
    // unit_test[17] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素が存在しない場合
    args.clear();
    args.push_back("-S");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");

    cout << "[18]--------------------------------------------- " << endl;
    // unit_test[18] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
    // unit_test[18] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = ONの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/123/abc");
    args.push_back("-S");
    args.push_back("/123/999");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Cannot set multiple option '-S/--sorry-uri'.");

    cout << "[19]--------------------------------------------- " << endl;
    // unit_test[19] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Option error."を設定する
    // unit_test[19] test data:オプション文字列"-F"、"-S" 以外の場合
    args.clear();
    args.push_back("-D");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Option error.");

    cout << "[20]--------------------------------------------- " << endl;
    // unit_test[20] オプション文字列"-S"、"--forwarded-for"の場合、チェック結果フラグにTRUEを設定する
    args.clear();
    args.push_back("-S");
    args.push_back("/123/abc");
    args.push_back("--forwarded-for");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[21]--------------------------------------------- " << endl;
    // unit_test[21] オプション文字列"-F"、"--sorry-uri" の場合、チェック結果フラグにTRUEを設定する
    args.clear();
    args.push_back("-F");
    args.push_back("--sorry-uri");
    args.push_back("/123/abc");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[22]--------------------------------------------- " << endl;
    // unit_test[22] オプション文字列"-F"、"--forwarded-for" の場合、チェック結果フラグにTRUEを設定する
    args.clear();
    args.push_back("-F");
    args.push_back("--forwarded-for");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[23]--------------------------------------------- " << endl;
    // unit_test[23] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set option value '-S/--sorry-uri'."を設定する
    // unit_test[23] test data:オプション文字列"-F"、"-S" の場合
    args.clear();
    args.push_back("-F");
    args.push_back("-S");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");

    cout << "[24]--------------------------------------------- " << endl;
    // unit_test[24] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set option value '-S/--sorry-uri'."を設定する
    // unit_test[24] test data:オプション文字列"--sorry-uri"、"--forwarded-for" の場合
    args.clear();
    args.push_back("--sorry-uri");
    args.push_back("--forwarded-for");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");

    cout << "[25]--------------------------------------------- " << endl;
    // unit_test[25] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Option error."を設定する
    // unit_test[25] test data:オプション文字列"--forwarded-for"、"-R"の場合
    args.clear();
    args.push_back("--forwarded-for");
    args.push_back("-R");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Option error.");

    cout << "[26]--------------------------------------------- " << endl;
    // unit_test[26] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set option value '-S/--sorry-uri'."を設定する
    // unit_test[26] test data:オプション文字列"S"、"-R"の場合
    args.clear();
    args.push_back("-S");
    args.push_back("-R");
    result = this->check_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");
}

//set_parameter 馬翠翠
void set_parameter_test() {
    check_message_result result;
    vector<string> args;

    cout << "[27]--------------------------------------------- " << endl;
    // unit_test[27] オプション文字列にデータなし場合、チェック結果フラグにTRUEを設定する
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[28]--------------------------------------------- " << endl;
    // unit_test[28] オプション文字列 = "-F"の場合、送信元設定指示設定フラグをON,チェック結果フラグにTRUEを設定する
    args.push_back("-F");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_ON);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[29]--------------------------------------------- " << endl;
    // unit_test[29] オプション文字列 = "--forwarded-for"の場合、送信元設定指示設定フラグをON,チェック結果フラグにTRUEを設定する
    args.clear();
    args.push_back("--forwarded-for");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_ON);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[30]--------------------------------------------- " << endl;
    // unit_test[30] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is too long."（%sは次要素）を設定する
    // unit_test[30] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 > 127の場合
    args.clear();
    args.push_back("-S");
    args.push_back("/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "'-S/--sorry-uri' option value '/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.");

    cout << "[31]--------------------------------------------- " << endl;
    // unit_test[31] sorry-uri設定フラグをON、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
    // unit_test[31] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 = 127 チェックOKの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/bcdef123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
    args.push_back("--sorry-uri");
    args.push_back("/bcdef123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.message, "Cannot set multiple option '-S/--sorry-uri'.");
    BOOST_CHECK_EQUAL(result.flag, false);

    cout << "[32]--------------------------------------------- " << endl;
    // unit_test[32] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is not a valid URI."（%sは次要素）を設定する
    // unit_test[32] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 = 127 チェックNGの場合
    args.clear();
    args.push_back("-S");
    args.push_back("abcdef12345\r\n/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "'-S/--sorry-uri' option value 'abcdef12345\r\n/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is not a valid URI.");

    cout << "[33]--------------------------------------------- " << endl;
    // unit_test[33] sorry-uriに次要素の文字列を設定する
    // unit_test[33] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素の文字列長 < 127 チェックOKの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/abc/123");
    result = this->set_parameter(args);
    int ret = strcmp(this->sorry_uri.data(), "/abc/123");
    BOOST_CHECK_EQUAL(ret, 0);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[34]--------------------------------------------- " << endl;
    // unit_test[34] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is not a valid URI."（%sは次要素）を設定する
    // unit_test[34] test data:次要素の文字列長 < 127 チェックNGの場合
    args.clear();
    args.push_back("-S");
    args.push_back("abc\r\n");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "'-S/--sorry-uri' option value 'abc\r\n' is not a valid URI.");

    cout << "[35]--------------------------------------------- " << endl;
    // unit_test[35] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set  option value '-S/--sorry-uri'."を設定する
    // unit_test[35] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = OFF、次要素が存在しない場合
    args.clear();
    args.push_back("-S");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");

    cout << "[36]--------------------------------------------- " << endl;
    // unit_test[36] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Cannot set multiple option ''-S/--sorry-uri'."を設定する
    // unit_test[36] test data:オプション文字列 = "-S"、sorry-uri設定フラグ = ONの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/abc/123");
    args.push_back("-S");
    args.push_back("/abc/123");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Cannot set multiple option '-S/--sorry-uri'.");

    cout << "[37]--------------------------------------------- " << endl;
    // unit_test[37] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Option error."を設定する
    // unit_test[37] test data:オプション文字列"-F"、"-S" 以外の場合
    args.clear();
    args.push_back("-D");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Option error.");

    cout << "[38]--------------------------------------------- " << endl;
    // unit_test[38] 送信元設定指示に0を設定する
    // unit_test[38] test data:チェック結果フラグ = TRUE、送信元設定指示設定フラグ = OFFの場合
    args.clear();
    args.push_back("-S");
    args.push_back("/abc/123");
    this->set_parameter(args);
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_OFF);

    cout << "[39]--------------------------------------------- " << endl;
    // unit_test[39] チェック結果フラグにTRUEを設定する、送信元設定指示に1を設定する
    // unit_test[39] test data:オプション文字列"--forwarded-for"、"-S" の場合
    args.clear();
    args.push_back("--forwarded-for");
    args.push_back("-S");
    args.push_back("/abc/123");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_ON);

    cout << "[40]--------------------------------------------- " << endl;
    // unit_test[40] チェック結果フラグにTRUEを設定する、送信元設定指示に1を設定する
    // unit_test[40] test data:オプション文字列"--sorry-uri"、"-F" の場合
    args.clear();
    args.push_back("--sorry-uri");
    args.push_back("/abc/123");
    args.push_back("-F");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_ON);

    cout << "[41]--------------------------------------------- " << endl;
    // unit_test[41] チェック結果フラグにTRUEを設定する、送信元設定指示に1を設定する
    // unit_test[41] test data:オプション文字列"--forwarded-for"、"-F" の場合
    args.clear();
    args.push_back("--forwarded-for");
    args.push_back("-F");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);
    BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_ON);

    cout << "[42]--------------------------------------------- " << endl;
    // unit_test[42] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set option value '-S/--sorry-uri'."を設定する
    // unit_test[42] test data:オプション文字列"-F"、"-S" の場合
    args.clear();
    args.push_back("-F");
    args.push_back("-S");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");

    cout << "[43]--------------------------------------------- " << endl;
    // unit_test[43] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set option value '-S/--sorry-uri'."を設定する
    // unit_test[43] test data:オプション文字列"--sorry-uri"、"--forwarded-for" の場合
    args.clear();
    args.push_back("--sorry-uri");
    args.push_back("--forwarded-for");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");

    cout << "[44]--------------------------------------------- " << endl;
    // unit_test[44] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Option error."を設定する
    // unit_test[44] test data:オプション文字列"--forwarded-for"、"-R"の場合
    args.clear();
    args.push_back("--forwarded-for");
    args.push_back("-R");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Option error.");

    cout << "[45]--------------------------------------------- " << endl;
    // unit_test[45] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to set option value '-S/--sorry-uri'."を設定する
    // unit_test[45] test data:オプション文字列"-S"、"-R"の場合
    args.clear();
    args.push_back("-S");
    args.push_back("-R");
    result = this->set_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "You have to set option value '-S/--sorry-uri'.");
}

//add_parameter 馬翠翠
void add_parameter_test() {
    check_message_result result;
    vector<string> args;

    cout << "[46]--------------------------------------------- " << endl;
    // unit_test[46] オプション文字列が存在しない場合,正常系で必ずtrueを返す
    result = this->add_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, true);

    cout << "[47]--------------------------------------------- " << endl;
    // unit_test[47] オプション文字列が存在する場合,チェック結果フラグにFALSEを設定する, チェック結果メッセージに"Cannot add option."を設定する
    args.push_back("-F");
    result = this->add_parameter(args);
    BOOST_CHECK_EQUAL(result.flag, false);
    BOOST_CHECK_EQUAL(result.message, "Cannot add option.");
}

//register_schedule(tcp) 董作方
void register_schedule_tcp_test() {
    cout << "[48]--------------------------------------------- " << endl;
    // unit_test[48]  Client振り分け処理関数のテスト
    tcp_schedule_func_type  func = schedule_tcp_stb;
    this->register_schedule(func);
    BOOST_CHECK_EQUAL(this->schedule_tcp, schedule_tcp_stb);

    cout << "[49]--------------------------------------------- " << endl;
    // unit_test[49]  Client振り分け処理関数空のテスト
    func.clear();
    this->register_schedule(func);
    BOOST_CHECK_EQUAL(this->schedule_tcp.empty(), true);
}

//handle_session_initialize 董作方
void handle_session_initialize_test() {
    cout << "[50]--------------------------------------------- " << endl;
    //unit_test[50] 遷移先ステータスを設定する status = ACCEPT
    boost::thread down_thread(down_thread_func);
    boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("100.100.100.100:8888");
    boost::asio::ip::udp::endpoint ep_udp = string_to_endpoint<boost::asio::ip::udp> ("100.100.100.100:8080");

    EVENT_TAG ret =    this->handle_session_initialize(boost::this_thread::get_id(), down_thread.get_id(),ep_tcp,ep_udp);
    BOOST_CHECK_EQUAL(ret, ACCEPT);


    cout << "[51]--------------------------------------------- " << endl;
    //unit_test[51] マップの上りスレッドテスト
    session_thread_data_map_it iter
        = this->session_thread_data_map.find(boost::this_thread::get_id());
    bool bret = (iter != this->session_thread_data_map.end());
    BOOST_REQUIRE_EQUAL(bret, true);
    thread_data_ptr data = iter->second;
    bret = (data != NULL);
    BOOST_REQUIRE_EQUAL(bret, true);

    BOOST_CHECK_EQUAL(data->thread_id, boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(data->thread_division, THREAD_DIVISION_UP_STREAM);
    BOOST_CHECK_EQUAL(data->pair_thread_id, down_thread.get_id());
    BOOST_CHECK_EQUAL(data->receive_data_map.empty(), false);
    BOOST_CHECK_EQUAL(data->end_flag, END_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->accept_end_flag, ACCEPT_END_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->sorry_flag, SORRY_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, SORRYSERVER_SWITCH_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->realserver_switch_flag, REALSERVER_SWITCH_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->client_endpoint_tcp, ep_tcp);

    cout << "[52]--------------------------------------------- " << endl;
    //unit_test[52]  下りスレッドマップテスト
    iter = this->session_thread_data_map.find(down_thread.get_id());
    bret = (iter != this->session_thread_data_map.end());
    BOOST_REQUIRE_EQUAL(bret, true);
    data = iter->second;
    bret = (data != NULL);
    BOOST_REQUIRE_EQUAL(bret, true);

    BOOST_CHECK_EQUAL(data->thread_id, down_thread.get_id());
    BOOST_CHECK_EQUAL(data->thread_division, THREAD_DIVISION_DOWN_STREAM);
    BOOST_CHECK_EQUAL(data->pair_thread_id, boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(data->receive_data_map.empty(), true);
    BOOST_CHECK_EQUAL(data->end_flag, END_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->accept_end_flag, ACCEPT_END_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->sorry_flag, SORRY_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, SORRYSERVER_SWITCH_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->realserver_switch_flag, REALSERVER_SWITCH_FLAG_OFF);
    BOOST_CHECK_EQUAL(data->client_endpoint_tcp, ep_tcp);
}

//handle_session_initialize 董作方
void handle_session_initialize_test_thread() {
    boost::thread down_thread(down_thread_func);
    boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("100.100.100.100:8888");
    boost::asio::ip::udp::endpoint ep_udp = string_to_endpoint<boost::asio::ip::udp> ("100.100.100.100:8080");

    cout << "[53]--------------------------------------------- " << endl;
    //unit_test[53] 多スレッドテスト
     session_thread_data_map_it iter;
     bool bret ;
     EVENT_TAG ret = this->handle_session_initialize(boost::this_thread::get_id(), down_thread.get_id(),ep_tcp,ep_udp);

    {
       boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
       iter = this->session_thread_data_map.find(boost::this_thread::get_id());
       bret = (iter != this->session_thread_data_map.end());
    }
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, ACCEPT);
        BOOST_REQUIRE_EQUAL(bret, true);
        thread_data_ptr data = iter->second;
        bret = (data != NULL);
        BOOST_REQUIRE_EQUAL(bret, true);

        BOOST_CHECK_EQUAL(data->thread_id, boost::this_thread::get_id());
        BOOST_CHECK_EQUAL(data->thread_division, THREAD_DIVISION_UP_STREAM);
        BOOST_CHECK_EQUAL(data->pair_thread_id, down_thread.get_id());
        BOOST_CHECK_EQUAL(data->receive_data_map.empty(), false);
        BOOST_CHECK_EQUAL(data->end_flag, END_FLAG_OFF);
        BOOST_CHECK_EQUAL(data->accept_end_flag, ACCEPT_END_FLAG_OFF);
        BOOST_CHECK_EQUAL(data->sorry_flag, SORRY_FLAG_OFF);
        BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, SORRYSERVER_SWITCH_FLAG_OFF);
        BOOST_CHECK_EQUAL(data->realserver_switch_flag, REALSERVER_SWITCH_FLAG_OFF);
        BOOST_CHECK_EQUAL(data->client_endpoint_tcp, ep_tcp);
    }
}

//handle_session_finalize 董作方
void handle_session_finalize_test() {
    cout << "[54]--------------------------------------------- " << endl;
    //unit_test[54] セッションスレッドに対応する終了処理
    //unit_test[54] test data:セッションスレッド初期化
    boost::thread down_thread(down_thread_func);
    boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    boost::asio::ip::udp::endpoint ep_udp = string_to_endpoint<boost::asio::ip::udp> ("10.10.100.100:8088");

    this->handle_session_initialize(boost::this_thread::get_id(),down_thread.get_id(),ep_tcp,ep_udp);
    protocol_module_base::EVENT_TAG ret =
    this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());

    //session_thread_data_map中にthis_thread無し
    session_thread_data_map_it thread_map_iterator
        = this->session_thread_data_map.find(boost::this_thread::get_id());
    bool bret = (thread_map_iterator == this->session_thread_data_map.end());
    BOOST_CHECK_EQUAL(bret,true);

    //session_thread_data_map中にdown_thread無し
    thread_map_iterator = this->session_thread_data_map.find(down_thread.get_id());
    bret = (thread_map_iterator == this->session_thread_data_map.end());
    BOOST_CHECK_EQUAL(bret,true);

    //遷移先ステータスを設定する status = ACCEPT
    BOOST_CHECK_EQUAL( ret, STOP);
}

//handle_session_finalize 董作方
void handle_session_finalize_test_thread() {
    cout << "[55]--------------------------------------------- " << endl;
    //unit_test[55] 多スレッドテスト/セッションスレッドに対応する終了処理
    //unit_test[55] test data:セッションスレッド初期化
    boost::thread down_thread(down_thread_func);
    boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    boost::asio::ip::udp::endpoint ep_udp = string_to_endpoint<boost::asio::ip::udp> ("10.10.100.100:8088");
    //session_thread_data_map中にthis_thread無し
    session_thread_data_map_it thread_map_iterator;
    bool bret;
    this->handle_session_initialize(boost::this_thread::get_id(),down_thread.get_id(),ep_tcp,ep_udp);
    protocol_module_base::EVENT_TAG ret = this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        thread_map_iterator = this->session_thread_data_map.find(boost::this_thread::get_id());
        bret = (thread_map_iterator == this->session_thread_data_map.end());
    }
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(bret,true);
        BOOST_CHECK_EQUAL(ret, STOP);
    }
}

//handle_accept 董作方
void handle_accept_test() {
    cout << "[56]--------------------------------------------- " << endl;
    //unit_test[56] session_thread_data_map中にthread_id無し
    EVENT_TAG ret = this->handle_accept(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[57]--------------------------------------------- " << endl;
    //unit_test[57] session_thread_data_map中にsession_thread_data無し
    thread_data_ptr  data1 ;
    this->session_thread_data_map[boost::this_thread::get_id()] = data1;
    ret = this->handle_accept(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[58]--------------------------------------------- " << endl;
    //unit_test[58] sorry状態の場合
    //unit_test[58] test data:accept_end_flag= on,sorry_flag= on
    thread_data_ptr data_pro(new session_thread_data_sessionless);
    data_pro->accept_end_flag = ACCEPT_END_FLAG_OFF;
    data_pro->sorry_flag = SORRY_FLAG_ON;
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;

    ret = this->handle_accept(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(data_pro->accept_end_flag, ACCEPT_END_FLAG_ON);//accept完了フラグをON
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);//遷移先ステータス=SORRYSERVER_SELECT

    cout << "[59]--------------------------------------------- " << endl;
    //unit_test[59] sorry状態以外の場合
    //unit_test[59] test data:accept_end_flag= on,sorry_flag= off
    data_pro->accept_end_flag = ACCEPT_END_FLAG_OFF;
    data_pro->sorry_flag = SORRY_FLAG_OFF;
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;

    ret = this->handle_accept(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(data_pro->accept_end_flag, ACCEPT_END_FLAG_ON);//accept完了フラグをON
    BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);//遷移先ステータス=REALSERVER_SELECT
}

//handle_accept 董作方
void handle_accept_test_thread() {
    cout << "[60]--------------------------------------------- " << endl;
    //unit_test[60] 多スレッドテスト/sorry状態の場合
    //unit_test[60] test data:accept_end_flag= on,sorry_flag= on
    thread_data_ptr data_pro(new session_thread_data_sessionless);
    data_pro->accept_end_flag = ACCEPT_END_FLAG_OFF;
    data_pro->sorry_flag = SORRY_FLAG_ON;
    {
        boost::mutex::scoped_lock  sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    }
    EVENT_TAG ret = this->handle_accept(boost::this_thread::get_id());
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(data_pro->accept_end_flag, ACCEPT_END_FLAG_ON);//accept完了フラグをON
        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);//遷移先ステータス=SORRYSERVER_SELECT
    }
}

//handle_client_recv 郎希倹
void handle_client_recv_test(){
    size_t request_len = USE_BUFFER_SIZE;
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> request;
    send_status send_status_temp;
    boost::thread down_thread(&down_thread_func);
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    receive_data receive_data_tmp;

    init_send_status(send_status_temp);
    // receive_dataを作成する
    init_receive_data(receive_data_tmp);

    //receive_data_mapを作成する
    psession_thread_data->end_flag = END_FLAG_OFF;
    psession_thread_data->last_status = CLIENT_RECV;
    boost::asio::ip::tcp::endpoint endpoint_local;
    endpoint_local = string_to_endpoint<boost::asio::ip::tcp> ("111.111.111.111:1111");
    boost::asio::ip::tcp::endpoint endpoint_not_used;
    psession_thread_data->client_endpoint_tcp = endpoint_local;
    endpoint_not_used = string_to_endpoint<boost::asio::ip::tcp> ("100.100.100.100:8888");

    //session_thread_dataを作成する
    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    this->forwarded_for = 1;

    cout << "[61]--------------------------------------------- " << endl;
    // unit_test[61] boost::this_thread::get_id()対応のデータsession_thread_dataなし
    this->session_thread_data_map.clear();
    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[62]--------------------------------------------- " << endl;
    // unit_test[62] boost::this_thread::get_id()対応のデータsession_thread_dataがNULL
    this->session_thread_data_map.clear();
    thread_data_ptr p;
    this->session_thread_data_map[boost::this_thread::get_id()] = p;
    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;

    cout << "[63]--------------------------------------------- " << endl;
    // unit_test[63] endpoint対応のreceive_dataなし
    // requestを設定する
    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[endpoint_not_used] = receive_data_tmp;

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=123456789012345\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;
    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(endpoint_local);
    receive_data& receive_data_global = it->second;

    cout << "[64]--------------------------------------------- " << endl;
    // unit_test[64] receive_data.receive_buffer＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = NULL;
    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[65]--------------------------------------------- " << endl;
    // unit_test[65] receive_data.receive_buffer1＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 20;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer1 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memset(request.c_array(), 'n', 80);
    request_len = 80;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[66]--------------------------------------------- " << endl;
    // unit_test[66] receive_data.receive_buffer2＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 20;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memset(request.c_array(), 'n', 80);
    request_len = 80;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[67]--------------------------------------------- " << endl;
    // unit_test[67] new失敗の場合
    receive_data_global.receive_buffer_max_size = 0;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = NULL;
    receive_data_global.receive_buffer2 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=123456789012345\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    this->getloglevel = &stb_getloglevel_is_none;
    new_install();
    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
    new_uninstall();
    this->getloglevel = &stb_getloglevel;
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[68]--------------------------------------------- " << endl;
    // unit_test[68] 終了フラグがONの場合、遷移先ステータスをチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    psession_thread_data->end_flag = END_FLAG_ON;
    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=123456789012345\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    psession_thread_data->end_flag = END_FLAG_OFF;

    cout << "[69]--------------------------------------------- " << endl;
    // unit_test[69] リクエストサイズが0である
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    ret = handle_client_recv(boost::this_thread::get_id(), request, 0);
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[70]--------------------------------------------- " << endl;
    // unit_test[70] リクエストサイズがUINT_MAXである リクエスト実際にサイズがMAX_BUFFER_SIZEである
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = MAX_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memset(request.c_array(), 'n', MAX_BUFFER_SIZE);
    ret = handle_client_recv(boost::this_thread::get_id(), request, UINT_MAX);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[71]--------------------------------------------- " << endl;
    // unit_test[71] リクエストサイズがMAX_BUFFER_SIZEである リクエスト実際にサイズがMAX_BUFFER_SIZEである
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = MAX_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    memcpy(request.c_array(), "GET / HTTP/1.0\r\n", 16);
    memset(request.c_array() + 16, 'n', MAX_BUFFER_SIZE - 16);

    ret = handle_client_recv(boost::this_thread::get_id(), request, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE);

    // unit_test_part1 begin:
    //（異常系）
    send_status status_END;
    init_send_status(status_END);
    status_END.status = SEND_END;
    status_END.send_offset = 0;
    status_END.send_end_size = 20;

    send_status status_OK;
    init_send_status(status_OK);
    status_OK.status = SEND_OK;
    status_OK.send_offset = 0;
    status_OK.send_possible_size = 20;

    send_status status_NG;
    init_send_status(status_NG);
    status_NG.status = SEND_NG;
    status_NG.send_offset = 0;
    status_NG.unsend_size = 20;

    send_status status_CONTINUE;
    init_send_status(status_CONTINUE);
    status_CONTINUE.status = SEND_CONTINUE;
    status_CONTINUE.send_offset = 0;
    status_CONTINUE.send_end_size = 20;
    status_CONTINUE.send_rest_size = 20;

    send_status* send_status_NG[27][3]
    = {
        // unit_test 入力の送信状態リストは下記の通り：（異常系）
        // unit_test[72] 送信状態リストの要素の送信状態が1：送信待の場合
        {&status_OK,NULL,NULL},
        // unit_test[73] 送信状態リストの要素の送信状態が1：送信済 2：送信待の場合
        {&status_END,&status_OK,NULL},
        // unit_test[74] 送信状態リストの要素の送信状態が1：送信継続 2：送信継続の場合
        {&status_CONTINUE,&status_CONTINUE,NULL},
        // unit_test[75] 送信状態リストの要素の送信状態が1：送信継続 2：送信不可の場合
        {&status_CONTINUE,&status_NG,NULL},
        // unit_test[76] 送信状態リストの要素の送信状態が1：送信継続 2：送信済の場合
        {&status_CONTINUE,&status_END,NULL},
        // unit_test[77] 送信状態リストの要素の送信状態が1：送信継続 2：送信待の場合
        {&status_CONTINUE,&status_OK,NULL},
        // unit_test[78] 送信状態リストの要素の送信状態が1：送信不可 2：送信継続の場合
        {&status_NG,&status_CONTINUE,NULL},
        // unit_test[79] 送信状態リストの要素の送信状態が1：送信不可 2：送信不可の場合
        {&status_NG,&status_NG,NULL},
        // unit_test[80] 送信状態リストの要素の送信状態が1：送信不可 2：送信済の場合
        {&status_NG,&status_END,NULL},
        // unit_test[81] 送信状態リストの要素の送信状態が1：送信不可 2：送信待の場合
        {&status_NG,&status_OK,NULL},
        // unit_test[82] 送信状態リストの要素の送信状態が1：送信待 2：送信継続の場合
        {&status_OK,&status_CONTINUE,NULL},
        // unit_test[83] 送信状態リストの要素の送信状態が1：送信待 2：送信不可の場合
        {&status_OK,&status_NG,NULL},
        // unit_test[84] 送信状態リストの要素の送信状態が1：送信待 2：送信済の場合
        {&status_OK,&status_END,NULL},
        // unit_test[85] 送信状態リストの要素の送信状態が1：送信待 2：送信待の場合
        {&status_OK,&status_OK,NULL},
        // unit_test[86] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信継続の場合
        {&status_END,&status_CONTINUE,&status_CONTINUE},
        // unit_test[87] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信不可の場合
        {&status_END,&status_CONTINUE,&status_NG},
        // unit_test[88] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信済の場合
        {&status_END,&status_CONTINUE,&status_END},
        // unit_test[89] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信待の場合
        {&status_END,&status_CONTINUE,&status_OK},
        // unit_test[90] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信継続の場合
        {&status_END,&status_NG,&status_CONTINUE},
        // unit_test[91] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信不可の場合
        {&status_END,&status_NG,&status_NG},
        // unit_test[92] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信済の場合
        {&status_END,&status_NG,&status_END},
        // unit_test[93] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信待の場合
        {&status_END,&status_NG,&status_OK},
        // unit_test[94] 送信状態リストの要素の送信状態が1：送信済 2：送信済 ３：送信待の場合
        {&status_END,&status_END,&status_OK},
        // unit_test[95] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信継続の場合
        {&status_END,&status_OK,&status_CONTINUE},
        // unit_test[96] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信不可の場合
        {&status_END,&status_OK,&status_NG},
        // unit_test[97] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信済の場合
        {&status_END,&status_OK,&status_END},
        // unit_test[98] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信待の場合
        {&status_END,&status_OK,&status_OK},
    };

    int i;
    int j;

    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    memset(receive_data_global.receive_buffer, 'n', USE_BUFFER_SIZE);
    memset(request.c_array(), 'm', USE_BUFFER_SIZE);
    for (i = 0; i < 27; i++)
    {
        receive_data_global.send_status_list.clear();

        for (j = 0; j < 3; j++)
        {
            if(send_status_NG[i][j] == NULL)
            {
             break;
            }

            receive_data_global.send_status_list.push_back(*(send_status_NG[i][j]));
        }

        ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);
        std::cout << "[" << 72 + i << "]------------------------------------------" << std::endl;
        BOOST_CHECK_EQUAL(ret, FINALIZE);
    }

    cout << "[99]--------------------------------------------- " << endl;
    // unit_test[99] 送信状態リストの要素の送信状態が1：送信済 2：送信済 3：送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 20;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 10;
    send_status_temp.send_rest_size = 11;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(request.c_array(), "12345678901POST / HTTP/1.0\r\nContent-Length: 20\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 39u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 11u);

    cout << "[100]--------------------------------------------- " << endl;
    // unit_test[100] 送信状態リストの要素の送信状態が1：送信済 2：送信済 3：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 20;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 40;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "GET / HTTP/", 10);

    memcpy(request.c_array(), "/1.0\r\nCookie: monkey=12345678901234567890123456789", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[101]--------------------------------------------- " << endl;
    // unit_test[101] 送信状態リストの要素の送信状態が1：送信済 2：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 40;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "GET / HTTP/", 10);

    memcpy(request.c_array(), "/1.0\r\nCookie: monkey=12345678901234567890123456789", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[102]--------------------------------------------- " << endl;
    // unit_test[102] 送信状態リストの要素の送信状態が1：送信済 2：送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 10;
    send_status_temp.send_rest_size = 11;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(request.c_array(), "12345678901POST / HTTP/1.0\r\nContent-Length: 20\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 39u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 11u);

    cout << "[103]--------------------------------------------- " << endl;
    // unit_test[103] 送信状態リストの要素の送信状態が１:送信済　２:送信済の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(request.c_array(), "POST / HTTP/1.0\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[104]--------------------------------------------- " << endl;
    // unit_test[104] 送信状態リストの要素の送信状態が1：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 10;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 10, "GET / HTTP/", 10);

    memcpy(request.c_array(), "/1.0\r\nCookie: monkey=1234567890123456789012345\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[105]--------------------------------------------- " << endl;
    // unit_test[105] 送信状態リストの要素の送信状態が１:送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 12;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 38;
    send_status_temp.send_rest_size = 20;

    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "GET / HTTP/1.0\r\nContent-Length: 20\r\n\r\n", 38);

    memcpy(request.c_array(), "12345678901234567890POST / HTTP/1.0\r\nContent-Lengt", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 30u);

    cout << "[106]--------------------------------------------- " << endl;
    // unit_test[106] 送信状態リストの要素の送信状態が１:送信済の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 80;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(request.c_array(), "POST / HTTP/1.0\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[107]--------------------------------------------- " << endl;
    // unit_test[107] 送信状態リストの要素数が０である場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "POST / HTTP/1.0\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    // unit_test_part2 begin:
    cout << "[108]--------------------------------------------- " << endl;
    // unit_test[108] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[108]  ●データバッファMAXサイズ　<　未送信データサイズ(0)　＋　リクエストデータサイズ。
    // unit_test[108]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 80;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nHOST: 192.168.120.1\r\nCookie: "
            "monkey=1\r\nContent-Length: 65455\r\n\r\n", 80);
    memset(request.c_array() + 80, 'x', 65455);
    request_len = MAX_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);

    //メモリの内容をチェックする
    int cmp_ret = memcmp(request.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[109]--------------------------------------------- " << endl;
    // unit_test[109] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[109]  ●データバッファMAXサイズ　<　未送信データサイズ(0)　＋　リクエストデータサイズ。
    // unit_test[109]  ●データバッファ上限値　＜　未送信データサイズ　＋　リクエストデータサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 80;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 10;
    send_status_temp.unsend_size = MAX_SESSIONLESS_MODULE_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nHOST: 192.168.120.1\r\nCookie: "
            "monkey=1\r\nContent-Length: 65455\r\n\r\n", 80);
    memset(request.c_array() + 80, 'x', 65455);
    request_len = MAX_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[110]--------------------------------------------- " << endl;
    // unit_test[110] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[110]  ●データバッファMAXサイズ　<　未送信データサイズ(20)　＋　リクエストデータサイズ。
    // unit_test[110]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=", 11);
    memset(request.c_array(), 'x', MAX_BUFFER_SIZE - 11);
    request_len = MAX_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE + 20);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, MAX_BUFFER_SIZE + 20);

    cout << "[111]--------------------------------------------- " << endl;
    // unit_test[111] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[111]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　>　未送信データサイズ(0)　＋　リクエストデータサイズ。
    // unit_test[111]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=12345678901234567890123456789012345678901", 72);
    request_len = 72;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);
    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, request.c_array(), 72);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[112]--------------------------------------------- " << endl;
    // unit_test[112] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[112]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　=　未送信データサイズ(0)　＋　リクエストデータサイズ。
    // unit_test[112]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=12345678901234567890123456789012345678901234567890123456789", 90);
    request_len = 90;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);
    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, request.c_array(), 90);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[113]--------------------------------------------- " << endl;
    // unit_test[113] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[113]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　>　未送信データサイズ(20)　＋　リクエストデータサイズ。
    // unit_test[113]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=12345678901234567890123456789012345678901", 52);
    request_len = 52;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), 52);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[114]--------------------------------------------- " << endl;
    // unit_test[114] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[114]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　=　未送信データサイズ(20)　＋　リクエストデータサイズ。
    // unit_test[114]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=12345678901234567890123456789012345678901234567890123456789", 70);
    request_len = 70;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[115]--------------------------------------------- " << endl;
    // unit_test[115] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[115]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　>　未送信データサイズ(0)　＋　リクエストデータサイズ。
    // unit_test[115]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=12345678901234567890123456789012345678901", 72);
    request_len = 72;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, request.c_array(), 72);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[116]--------------------------------------------- " << endl;
    // unit_test[116] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[116]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　=　未送信データサイズ(0)　＋　リクエストデータサイズ。
    // unit_test[116]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(),
            "GET / HTTP/1.0\r\nCookie: monkey=12345678901234567890123456789012345678901234567890123456789", 90);
    request_len = 90;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, request.c_array(), 90);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[117]--------------------------------------------- " << endl;
    // unit_test[117] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[117]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　>　未送信データサイズ(20)　＋　リクエストデータサイズ。
    // unit_test[117]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=12345678901234567890123456789012345678901", 52);
    request_len = 52;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), 52);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[118]--------------------------------------------- " << endl;
    // unit_test[118] データバッファ残サイズ　<　リクエストデータサイズ
    // unit_test[118]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　=　未送信データサイズ(20)　＋　リクエストデータサイズ。
    // unit_test[118]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=12345678901234567890123456789012345678901234567890123456789", 70);
    request_len = 70;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[119]--------------------------------------------- " << endl;
    // unit_test[119] データバッファ残サイズ　>　リクエストデータサイズ
    // unit_test[119]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=1234567890123456789012345678901", 62);
    request_len = 62;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 8u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), 62);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 62u);

    cout << "[120]--------------------------------------------- " << endl;
    // unit_test[120] データバッファ残サイズ　=　リクエストデータサイズ
    // unit_test[120]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=123456789012345678901234567890123456789", 70);
    request_len = 70;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, request.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 70u);

    // unit_test_part3 begin:
    cout << "[121]--------------------------------------------- " << endl;
    // unit_test[121] 送信状態リストの要素の送信状態が送信継続
    // unit_test[121]  ●送信データ残サイズ　<　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "POST / HTTP/1.0\r\nContent-Length: 20\r\n\r\n", 39);

    memcpy(request.c_array(), "12345678901234567890POST / HTTP/1.0\r\nContent-Lengt", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 30u);

    cout << "[122]--------------------------------------------- " << endl;
    // unit_test[122] 送信状態リストの要素の送信状態が送信継続
    // unit_test[122]  ●送信データ残サイズ　=　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "POST / HTTP/1.0\r\nContent-Length: 50\r\n\r\n", 39);

    memcpy(request.c_array(), "12345678901234567890123456789012345678901234567890", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[123]--------------------------------------------- " << endl;
    // unit_test[123] 送信状態リストの要素の送信状態が送信継続
    // unit_test[123]  ●送信データ残サイズ　>　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 80;
    receive_data_global.send_status_list.push_back(send_status_temp);
    // 未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "POST / HTTP/1.0\r\nContent-Length: 80\r\n\r\n", 39);

    memcpy(request.c_array(), "12345678901234567890123456789012345678901234567890", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[124]--------------------------------------------- " << endl;
    // unit_test[124] 送信状態が送信不可の場合
    // unit_test[124]  ●HTTPメソッドの妥当性をチェックして、チェック不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "OST /HTTP ", 10);
    request_len = 10;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 11u);

    cout << "[125]--------------------------------------------- " << endl;
    // unit_test[125] 送信状態が送信不可の場合
    // unit_test[125]  ●HTTPメソッドの妥当性をチェックして、チェック異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "EST / HTTP/1.0\r\n4567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[126]--------------------------------------------- " << endl;
    // unit_test[126] 送信状態が送信不可の場合
    // unit_test[126]  ●HTTPバージョンの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "OST / HTTP/1.2234\r\n7890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[127]--------------------------------------------- " << endl;
    // unit_test[127] 送信状態が送信不可の場合
    // unit_test[127]  ●HTTPバージョンの妥当性をチェックして、チェック不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "OST / HTT", 10);
    request_len = 10;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 11u);

    cout << "[128]--------------------------------------------- " << endl;
    // unit_test[128] 送信状態が送信不可の場合
    // unit_test[128]  ●HTTPヘッダの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "OST / HTTP/1.0\r\nCookie: monkey=1111111111111111111", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 51u);

    cout << "[129]--------------------------------------------- " << endl;
    // unit_test[129] 送信状態が送信不可の場合
    // unit_test[129]  ●HTTPヘッダ（ContentLength）があり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "OST / HTTP/1.0\r\nContent-Length: 15\r\n\r\n123456789012", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 3u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[130]--------------------------------------------- " << endl;
    // unit_test[130] 送信状態が送信不可の場合
    // unit_test[130]  ●HTTPヘッダ（ContentLength）なし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'P';

    memcpy(request.c_array(), "OST / HTTP/1.0\r\nCookie: monkey=111111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[131]--------------------------------------------- " << endl;
    // unit_test[131] 送信状態が送信不可の場合
    // unit_test[131]  ●送信データ残サイズ　>　未送信データサイズ　＋　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=123456789012345678901\r\n\r\nGET ", 40);
    request_len = 40;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 30u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 56u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 56u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 4u);

    cout << "[132]--------------------------------------------- " << endl;
    // unit_test[132] 送信状態が送信不可の場合
    // unit_test[132]  ●送信データ残サイズ　<　未送信データサイズ　＋　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nCoo", 20);

    //リクエストデータ
    memcpy(request.c_array(), "kie: monkey=123\r\nContent-Length: 100\r\n\r\n0123456789", 50);
    request_len = 50;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 90u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[133]--------------------------------------------- " << endl;
    // unit_test[133] 送信状態が送信不可の場合
    // unit_test[133]  ●送信データ残サイズ　=　未送信データサイズ　＋　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nCoo", 20);

    //リクエストデータ
    memcpy(request.c_array(), "kie: monkey=123\r\nContent-Length: 11\r\n\r\n01234567890", 50);
    request_len = 50;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[134]--------------------------------------------- " << endl;
    // unit_test[134] リクエストデータ残サイズ　<　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nCook", 20);

    //リクエストデータ
    memcpy(request.c_array(), "ie: monkey=123456789012345678901\r\n\r\nGET ", 40);
    request_len = 40;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 30u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 56u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 56u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 4u);

    cout << "[135]--------------------------------------------- " << endl;
    // unit_test[135] リクエストデータ残サイズ　=　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nCoo", 20);

    //リクエストデータ
    memcpy(request.c_array(), "kie: monkey=123\r\nContent-Length: 11\r\n\r\n01234567890", 50);
    request_len = 50;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    // unit_test_part4 begin:
    cout << "[136]--------------------------------------------- " << endl;
    // unit_test[136] リクエストデータ残サイズ　＞　0
    // unit_test[136]  ●HTTPメソッドの妥当性をチェックして、チェック不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    psession_thread_data->sorry_flag = SORRY_FLAG_OFF;

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nContent-Length: 10\r\n\r\n1234567890GE", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 48u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 48u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 2u);

    cout << "[137]--------------------------------------------- " << endl;
    // unit_test[137] リクエストデータ残サイズ　＞　0
    // unit_test[137]  ●HTTPメソッドの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "EST / HTTP/1.0\r\n4567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[138]--------------------------------------------- " << endl;
    // unit_test[138] リクエストデータ残サイズ　＞　0
    // unit_test[138]  ●HTTPバージョンの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    psession_thread_data->sorry_flag = SORRY_FLAG_OFF;

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nContent-Length: 10\r\n\r\n1234567890GE", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 48u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 48u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 2u);

    cout << "[139]--------------------------------------------- " << endl;
    // unit_test[139] リクエストデータ残サイズ　＞　0
    // unit_test[139]  ●HTTPバージョンの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.22\r\n567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[140]--------------------------------------------- " << endl;
    // unit_test[140] リクエストデータ残サイズ　＞　0
    // unit_test[140]  ●HTTPヘッダの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=1111111111111111111", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[141]--------------------------------------------- " << endl;
    // unit_test[141] リクエストデータ残サイズ　＞　0
    // unit_test[141]  ●HTTPヘッダ（ContentLength）があり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "POST / HTTP/1.0\r\nContent-Length: 15\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 4u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[142]--------------------------------------------- " << endl;
    // unit_test[142] リクエストデータ残サイズ　＞　0
    // unit_test[142]  ●HTTPヘッダ（ContentLength）なし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=111111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[143]--------------------------------------------- " << endl;
    // unit_test[143] リクエストデータ残サイズ　＞　0
    // unit_test[143]  ●送信データ残サイズ = リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=111111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[144]--------------------------------------------- " << endl;
    // unit_test[144] リクエストデータ残サイズ　＞　0
    // unit_test[144]  ●送信データ残サイズ < リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "POST / HTTP/1.0\r\nContent-Length: 0\r\n\r\nPOST / HTTP/", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 38u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);

    cout << "[145]--------------------------------------------- " << endl;
    // unit_test[145] リクエストデータ残サイズ　＞　0
    // unit_test[145]  ●送信データ残サイズ　＞　リクエストデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nContent-Length: 20\r\n\r\n123456789012", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 8u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, REQUEST_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[146]--------------------------------------------- " << endl;
    // unit_test[146] リクエストデータ残サイズ　＞　0
    // unit_test[146]  ●出力の送信状態リスト：　１：送信待　２：送信待
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 78;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 12;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1", 12);

    memcpy(request.c_array(), ".0\r\nCookie: m=1\r\n\r\nGET / HTTP/1.0\r\nCookie: m=1\r\n\r\n", 50);
    request_len = 50;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 31u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->status, SEND_OK);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_rest_size, 0u);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_possible_size, 31u);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_offset, 31u);

    cout << "[147]--------------------------------------------- " << endl;
    // unit_test[147] リクエストデータ残サイズ　＞　0
    // unit_test[147]  ●出力の送信状態リスト：　１：送信待　２：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    psession_thread_data->sorry_flag = SORRY_FLAG_OFF;

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nContent-Length: 10\r\n\r\n1234567890GE", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 48u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 48u);

    cout << "[148]--------------------------------------------- " << endl;
    // unit_test[148] リクエストデータ残サイズ　＞　0
    // unit_test[148]  ●出力の送信状態リスト：　１：送信待
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "POST / HTTP/1.0\r\nContent-Length: 15\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 4u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[149]--------------------------------------------- " << endl;
    // unit_test[149] リクエストデータ残サイズ　＞　0
    // unit_test[149]  ●出力の送信状態リスト：　１：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\n4567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[150]--------------------------------------------- " << endl;
    // unit_test[150] 送信可能データがあり
    // unit_test[150]  ●SORRY状態の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    psession_thread_data->sorry_flag = SORRY_FLAG_ON;

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nHOST: KKKK\r\n\r\nGNT / HTTP/1.1\r\nHOST", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
    psession_thread_data->sorry_flag = SORRY_FLAG_OFF;

    cout << "[151]--------------------------------------------- " << endl;
    // unit_test[151] 送信可能データがあり
    // unit_test[151]  ●SORRY状態以外の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    psession_thread_data->sorry_flag = SORRY_FLAG_OFF;

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nHOST: KKKK\r\n\r\nGNT / HTTP/1.1\r\nHOST", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    psession_thread_data->sorry_flag = SORRY_FLAG_OFF;

    cout << "[152]--------------------------------------------- " << endl;
    // unit_test[152] 送信可能データなし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(request.c_array(), "GET / HTTP/1.0\r\nCookie: monkey=1111111111111111111", REQUEST_BUFFER_SIZE);
    request_len = REQUEST_BUFFER_SIZE;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);
}

//handle_client_recv_test_thread
void handle_client_recv_test_thread(){
    size_t request_len = USE_BUFFER_SIZE;
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> request;
    send_status send_status_temp;
    receive_data receive_data_tmp;
    boost::thread down_thread(&down_thread_func);
    boost::asio::ip::tcp::endpoint endpoint_local = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    psession_thread_data->client_endpoint_tcp = endpoint_local;

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;
        this->forwarded_for = 1;
    }

    init_receive_data(receive_data_tmp);

    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(endpoint_local);
    receive_data& receive_data_global = it->second;

    cout << "[153]--------------------------------------------- " << endl;
    // unit_test[153] リクエストデータ残サイズ　＞　0
    // unit_test[153]  ●出力の送信状態リスト：　１：送信待　２：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 78;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 12;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1", 12);

    memcpy(request.c_array(), ".0\r\nCookie: m=1\r\n\r\nGET / HTTP/1.0\r\nCookie: m=1\r\n\r", 49);
    request_len = 49;

    ret = handle_client_recv(boost::this_thread::get_id(), request, request_len);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 31u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->status, SEND_NG);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->edit_division, EDIT_DIVISION_NO_EDIT);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_rest_size, 0u);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->unsend_size, 30u);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_offset, 31u);
    }
}

//handle_realserver_select(tcp) 馮家純
void handle_realserver_select_tcp_test(){
    EVENT_TAG ret;

    cout << "[154]--------------------------------------------- " << endl;
    //unit_test[154] 異常系 functionがなし場合遷移先ステータスにFINALIZEを設定する
    //register function
    tcp_schedule_func_type func_err1 = NULL;
    this->register_schedule(func_err1);
    boost::asio::ip::tcp::endpoint ep_err1;

    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_err1);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[155]--------------------------------------------- " << endl;
    //unit_test[155] 異常系 session_thread_data_map中にThreadID対応のデータがない
    this->session_thread_data_map.clear();
    //register function
    tcp_schedule_func_type func_cerr2 = &schedule_tcp_determinate;
    this->register_schedule(func_cerr2);
    this->session_thread_data_map[boost::this_thread::get_id()].reset();
    boost::asio::ip::tcp::endpoint ep_err2;
    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_err2);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[156]--------------------------------------------- " << endl;
    //unit_test[156] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
    this->session_thread_data_map.clear();
    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_err2);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[157]--------------------------------------------- " << endl;
    //unit_test[157] endpoint = 決定 rs endpointなし場合
    this->initialize(rslist_begin, rslist_end, rslist_next, rslist_lock, rslist_unlock);
    boost::thread down_thread1(down_thread_func);
    thread_data_ptr dataup1(new session_thread_data_sessionless);
    thread_data_ptr datadown1(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = dataup1;
    this->session_thread_data_map[down_thread1.get_id()] = datadown1;
    dataup1->thread_division = THREAD_DIVISION_UP_STREAM;
    dataup1->pair_thread_id = down_thread1.get_id();
    datadown1->thread_division = THREAD_DIVISION_DOWN_STREAM;
    datadown1->pair_thread_id = boost::this_thread::get_id();
    datadown1->receive_data_map.clear();
    //register function
    tcp_schedule_func_type func1 = &schedule_tcp_determinate;

    this->register_schedule(func1);

    boost::asio::ip::tcp::endpoint ep1;
    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep1);

    //endpoint = 決定
    BOOST_CHECK_EQUAL(ep1, string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888"));
    //遷移先ステータスを設定する status = REALSERVER_CONNECT
    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    cout << "[158]--------------------------------------------- " << endl;
    //unit_test[158] realserverが決定済みで、送信可能データがない場合、REALSERVER_CONNECTを返す。
    this->session_thread_data_map.clear();
    boost::asio::ip::tcp::endpoint ep_1;
    thread_data_ptr data_1(new session_thread_data_sessionless);
    data_1->client_endpoint_tcp = ep_1;
    data_1->receive_data_map[ep_1];
    this->session_thread_data_map[boost::this_thread::get_id()] = data_1;

    //register function
    tcp_schedule_func_type func_1 = &schedule_tcp_determinate;

    this->register_schedule(func_1);

    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_1);
    //endpoint = 決定
    BOOST_CHECK_EQUAL(ep_1, string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888"));
    //遷移先ステータスを設定する status = REALSERVER_CONNECT
    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    cout << "[159]--------------------------------------------- " << endl;
    //unit_test[159] realserverが決定済みで、送信可能データがある場合、REALSERVER_CONNECTを返す。
    this->session_thread_data_map.clear();
    boost::asio::ip::tcp::endpoint ep_2;
    thread_data_ptr data_2(new session_thread_data_sessionless);
    data_2->client_endpoint_tcp = ep_2;
    send_status s_t2;
    s_t2.status = SEND_OK;
    s_t2.send_possible_size = 2u;
    data_2->receive_data_map[ep_2].send_status_list.push_back(s_t2);
    this->session_thread_data_map[boost::this_thread::get_id()] = data_2;

    //register function
    tcp_schedule_func_type func_2 = &schedule_tcp_determinate;

    this->register_schedule(func_2);

    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_2);
    //endpoint = 決定
    BOOST_CHECK_EQUAL(ep_2, string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888"));
    //遷移先ステータスを設定する status = REALSERVER_CONNECT
    BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);

    cout << "[160]--------------------------------------------- " << endl;
    //unit_test[160] endpoint = 未決定
    this->session_thread_data_map.clear();

    thread_data_ptr dataup2(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = dataup2;
    dataup2->end_flag = END_FLAG_OFF;
    //register function
    tcp_schedule_func_type func2 = &schedule_tcp_nodeterminate;

    this->register_schedule(func2);

    boost::asio::ip::tcp::endpoint ep2, ep_chk2;
    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep2);

    //endpoint = 未決定
    BOOST_CHECK_EQUAL(ep_chk2, ep2);
    //sorryフラグをONにする
    BOOST_CHECK_EQUAL(dataup2->sorry_flag, SORRY_FLAG_ON);
    //遷移先ステータスを設定する status = SORRYSERVER_SELECT
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
}

//handle_realserver_select(tcp)_thread 馮家純
void handle_realserver_select_tcp_test_thread(){
    EVENT_TAG ret;
    cout << "[161]--------------------------------------------- " << endl;
    //unit_test[161] endpoint = 決定
    this->initialize(rslist_begin, rslist_end, rslist_next, rslist_lock, rslist_unlock);
    this->getloglevel = &stb_getloglevel_is_none;
    boost::thread down_thread(down_thread_func);
    thread_data_ptr dataup(new session_thread_data_sessionless);
    thread_data_ptr datadown(new session_thread_data_sessionless);
    dataup->thread_division = THREAD_DIVISION_UP_STREAM;
    dataup->pair_thread_id = down_thread.get_id();
    datadown->thread_division = THREAD_DIVISION_DOWN_STREAM;
    datadown->pair_thread_id = boost::this_thread::get_id();
    datadown->receive_data_map.clear();

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
        this->session_thread_data_map[down_thread.get_id()] = datadown;
    }

    boost::asio::ip::tcp::endpoint ep;
    ret = this->handle_realserver_select(boost::this_thread::get_id(), ep);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        //endpoint = 決定
        BOOST_CHECK_EQUAL(ep, string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888"));
        //遷移先ステータスを設定する status = REALSERVER_CONNECT
        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
    }
}

//handle_realserver_select(udp) 馮家純
void handle_realserver_select_udp_test(){

    cout << "[162]--------------------------------------------- " << endl;
    //unit_test[162] handle_realserver_select_udp_test 戻り値が「STOP」に設定する。
    boost::asio::ip::udp::endpoint ep;
    boost::array<char, MAX_BUFFER_SIZE> sbf;
    std::size_t d;

    EVENT_TAG ret = this->handle_realserver_select(boost::this_thread::get_id(), ep, sbf, d);

    BOOST_CHECK_EQUAL(ret, STOP);
}

//handle_realserver_connect 郎希倹
void handle_realserver_connect_test(){
    size_t send_buffer_len = USE_BUFFER_SIZE;
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> send_buffer;
    send_status send_status_temp;
    edit_data edit_data_temp;
    boost::thread down_thread(&down_thread_func);
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    receive_data receive_data_tmp;
    init_send_status(send_status_temp);
    // receive_dataを作成する
    init_receive_data(receive_data_tmp);
    receive_data_tmp.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_tmp.receive_buffer_rest_size = USE_BUFFER_SIZE;

    //receive_data_mapを作成する
    psession_thread_data->end_flag = END_FLAG_OFF;
    psession_thread_data->last_status = CLIENT_RECV;
    boost::asio::ip::tcp::endpoint endpoint_local = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");;

    //session_thread_dataを作成する
    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    psession_thread_data->client_endpoint_tcp = endpoint_local;
    this->forwarded_for = 1;

    cout << "[163]--------------------------------------------- " << endl;
    // unit_test[163] boost::this_thread::get_id()対応のデータなし
    this->session_thread_data_map.clear();
    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[164]--------------------------------------------- " << endl;
    // unit_test[164] boost::this_thread::get_id()対応のデータsession_thread_dataがNULL
    this->session_thread_data_map.clear();
    thread_data_ptr p;
    this->session_thread_data_map[boost::this_thread::get_id()] = p;
    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;

    cout << "[165]--------------------------------------------- " << endl;
    // unit_test[165] endpoint対応のreceive_dataなし
    psession_thread_data->receive_data_map.clear();
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(endpoint_local);
    receive_data& receive_data_global = it->second;

    cout << "[166]--------------------------------------------- " << endl;
    // unit_test[166] receive_data.receive_buffer＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = NULL;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[167]--------------------------------------------- " << endl;
    // unit_test[167] 送信状態リストが空の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;

    receive_data_global.send_status_list.clear();
    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[168]--------------------------------------------- " << endl;
    // unit_test[168] 送信状態リストの要素の送信状態が１:送信待　２:送信待の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 30;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 30;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 35;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer,
            "GET / HTTP/1.0\r\nHOST: KKKK\r\n\r\nGET / HTTP/1.0\r\nHOST: KKKKKKKKK\r\n\r\n", 65);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 30u);
    //送信バッファの内容をチェックする。
    int cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 30u);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_end_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 35u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 30u);

    cout << "[169]--------------------------------------------- " << endl;
    // unit_test[169] 送信状態リストの要素の送信状態が１:送信待　２:送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = 30;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 30;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 34;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer,
            "GET / HTTP/1.0\r\nHOST: KKKK\r\n\r\nGET / HTTP/1.0\r\nHOST: KKKKKKKKK\r\n\r", 64);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 30u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 30);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_end_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 34u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 30u);

    cout << "[170]--------------------------------------------- " << endl;
    // unit_test[170] 送信状態リストの要素の送信状態が１:送信待の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 50u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 50);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    const char* forwarded_exist = "POST / HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2\r\nContent-Length: 65465\r\n\r\n";
    const char* forwarded_not_exist = "POST / HTTP/1.0\r\nContent-Length: 65493\r\n\r\n";
    const int exist_len = 70;
    const int exist_leave_len = 65465;
    const int not_exist_leave_len = 65493;
    const int not_exist_len = 42;
    const int forwarded_offset = 17;

    cout << "[171]--------------------------------------------- " << endl;
    // unit_test[171] 編集区分 = 編集あり、編集データリストが空の場合
    // unit_test[171] ●送信可能データからHTTPヘッダ（X-Forwarded-For）の検索結果 = 該当あり。
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, forwarded_offset);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset + 40, receive_data_global.receive_buffer + forwarded_offset
            + 28, MAX_BUFFER_SIZE - forwarded_offset - 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[172]--------------------------------------------- " << endl;
    // unit_test[172] 編集区分 = 編集あり、編集データリストが空の場合
    // unit_test[172] ●送信可能データからHTTPヘッダ（X-Forwarded-For）の検索結果 = 該当なし。
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_not_exist, not_exist_len);
    memset(receive_data_global.receive_buffer + not_exist_len, 'x', not_exist_leave_len);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, forwarded_offset);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset, "X-Forwarded-For: 10.10.10.1\r\n", 29);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset + 29,
            receive_data_global.receive_buffer + forwarded_offset, MAX_BUFFER_SIZE - forwarded_offset - 29);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 29u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 29u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[173]--------------------------------------------- " << endl;
    // unit_test[173] 編集区分 = 編集あり、編集データリストが空でないの場合
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 32;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 60);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = MAX_BUFFER_SIZE;
    send_status_temp.send_possible_size = 32;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_edit_data(edit_data_temp);
    edit_data_temp.data = "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n";
    edit_data_temp.data_size = 40;
    edit_data_temp.insert_posission = MAX_BUFFER_SIZE + 2;
    edit_data_temp.replace_size = 28;
    receive_data_global.send_status_list.begin()->edit_data_list.clear();
    receive_data_global.send_status_list.begin()->edit_data_list.push_back(edit_data_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 44u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer + MAX_BUFFER_SIZE, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 2, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 42, receive_data_global.receive_buffer + MAX_BUFFER_SIZE + 30, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE + 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[174]--------------------------------------------- " << endl;
    // unit_test[174] 編集データリストが空の場合
    // unit_test[174] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[174] ●送信バッファ残サイズ　＞　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 32;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 60);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = MAX_BUFFER_SIZE;
    send_status_temp.send_possible_size = 32;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_edit_data(edit_data_temp);
    edit_data_temp.data = "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n";
    edit_data_temp.data_size = 40;
    edit_data_temp.insert_posission = MAX_BUFFER_SIZE + 2;
    edit_data_temp.replace_size = 28;
    receive_data_global.send_status_list.begin()->edit_data_list.clear();
    receive_data_global.send_status_list.begin()->edit_data_list.push_back(edit_data_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 44u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer + MAX_BUFFER_SIZE, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 2, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 42, receive_data_global.receive_buffer + MAX_BUFFER_SIZE + 30, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE + 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[175]--------------------------------------------- " << endl;
    // unit_test[175] 編集データリストが空の場合
    // unit_test[175] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[175] ●送信バッファ残サイズ　＝　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE - 12;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 104);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 44, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE - 12;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE - 42);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 42, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 2, receive_data_global.receive_buffer + MAX_BUFFER_SIZE
            - 14, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[176]--------------------------------------------- " << endl;
    // unit_test[176] 編集データリストが空の場合
    // unit_test[176] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[176] ●送信バッファ残サイズ　＜　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, forwarded_offset);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset + 40, receive_data_global.receive_buffer + forwarded_offset
            + 28, MAX_BUFFER_SIZE - forwarded_offset - 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[177]--------------------------------------------- " << endl;
    // unit_test[177] 編集データリストが空の場合
    // unit_test[177] ●送信バッファ残サイズ　＝　0
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 102);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 42, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE - 10;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE - 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 40, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[178]--------------------------------------------- " << endl;
    // unit_test[178] 編集データリストが空の場合
    // unit_test[178] ●送信可能データサイズ　＝　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 79u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(),
                     "GET / HTTP/1.0\r\nX-Forwarded-For: 10.10.10.1\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 79);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[179]--------------------------------------------- " << endl;
    // unit_test[179] 編集データリストが空でない場合
    // unit_test[179] ●送信バッファ残サイズ　＞　編集データ設定位置　－　送信済サイズ
    // unit_test[179] ●送信バッファ残サイズ　＞　編集データサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 79u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, 16);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 16, "X-Forwarded-For: 10.10.10.1\r\n", 29);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 45, receive_data_global.receive_buffer + 16, 34);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[180]--------------------------------------------- " << endl;
    // unit_test[180] 編集データリストが空でない場合
    // unit_test[180] ●送信バッファ残サイズ　＞　編集データ設定位置　－　送信済サイズ
    // unit_test[180] ●送信バッファ残サイズ　＝　編集データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 102);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 42, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE - 10;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE - 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 40, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[181]--------------------------------------------- " << endl;
    // unit_test[181] 編集データリストが空でない場合
    // unit_test[181] ●送信バッファ残サイズ　＞　編集データ設定位置　－　送信済サイズ
    // unit_test[181] ●送信バッファ残サイズ　＜　編集データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 29;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 63);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 3, "\r\nX", 3);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "-Forwarded-For: 10.10.2.2\r\n\r\n", 29);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 29;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE - 1u);

    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE - 1);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data,
            "X-Forwarded-For: 10.10.2.2, 10.10.10.1");
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data_size, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->insert_posission,
            MAX_BUFFER_SIZE - 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->replace_size, 26u);

    cout << "[182]--------------------------------------------- " << endl;
    // unit_test[182] 編集データリストが空でない場合
    // unit_test[182] ●送信バッファ残サイズ　＝　編集データ設定位置　－　送信済サイズ
    // unit_test[182] ●送信バッファ残サイズ　＜　編集データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 30;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 62);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 2, "\r\n", 2);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "X-Forwarded-For: 10.10.2.2\r\n\r\n", 30);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 30;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data,
            "X-Forwarded-For: 10.10.2.2, 10.10.10.1");
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data_size, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->insert_posission,
            MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->replace_size, 26u);

    cout << "[183]--------------------------------------------- " << endl;
    // unit_test[183] 編集データリストが空でない場合
    // unit_test[183] ●送信バッファ残サイズ　＜　編集データ設定位置　－　送信済サイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 32;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST / HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 60);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 32;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data,
            "X-Forwarded-For: 10.10.2.2, 10.10.10.1");
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data_size, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->insert_posission,
            MAX_BUFFER_SIZE + 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->replace_size, 26u);

    cout << "[184]--------------------------------------------- " << endl;
    // unit_test[184] 編集区分 = 編集なし
    // unit_test[184] ●送信バッファサイズ　＞　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 50u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 50);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[185]--------------------------------------------- " << endl;
    // unit_test[185] 編集区分 = 編集なし
    // unit_test[185] ●送信バッファサイズ　＝　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[186]--------------------------------------------- " << endl;
    // unit_test[186] 編集区分 = 編集なし
    // unit_test[186] ●送信バッファサイズ　＜　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 1;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 1;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len + 1);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[187]--------------------------------------------- " << endl;
    // unit_test[187] forwarded_forを設定するの場合、編集区分が１である。
    this->forwarded_for = 1;
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 79u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, 16);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 16, "X-Forwarded-For: 10.10.10.1\r\n", 29);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 45, receive_data_global.receive_buffer + 16, 34);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[188]--------------------------------------------- " << endl;
    // unit_test[188] forwarded_forを設定しないの場合、編集区分が１である。
    this->forwarded_for = 0;
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 50u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, 50);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
}

//handle_realserver_connect_test_thread
void handle_realserver_connect_test_thread(){
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> send_buffer;
    send_status send_status_temp;
    receive_data receive_data_tmp;
    boost::thread down_thread(&down_thread_func);
    boost::asio::ip::tcp::endpoint endpoint_local = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    psession_thread_data->client_endpoint_tcp = endpoint_local;

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;
        this->forwarded_for = 1;
    }

    init_receive_data(receive_data_tmp);

    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(endpoint_local);
    receive_data& receive_data_global = it->second;

    const char* forwarded_exist = "POST / HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2\r\nContent-Length: 65465\r\n\r\n";
    const int exist_len = 70;
    const int exist_leave_len = 65465;
    const int forwarded_offset = 17;

    cout << "[189]--------------------------------------------- " << endl;
    // unit_test[189] 編集データリストが空の場合
    // unit_test[189] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[189] ●送信バッファ残サイズ　＜　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    size_t send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);
        BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
        //送信バッファの内容をチェックする。
        int cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, forwarded_offset);
        BOOST_CHECK_EQUAL(cmp_ret, 0);
        cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
        BOOST_CHECK_EQUAL(cmp_ret, 0);
        cmp_ret = memcmp(send_buffer.c_array() + forwarded_offset + 40, receive_data_global.receive_buffer + forwarded_offset
                + 28, MAX_BUFFER_SIZE - forwarded_offset - 40);
        BOOST_CHECK_EQUAL(cmp_ret, 0);

        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 12u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 12u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    }
}

//handle_realserver_connection_fail 馮家純
void handle_realserver_connection_fail_test() {
    EVENT_TAG ret;

    cout << "[190]--------------------------------------------- " << endl;
    //unit_test[190] 異常系 session_thread_data_map中にThreadID対応のデータがない
    this->session_thread_data_map.clear();

    this->session_thread_data_map[boost::this_thread::get_id()].reset();
    boost::asio::ip::tcp::endpoint ep_err1;
    ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep_err1);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[191]--------------------------------------------- " << endl;
    //unit_test[191] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
    this->session_thread_data_map.clear();

    ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep_err1);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[192]--------------------------------------------- " << endl;
    //unit_test[192] 終了フラグ,遷移先ステータス
    boost::asio::ip::tcp::endpoint ep;

    thread_data_ptr data(new session_thread_data_sessionless);
    data->thread_id = boost::this_thread::get_id();
    this->session_thread_data_map[boost::this_thread::get_id()] = data;

    ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);

    //終了フラグをON
    BOOST_CHECK_EQUAL(data->end_flag, END_FLAG_ON);
    //遷移先ステータス status = CLIENT_DISCONNECT
    BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
}

//handle_realserver_connection_fail_thread 馮家純
void handle_realserver_connection_fail_test_thread() {
    cout << "[193]--------------------------------------------- " << endl;
    //unit_test[193] 終了フラグ,遷移先ステータス
    EVENT_TAG ret;
    boost::asio::ip::tcp::endpoint ep;

    thread_data_ptr data(new session_thread_data_sessionless);
    data->thread_id = boost::this_thread::get_id();
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data;
    }

    ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        //終了フラグをON
        BOOST_CHECK_EQUAL(data->end_flag, END_FLAG_ON);
        //遷移先ステータス status = CLIENT_DISCONNECT
        BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
    }
}

//handle_realserver_send 董作方
void handle_realserver_send_test(){
    boost::asio::ip::tcp::endpoint endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    send_status real_send_status ,real_send_status1 ;
    receive_data real_receive_data;
    thread_data_ptr data_pro(new session_thread_data_sessionless);

    cout << "[194]--------------------------------------------- " << endl;
    //unit_test[194] session_thread_data_map中にthread_id無し
    EVENT_TAG event_status
        = this->handle_realserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[195]--------------------------------------------- " << endl;
    //unit_test[195] session_thread_data_map中にsession_thread_data_sessionless無し
    thread_data_ptr thread_data1 ;

    this->session_thread_data_map[boost::this_thread::get_id()] = thread_data1;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[196]--------------------------------------------- " << endl;
    //unit_test[196] receive_data_map中にendpoint無し
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[197]--------------------------------------------- " << endl;
    //unit_test[197] send_status_list中にsend_status無し
    real_receive_data.send_status_list.clear();
    data_pro->receive_data_map[endpoint] = real_receive_data;

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;

    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[198]--------------------------------------------- " << endl;
    //unit_test[198] 送信状態->SEND_OK,送信可能データサイズ　>　0/送信可能データあり
    //unit_test[198] test data: 送信状態に送信待を設定,送信データオフセットに送信済データサイズを加算する,送信済データサイズに0を設定する,遷移先ステータスを設定する
    real_send_status.send_possible_size = 1u;//送信可能データサイズ　>　0
    real_send_status.edit_data_list.clear();//編集データリスト=0
    real_send_status.send_offset = 0u;
    real_send_status.send_end_size = 1u;
    real_send_status.status = SEND_OK;//送信状態->SEND_OK
    real_receive_data.send_status_list.push_back(real_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, REALSERVER_CONNECT);//遷移先ステータスを設定する

    SEND_STATUS_TAG send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;
    size_t offset = data_pro->receive_data_map[endpoint].send_status_list.front().send_offset;
    size_t end_size = data_pro->receive_data_map[endpoint].send_status_list.front().send_end_size;

    BOOST_CHECK_EQUAL(send_status,SEND_OK); //送信状態に送信待を設定する
    BOOST_CHECK_EQUAL(offset,1u);//送信データオフセットに送信済データサイズを加算する
    BOOST_CHECK_EQUAL(end_size,0u); //送信済データサイズに0を設定する

    cout << "[199]--------------------------------------------- " << endl;
    //unit_test[199] 送信状態->SEND_NG
    //unit_test[199] test data:遷移先ステータスを設定する
    real_send_status.status = SEND_NG;//送信状態->SEND_NG
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    data_pro->receive_data_map.clear();
    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[200]--------------------------------------------- " << endl;
    //unit_test[200] 送信状態->SEND_CONTINUE
    //unit_test[200] test data:遷移先ステータスを設定する
    real_send_status.status = SEND_CONTINUE; //送信状態->SEND_CONTINUE
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[201]--------------------------------------------- " << endl;
    //unit_test[201] 送信状態->SEND_OK,送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし
    //unit_test[201] test data: 送信状態に送信待を設定
    real_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    real_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    real_send_status.status = SEND_OK;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する

    cout << "[202]--------------------------------------------- " << endl;
    //unit_test[202] 送信状態->SEND_OK,送信可能データサイズ　=　0/送信データ残サイズ　=　0/送信可能データなし
    //unit_test[202] test data: 送信状態に送信待を設定
    real_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    real_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    real_send_status.status = SEND_OK;//送信状態->SEND_OK
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する

    cout << "[203]--------------------------------------------- " << endl;
    //unit_test[203] 送信状態->SEND_OK,SEND_OK,送信可能データサイズ = 0,送信データ残サイズ　＞　0
    //unit_test[203] test data: 送信状態に送信継続を設定,遷移先ステータスを設定する
    real_send_status.send_possible_size = 0u;//送信可能データサイズ　= 0
    real_send_status.status = SEND_OK; //送信状態->SEND_OK
    real_send_status.send_rest_size = 1u; //送信データ残サイズ　> 0
    real_send_status1.status = SEND_OK;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[204]--------------------------------------------- " << endl;
    //unit_test[204] 送信状態->SEND_OK,SEND_OK,送信可能データサイズ = 0/送信データ残サイズ　=　0
    //unit_test[204] test data: 送信状態に送信継続を設定,遷移先ステータスを設定する
    real_send_status.send_possible_size = 0u;//送信可能データサイズ　= 0
    real_send_status.status = SEND_OK; //送信状態->SEND_OK
    real_send_status.send_rest_size = 0u; //送信データ残サイズ　= 0
    real_send_status1.status = SEND_OK;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, REALSERVER_CONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信データ残サイズ　= 0,送信状態に送信済を設定する

    cout << "[205]--------------------------------------------- " << endl;
    //unit_test[205] 送信状態->SEND_OK,SEND_NG,送信可能データサイズ = 0/送信データ残サイズ　>　0
    //unit_test[205] test data: 遷移先ステータスを設定する
    real_send_status.send_possible_size = 0u;//送信可能データサイズ　= 0
    real_send_status.status = SEND_OK; //送信状態->SEND_OK
    real_send_status.send_rest_size = 1u; //送信データ残サイズ　> 0
    real_send_status1.status = SEND_NG;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[206]--------------------------------------------- " << endl;
    //unit_test[206] 送信状態->SEND_OK,SEND_NG,送信可能データサイズ = 0/送信データ残サイズ　=　0
    //unit_test[206] test data: 送信状態に送信継続を設定,遷移先ステータスを設定する
    real_send_status.send_possible_size = 0u;//送信可能データサイズ　= 0
    real_send_status.status = SEND_OK; //送信状態->SEND_OK
    real_send_status.send_rest_size = 0u; //送信データ残サイズ　= 0
    real_send_status1.status = SEND_NG;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信データ残サイズ　= 0,送信状態に送信済を設定する

    cout << "[207]--------------------------------------------- " << endl;
    //unit_test[207] 送信状態->SEND_END,SEND_OK,送信可能データサイズ > 0
    //unit_test[207] test data: 送信状態に送信継続を設定,遷移先ステータスを設定する
    real_send_status.status = SEND_END; //送信状態->SEND_OK
    real_send_status1.send_possible_size = 1u;
    real_send_status1.status = SEND_OK;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

     data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.back().status;

    BOOST_CHECK_EQUAL(event_status, REALSERVER_CONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_OK); //送信データ残サイズ　> 0,送信状態に送信継続を設定する

    cout << "[208]--------------------------------------------- " << endl;
    //unit_test[208] 送信状態->SEND_END,SEND_OK,送信可能データサイズ = 0/送信データ残サイズ　>　0
    //unit_test[208] test data: 送信状態に送信継続を設定,遷移先ステータスを設定する
    real_send_status.status = SEND_END; //送信状態->SEND_END
    real_send_status1.send_rest_size = 1u; //送信データ残サイズ　> 0
    real_send_status1.send_possible_size = 0u;//送信可能データサイズ　= 0
    real_send_status1.status = SEND_OK;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.back().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信データ残サイズ　> 0,送信状態に送信済を設定する

    cout << "[209]--------------------------------------------- " << endl;
    //unit_test[209] 送信状態->SEND_END
    //unit_test[209] test data:遷移先ステータスを設定する
    real_send_status.status = SEND_END; //送信状態->SEND_END
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[210]--------------------------------------------- " << endl;
    //unit_test[210]  送信状態->SEND_END,SEND_OK/送信可能データサイズ　= 0/送信データ残サイズ　= 0
    real_send_status.status = SEND_END; //送信状態->SEND_END
    real_send_status1.status = SEND_OK; //送信状態->SEND_OK
    real_send_status1.send_possible_size  = 0u;
    real_send_status1.send_rest_size = 0u;//送信データ残サイズ　= 0
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.back().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信済を設定する

    cout << "[211]--------------------------------------------- " << endl;
    //unit_test[211] 送信状態->SEND_CONTINUE,SEND_OK/送信データ残サイズ > 0
    //unit_test[211] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    real_send_status.status = SEND_CONTINUE; //送信状態->SEND_CONTINUE
    real_send_status.send_rest_size = 1u; //送信データ残サイズ > 0
    real_send_status.send_possible_size = 0u;//送信可能データサイズ　= 0
    real_send_status1.status = SEND_OK; //送信状態->SEND_OK
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);
    real_receive_data.send_status_list.push_back(real_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_realserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(send_status,SEND_CONTINUE); //送信状態に送信待を設定する
    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する
}

//handle_realserver_send 董作方
void handle_realserver_send_test_thread(){
    boost::asio::ip::tcp::endpoint endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    send_status real_send_status;
    receive_data real_receive_data;
    thread_data_ptr data_pro(new session_thread_data_sessionless);

    cout << "[212]--------------------------------------------- " << endl;
    //unit_test[212] 多スレッドテスト/送信状態->SEND_OK,送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし
    //unit_test[212] test data: 送信状態に送信待を設定
    real_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    real_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    real_send_status.status = SEND_OK;
    real_receive_data.send_status_list.clear();
    real_receive_data.send_status_list.push_back(real_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,real_receive_data));

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    }
    EVENT_TAG event_status = this->handle_realserver_send(boost::this_thread::get_id());
    SEND_STATUS_TAG send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
        BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する
    }
}

//handle_sorryserver_select 馮家純
void handle_sorryserver_select_test(){
    cout << "[213]--------------------------------------------- " << endl;
    //unit_test[213] 該当スレッドの送信可能データを取得する,下りスレッドにsorryserver受信バッファを確保する,送信先endpoint,遷移先ステータス
    boost::thread downt(down_thread_func);
    boost::asio::ip::tcp::endpoint ep;

    //up,down thread map
    thread_data_ptr dataup(new session_thread_data_sessionless);
    thread_data_ptr datadown(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
    this->session_thread_data_map[downt.get_id()] = datadown;
    dataup->thread_id = boost::this_thread::get_id();
    dataup->thread_division = THREAD_DIVISION_UP_STREAM;
    dataup->pair_thread_id = downt.get_id();
    datadown->thread_id = downt.get_id();
    datadown->thread_division = THREAD_DIVISION_DOWN_STREAM;
    datadown->pair_thread_id = boost::this_thread::get_id();
    datadown->receive_data_map.clear();

    //send_status送信可能データ
    send_status st;
    st.status = SEND_OK;
    st.send_possible_size = 2u;
    dataup->receive_data_map[ep].send_status_list.push_back(st);

    EVENT_TAG ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

    //送信先endpointにendpointを設定する
    BOOST_CHECK_EQUAL(dataup->target_endpoint, ep);
    //遷移先ステータス status = CLIENT_RECV
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);

    cout << "[214]--------------------------------------------- " << endl;
    //unit_test[214]     session_thread_data_map中に上りスレッド中にThreadID対応のデータがない
    this->session_thread_data_map[boost::this_thread::get_id()].reset();
    ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[215]--------------------------------------------- " << endl;
    //unit_test[215]     session_thread_data_map中に上りスレッドのデータ無し場合のテスト
    this->session_thread_data_map.erase(boost::this_thread::get_id());
    ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[216]--------------------------------------------- " << endl;
    //unit_test[216]     session_thread_data_map中に上りスレッドと下りスレッドのデータ無し場合のテスト
    this->session_thread_data_map.clear();
    ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[217]--------------------------------------------- " << endl;
    //unit_test[217]     session_thread_data_map中に下りスレッドのデータ無し場合のテスト
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
    ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);
    //送信先endpointにendpointを設定する
    BOOST_CHECK_EQUAL(dataup->target_endpoint, ep);
    //遷移先ステータス status = SORRYSERVER_CONNECT
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);

    cout << "[218]--------------------------------------------- " << endl;
    //unit_test[218] sorryserverが決定済みで、送信可能データがない場合、CLIENT_RECVを返す。
    this->session_thread_data_map.clear();
    boost::asio::ip::tcp::endpoint ep_1;
    thread_data_ptr data_1(new session_thread_data_sessionless);
    data_1->client_endpoint_tcp = ep_1;
    data_1->receive_data_map[ep_1];
    this->session_thread_data_map[boost::this_thread::get_id()] = data_1;
    ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep_1);
    //送信先endpointにendpointを設定する
    BOOST_CHECK_EQUAL(dataup->target_endpoint, ep_1);
    //遷移先ステータス status = CLIENT_RECV
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[219]--------------------------------------------- " << endl;
    //unit_test[219] sorryserverが決定済みで、送信可能データがある場合、SORRYSERVER_CONNECTを返す
    this->session_thread_data_map.clear();
    boost::asio::ip::tcp::endpoint ep_2;
    thread_data_ptr data_2(new session_thread_data_sessionless);
    data_2->client_endpoint_tcp = ep_2;
    //send_status送信可能データ
    send_status s_t;
    s_t.status = SEND_OK;
    s_t.send_possible_size = 2u;
    data_2->receive_data_map[ep_2];
    data_2->receive_data_map[ep_2].send_status_list.push_back(s_t);
    this->session_thread_data_map[boost::this_thread::get_id()] = data_2;
    ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep_2);
    //送信先endpointにendpointを設定する
    BOOST_CHECK_EQUAL(dataup->target_endpoint, ep_1);
    //遷移先ステータス status = SORRYSERVER_CONNECT
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
}


void handle_sorryserver_select_test_thread(){
    cout << "[220]--------------------------------------------- " << endl;
    //unit_test[220] 該当スレッドの送信可能データを取得する,下りスレッドにsorryserver受信バッファを確保する,送信先endpoint,遷移先ステータス
    boost::thread downt(down_thread_func);
    boost::asio::ip::tcp::endpoint ep;

    //up,down thread map
    thread_data_ptr dataup(new session_thread_data_sessionless);
    thread_data_ptr datadown(new session_thread_data_sessionless);
    dataup->thread_id = boost::this_thread::get_id();
    dataup->thread_division = THREAD_DIVISION_UP_STREAM;
    dataup->pair_thread_id = downt.get_id();
    datadown->thread_id = downt.get_id();
    datadown->thread_division = THREAD_DIVISION_DOWN_STREAM;
    datadown->pair_thread_id = boost::this_thread::get_id();
    datadown->receive_data_map.clear();

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
        this->session_thread_data_map[downt.get_id()] = datadown;
    }

    //send_status送信可能データ
    send_status st;
    st.status = SEND_OK;
    st.send_possible_size = 2u;
    dataup->receive_data_map[ep].send_status_list.push_back(st);

    EVENT_TAG ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        //送信先endpointにendpointを設定する
        BOOST_CHECK_EQUAL(dataup->target_endpoint, ep);
        //遷移先ステータス status = CLIENT_RECV
        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
    }
}

//handle_sorryserver_connect 郎希倹
void handle_sorryserver_connect_test(){
    size_t send_buffer_len = USE_BUFFER_SIZE;
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> send_buffer;
    send_status send_status_temp;
    edit_data edit_data_temp;
    boost::thread up_thread(&up_thread_func);
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, up_thread.get_id());
    receive_data receive_data_tmp;
    init_send_status(send_status_temp);
    // receive_dataを作成する
    init_receive_data(receive_data_tmp);
    receive_data_tmp.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_tmp.receive_buffer_rest_size = USE_BUFFER_SIZE;

    //receive_data_mapを作成する
    psession_thread_data->end_flag = END_FLAG_OFF;
    psession_thread_data->last_status = CLIENT_RECV;
    boost::asio::ip::tcp::endpoint endpoint_local = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");

    //session_thread_dataを作成する
    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    psession_thread_data->client_endpoint_tcp = endpoint_local;
    this->forwarded_for = 1;
    memcpy(sorry_uri.c_array(), "/sorry.htm", 10);

    cout << "[221]--------------------------------------------- " << endl;
    // unit_test[221] boost::this_thread::get_id()対応のデータなし
    this->session_thread_data_map.clear();
    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[222]--------------------------------------------- " << endl;
    // unit_test[222] boost::this_thread::get_id()対応のデータsession_thread_dataがNULL
    this->session_thread_data_map.clear();
    thread_data_ptr p;
    this->session_thread_data_map[boost::this_thread::get_id()] = p;
    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;

    cout << "[223]--------------------------------------------- " << endl;
    // unit_test[223] endpoint対応のreceive_dataなし
    psession_thread_data->receive_data_map.clear();
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(endpoint_local);
    receive_data& receive_data_global = it->second;

    cout << "[224]--------------------------------------------- " << endl;
    // unit_test[224] receive_data.receive_buffer＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = NULL;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[225]--------------------------------------------- " << endl;
    // unit_test[225] 送信状態リストが空の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;

    receive_data_global.send_status_list.clear();
    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[226]--------------------------------------------- " << endl;
    // unit_test[226] 送信状態リストの要素の送信状態が１:送信待　２:送信待の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 35;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer,
            "GET /Login.aspx HTTP/1.0\r\nHOST: KKKK\r\n\r\nGET / HTTP/1.0\r\nHOST: KKKKKKKKK\r\n\r\n", 75);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 40u);
    //送信バッファの内容をチェックする。
    int cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 40u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_end_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 35u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 40u);

    cout << "[227]--------------------------------------------- " << endl;
    // unit_test[227] 送信状態リストの要素の送信状態が１:送信待　２:送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = 30;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 30;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 34;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer,
            "GET / HTTP/1.0\r\nHOST: KKKK\r\n\r\nGET / HTTP/1.0\r\nHOST: KKKKKKKKK\r\n\r", 64);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 30u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 30);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_end_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 34u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 30u);

    cout << "[228]--------------------------------------------- " << endl;
    // unit_test[228] 送信状態リストの要素の送信状態が１:送信待の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET / HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 50u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 50);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    const char* forwarded_exist = "POST /index.aspx HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2\r\nContent-Length: 65455\r\n\r\n";
    const char* forwarded_not_exist = "POST /index.aspx HTTP/1.0\r\nContent-Length: 65483\r\n\r\n";
    const int exist_len = 80;
    const int exist_leave_len = 65455;
    const int not_exist_leave_len = 65483;
    const int not_exist_len = 52;

    cout << "[229]--------------------------------------------- " << endl;
    // unit_test[229] 編集区分 = 編集あり、編集データリストが空の場合
    // unit_test[229] ●送信可能データからHTTPヘッダ（X-Forwarded-For）の検索結果 = 該当あり。
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 66);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, 5);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 66,
                     receive_data_global.receive_buffer + 55 ,
                     MAX_BUFFER_SIZE - 66);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[230]--------------------------------------------- " << endl;
    // unit_test[230] 編集区分 = 編集あり、編集データリストが空の場合
    // unit_test[230] ●送信可能データからHTTPヘッダ（X-Forwarded-For）の検索結果 = 該当なし。
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_not_exist, not_exist_len);
    memset(receive_data_global.receive_buffer + not_exist_len, 'x', not_exist_leave_len);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.10.1\r\n", 55);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 55,
                     receive_data_global.receive_buffer + 27, MAX_BUFFER_SIZE - 55);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 28u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 28u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[231]--------------------------------------------- " << endl;
    // unit_test[231] 編集区分 = 編集あり、編集データリストが空でないの場合
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 32;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /sorry.htm HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 69);
    memset(receive_data_global.receive_buffer + 69, 'x', MAX_BUFFER_SIZE - 69);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = MAX_BUFFER_SIZE;
    send_status_temp.send_possible_size = 32;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_edit_data(edit_data_temp);
    edit_data_temp.data = "X-Forwarded-For: 10.10.2.2, 10.10.10.1";
    edit_data_temp.data_size = 38;
    edit_data_temp.insert_posission = MAX_BUFFER_SIZE + 2;
    edit_data_temp.replace_size = 26;
    receive_data_global.send_status_list.begin()->edit_data_list.clear();
    receive_data_global.send_status_list.begin()->edit_data_list.push_back(edit_data_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 44u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer + MAX_BUFFER_SIZE, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 2, "X-Forwarded-For: 10.10.2.2, 10.10.10.1", 38);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 40, receive_data_global.receive_buffer + MAX_BUFFER_SIZE + 30, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE + 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[232]--------------------------------------------- " << endl;
    // unit_test[232] 編集データリストが空の場合
    // unit_test[232] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[232] ●送信バッファ残サイズ　＞　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 32;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /sorry.htm HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 69);
    memset(receive_data_global.receive_buffer + 69, 'x', MAX_BUFFER_SIZE - 69);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = MAX_BUFFER_SIZE;
    send_status_temp.send_possible_size = 32;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_edit_data(edit_data_temp);
    edit_data_temp.data = "X-Forwarded-For: 10.10.2.2, 10.10.10.1";
    edit_data_temp.data_size = 38;
    edit_data_temp.insert_posission = MAX_BUFFER_SIZE + 2;
    edit_data_temp.replace_size = 26;
    receive_data_global.send_status_list.begin()->edit_data_list.clear();
    receive_data_global.send_status_list.begin()->edit_data_list.push_back(edit_data_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 44u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer + MAX_BUFFER_SIZE, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 2, "X-Forwarded-For: 10.10.2.2, 10.10.10.1", 38);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 40, receive_data_global.receive_buffer + MAX_BUFFER_SIZE + 30, 2);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE + 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[233]--------------------------------------------- " << endl;
    // unit_test[233] 編集データリストが空の場合
    // unit_test[233] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[233] ●送信バッファ残サイズ　＝　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE - 11;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /index.aspx HTTP/1.0\r\nHOST:123456789012345678901\r\nCookie: monkey=", 70);
    memset(receive_data_global.receive_buffer + 70, 'x', MAX_BUFFER_SIZE - 113);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 43, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE - 11;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST ", 5);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 5, "/sorry.htm", 10);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 15, receive_data_global.receive_buffer + 16, MAX_BUFFER_SIZE - 57);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 42, "X-Forwarded-For: 10.10.2.2, 10.10.10.1", 38);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 4, "\r\n\r\n", 4);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[234]--------------------------------------------- " << endl;
    // unit_test[234] 編集データリストが空の場合
    // unit_test[234] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[234] ●送信バッファ残サイズ　＜　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 66);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, 5);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 66,
                     receive_data_global.receive_buffer + 55 ,
                     MAX_BUFFER_SIZE - 66);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[235]--------------------------------------------- " << endl;
    // unit_test[235] 編集データリストが空の場合
    // unit_test[235] ●送信バッファ残サイズ　＝　0
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE - 9;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /index.aspx HTTP/1.0\r\nHOST:12345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 101);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 41, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE - 9;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm", 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 15, receive_data_global.receive_buffer + 16, MAX_BUFFER_SIZE - 55);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 40, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[236]--------------------------------------------- " << endl;
    // unit_test[236] 編集データリストが空の場合
    // unit_test[236] ●送信可能データサイズ　＝　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 60;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET /index.aspx HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 60);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 88u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(),
                     "GET /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.10.1\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 88);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[237]--------------------------------------------- " << endl;
    // unit_test[237] 編集データリストが空でない場合
    // unit_test[237] ●送信バッファ残サイズ　＞　編集データ設定位置　－　送信済サイズ
    // unit_test[237] ●送信バッファ残サイズ　＞　編集データサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 60;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET /index.aspx HTTP/1.0\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 60);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 88u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(),
                     "GET /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.10.1\r\nHOST: KKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 88);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[238]--------------------------------------------- " << endl;
    // unit_test[238] 編集データリストが空でない場合
    // unit_test[238] ●送信バッファ残サイズ　＞　編集データ設定位置　－　送信済サイズ
    // unit_test[238] ●送信バッファ残サイズ　＝　編集データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE - 9;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /index.aspx HTTP/1.0\r\nHOST:12345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 101);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 41, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE - 9;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm", 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 15, receive_data_global.receive_buffer + 16, MAX_BUFFER_SIZE - 55);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + MAX_BUFFER_SIZE - 40, "X-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 40);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[239]--------------------------------------------- " << endl;
    // unit_test[239] 編集データリストが空でない場合
    // unit_test[239] ●送信バッファ残サイズ　＞　編集データ設定位置　－　送信済サイズ
    // unit_test[239] ●送信バッファ残サイズ　＜　編集データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 29;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /index.aspx HTTP/1.0\r\nHOST:12345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 63);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 3, "\r\nX", 3);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "-Forwarded-For: 10.10.2.2\r\n\r\n", 29);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 29;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE - 2u);

    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm", 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 15, receive_data_global.receive_buffer + 16, MAX_BUFFER_SIZE - 17);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data,
            "X-Forwarded-For: 10.10.2.2, 10.10.10.1");
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data_size, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->insert_posission,
            MAX_BUFFER_SIZE - 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->replace_size, 26u);

    cout << "[240]--------------------------------------------- " << endl;
    // unit_test[240] 編集データリストが空でない場合
    // unit_test[240] ●送信バッファ残サイズ　＝　編集データ設定位置　－　送信済サイズ
    // unit_test[240] ●送信バッファ残サイズ　＜　編集データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 31;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /index.aspx HTTP/1.0\r\nHOST:12345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 61);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE - 1, "\r", 1);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 31);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 31;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm", 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 15, receive_data_global.receive_buffer + 16, MAX_BUFFER_SIZE - 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE + 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data,
            "X-Forwarded-For: 10.10.2.2, 10.10.10.1");
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data_size, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->insert_posission,
            MAX_BUFFER_SIZE + 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->replace_size, 26u);

    cout << "[241]--------------------------------------------- " << endl;
    // unit_test[241] 編集データリストが空でない場合
    // unit_test[241] ●送信バッファ残サイズ　＜　編集データ設定位置　－　送信済サイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 32;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    memcpy(receive_data_global.receive_buffer, "POST /index.aspx HTTP/1.0\r\nHOST:12345678901\r\nCookie: monkey=", 60);
    memset(receive_data_global.receive_buffer + 60, 'x', MAX_BUFFER_SIZE - 60);
    memcpy(receive_data_global.receive_buffer + MAX_BUFFER_SIZE, "\r\nX-Forwarded-For: 10.10.2.2\r\n\r\n", 32);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 32;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    receive_data_global.send_status_list.push_back(send_status_temp);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm", 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(send_buffer.c_array() + 15, receive_data_global.receive_buffer + 16, MAX_BUFFER_SIZE - 15);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE + 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 31u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_EDIT);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data,
            "X-Forwarded-For: 10.10.2.2, 10.10.10.1");
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->data_size, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->insert_posission,
            MAX_BUFFER_SIZE + 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.begin()->replace_size, 26u);

    cout << "[242]--------------------------------------------- " << endl;
    // unit_test[242] 編集区分 = 編集なし
    // unit_test[242] ●送信バッファサイズ　＞　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "KKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 50u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), 50);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[243]--------------------------------------------- " << endl;
    // unit_test[243] 編集区分 = 編集なし
    // unit_test[243] ●送信バッファサイズ　＝　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memset(receive_data_global.receive_buffer, 'x', MAX_BUFFER_SIZE);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[244]--------------------------------------------- " << endl;
    // unit_test[244] 編集区分 = 編集なし
    // unit_test[244] ●送信バッファサイズ　＜　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE + 1;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_NO_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE + 1;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memset(receive_data_global.receive_buffer, 'x', MAX_BUFFER_SIZE + 1);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(receive_data_global.receive_buffer, send_buffer.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[245]--------------------------------------------- " << endl;
    // unit_test[245] forwarded_forを設定するの場合、編集区分が１である。
    this->forwarded_for = 1;
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET /index.aspx HTTP/1.0\r\nHOST: KKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 78u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(),
                "GET /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.10.1\r\nHOST: KKKKKKKKKKKKKK\r\n\r\n", 78);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);

    cout << "[246]--------------------------------------------- " << endl;
    // unit_test[246] forwarded_forを設定しないの場合、編集区分が１である。
    this->forwarded_for = 0;
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, "GET /index.aspx HTTP/1.0\r\nHOST: KKKKKKKKKKKKKK\r\n\r\n", 50);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
    BOOST_CHECK_EQUAL(send_buffer_len, 49u);
    //送信バッファの内容をチェックする。
    cmp_ret = memcmp(send_buffer.c_array(), "GET /sorry.htm HTTP/1.0\r\nHOST: KKKKKKKKKKKKKK\r\n\r\n", 49);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
}

//handle_sorryserver_connect_test_thread
void handle_sorryserver_connect_test_thread(){
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> send_buffer;
    send_status send_status_temp;
    receive_data receive_data_tmp;
    boost::thread down_thread(&down_thread_func);
    boost::asio::ip::tcp::endpoint endpoint_local = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    psession_thread_data->client_endpoint_tcp = endpoint_local;

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;
        this->forwarded_for = 1;
        memcpy(sorry_uri.c_array(), "/sorry.htm", 10);
    }

    init_receive_data(receive_data_tmp);

    psession_thread_data->receive_data_map[endpoint_local] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(endpoint_local);
    receive_data& receive_data_global = it->second;

    const char* forwarded_exist = "POST /index.aspx HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2\r\nContent-Length: 65455\r\n\r\n";
    const int exist_len = 80;
    const int exist_leave_len = 65455;

    cout << "[247]--------------------------------------------- " << endl;
    // unit_test[247] 編集データリストが空の場合
    // unit_test[247] ●送信バッファ残サイズ　＞　0　かつ　送信可能データサイズ　＞　0
    // unit_test[247] ●送信バッファ残サイズ　＜　送信可能データサイズ
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    size_t send_buffer_len = 0;
    memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_OK;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.edit_division = EDIT_DIVISION_EDIT;
    send_status_temp.send_possible_size = MAX_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(receive_data_global.receive_buffer, forwarded_exist, exist_len);
    memset(receive_data_global.receive_buffer + exist_len, 'x', exist_leave_len);

    ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);
        BOOST_CHECK_EQUAL(send_buffer_len, MAX_BUFFER_SIZE);
        //送信バッファの内容をチェックする。
        int cmp_ret = memcmp(send_buffer.c_array(), "POST /sorry.htm HTTP/1.0\r\nX-Forwarded-For: 10.10.2.2, 10.10.10.1\r\n", 66);
        BOOST_CHECK_EQUAL(cmp_ret, 0);
        cmp_ret = memcmp(send_buffer.c_array(), receive_data_global.receive_buffer, 5);
        BOOST_CHECK_EQUAL(cmp_ret, 0);
        cmp_ret = memcmp(send_buffer.c_array() + 66,
                         receive_data_global.receive_buffer + 55 ,
                         MAX_BUFFER_SIZE - 66);
        BOOST_CHECK_EQUAL(cmp_ret, 0);

        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.begin()->edit_data_list.size(), 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_end_size, MAX_BUFFER_SIZE - 11u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->edit_division, EDIT_DIVISION_NO_EDIT);
    }
}

//handle_sorryserver_connection_fail 董作方
void handle_sorryserver_connection_fail_test() {
    boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    thread_data_ptr data_pro(new session_thread_data_sessionless);

    cout << "[248]--------------------------------------------- " << endl;
    //unit_test[248] session_thread_data_map中にthread_id無し
    EVENT_TAG ret =    this->handle_sorryserver_connection_fail(boost::this_thread::get_id(), ep_tcp);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[249]--------------------------------------------- " << endl;
    //unit_test[249] session_thread_data_map中にsession_thread_data無し
    thread_data_ptr thread_data1 ;
    this->session_thread_data_map[boost::this_thread::get_id()] = thread_data1;
    ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(), ep_tcp);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[250]--------------------------------------------- " << endl;
    //unit_test[250] handle_sorryserver_connection_failメソッドのテスト
    //unit_test[250] test data:end_flag = on
    data_pro->end_flag = END_FLAG_OFF;// 終了フラグを設定する
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(),ep_tcp);

    BOOST_CHECK_EQUAL(data_pro->end_flag, END_FLAG_ON);
    BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT );
}

//handle_sorryserver_connection_fail 董作方
void handle_sorryserver_connection_fail_test_thread() {
    boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    thread_data_ptr data_pro(new session_thread_data_sessionless);

    cout << "[251]--------------------------------------------- " << endl;
    //unit_test[251] 多スレッドテスト/handle_sorryserver_connection_failメソッドの多スレッドテスト
    //unit_test[251] test data:end_flag = on
    data_pro->end_flag = END_FLAG_OFF;// 終了フラグを設定する

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    }
    EVENT_TAG ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(),ep_tcp);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(data_pro->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT );
    }
}

//handle_sorryserver_send 郎希倹
void handle_sorryserver_send_test() {
    boost::asio::ip::tcp::endpoint endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");;
    send_status sorr_send_status,sorr_send_status1;
    receive_data sorr_receive_data;
    thread_data_ptr data_pro(new session_thread_data_sessionless);

    cout << "[252]--------------------------------------------- " << endl;
    //unit_test[252] session_thread_data_map中にthread_id無し
    EVENT_TAG event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[253]--------------------------------------------- " << endl;
    //unit_test[253] session_thread_data_map中にsession_thread_data_sessionless無し
    thread_data_ptr data_pro1;
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro1;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[254]--------------------------------------------- " << endl;
    //unit_test[254] receive_data_map中にendpoint無し
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[255]--------------------------------------------- " << endl;
    //unit_test[255] send_status_list中にsend_status無し
    sorr_receive_data.send_status_list.clear();
    data_pro->receive_data_map[endpoint] = sorr_receive_data;
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;

    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[256]--------------------------------------------- " << endl;
    //unit_test[256] 送信状態->SEND_OK,送信可能データサイズ　>　0/送信可能データあり
    //unit_test[256] test data: 送信状態に送信待を設定,送信データオフセットに送信済データサイズを加算する,送信済データサイズに0を設定する,遷移先ステータスを設定する
    sorr_send_status.send_possible_size = 1u;//送信可能データサイズ　>　0
    sorr_send_status.send_offset = 0u;
    sorr_send_status.send_end_size = 1u;
    sorr_send_status.status = SEND_OK; //送信状態->SEND_OK
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    SEND_STATUS_TAG send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;
    size_t offset = data_pro->receive_data_map[endpoint].send_status_list.front().send_offset;
    size_t end_size = data_pro->receive_data_map[endpoint].send_status_list.front().send_end_size;

    BOOST_CHECK_EQUAL(event_status, SORRYSERVER_CONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_OK); //送信状態に送信待を設定する
    BOOST_CHECK_EQUAL(offset,1u); //送信データオフセットに送信済データサイズを加算する
    BOOST_CHECK_EQUAL(end_size,0u); //送信済データサイズに0を設定する

    cout << "[257]--------------------------------------------- " << endl;
    //unit_test[257] 送信状態->SEND_NG
    //unit_test[257] test data: 遷移先ステータスを設定する
    sorr_send_status.status = SEND_NG; //送信状態->SEND_NG
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[258]--------------------------------------------- " << endl;
    //unit_test[258] 送信状態->SEND_CONTINUE
    //unit_test[258] test data: 遷移先ステータスを設定する
    sorr_send_status.status = SEND_CONTINUE; //送信状態->SEND_CONTINUE
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[259]--------------------------------------------- " << endl;
    //unit_test[259] 送信状態->SEND_OK,送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし
    //unit_test[259] test data: 送信状態に送信待を設定
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    sorr_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    sorr_send_status.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する

    cout << "[260]--------------------------------------------- " << endl;
    //unit_test[260] 送信状態->SNED_NG,SEND_OK,/送信データ残サイズ　＞　0
    //unit_test[260] test data: 送信状態に送信待を設定
    sorr_send_status.status = SEND_NG;
    sorr_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    sorr_send_status1.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[261]--------------------------------------------- " << endl;
    //unit_test[261] 送信状態->SEND_OK,送信可能データサイズ　=　0/送信データ残サイズ　=　0/送信可能データなし
    //unit_test[261] test data: 送信状態に送信待を設定
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    sorr_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    sorr_send_status.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する

    cout << "[262]--------------------------------------------- " << endl;
    //unit_test[262] 送信状態->SEND_OK,SEND_OK/送信可能データサイズ = 0/送信データ残サイズ　> 0
    //unit_test[262] test data: 遷移先ステータスを設定する
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ　=　0
    sorr_send_status.status = SEND_OK;
    sorr_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    sorr_send_status1.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[263]--------------------------------------------- " << endl;
    //unit_test[263] 送信状態->SEND_OK,SEND_OK/送信可能データサイズ = 0/送信データ残サイズ　= 0/送信可能データあり
    //unit_test[263] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ　=　0
    sorr_send_status.status = SEND_OK;
    sorr_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    sorr_send_status1.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, SORRYSERVER_CONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信可能データサイズ　=　0,送信データ残サイズ　= 0,送信状態に送信待を設定する

    cout << "[264]--------------------------------------------- " << endl;
    //unit_test[264] 送信状態->SEND_OK,SEND_NG/送信可能データサイズ = 0/送信データ残サイズ　> 0
    //unit_test[264] test data: 遷移先ステータスを設定する
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ　=　0
    sorr_send_status.status = SEND_OK;
    sorr_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    sorr_send_status1.status = SEND_NG;//送信状態->SEND_NG
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[265]--------------------------------------------- " << endl;
    //unit_test[265] 送信状態->SEND_OK,SEND_NG/送信可能データサイズ = 0/送信データ残サイズ　= 0/送信可能データあり
    //unit_test[265] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ　=　0
    sorr_send_status.status = SEND_OK;
    sorr_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    sorr_send_status1.status = SEND_NG;//送信状態->SEND_NG
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信可能データサイズ　=　0,送信データ残サイズ　= 0,送信状態に送信待を設定する

    cout << "[266]--------------------------------------------- " << endl;
    //unit_test[266] 送信状態->SEND_END,SEND_OK/送信可能データサイズ > 0
    //unit_test[266] test data: 遷移先ステータスを設定する
    sorr_send_status.status = SEND_END;
    sorr_send_status1.send_possible_size = 1u;//送信可能データサイズ　>　0
    sorr_send_status1.status = SEND_OK;//送信状態->SEND_CONTINUE
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, SORRYSERVER_CONNECT); //遷移先ステータスを設定する

    cout << "[267]--------------------------------------------- " << endl;
    //unit_test[267] 送信状態->SEND_END,SEND_OK/送信可能データサイズ = 0
    //unit_test[267] test data: 遷移先ステータスを設定する
    sorr_send_status.status = SEND_END;
    sorr_send_status1.send_rest_size = 1u;//送信データ残サイズ　> 0
    sorr_send_status1.send_possible_size = 0u;//送信可能データサイズ　=　0
    sorr_send_status1.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する

    cout << "[268]--------------------------------------------- " << endl;
    //unit_test[268] 送信状態->SEND_END
    //unit_test[268] test data: 遷移先ステータスを設定する
    sorr_send_status.status = SEND_END;
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[269]--------------------------------------------- " << endl;
    //unit_test[269]  送信状態->SEND_END,SEND_OK/送信可能データサイズ = 0/送信データ残サイズ　= 0
    sorr_send_status.status = SEND_END; //送信状態->SEND_END
    sorr_send_status1.status = SEND_OK; //送信状態->SEND_OK
    sorr_send_status1.send_rest_size = 0u;//送信データ残サイズ　= 0
    sorr_send_status1.send_possible_size = 0u; //送信可能データサイズ = 0
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status,  CLIENT_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信済を設定する

    cout << "[270]--------------------------------------------- " << endl;
    //unit_test[270] 送信状態->SEND_CONTINUE,SEND_OK/送信可能データサイズ > 0
    //unit_test[270] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    sorr_send_status.status = SEND_CONTINUE; //送信状態->SEND_CONTINUE
    sorr_send_status.send_possible_size = 1u;//送信可能データサイズ　= 1
    sorr_send_status.send_offset = 0u;
    sorr_send_status.send_end_size = 1u;
    sorr_send_status1.status = SEND_OK; //送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);
    sorr_receive_data.send_status_list.push_back(sorr_send_status1);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_sorryserver_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status,FINALIZE); //送信状態に送信待を設定する
}

//handle_sorryserver_send 郎希倹
void handle_sorryserver_send_test_thread() {
    boost::asio::ip::tcp::endpoint endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");;
    send_status sorr_send_status;
    receive_data sorr_receive_data;
    thread_data_ptr data_pro(new session_thread_data_sessionless);

    cout << "[271]--------------------------------------------- " << endl;
    //unit_test[271] 多スレッドテスト/送信状態->SEND_OK,送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし
    //unit_test[271] test data: 送信状態に送信待を設定
    sorr_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    sorr_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    sorr_send_status.status = SEND_OK;//送信状態->SEND_OK
    sorr_receive_data.send_status_list.clear();
    sorr_receive_data.send_status_list.push_back(sorr_send_status);

    data_pro->client_endpoint_tcp = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,sorr_receive_data));
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    }
    EVENT_TAG event_status = this->handle_sorryserver_send(boost::this_thread::get_id());
    SEND_STATUS_TAG send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(event_status, CLIENT_RECV); //遷移先ステータスを設定する
        BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する
    }
}

//handle_realserver_recv(tcp) 郎希倹
void handle_realserver_recv_tcp_test() {
    size_t response_len = USE_BUFFER_SIZE;;
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> response;
    send_status send_status_temp;
    boost::thread up_thread(&up_thread_func);
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, up_thread.get_id());
    receive_data receive_data_tmp;

    init_send_status(send_status_temp);
    // receive_dataを作成する
    init_receive_data(receive_data_tmp);
    receive_data_tmp.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_tmp.receive_buffer_rest_size = USE_BUFFER_SIZE;

    //receive_data_mapを作成する
    psession_thread_data->end_flag = END_FLAG_OFF;
    psession_thread_data->last_status = REALSERVER_RECV;
    boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");
    psession_thread_data->target_endpoint = rs_endpoint;

    //session_thread_dataを作成する
    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[rs_endpoint] = receive_data_tmp;
    this->forwarded_for = 1;

    cout << "[272]--------------------------------------------- " << endl;
    // unit_test[272] boost::this_thread::get_id()対応のデータなし
    this->session_thread_data_map.clear();
    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[273]--------------------------------------------- " << endl;
    // unit_test[273] boost::this_thread::get_id()対応のデータsession_thread_dataがNULL
    this->session_thread_data_map.clear();
    thread_data_ptr p;
    this->session_thread_data_map[boost::this_thread::get_id()] = p;
    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;

    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[rs_endpoint] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(rs_endpoint);
    receive_data& receive_data_global = it->second;

    cout << "[274]--------------------------------------------- " << endl;
    // unit_test[274] receive_data.receive_buffer＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = NULL;
    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[275]--------------------------------------------- " << endl;
    // unit_test[275] receive_data.receive_buffer1＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 20;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer1 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memset(response.c_array(), 'n', 80);
    response_len = 80;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[276]--------------------------------------------- " << endl;
    // unit_test[276] receive_data.receive_buffer2＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 20;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memset(response.c_array(), 'n', 80);
    response_len = 80;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[277]--------------------------------------------- " << endl;
    // unit_test[277] new失敗の場合
    receive_data_global.receive_buffer_max_size = 0;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = NULL;
    receive_data_global.receive_buffer2 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n01234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    this->getloglevel = &stb_getloglevel_is_none;
    new_install();
    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
    new_uninstall();
    this->getloglevel = &stb_getloglevel;
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[278]--------------------------------------------- " << endl;
    // unit_test[278] レスポンスサイズが0である
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, 0);
    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);

    cout << "[279]--------------------------------------------- " << endl;
    // unit_test[279] レスポンスサイズがUINT_MAXである レスポンス実際にサイズがMAX_BUFFER_SIZEである
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = MAX_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memset(response.c_array(), 'n', MAX_BUFFER_SIZE);
    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, UINT_MAX);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[280]--------------------------------------------- " << endl;
    // unit_test[280] レスポンスサイズがMAX_BUFFER_SIZEである レスポンス実際にサイズがMAX_BUFFER_SIZEである
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = MAX_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\n", 17);
    memset(response.c_array() + 17, 'n', MAX_BUFFER_SIZE - 17);

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE);

    // unit_test_part1 begin:
    send_status status_END;
    init_send_status(status_END);
    status_END.status = SEND_END;
    status_END.send_offset = 0;
    status_END.send_end_size = 20;

    send_status status_OK;
    init_send_status(status_OK);
    status_OK.status = SEND_OK;
    status_OK.send_offset = 0;
    status_OK.send_possible_size = 20;

    send_status status_NG;
    init_send_status(status_NG);
    status_NG.status = SEND_NG;
    status_NG.send_offset = 0;
    status_NG.unsend_size = 20;

    send_status status_CONTINUE;
    init_send_status(status_CONTINUE);
    status_CONTINUE.status = SEND_CONTINUE;
    status_CONTINUE.send_offset = 0;
    status_CONTINUE.send_end_size = 20;
    status_CONTINUE.send_rest_size = 20;

    send_status* send_status_NG[27][3]
    = {
        // unit_test 入力の送信状態リストは下記の通り：（異常系）
        // unit_test[281] 送信状態リストの要素の送信状態が1：送信待の場合
        {&status_OK,NULL,NULL},
        // unit_test[282] 送信状態リストの要素の送信状態が1：送信済 2：送信待の場合
        {&status_END,&status_OK,NULL},
        // unit_test[283] 送信状態リストの要素の送信状態が1：送信継続 2：送信継続の場合
        {&status_CONTINUE,&status_CONTINUE,NULL},
        // unit_test[284] 送信状態リストの要素の送信状態が1：送信継続 2：送信不可の場合
        {&status_CONTINUE,&status_NG,NULL},
        // unit_test[285] 送信状態リストの要素の送信状態が1：送信継続 2：送信済の場合
        {&status_CONTINUE,&status_END,NULL},
        // unit_test[286] 送信状態リストの要素の送信状態が1：送信継続 2：送信待の場合
        {&status_CONTINUE,&status_OK,NULL},
        // unit_test[287] 送信状態リストの要素の送信状態が1：送信不可 2：送信継続の場合
        {&status_NG,&status_CONTINUE,NULL},
        // unit_test[288] 送信状態リストの要素の送信状態が1：送信不可 2：送信不可の場合
        {&status_NG,&status_NG,NULL},
        // unit_test[289] 送信状態リストの要素の送信状態が1：送信不可 2：送信済の場合
        {&status_NG,&status_END,NULL},
        // unit_test[290] 送信状態リストの要素の送信状態が1：送信不可 2：送信待の場合
        {&status_NG,&status_OK,NULL},
        // unit_test[291] 送信状態リストの要素の送信状態が1：送信待 2：送信継続の場合
        {&status_OK,&status_CONTINUE,NULL},
        // unit_test[292] 送信状態リストの要素の送信状態が1：送信待 2：送信不可の場合
        {&status_OK,&status_NG,NULL},
        // unit_test[293] 送信状態リストの要素の送信状態が1：送信待 2：送信済の場合
        {&status_OK,&status_END,NULL},
        // unit_test[294] 送信状態リストの要素の送信状態が1：送信待 2：送信待の場合
        {&status_OK,&status_OK,NULL},
        // unit_test[295] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信継続の場合
        {&status_END,&status_CONTINUE,&status_CONTINUE},
        // unit_test[296] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信不可の場合
        {&status_END,&status_CONTINUE,&status_NG},
        // unit_test[297] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信済の場合
        {&status_END,&status_CONTINUE,&status_END},
        // unit_test[298] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信待の場合
        {&status_END,&status_CONTINUE,&status_OK},
        // unit_test[299] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信継続の場合
        {&status_END,&status_NG,&status_CONTINUE},
        // unit_test[300] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信不可の場合
        {&status_END,&status_NG,&status_NG},
        // unit_test[301] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信済の場合
        {&status_END,&status_NG,&status_END},
        // unit_test[302] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信待の場合
        {&status_END,&status_NG,&status_OK},
        // unit_test[303] 送信状態リストの要素の送信状態が1：送信済 2：送信済 ３：送信待の場合
        {&status_END,&status_END,&status_OK},
        // unit_test[304] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信継続の場合
        {&status_END,&status_OK,&status_CONTINUE},
        // unit_test[305] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信不可の場合
        {&status_END,&status_OK,&status_NG},
        // unit_test[306] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信済の場合
        {&status_END,&status_OK,&status_END},
        // unit_test[307] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信待の場合
        {&status_END,&status_OK,&status_OK},
    };

    int i;
    int j;

    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    memset(receive_data_global.receive_buffer, 'n', USE_BUFFER_SIZE);
    memset(response.c_array(), 'm', USE_BUFFER_SIZE);
    for (i = 0; i < 27; i++)
    {
        receive_data_global.send_status_list.clear();

        for (j = 0; j < 3; j++)
        {
            if(send_status_NG[i][j] == NULL)
            {
             break;
            }

            receive_data_global.send_status_list.push_back(*(send_status_NG[i][j]));
        }

        ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);
        std::cout << "[" << 281 + i << "]------------------------------------------" << std::endl;
        BOOST_CHECK_EQUAL(ret, FINALIZE);
    }

    cout << "[308]--------------------------------------------- " << endl;
    // unit_test[308] 送信状態リストの要素の送信状態が1：送信済 2：送信済 3：送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 20;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 10;
    send_status_temp.send_rest_size = 11;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "12345678901HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 39u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 11u);

    cout << "[309]--------------------------------------------- " << endl;
    // unit_test[309] 送信状態リストの要素の送信状態が1：送信済 2：送信済 3：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 20;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 40;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 2", 10);

    memcpy(response.c_array(), "00 OK\r\nCookie: monkey=1234567890123456789012345678", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[310]--------------------------------------------- " << endl;
    // unit_test[310] 送信状態リストの要素の送信状態が1：送信済 2：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 40;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 2", 10);

    memcpy(response.c_array(), "00 OK\r\nCookie: monkey=1234567890123456789012345678", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[311]--------------------------------------------- " << endl;
    // unit_test[311] 送信状態リストの要素の送信状態が1：送信済 2：送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 10;
    send_status_temp.send_rest_size = 11;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "12345678901HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 39u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 11u);

    cout << "[312]--------------------------------------------- " << endl;
    // unit_test[312] 送信状態リストの要素の送信状態が１:送信済　２:送信済の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[313]--------------------------------------------- " << endl;
    // unit_test[313] 送信状態リストの要素の送信状態が1：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 10;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 10, "HTTP/1.1 2", 10);

    memcpy(response.c_array(), "00 OK\r\nContent-Length: 21\r\n\r\n012345678901234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[314]--------------------------------------------- " << endl;
    // unit_test[314] 送信状態リストの要素の送信状態が１:送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 20;

    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890HTTP/1.1 200 OK\r\nContent-Lengt", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 30u);

    cout << "[315]--------------------------------------------- " << endl;
    // unit_test[315] 送信状態リストの要素の送信状態が１:送信済の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 80;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[316]--------------------------------------------- " << endl;
    // unit_test[316] 送信状態リストの要素数が０である場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    // unit_test_part2 begin:
    cout << "[317]--------------------------------------------- " << endl;
    // unit_test[317] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[317]  ●データバッファMAXサイズ　<　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[317]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 80;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=123456789012345678901"
        "23456789012345678901234567890123456789012345678901234567890", 112);
    response_len = 112;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, MAX_BUFFER_SIZE - 112);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);

    //メモリの内容をチェックする
    int cmp_ret = memcmp(response.c_array(), receive_data_global.receive_buffer, 112u);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 112u);

    cout << "[318]--------------------------------------------- " << endl;
    // unit_test[318] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[318]  ●データバッファMAXサイズ　<　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[318]  ●データバッファ上限値　＜　未送信データサイズ　＋　レスポンスデータサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 80;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 10;
    send_status_temp.unsend_size = MAX_SESSIONLESS_MODULE_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=123456789012345678901"
        "23456789012345678901234567890123456789012345678901234567890", 112);
    response_len = 112;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[319]--------------------------------------------- " << endl;
    // unit_test[319] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[319]  ●データバッファMAXサイズ　<　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[319]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=", 12);
    memset(response.c_array(), 'x', MAX_BUFFER_SIZE - 12);
    response_len = MAX_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE + 20);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, MAX_BUFFER_SIZE + 20);

    cout << "[320]--------------------------------------------- " << endl;
    // unit_test[320] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[320]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　>　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[320]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890", 72);
    response_len = 72;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);
    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 72);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[321]--------------------------------------------- " << endl;
    // unit_test[321] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[321]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　=　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[321]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890123456789012345678", 90);
    response_len = 90;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);
    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 90);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[322]--------------------------------------------- " << endl;
    // unit_test[322] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[322]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　>　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[322]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890", 52);
    response_len = 52;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 52);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[323]--------------------------------------------- " << endl;
    // unit_test[323] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[323]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　=　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[323]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890123456789012345678", 70);
    response_len = 70;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[324]--------------------------------------------- " << endl;
    // unit_test[324] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[324]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　>　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[324]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890", 72);
    response_len = 72;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 72);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[325]--------------------------------------------- " << endl;
    // unit_test[325] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[325]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　=　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[325]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(),
            "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890123456789012345678", 90);
    response_len = 90;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 90);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[326]--------------------------------------------- " << endl;
    // unit_test[326] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[326]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　>　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[326]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890", 52);
    response_len = 52;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 52);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[327]--------------------------------------------- " << endl;
    // unit_test[327] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[327]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　=　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[327]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890123456789012345678", 70);
    response_len = 70;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[328]--------------------------------------------- " << endl;
    // unit_test[328] データバッファ残サイズ　>　レスポンスデータサイズ
    // unit_test[328]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=123456789012345678901234567890", 62);
    response_len = 62;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 8u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 62);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 62u);

    cout << "[329]--------------------------------------------- " << endl;
    // unit_test[329] データバッファ残サイズ　=　レスポンスデータサイズ
    // unit_test[329]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=12345678901234567890123456789012345678", 70);
    response_len = 70;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 70u);

    // unit_test_part3 begin:
    cout << "[330]--------------------------------------------- " << endl;
    // unit_test[330] 送信状態リストの要素の送信状態が送信継続
    // unit_test[330]  ●送信データ残サイズ　<　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890HTTP/1.1 200 OK\r\nContent-Lengt", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 30u);

    cout << "[331]--------------------------------------------- " << endl;
    // unit_test[331] 送信状態リストの要素の送信状態が送信継続
    // unit_test[331]  ●送信データ残サイズ　=　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 50\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890123456789012345678901234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[332]--------------------------------------------- " << endl;
    // unit_test[332] 送信状態リストの要素の送信状態が送信継続
    // unit_test[332]  ●送信データ残サイズ　>　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 80;
    receive_data_global.send_status_list.push_back(send_status_temp);
    // 未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 80\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890123456789012345678901234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[333]--------------------------------------------- " << endl;
    // unit_test[333] 送信状態が送信不可の場合
    // unit_test[333]  ●HTTPバージョンの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.9 200 OK\r\n4567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[334]--------------------------------------------- " << endl;
    // unit_test[334] 送信状態が送信不可の場合
    // unit_test[334]  ●HTTPバージョンの妥当性をチェックして、チェック不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "OST / HTT", 10);
    response_len = 10;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 11u);

    cout << "[335]--------------------------------------------- " << endl;
    // unit_test[335] 送信状態が送信不可の場合
    // unit_test[335]  ●HTTPヘッダの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.0 200 OK\r\nCookie: monkey=1111111111111111111", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 51u);

    cout << "[336]--------------------------------------------- " << endl;
    // unit_test[336] 送信状態が送信不可の場合
    // unit_test[336]  ●HTTPヘッダ（ContentLength）があり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.0 200 OK\r\nContent-Length: 15\r\n\r\n123456789012", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 3u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[337]--------------------------------------------- " << endl;
    // unit_test[337] 送信状態が送信不可の場合
    // unit_test[337]  ●HTTPヘッダ（ContentLength）なし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.0 200 OK\r\nCookie: monkey=111111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[338]--------------------------------------------- " << endl;
    // unit_test[338] 送信状態が送信不可の場合
    // unit_test[338]  ●送信データ残サイズ　>　未送信データサイズ　＋　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123456789012345678901\r\n\r\nGET", 40);
    response_len = 40;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 30u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 3u);

    cout << "[339]--------------------------------------------- " << endl;
    // unit_test[339] 送信状態が送信不可の場合
    // unit_test[339]  ●送信データ残サイズ　<　未送信データサイズ　＋　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123\r\nContent-Length: 100\r\n\r\n0123456789", 50);
    response_len = 50;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 90u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[340]--------------------------------------------- " << endl;
    // unit_test[340] 送信状態が送信不可の場合
    // unit_test[340]  ●送信データ残サイズ　=　未送信データサイズ　＋　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123\r\nContent-Length: 11\r\n\r\n01234567890", 50);
    response_len = 50;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[341]--------------------------------------------- " << endl;
    // unit_test[341] レスポンスデータ残サイズ　<　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123456789012345678901\r\n\r\nGET", 40);
    response_len = 40;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 30u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 3u);

    cout << "[342]--------------------------------------------- " << endl;
    // unit_test[342] レスポンスデータ残サイズ　=　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123\r\nContent-Length: 11\r\n\r\n01234567890", 50);
    response_len = 50;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    // unit_test_part4 begin:
    cout << "[343]--------------------------------------------- " << endl;
    // unit_test[343] レスポンスデータ残サイズ　＞　0
    // unit_test[343]  ●HTTPバージョンの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 10\r\n\r\n1234567890G", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 49u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 49u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 1u);

    cout << "[344]--------------------------------------------- " << endl;
    // unit_test[344] レスポンスデータ残サイズ　＞　0
    // unit_test[344]  ●HTTPバージョンの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/2.2 200 OK\r\n567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[345]--------------------------------------------- " << endl;
    // unit_test[345] レスポンスデータ残サイズ　＞　0
    // unit_test[345]  ●HTTPヘッダの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=111111111111111111", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);

    cout << "[346]--------------------------------------------- " << endl;
    // unit_test[346] レスポンスデータ残サイズ　＞　0
    // unit_test[346]  ●HTTPヘッダ（ContentLength）があり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 15\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 4u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[347]--------------------------------------------- " << endl;
    // unit_test[347] レスポンスデータ残サイズ　＞　0
    // unit_test[347]  ●HTTPヘッダ（ContentLength）なし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=11111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[348]--------------------------------------------- " << endl;
    // unit_test[348] レスポンスデータ残サイズ　＞　0
    // unit_test[348]  ●送信データ残サイズ = レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=11111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[349]--------------------------------------------- " << endl;
    // unit_test[349] レスポンスデータ残サイズ　＞　0
    // unit_test[349]  ●送信データ残サイズ < レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\nHTTP/1.0 200", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 38u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);

    cout << "[350]--------------------------------------------- " << endl;
    // unit_test[350] レスポンスデータ残サイズ　＞　0
    // unit_test[350]  ●送信データ残サイズ　＞　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 20\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 9u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, REQUEST_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[351]--------------------------------------------- " << endl;
    // unit_test[351] レスポンスデータ残サイズ　＞　0
    // unit_test[351]  ●出力の送信状態リスト：　１：送信待　２：送信待
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 76;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 14;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 O", 14);

    memcpy(response.c_array(), "K\r\nCookie: m=1\r\n\r\nHTTP/1.0 200 OK\r\nCookie: m=1\r\n\r\n", 50);
    response_len = 50;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->status, SEND_OK);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_rest_size, 0u);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_possible_size, 32u);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_offset, 32u);

    cout << "[352]--------------------------------------------- " << endl;
    // unit_test[352] レスポンスデータ残サイズ　＞　0
    // unit_test[352]  ●出力の送信状態リスト：　１：送信待　２：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 10\r\n\r\n1234567890G", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 49u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 49u);

    cout << "[353]--------------------------------------------- " << endl;
    // unit_test[353] レスポンスデータ残サイズ　＞　0
    // unit_test[353]  ●出力の送信状態リスト：　１：送信待
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 15\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 4u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[354]--------------------------------------------- " << endl;
    // unit_test[354] レスポンスデータ残サイズ　＞　0
    // unit_test[354]  ●出力の送信状態リスト：　１：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\n456789012345678901234567890123456", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);

    cout << "[355]--------------------------------------------- " << endl;
    // unit_test[355] 送信可能データがあり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nHOST: KKK\r\n\r\nHTTP/1.0 200 OK\r\nHOS", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    cout << "[356]--------------------------------------------- " << endl;
    // unit_test[356] 送信可能データなし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=111111111111111111", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);
}

//handle_realserver_recv_tcp_test_thread
void handle_realserver_recv_tcp_test_thread(){
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> response;
    send_status send_status_temp;
    receive_data receive_data_tmp;
    boost::thread down_thread(&down_thread_func);
    boost::asio::ip::tcp::endpoint rs_endpoint = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    psession_thread_data->target_endpoint = rs_endpoint;

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;
        this->forwarded_for = 1;
    }

    init_receive_data(receive_data_tmp);

    psession_thread_data->receive_data_map[rs_endpoint] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(rs_endpoint);
    receive_data& receive_data_global = it->second;

    cout << "[357]--------------------------------------------- " << endl;
    // unit_test[357] レスポンスデータ残サイズ　＞　0
    // unit_test[357]  ●出力の送信状態リスト：　１：送信待　２：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 76;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 14;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 O", 14);

    memcpy(response.c_array(), "K\r\nCookie: m=1\r\n\r\nHTTP/1.0 200 OK\r\nCookie: m=1\r\n\r", 49);
    size_t response_len = 49;

    ret = handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, response, response_len);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 32u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->status, SEND_NG);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_rest_size, 0u);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->unsend_size, 31u);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_offset, 32u);
    }
}

//handle_realserver_recv(udp) 郎希倹
void handle_realserver_recv_udp_test() {
    boost::asio::ip::udp::endpoint rs_endpoint;
    boost::array<char,MAX_BUFFER_SIZE> recvbuffer;
    size_t recvlen = 0;

    cout << "[358]--------------------------------------------- " << endl;
    // unit_test[358] 異常系で必ずSTOPを返す
    BOOST_CHECK_EQUAL(this->handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, recvbuffer, recvlen), STOP);
}

//handle_sorryserver_recv 郎希倹
void handle_sorryserver_recv_test() {
    size_t response_len = USE_BUFFER_SIZE;
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> response;
    send_status send_status_temp;
    boost::thread up_thread(&up_thread_func);
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, up_thread.get_id());
    receive_data receive_data_tmp;

    init_send_status(send_status_temp);
    // receive_dataを作成する
    init_receive_data(receive_data_tmp);
    receive_data_tmp.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_tmp.receive_buffer_rest_size = USE_BUFFER_SIZE;

    //receive_data_mapを作成する
    psession_thread_data->end_flag = END_FLAG_OFF;
    psession_thread_data->last_status = SORRYSERVER_RECV;
    boost::asio::ip::tcp::endpoint sorry_endpoint = string_to_endpoint<boost::asio::ip::tcp>("111.111.111.111:8080");
    psession_thread_data->target_endpoint = sorry_endpoint;

    //session_thread_dataを作成する
    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[sorry_endpoint] = receive_data_tmp;
    this->forwarded_for = 1;

    cout << "[359]--------------------------------------------- " << endl;
    // unit_test[359] boost::this_thread::get_id()対応のデータなし
    this->session_thread_data_map.clear();
    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[360]--------------------------------------------- " << endl;
    // unit_test[360] boost::this_thread::get_id()対応のデータsession_thread_dataがNULL
    this->session_thread_data_map.clear();
    thread_data_ptr p;
    this->session_thread_data_map[boost::this_thread::get_id()] = p;
    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;

    psession_thread_data->receive_data_map.clear();
    psession_thread_data->receive_data_map[sorry_endpoint] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(sorry_endpoint);
    receive_data& receive_data_global = it->second;

    cout << "[361]--------------------------------------------- " << endl;
    // unit_test[361] receive_data.receive_buffer＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = NULL;
    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[362]--------------------------------------------- " << endl;
    // unit_test[362] receive_data.receive_buffer1＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 20;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer1 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memset(response.c_array(), 'n', 80);
    response_len = 80;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[363]--------------------------------------------- " << endl;
    // unit_test[363] receive_data.receive_buffer2＝Null
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 20;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memset(response.c_array(), 'n', 80);
    response_len = 80;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[364]--------------------------------------------- " << endl;
    // unit_test[364] new失敗の場合
    receive_data_global.receive_buffer_max_size = 0;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = NULL;
    receive_data_global.receive_buffer2 = NULL;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n01234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    this->getloglevel = &stb_getloglevel_is_none;
    new_install();
    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
    new_uninstall();
    this->getloglevel = &stb_getloglevel;
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[365]--------------------------------------------- " << endl;
    // unit_test[365] レスポンスサイズが0である
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, 0);
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);

    cout << "[366]--------------------------------------------- " << endl;
    // unit_test[366] レスポンスサイズがUINT_MAXである レスポンス実際にサイズがMAX_BUFFER_SIZEである
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = MAX_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memset(response.c_array(), 'n', MAX_BUFFER_SIZE);
    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, UINT_MAX);
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[367]--------------------------------------------- " << endl;
    // unit_test[367] レスポンスサイズがMAX_BUFFER_SIZEである レスポンス実際にサイズがMAX_BUFFER_SIZEである
    receive_data_global.receive_buffer_max_size = MAX_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = MAX_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();
    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\n", 17);
    memset(response.c_array() + 17, 'n', MAX_BUFFER_SIZE - 17);

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE);

    // unit_test_part1 begin:
    send_status status_END;
    init_send_status(status_END);
    status_END.status = SEND_END;
    status_END.send_offset = 0;
    status_END.send_end_size = 20;

    send_status status_OK;
    init_send_status(status_OK);
    status_OK.status = SEND_OK;
    status_OK.send_offset = 0;
    status_OK.send_possible_size = 20;

    send_status status_NG;
    init_send_status(status_NG);
    status_NG.status = SEND_NG;
    status_NG.send_offset = 0;
    status_NG.unsend_size = 20;

    send_status status_CONTINUE;
    init_send_status(status_CONTINUE);
    status_CONTINUE.status = SEND_CONTINUE;
    status_CONTINUE.send_offset = 0;
    status_CONTINUE.send_end_size = 20;
    status_CONTINUE.send_rest_size = 20;

    send_status* send_status_NG[27][3]
    = {
        // unit_test 入力の送信状態リストは下記の通り：（異常系）
        // unit_test[368] 送信状態リストの要素の送信状態が1：送信待の場合
        {&status_OK,NULL,NULL},
        // unit_test[369] 送信状態リストの要素の送信状態が1：送信済 2：送信待の場合
        {&status_END,&status_OK,NULL},
        // unit_test[370] 送信状態リストの要素の送信状態が1：送信継続 2：送信継続の場合
        {&status_CONTINUE,&status_CONTINUE,NULL},
        // unit_test[371] 送信状態リストの要素の送信状態が1：送信継続 2：送信不可の場合
        {&status_CONTINUE,&status_NG,NULL},
        // unit_test[372] 送信状態リストの要素の送信状態が1：送信継続 2：送信済の場合
        {&status_CONTINUE,&status_END,NULL},
        // unit_test[373] 送信状態リストの要素の送信状態が1：送信継続 2：送信待の場合
        {&status_CONTINUE,&status_OK,NULL},
        // unit_test[374] 送信状態リストの要素の送信状態が1：送信不可 2：送信継続の場合
        {&status_NG,&status_CONTINUE,NULL},
        // unit_test[375] 送信状態リストの要素の送信状態が1：送信不可 2：送信不可の場合
        {&status_NG,&status_NG,NULL},
        // unit_test[376] 送信状態リストの要素の送信状態が1：送信不可 2：送信済の場合
        {&status_NG,&status_END,NULL},
        // unit_test[377] 送信状態リストの要素の送信状態が1：送信不可 2：送信待の場合
        {&status_NG,&status_OK,NULL},
        // unit_test[378] 送信状態リストの要素の送信状態が1：送信待 2：送信継続の場合
        {&status_OK,&status_CONTINUE,NULL},
        // unit_test[379] 送信状態リストの要素の送信状態が1：送信待 2：送信不可の場合
        {&status_OK,&status_NG,NULL},
        // unit_test[380] 送信状態リストの要素の送信状態が1：送信待 2：送信済の場合
        {&status_OK,&status_END,NULL},
        // unit_test[381] 送信状態リストの要素の送信状態が1：送信待 2：送信待の場合
        {&status_OK,&status_OK,NULL},
        // unit_test[382] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信継続の場合
        {&status_END,&status_CONTINUE,&status_CONTINUE},
        // unit_test[383] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信不可の場合
        {&status_END,&status_CONTINUE,&status_NG},
        // unit_test[384] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信済の場合
        {&status_END,&status_CONTINUE,&status_END},
        // unit_test[385] 送信状態リストの要素の送信状態が1：送信済 2：送信継続 ３：送信待の場合
        {&status_END,&status_CONTINUE,&status_OK},
        // unit_test[386] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信継続の場合
        {&status_END,&status_NG,&status_CONTINUE},
        // unit_test[387] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信不可の場合
        {&status_END,&status_NG,&status_NG},
        // unit_test[388] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信済の場合
        {&status_END,&status_NG,&status_END},
        // unit_test[389] 送信状態リストの要素の送信状態が1：送信済 2：送信不可 ３：送信待の場合
        {&status_END,&status_NG,&status_OK},
        // unit_test[390] 送信状態リストの要素の送信状態が1：送信済 2：送信済 ３：送信待の場合
        {&status_END,&status_END,&status_OK},
        // unit_test[391] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信継続の場合
        {&status_END,&status_OK,&status_CONTINUE},
        // unit_test[392] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信不可の場合
        {&status_END,&status_OK,&status_NG},
        // unit_test[393] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信済の場合
        {&status_END,&status_OK,&status_END},
        // unit_test[394] 送信状態リストの要素の送信状態が1：送信済 2：送信待 ３：送信待の場合
        {&status_END,&status_OK,&status_OK},
    };

    int i;
    int j;

    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    memset(receive_data_global.receive_buffer, 'n', USE_BUFFER_SIZE);
    memset(response.c_array(), 'm', USE_BUFFER_SIZE);
    for (i = 0; i < 27; i++)
    {
        receive_data_global.send_status_list.clear();

        for (j = 0; j < 3; j++)
        {
            if(send_status_NG[i][j] == NULL)
            {
             break;
            }

            receive_data_global.send_status_list.push_back(*(send_status_NG[i][j]));
        }

        ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);
        std::cout << "[" << 368 + i << "]------------------------------------------" << std::endl;
        BOOST_CHECK_EQUAL(ret, FINALIZE);
    }

    cout << "[395]--------------------------------------------- " << endl;
    // unit_test[395] 送信状態リストの要素の送信状態が1：送信済 2：送信済 3：送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 20;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 10;
    send_status_temp.send_rest_size = 11;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "12345678901HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 39u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 11u);

    cout << "[396]--------------------------------------------- " << endl;
    // unit_test[396] 送信状態リストの要素の送信状態が1：送信済 2：送信済 3：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 20;
    send_status_temp.send_end_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 40;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 2", 10);

    memcpy(response.c_array(), "00 OK\r\nCookie: monkey=1234567890123456789012345678", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[397]--------------------------------------------- " << endl;
    // unit_test[397] 送信状態リストの要素の送信状態が1：送信済 2：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 40;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 2", 10);

    memcpy(response.c_array(), "00 OK\r\nCookie: monkey=1234567890123456789012345678", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[398]--------------------------------------------- " << endl;
    // unit_test[398] 送信状態リストの要素の送信状態が1：送信済 2：送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 10;
    send_status_temp.send_rest_size = 11;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "12345678901HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 11u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 39u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 11u);

    cout << "[399]--------------------------------------------- " << endl;
    // unit_test[399] 送信状態リストの要素の送信状態が１:送信済　２:送信済の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 40;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[400]--------------------------------------------- " << endl;
    // unit_test[400] 送信状態リストの要素の送信状態が1：送信不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 40;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 10;
    send_status_temp.unsend_size = 10;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 10, "HTTP/1.1 2", 10);

    memcpy(response.c_array(), "00 OK\r\nContent-Length: 21\r\n\r\n012345678901234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 60u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[401]--------------------------------------------- " << endl;
    // unit_test[401] 送信状態リストの要素の送信状態が１:送信継続の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 20;

    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890HTTP/1.1 200 OK\r\nContent-Lengt", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 30u);

    cout << "[402]--------------------------------------------- " << endl;
    // unit_test[402] 送信状態リストの要素の送信状態が１:送信済の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_END;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 80;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[403]--------------------------------------------- " << endl;
    // unit_test[403] 送信状態リストの要素数が０である場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 10;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nContent-Length: 11\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    // unit_test_part2 begin:
    cout << "[404]--------------------------------------------- " << endl;
    // unit_test[404] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[404]  ●データバッファMAXサイズ　<　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[404]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 80;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=123456789012345678901"
        "23456789012345678901234567890123456789012345678901234567890", 112);
    response_len = 112;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, MAX_BUFFER_SIZE - 112);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);

    //メモリの内容をチェックする
    int cmp_ret = memcmp(response.c_array(), receive_data_global.receive_buffer, 112u);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 112u);

    cout << "[405]--------------------------------------------- " << endl;
    // unit_test[405] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[405]  ●データバッファMAXサイズ　<　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[405]  ●データバッファ上限値　＜　未送信データサイズ　＋　レスポンスデータサイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 80;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 10;
    send_status_temp.unsend_size = MAX_SESSIONLESS_MODULE_BUFFER_SIZE;
    receive_data_global.send_status_list.push_back(send_status_temp);

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=123456789012345678901"
        "23456789012345678901234567890123456789012345678901234567890", 112);
    response_len = 112;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[406]--------------------------------------------- " << endl;
    // unit_test[406] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[406]  ●データバッファMAXサイズ　<　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[406]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=", 12);
    memset(response.c_array(), 'x', MAX_BUFFER_SIZE - 12);
    response_len = MAX_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, MAX_BUFFER_SIZE + 20);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), MAX_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, MAX_BUFFER_SIZE + 20);

    cout << "[407]--------------------------------------------- " << endl;
    // unit_test[407] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[407]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　>　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[407]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890", 72);
    response_len = 72;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);
    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 72);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[408]--------------------------------------------- " << endl;
    // unit_test[408] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[408]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　=　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[408]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890123456789012345678", 90);
    response_len = 90;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);
    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 90);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[409]--------------------------------------------- " << endl;
    // unit_test[409] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[409]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　>　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[409]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890", 52);
    response_len = 52;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 52);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[410]--------------------------------------------- " << endl;
    // unit_test[410] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[410]  ●使用中データバッファがデータバッファ１の場合、データバッファMAXサイズ　=　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[410]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890123456789012345678", 70);
    response_len = 70;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[411]--------------------------------------------- " << endl;
    // unit_test[411] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[411]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　>　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[411]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890", 72);
    response_len = 72;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 72);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[412]--------------------------------------------- " << endl;
    // unit_test[412] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[412]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　=　未送信データサイズ(0)　＋　レスポンスデータサイズ。
    // unit_test[412]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(),
            "HTTP/1.1 200 OK\r\nCookie: monkey=1234567890123456789012345678901234567890123456789012345678", 90);
    response_len = 90;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, response.c_array(), 90);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[413]--------------------------------------------- " << endl;
    // unit_test[413] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[413]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　>　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[413]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890", 52);
    response_len = 52;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 18u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 52);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 72u);

    cout << "[414]--------------------------------------------- " << endl;
    // unit_test[414] データバッファ残サイズ　<　レスポンスデータサイズ
    // unit_test[414]  ●使用中データバッファがデータバッファ2の場合、データバッファMAXサイズ　=　未送信データサイズ(20)　＋　レスポンスデータサイズ。
    // unit_test[414]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 50;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 20;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer + 20, "HTTP/1.1 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=1234567890123456789012345678901234567890123456789012345678", 70);
    response_len = 70;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer, "HTTP/1.1 200 OK\r\nCoo", 20);
    BOOST_CHECK_EQUAL(cmp_ret, 0);
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 90u);

    cout << "[415]--------------------------------------------- " << endl;
    // unit_test[415] データバッファ残サイズ　>　レスポンスデータサイズ
    // unit_test[415]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=123456789012345678901234567890", 62);
    response_len = 62;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 8u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer2);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 62);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 62u);

    cout << "[416]--------------------------------------------- " << endl;
    // unit_test[416] データバッファ残サイズ　=　レスポンスデータサイズ
    // unit_test[416]  ●メモリの内容をチェックする
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.1 200 OK\r\nCookie: monkey=12345678901234567890123456789012345678", 70);
    response_len = 70;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer, receive_data_global.receive_buffer1);

    //メモリの内容をチェックする
    cmp_ret = memcmp(receive_data_global.receive_buffer + 20, response.c_array(), 70);
    BOOST_CHECK_EQUAL(cmp_ret, 0);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 70u);

    // unit_test_part3 begin:
    cout << "[417]--------------------------------------------- " << endl;
    // unit_test[417] 送信状態リストの要素の送信状態が送信継続
    // unit_test[417]  ●送信データ残サイズ　<　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 20\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890HTTP/1.1 200 OK\r\nContent-Lengt", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 20u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 30u);

    cout << "[418]--------------------------------------------- " << endl;
    // unit_test[418] 送信状態リストの要素の送信状態が送信継続
    // unit_test[418]  ●送信データ残サイズ　=　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 50;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 50\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890123456789012345678901234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[419]--------------------------------------------- " << endl;
    // unit_test[419] 送信状態リストの要素の送信状態が送信継続
    // unit_test[419]  ●送信データ残サイズ　>　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 11;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_CONTINUE;
    send_status_temp.send_offset = 40;
    send_status_temp.send_end_size = 39;
    send_status_temp.send_rest_size = 80;
    receive_data_global.send_status_list.push_back(send_status_temp);
    // 未送信データを生成する
    memcpy(receive_data_global.receive_buffer + 40, "HTTP/1.1 200 OK\r\nContent-Length: 80\r\n\r\n", 39);

    memcpy(response.c_array(), "12345678901234567890123456789012345678901234567890", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 30u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[420]--------------------------------------------- " << endl;
    // unit_test[420] 送信状態が送信不可の場合
    // unit_test[420]  ●HTTPバージョンの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.9 200 OK\r\n4567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[421]--------------------------------------------- " << endl;
    // unit_test[421] 送信状態が送信不可の場合
    // unit_test[421]  ●HTTPバージョンの妥当性をチェックして、チェック不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "OST / HTT", 10);
    response_len = 10;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 11u);

    cout << "[422]--------------------------------------------- " << endl;
    // unit_test[422] 送信状態が送信不可の場合
    // unit_test[422]  ●HTTPヘッダの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.0 200 OK\r\nCookie: monkey=1111111111111111111", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 51u);

    cout << "[423]--------------------------------------------- " << endl;
    // unit_test[423] 送信状態が送信不可の場合
    // unit_test[423]  ●HTTPヘッダ（ContentLength）があり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.0 200 OK\r\nContent-Length: 15\r\n\r\n123456789012", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 3u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[424]--------------------------------------------- " << endl;
    // unit_test[424] 送信状態が送信不可の場合
    // unit_test[424]  ●HTTPヘッダ（ContentLength）なし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 89;
    send_status_temp.unsend_size = 1;
    receive_data_global.send_status_list.push_back(send_status_temp);
    //未送信データを生成する
    receive_data_global.receive_buffer[89] = 'H';

    memcpy(response.c_array(), "TTP/1.0 200 OK\r\nCookie: monkey=111111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 51u);

    cout << "[425]--------------------------------------------- " << endl;
    // unit_test[425] 送信状態が送信不可の場合
    // unit_test[425]  ●送信データ残サイズ　>　未送信データサイズ　＋　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123456789012345678901\r\n\r\nGET", 40);
    response_len = 40;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 30u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 3u);

    cout << "[426]--------------------------------------------- " << endl;
    // unit_test[426] 送信状態が送信不可の場合
    // unit_test[426]  ●送信データ残サイズ　<　未送信データサイズ　＋　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123\r\nContent-Length: 100\r\n\r\n0123456789", 50);
    response_len = 50;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 90u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[427]--------------------------------------------- " << endl;
    // unit_test[427] 送信状態が送信不可の場合
    // unit_test[427]  ●送信データ残サイズ　=　未送信データサイズ　＋　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123\r\nContent-Length: 11\r\n\r\n01234567890", 50);
    response_len = 50;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    cout << "[428]--------------------------------------------- " << endl;
    // unit_test[428] レスポンスデータ残サイズ　<　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123456789012345678901\r\n\r\nGET", 40);
    response_len = 40;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 30u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 57u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 3u);

    cout << "[429]--------------------------------------------- " << endl;
    // unit_test[429] レスポンスデータ残サイズ　=　0
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 70;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer2;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.unsend_size = 20;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 OK\r\nCoo", 20);

    //レスポンスデータ
    memcpy(response.c_array(), "kie: monkey=123\r\nContent-Length: 11\r\n\r\n01234567890", 50);
    response_len = 50;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_max_size, USE_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.receive_buffer_rest_size, 20u);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 70u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    // unit_test_part4 begin:
    cout << "[430]--------------------------------------------- " << endl;
    // unit_test[430] レスポンスデータ残サイズ　＞　0
    // unit_test[430]  ●HTTPバージョンの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 10\r\n\r\n1234567890G", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 49u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 49u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 1u);

    cout << "[431]--------------------------------------------- " << endl;
    // unit_test[431] レスポンスデータ残サイズ　＞　0
    // unit_test[431]  ●HTTPバージョンの妥当性をチェックして、異常の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/2.2 200 OK\r\n567890123456789012345678901234567", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[432]--------------------------------------------- " << endl;
    // unit_test[432] レスポンスデータ残サイズ　＞　0
    // unit_test[432]  ●HTTPヘッダの妥当性をチェックして、不可の場合
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=111111111111111111", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);

    cout << "[433]--------------------------------------------- " << endl;
    // unit_test[433] レスポンスデータ残サイズ　＞　0
    // unit_test[433]  ●HTTPヘッダ（ContentLength）があり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 15\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 4u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[434]--------------------------------------------- " << endl;
    // unit_test[434] レスポンスデータ残サイズ　＞　0
    // unit_test[434]  ●HTTPヘッダ（ContentLength）なし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=11111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[435]--------------------------------------------- " << endl;
    // unit_test[435] レスポンスデータ残サイズ　＞　0
    // unit_test[435]  ●送信データ残サイズ = レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=11111111111111\r\n\r\n", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[436]--------------------------------------------- " << endl;
    // unit_test[436] レスポンスデータ残サイズ　＞　0
    // unit_test[436]  ●送信データ残サイズ < レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 0;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 0\r\n\r\nHTTP/1.0 200", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 38u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 38u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 12u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);

    cout << "[437]--------------------------------------------- " << endl;
    // unit_test[437] レスポンスデータ残サイズ　＞　0
    // unit_test[437]  ●送信データ残サイズ　＞　レスポンスデータ残サイズ
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 20\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 9u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, REQUEST_BUFFER_SIZE);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    cout << "[438]--------------------------------------------- " << endl;
    // unit_test[438] レスポンスデータ残サイズ　＞　0
    // unit_test[438]  ●出力の送信状態リスト：　１：送信待　２：送信待
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 76;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 14;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 O", 14);

    memcpy(response.c_array(), "K\r\nCookie: m=1\r\n\r\nHTTP/1.0 200 OK\r\nCookie: m=1\r\n\r\n", 50);
    response_len = 50;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 32u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->status, SEND_OK);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_rest_size, 0u);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_possible_size, 32u);
    BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_offset, 32u);

    cout << "[439]--------------------------------------------- " << endl;
    // unit_test[439] レスポンスデータ残サイズ　＞　0
    // unit_test[439]  ●出力の送信状態リスト：　１：送信待　２：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 10\r\n\r\n1234567890G", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 49u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 0u);

    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->unsend_size, 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.rbegin()->send_offset, 49u);

    cout << "[440]--------------------------------------------- " << endl;
    // unit_test[440] レスポンスデータ残サイズ　＞　0
    // unit_test[440]  ●出力の送信状態リスト：　１：送信待
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nContent-Length: 15\r\n\r\n12345678901", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 4u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 50u);

    cout << "[441]--------------------------------------------- " << endl;
    // unit_test[441] レスポンスデータ残サイズ　＞　0
    // unit_test[441]  ●出力の送信状態リスト：　１：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\n456789012345678901234567890123456", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);

    cout << "[442]--------------------------------------------- " << endl;
    // unit_test[442] 送信可能データがあり
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    delete[] receive_data_global.receive_buffer1;
    delete[] receive_data_global.receive_buffer2;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nHOST: KKK\r\n\r\nHTTP/1.0 200 OK\r\nHOS", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    cout << "[443]--------------------------------------------- " << endl;
    // unit_test[443] 送信可能データなし
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    memcpy(response.c_array(), "HTTP/1.0 200 OK\r\nCookie: monkey=111111111111111111", REQUEST_BUFFER_SIZE);
    response_len = REQUEST_BUFFER_SIZE;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
    BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 1u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_NG);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);
    BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->unsend_size, 50u);
}

//handle_sorryserver_recv_test_thread
void handle_sorryserver_recv_test_thread(){
    EVENT_TAG ret;
    boost::array<char, MAX_BUFFER_SIZE> response;
    send_status send_status_temp;
    receive_data receive_data_tmp;
    boost::thread down_thread(&down_thread_func);
    boost::asio::ip::tcp::endpoint sorry_endpoint = string_to_endpoint<boost::asio::ip::tcp>("10.10.10.1:8080");
    thread_data_ptr psession_thread_data(new session_thread_data_sessionless);
    init_session_thread_data_sessionless(psession_thread_data, down_thread.get_id());
    psession_thread_data->target_endpoint = sorry_endpoint;

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = psession_thread_data;
        this->forwarded_for = 1;
    }

    init_receive_data(receive_data_tmp);

    psession_thread_data->receive_data_map[sorry_endpoint] = receive_data_tmp;
    std::map<boost::asio::ip::tcp::endpoint, receive_data>::iterator it =
                psession_thread_data->receive_data_map.find(sorry_endpoint);
    receive_data& receive_data_global = it->second;

    cout << "[444]--------------------------------------------- " << endl;
    // unit_test[444] レスポンスデータ残サイズ　＞　0
    // unit_test[444]  ●出力の送信状態リスト：　１：送信待　２：送信不可
    receive_data_global.receive_buffer_max_size = USE_BUFFER_SIZE;
    receive_data_global.receive_buffer_rest_size = 76;
    receive_data_global.receive_buffer1 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer2 = new char[receive_data_global.receive_buffer_max_size];
    receive_data_global.receive_buffer = receive_data_global.receive_buffer1;
    receive_data_global.send_status_list.clear();

    init_send_status(send_status_temp);
    send_status_temp.status = SEND_NG;
    send_status_temp.send_offset = 0;
    send_status_temp.send_end_size = 0;
    send_status_temp.unsend_size = 14;
    receive_data_global.send_status_list.push_back(send_status_temp);

    //未送信データ
    memcpy(receive_data_global.receive_buffer, "HTTP/1.0 200 O", 14);

    memcpy(response.c_array(), "K\r\nCookie: m=1\r\n\r\nHTTP/1.0 200 OK\r\nCookie: m=1\r\n\r", 49);
    size_t response_len = 49;

    ret = handle_sorryserver_recv(boost::this_thread::get_id(), sorry_endpoint, response, response_len);

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);
        BOOST_REQUIRE_EQUAL(receive_data_global.send_status_list.size(), 2u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->status, SEND_OK);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_rest_size, 0u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_possible_size, 32u);
        BOOST_CHECK_EQUAL(receive_data_global.send_status_list.begin()->send_offset, 0u);

        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->status, SEND_NG);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_rest_size, 0u);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->unsend_size, 31u);
        BOOST_CHECK_EQUAL((receive_data_global.send_status_list.rbegin())->send_offset, 32u);
    }
}

//handle_response_send_inform 馮家純
void handle_response_send_inform_test(){

    cout << "[445]--------------------------------------------- " << endl;
    //unit_test[445] handle_response_send_inform 戻り値が「STOP」に設定する。
    EVENT_TAG ret = this->handle_response_send_inform(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(ret, STOP);
}

//handle_client_connection_check 馮家純
void handle_client_connection_check_test(){
    EVENT_TAG ret;
    cout << "[446]--------------------------------------------- " << endl;
    //unit_test[446] 異常系 session_thread_data_map中にThreadID対応のデータがない
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()].reset();
    boost::array<char, MAX_BUFFER_SIZE> sbf_err1;
    std::size_t d_err1;
    ret = this->handle_client_connection_check(boost::this_thread::get_id(), sbf_err1, d_err1);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[447]--------------------------------------------- " << endl;
    //unit_test[447] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
    this->session_thread_data_map.clear();

    ret = this->handle_client_connection_check(boost::this_thread::get_id(), sbf_err1, d_err1);

    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[448]--------------------------------------------- " << endl;
    //unit_test[448] 送信バッファサイズ　>　送信可能データサイズ
    boost::array<char, MAX_BUFFER_SIZE> sbf1;
    std::size_t d1;
    boost::asio::ip::tcp::endpoint ep1;

    //session_thread_data_sessionless
    thread_data_ptr data1(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data1;
    //send_status
    send_status sd1;
    sd1.status = SEND_OK;
    sd1.send_possible_size = 2u;
    sd1.send_offset = 1u;
    //receive_data
    receive_data rd1;
    rd1.receive_buffer = "abcd";
    //send_status->receive_data
    rd1.send_status_list.push_back(sd1);
    //receive_data->session_thread_data_sessionless
    data1->receive_data_map[ep1] = rd1;

    ret = this->handle_client_connection_check(boost::this_thread::get_id(), sbf1, d1);

    std::string chk1 = "bc";
    //送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
    BOOST_CHECK_EQUAL(memcmp(sbf1.data(), chk1.c_str(), 2u), 0);
    //送信済データサイズに送信可能データサイズを設定する
    BOOST_CHECK_EQUAL(data1->receive_data_map[ep1].send_status_list.rbegin()->send_end_size, 2u);
    //送信可能データサイズに0を設定する
    BOOST_CHECK_EQUAL(data1->receive_data_map[ep1].send_status_list.rbegin()->send_possible_size, 0u);
    //遷移先ステータスを設定する status = CLIENT_SEND
    BOOST_CHECK_EQUAL(ret, CLIENT_SEND);

    cout << "[449]--------------------------------------------- " << endl;
    //unit_test[449] 送信バッファサイズ　<　送信可能データサイズ
    this->session_thread_data_map.clear();

    boost::array<char, MAX_BUFFER_SIZE> sbf2;
    std::size_t d2;
    boost::asio::ip::tcp::endpoint ep2;

    //session_thread_data_sessionless
    thread_data_ptr data2(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data2;
    //send_status
    send_status sd2;
    sd2.status = SEND_OK;
    sd2.send_possible_size = sbf2.max_size() + 1u;
    sd2.send_offset = 1u;
    //receive_data
    receive_data rd2;
    rd2.receive_buffer = "abcdefg";
    //send_status->receive_data
    rd2.send_status_list.push_back(sd2);
    //receive_data->session_thread_data_sessionless
    data2->receive_data_map[ep2] = rd2;

    ret = this->handle_client_connection_check(boost::this_thread::get_id(), sbf2, d2);

    std::string chk2 = "bcdefg";
    //送信可能データを先頭から送信バッファサイズ分、送信バッファにコピーする
    BOOST_CHECK_EQUAL(memcmp(sbf2.data(), chk2.c_str(), 6u), 0);

    std::size_t sbfsize = sbf2.max_size();
    //送信済データサイズに送信バッファサイズを設定する
    BOOST_CHECK_EQUAL(data2->receive_data_map[ep2].send_status_list.rbegin()->send_end_size, sbfsize);
    //送信可能データサイズから送信バッファサイズを減算する
    BOOST_CHECK_EQUAL(data2->receive_data_map[ep2].send_status_list.rbegin()->send_possible_size, 1u);
    //遷移先ステータスを設定する status = CLIENT_SEND
    BOOST_CHECK_EQUAL(ret, CLIENT_SEND);

    cout << "[450]--------------------------------------------- " << endl;
    //unit_test[450] 送信バッファサイズ　=　送信可能データサイズ
    this->session_thread_data_map.clear();

    boost::array<char, MAX_BUFFER_SIZE> sbf3;
    std::size_t d3;
    boost::asio::ip::tcp::endpoint ep3;

    //session_thread_data_sessionless
    thread_data_ptr data3(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data3;
    //send_status
    send_status sd3;
    sd3.status = SEND_OK;
    sd3.send_possible_size = sbf3.max_size();
    sd3.send_offset = 2u;
    //receive_data
    receive_data rd3;
    const std::size_t sbf_size = sbf3.max_size();
    rd3.receive_buffer = (char*)malloc(sbf_size);
    memset(rd3.receive_buffer, 'a', sbf_size);
    //send_status->receive_data
    rd3.send_status_list.push_back(sd3);
    //receive_data->session_thread_data_sessionless
    data3->receive_data_map[ep3] = rd3;

    ret = this->handle_client_connection_check(boost::this_thread::get_id(), sbf3, d3);

    char *chk3;
    chk3 = (char*)malloc(sbf_size - 2);
    memset(chk3, 'a', sbf_size - 2);
    //送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
    BOOST_CHECK_EQUAL(memcmp(sbf3.data(), chk3, sbf_size - 2), 0);
    //送信済データサイズに送信可能データサイズを設定する
    BOOST_CHECK_EQUAL(data3->receive_data_map[ep3].send_status_list.rbegin()->send_end_size, sbf_size);
    //送信可能データサイズに0を設定する
    BOOST_CHECK_EQUAL(data3->receive_data_map[ep3].send_status_list.rbegin()->send_possible_size, 0u);
    //遷移先ステータスを設定する status = CLIENT_SEND
    BOOST_CHECK_EQUAL(ret, CLIENT_SEND);

}

void handle_client_connection_check_test_thread(){
    EVENT_TAG ret;
    cout << "[451]--------------------------------------------- " << endl;
    //unit_test[451] 送信バッファサイズ　>　送信可能データサイズ
    boost::array<char, MAX_BUFFER_SIZE> sbf1;
    std::size_t d1;
    boost::asio::ip::tcp::endpoint ep1;

    //session_thread_data_sessionless
    thread_data_ptr data1(new session_thread_data_sessionless);

    //send_status
    send_status sd1;
    sd1.status = SEND_OK;
    sd1.send_possible_size = 2u;
    sd1.send_offset = 1u;
    //receive_data
    receive_data rd1;
    rd1.receive_buffer = "abcd";
    //send_status->receive_data
    rd1.send_status_list.push_back(sd1);
    //receive_data->session_thread_data_sessionless
    data1->receive_data_map[ep1] = rd1;

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data1;
    }

    ret = this->handle_client_connection_check(boost::this_thread::get_id(), sbf1, d1);

    std::string chk1 = "bc";
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        //送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
        BOOST_CHECK_EQUAL(memcmp(sbf1.data(), chk1.c_str(), 2u), 0);
        //送信済データサイズに送信可能データサイズを設定する
        BOOST_CHECK_EQUAL(data1->receive_data_map[ep1].send_status_list.rbegin()->send_end_size, 2u);
        //送信可能データサイズに0を設定する
        BOOST_CHECK_EQUAL(data1->receive_data_map[ep1].send_status_list.rbegin()->send_possible_size, 0u);
        //遷移先ステータスを設定する status = CLIENT_SEND
        BOOST_CHECK_EQUAL(ret, CLIENT_SEND);
    }
}

//handle_client_select 馮家純
void handle_client_select_test(){

    cout << "[452]--------------------------------------------- " << endl;
    //unit_test[452] handle_client_select 戻り値が「STOP」に設定する。
    boost::asio::ip::udp::endpoint ep;
    boost::array<char, MAX_BUFFER_SIZE> sbf;
    std::size_t d;
    EVENT_TAG ret = this->handle_client_select(boost::this_thread::get_id(), ep, sbf, d);

    BOOST_CHECK_EQUAL(ret, STOP);
}

//handle_client_send 郎希倹
void handle_client_send_test(){
    boost::asio::ip::tcp::endpoint endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    send_status client_send_status, client_send_status1;
    receive_data client_receive_data;
    thread_data_ptr data_pro(new session_thread_data_sessionless) ;

    cout << "[453]--------------------------------------------- " << endl;
    //unit_test[453] session_thread_data_map中にthread_id無し
    EVENT_TAG event_status = this->handle_client_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[454]--------------------------------------------- " << endl;
    //unit_test[454] session_thread_data_map中にsession_thread_data_sessionless無し
    thread_data_ptr thread_data1 ;
    this->session_thread_data_map[boost::this_thread::get_id()] = thread_data1;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[455]--------------------------------------------- " << endl;
    //unit_test[455] receive_data_map中にendpoint無し
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[456]--------------------------------------------- " << endl;
    //unit_test[456] send_status_list中にsend_status無し
    client_receive_data.send_status_list.clear();
    data_pro->receive_data_map[endpoint] = client_receive_data;
    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;

    event_status = this->handle_client_send(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(event_status, FINALIZE);

    cout << "[457]--------------------------------------------- " << endl;
    //unit_test[457] 送信状態->SEND_OK/送信可能データサイズ　>　0/送信可能データあり
    //unit_test[457] test data: 送信状態に送信待を設定,送信データオフセットに送信済データサイズを加算する,送信済データサイズに0を設定する,遷移先ステータスを設定する
    client_send_status.send_possible_size = 1u;//送信可能データサイズ　>　0
    client_send_status.edit_data_list.clear();//編集データリスト=0
    client_send_status.status = SEND_OK; //送信状態->SEND_OK
    client_send_status.send_offset = 0u;
    client_send_status.send_end_size = 1u;
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    SEND_STATUS_TAG send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;
    size_t offset = data_pro->receive_data_map[endpoint].send_status_list.front().send_offset;
    size_t end_size = data_pro->receive_data_map[endpoint].send_status_list.front().send_end_size;

    BOOST_CHECK_EQUAL(event_status, CLIENT_CONNECTION_CHECK); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_OK); //送信状態に送信待を設定する
    BOOST_CHECK_EQUAL(offset,1u); //送信データオフセットに送信済データサイズを加算する
    BOOST_CHECK_EQUAL(end_size,0u); //送信済データサイズに0を設定する

    cout << "[458]--------------------------------------------- " << endl;
    //unit_test[458] 送信状態->SEND_NG
    //unit_test[458] test data:遷移先ステータスを設定する
    client_send_status.status = SEND_NG; //送信状態->SEND_NG
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[459]--------------------------------------------- " << endl;
    //unit_test[459] 送信状態->SEND_CONTINUE
    //unit_test[459] test data:遷移先ステータスを設定する
    client_send_status.status = SEND_CONTINUE; //送信状態->SEND_CONTINUE
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[460]--------------------------------------------- " << endl;
    //unit_test[460] 送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし/終了フラグがONの場合
    //unit_test[460] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    client_send_status.status = SEND_OK; //送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON;//終了フラグがON

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_DISCONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する

    cout << "[461]--------------------------------------------- " << endl;
    //unit_test[461] 送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし/終了フラグがOFFの場合/sorry状態の場合
    //unit_test[461] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_OFF;//終了フラグがOFF
    data_pro->sorry_flag = SORRY_FLAG_ON;//sorry状態の場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, SORRYSERVER_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する

    cout << "[462]--------------------------------------------- " << endl;
    //unit_test[462] 送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　＞　0/送信可能データなし/終了フラグがOFFの場合/sorry状態以外の場合
    //unit_test[462] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 1u;//送信データ残サイズ　> 0
    client_send_status.status = SEND_OK; //送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_OFF;//終了フラグがOFF
    data_pro->sorry_flag = SORRY_FLAG_OFF;//sorry状態以外の場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, REALSERVER_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信状態に送信待を設定する

    cout << "[463]--------------------------------------------- " << endl;
    //unit_test[463] 送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　=　0/送信可能データなし/終了フラグがONの場合
    //unit_test[463] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON;//終了フラグがON

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_DISCONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する

    cout << "[464]--------------------------------------------- " << endl;
    //unit_test[464] 送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　=　0/送信可能データなし/終了フラグがOFFの場合/sorry状態の場合
    //unit_test[464] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    client_send_status.status = SEND_OK; //送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_OFF;//終了フラグがOFF
    data_pro->sorry_flag = SORRY_FLAG_ON;//sorry状態の場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, SORRYSERVER_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する

    cout << "[465]--------------------------------------------- " << endl;
    //unit_test[465] 送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　=　0/送信可能データなし/終了フラグがOFFの場合/sorry状態以外の場合
    //unit_test[465] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_OFF;//終了フラグがOFF
    data_pro->sorry_flag = SORRY_FLAG_OFF;//sorry状態以外の場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, REALSERVER_RECV); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する

    cout << "[466]--------------------------------------------- " << endl;
    //unit_test[466] 送信状態->SEND_OK,SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ > 0
    //unit_test[466] test data: 遷移先ステータスを設定する
    client_send_status.send_possible_size = 0u; //送信可能データサイズ　=　0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_send_status.send_rest_size = 1u; //送信データ残サイズ > 0
    client_send_status1.status = SEND_OK;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[467]--------------------------------------------- " << endl;
    //unit_test[467] 送信状態->SEND_OK,SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ = 0/送信可能データあり
    //unit_test[467] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    client_send_status.send_possible_size = 0u; //送信可能データサイズ　=　0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_send_status.send_rest_size = 0u; //送信データ残サイズ = 0
    client_send_status1.status = SEND_OK;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_CONNECTION_CHECK); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信可能データサイズ　=　0,送信データ残サイズ = 0,送信状態に送信待を設定する

    cout << "[468]--------------------------------------------- " << endl;
    //unit_test[468] 送信状態->SEND_OK,SEND_NG/送信可能データサイズ　=　0
    //unit_test[468] test data: 遷移先ステータスを設定する
    client_send_status.send_possible_size = 0u; //送信可能データサイズ　=　0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_send_status.send_rest_size = 1u; //送信データ残サイズ > 0
    client_send_status1.status = SEND_NG;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON; //終了フラグがONの場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[469]--------------------------------------------- " << endl;
    //unit_test[469] 送信状態->SEND_OK,SEND_NG/送信可能データサイズ　=　0/送信データ残サイズ = 0/送信可能データなし/終了フラグがONの場合
    //unit_test[469] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    client_send_status.send_possible_size = 0u; //送信可能データサイズ　=　0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_send_status.send_rest_size = 0u; //送信データ残サイズ = 0
    client_send_status1.status = SEND_NG;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON; //終了フラグがONの場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_DISCONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信可能データサイズ　=　0,送信データ残サイズ = 0,送信状態に送信待を設定する

    cout << "[470]--------------------------------------------- " << endl;
    //unit_test[470] 送信状態->SEND_END,SEND_OK/送信可能データサイズ　>　0
    //unit_test[470] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    client_send_status.status = SEND_END;//送信状態->SEND_END
    client_send_status1.send_possible_size = 1u;
    client_send_status1.status = SEND_OK;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON; //終了フラグがONの場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.back().status;

    BOOST_CHECK_EQUAL(event_status,  CLIENT_CONNECTION_CHECK); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_OK);

    cout << "[471]--------------------------------------------- " << endl;
    //unit_test[471] 送信状態->SEND_END,SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ > 0/送信可能データなし/終了フラグがONの場合
    //unit_test[471] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    client_send_status.status = SEND_END;//送信状態->SEND_END
    client_send_status1.send_rest_size = 1u; //送信データ残サイズ > 0
    client_send_status1.send_possible_size = 0u; //送信可能データサイズ　=　0
    client_send_status1.status = SEND_OK;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON; //終了フラグがONの場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.back().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_DISCONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_CONTINUE); //送信可能データサイズ　=　0,送信データ残サイズ > 0,送信状態に送信待を設定する

    cout << "[472]--------------------------------------------- " << endl;
    //unit_test[472] 送信状態->SEND_END
    //unit_test[472] test data: 遷移先ステータスを設定する
    client_send_status.status = SEND_END;//送信状態->SEND_END
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON; //終了フラグがONの場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する

    cout << "[473]--------------------------------------------- " << endl;
    //unit_test[473] 送信状態->SEND_END,SEND_OK/終了フラグがONの場合
    //unit_test[473] test data: 送信状態に送信待を設定,遷移先ステータスを設定する
    client_send_status.status = SEND_END;// 送信状態->SEND_END
    client_send_status1.status = SEND_OK; //送信状態->SEND_OK
    client_send_status1.send_possible_size = 0u;
    client_send_status1.send_rest_size = 0u;
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_ON; //終了フラグがONの場合

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());
    send_status = data_pro->receive_data_map[endpoint].send_status_list.back().status;

    BOOST_CHECK_EQUAL(event_status, CLIENT_DISCONNECT); //遷移先ステータスを設定する
    BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する

    cout << "[474]--------------------------------------------- " << endl;
    //unit_test[474] 送信状態->SEND_CONTINUE,SEND_OK/送信可能データサイズ　>　0
    //unit_test[474] test data: 遷移先ステータスを設定する
    client_send_status.status = SEND_CONTINUE; //送信状態->SEND_CONTINUE
    client_send_status1.status = SEND_OK; //送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);
    client_receive_data.send_status_list.push_back(client_send_status1);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, receive_data>(endpoint,client_receive_data));

    this->session_thread_data_map.clear();
    this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    event_status = this->handle_client_send(boost::this_thread::get_id());

    BOOST_CHECK_EQUAL(event_status, FINALIZE); //遷移先ステータスを設定する
}

//handle_client_send 郎希倹
void handle_client_send_test_thread(){
    boost::asio::ip::tcp::endpoint endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
    send_status client_send_status;
    receive_data client_receive_data;
    thread_data_ptr data_pro(new session_thread_data_sessionless) ;

    cout << "[475]--------------------------------------------- " << endl;
    //unit_test[475] 多スレッドテスト/送信状態->SEND_OK/送信可能データサイズ　=　0/送信データ残サイズ　=　0/送信可能データなし/終了フラグがOFFの場合/sorry状態以外の場合
    //unit_test[475] test data: 送信状態に送信待を設定
    client_send_status.send_possible_size = 0u;//送信可能データサイズ=0
    client_send_status.send_rest_size = 0u;//送信データ残サイズ　= 0
    client_send_status.status = SEND_OK;//送信状態->SEND_OK
    client_receive_data.send_status_list.clear();
    client_receive_data.send_status_list.push_back(client_send_status);

    data_pro->target_endpoint = endpoint;
    data_pro->receive_data_map.clear();
    data_pro->receive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::receive_data>(endpoint,client_receive_data));
    data_pro->end_flag = END_FLAG_OFF;//終了フラグがOFF
    data_pro->sorry_flag = SORRY_FLAG_OFF;//sorry状態以外の場合

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data_pro;
    }
    EVENT_TAG event_status = this->handle_client_send(boost::this_thread::get_id());
    SEND_STATUS_TAG send_status = data_pro->receive_data_map[endpoint].send_status_list.front().status;

    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(event_status, REALSERVER_RECV); //遷移先ステータスを設定する
        BOOST_CHECK_EQUAL(send_status, SEND_END); //送信状態に送信待を設定する
    }
}

//handle_client_disconnect 馮家純
void handle_client_disconnect_test(){

    cout << "[476]--------------------------------------------- " << endl;
    //unit_test[476] 遷移先ステータスを設定する status = FINALIZE
    EVENT_TAG ret = this->handle_client_disconnect(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[477]--------------------------------------------- " << endl;
    //unit_test[477] 上りスレッドと下りスレッドの場合
    boost::thread tdown_for_get_id(down_thread_func);
    boost::thread_group threads;
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_client_disconnect_test_thread_func,
                                   this,
                                   boost::this_thread::get_id()));
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_client_disconnect_test_thread_func,
                                   this,
                                   tdown_for_get_id.get_id()));
    threads.join_all();
}

void handle_client_disconnect_test_thread_func(const boost::thread::id thread_id){
    EVENT_TAG ret = this->handle_client_disconnect(thread_id);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, FINALIZE);
    }
}

//handle_sorry_enable 馬翠翠
void handle_sorry_enable_test() {
    EVENT_TAG status;
    send_status sendstatus;
    receive_data receivedata;
    thread_data_ptr thread_data(new session_thread_data_sessionless);
    boost::thread thread_up(&up_thread_func);
    boost::thread thread_down(&down_thread_func);

    // 上りスレッドの場合
    thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
    thread_data->thread_id = boost::this_thread::get_id();
    this->session_thread_data_map.insert(pair<boost::thread::id,
                thread_data_ptr> (boost::this_thread::get_id(),
                thread_data));

    cout << "[478]--------------------------------------------- " << endl;
    // unit_test[478] 上りスレッド対応のendpointなし,status = FINALIZE
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.1:8800"),
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[479]--------------------------------------------- " << endl;
    // unit_test[479] accept完了フラグOFFの場合,sorry状態をON,status = ACCEPT
    this->session_thread_data_map[boost::this_thread::get_id()]->accept_end_flag = ACCEPT_END_FLAG_OFF;
    this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp
                = string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.109:8800");
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_ON);
    BOOST_CHECK_EQUAL(status, ACCEPT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[480]--------------------------------------------- " << endl;
    // unit_test[480] accept完了フラグON,sorry状態,送信可能データあり list 1件の場合,status = SORRYSERVER_CONNECT
    this->session_thread_data_map[boost::this_thread::get_id()]->accept_end_flag = ACCEPT_END_FLAG_ON;
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, SORRYSERVER_CONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[481]--------------------------------------------- " << endl;
    // unit_test[481] 送信可能データあり list 2件 1件あり場合,status = SORRYSERVER_CONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, SORRYSERVER_CONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[482]--------------------------------------------- " << endl;
    // unit_test[482] 送信可能データなし list 3件の場合,status =SORRYSERVER_SELECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.size();
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status,SORRYSERVER_SELECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[483]--------------------------------------------- " << endl;
    // unit_test[483] sorry状態以外,送信継続データあり list 1件の場合,終了フラグをON,status = REALSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag,END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[484]--------------------------------------------- " << endl;
    // unit_test[484] 送信継続データあり list 2件 1件あり場合,終了フラグをON,status = REALSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[485]--------------------------------------------- " << endl;
    // unit_test[485] 送信継続データなし list 3件の場合,sorryserver切替中フラグON,sorry状態をON,status = REALSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_OK;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorryserver_switch_flag, SORRYSERVER_SWITCH_FLAG_ON);
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_ON);
    BOOST_CHECK_EQUAL(status,REALSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    // 下りスレッドの場合
    this->session_thread_data_map.clear();
    thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
    thread_data->thread_id = boost::this_thread::get_id();
    this->session_thread_data_map.insert(pair<boost::thread::id,
                thread_data_ptr> (boost::this_thread::get_id(),
                thread_data));
    this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint
                        = string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.243:8800");

    cout << "[486]--------------------------------------------- " << endl;
    // unit_test[486] sorry状態,送信可能データあり list 1件の場合,status = CLIENT_CONNECTION_CHECK
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[487]--------------------------------------------- " << endl;
    // unit_test[487] 送信可能データあり list 2件 1件あり場合,status =CLIENT_CONNECTION_CHECK
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[488]--------------------------------------------- " << endl;
    // unit_test[488] 送信可能データなし list 3件の場合,sorryフラグをON,sorryserver切替中フラグON,status = SORRYSERVER_RECV
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_ON);
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorryserver_switch_flag, SORRYSERVER_SWITCH_FLAG_ON);
    BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[489]--------------------------------------------- " << endl;
    // unit_test[489] sorry状態以外,送信不可データあり list 1件の場合,終了フラグをON,status = REALSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag , END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[490]--------------------------------------------- " << endl;
    // unit_test[490] 送信データ残サイズ ＞ ０　が存在する list 2件の場合,終了フラグをON,status = REALSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 20;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag , END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[491]--------------------------------------------- " << endl;
    // unit_test[491] 送信不可データなしかつ送信データ残サイズ ＞ ０　が存在しない list 1件の場合,sorry状態をON
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_ON);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[492]--------------------------------------------- " << endl;
    // unit_test[492] 送信不可データなしかつ送信データ残サイズ ＞ ０が存在しない,送信可能データなし list 1件の場合,status = SORRYSERVER_RECV
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[493]--------------------------------------------- " << endl;
    // unit_test[493] status = CLIENT_CONNECTION_CHECK
    // unit_test[493] test data:送信不可データなし かつ　送信データ残サイズ ＞ ０が存在しない場合,送信可能データあり list 3件
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_CONTINUE;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[494]--------------------------------------------- " << endl;
    // unit_test[494] 下りスレッドのendpoint対応のreceive_dataなし場合,sorry状態をON,status = SORRYSERVER_RECV
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.1:8800"),
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_ON);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[495]--------------------------------------------- " << endl;
    // unit_test[495] session_thread_data NULLの場合,status = FINALIZE
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    thread_data_ptr thread_data_null;
    this->session_thread_data_map[boost::this_thread::get_id()] = thread_data_null;
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);

    cout << "[496]--------------------------------------------- " << endl;
    // unit_test[496] thread_id対応のsession_thread_dataなし場合,status = FINALIZE
    this->session_thread_data_map.clear();
    thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
    thread_data->thread_id = thread_down.get_id();
    this->session_thread_data_map.insert(pair<boost::thread::id,
                thread_data_ptr> (thread_down.get_id(),
                thread_data));
    this->session_thread_data_map[thread_down.get_id()]->target_endpoint
                        = string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.243:8800");
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_OK;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[thread_down.get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[thread_down.get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_enable(thread_up.get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);
    this->session_thread_data_map[thread_down.get_id()]->receive_data_map.clear();

    cout << "[497]--------------------------------------------- " << endl;
    // unit_test[497] 上りスレッドの戻り値が「ACCEPT」を設定する、下りスレッドの戻り値が「CLIENT_CONNECTION_CHECK」を設定する
    // unit_test[497] test data:上りスレッドと下りスレッドの場合
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    thread_data_ptr thread_data_up(new session_thread_data_sessionless);
    // 上りスレッド,accept完了フラグOFFの場合
    thread_data_up->thread_division = THREAD_DIVISION_UP_STREAM;
    thread_data_up->accept_end_flag = ACCEPT_END_FLAG_OFF;
    thread_data_up->thread_id = thread_up.get_id();
    thread_data_up->pair_thread_id = thread_down.get_id();
    thread_data_up->receive_data_map[thread_data_up->client_endpoint_tcp];
    // 下りスレッド,sorry状態の場合,送信可能データありの場合
    thread_data_ptr thread_data_down(new session_thread_data_sessionless);
    thread_data_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
    thread_data_down->thread_id = thread_down.get_id();
    thread_data_down->pair_thread_id = thread_up.get_id();
    thread_data_down->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    thread_data_down->receive_data_map[thread_data_down->target_endpoint].send_status_list.push_back(sendstatus);

    this->session_thread_data_map[thread_up.get_id()] = thread_data_up;
    this->session_thread_data_map[thread_down.get_id()] = thread_data_down;
    boost::thread_group threads;
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorry_enable_test_thread_func,
                                   this,
                                   thread_up.get_id(),
                                   ACCEPT));
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorry_enable_test_thread_func,
                                   this,
                                   thread_down.get_id(),
                                   CLIENT_CONNECTION_CHECK));
    threads.join_all();
    // sorry状態をON
    BOOST_CHECK_EQUAL(thread_data_up->sorry_flag, SORRY_FLAG_ON);
}
void handle_sorry_enable_test_thread_func(const boost::thread::id thread_id,
                            EVENT_TAG check_value){
    EVENT_TAG ret = this->handle_sorry_enable(thread_id);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, check_value);
    }
}

void handle_sorry_enable_test_thread(){

    EVENT_TAG status;
    send_status sendstatus;
    receive_data receivedata;
    thread_data_ptr thread_data(new session_thread_data_sessionless);

    // 上りスレッドの場合
    thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
    thread_data->thread_id = boost::this_thread::get_id();
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
    }

    cout << "[498]--------------------------------------------- " << endl;
    // unit_test[498] accept完了フラグON,sorry状態,送信可能データあり list 1件の場合,status = SORRYSERVER_CONNECT
    this->session_thread_data_map[boost::this_thread::get_id()]->accept_end_flag = ACCEPT_END_FLAG_ON;
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_enable(boost::this_thread::get_id());
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(status, SORRYSERVER_CONNECT);
    }

}

//handle_sorry_disable 馬翠翠
void handle_sorry_disable_test(){
    EVENT_TAG status;
    send_status sendstatus;
    receive_data receivedata;
    thread_data_ptr thread_data(new session_thread_data_sessionless);
    boost::thread thread_up(&up_thread_func);
    boost::thread thread_down(&down_thread_func);

    // 上りスレッドの場合
    thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
    thread_data->thread_id = boost::this_thread::get_id();
    this->session_thread_data_map.insert(pair<boost::thread::id,
                thread_data_ptr> (boost::this_thread::get_id(),
                thread_data));
    this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp
                    = string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.109:8800");

    cout << "[499]--------------------------------------------- " << endl;
    // unit_test[499] 上りスレッド対応のendpointなし,status = FINALIZE
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.1:8800"),
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();


    cout << "[500]--------------------------------------------- " << endl;
    // unit_test[500] accept完了フラグOFFの場合,sorry状態をOFF,status = ACCEPT
    this->session_thread_data_map[boost::this_thread::get_id()]->accept_end_flag = ACCEPT_END_FLAG_OFF;
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_OFF);
    BOOST_CHECK_EQUAL(status, ACCEPT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[501]--------------------------------------------- " << endl;
    // unit_test[501] 終了フラグをON,status = SORRYSERVER_DISCONNECT
    // unit_test[501] test data:accept完了フラグONの場合,sorry状態の場合,送信継続データあり list 1件
    this->session_thread_data_map[boost::this_thread::get_id()]->accept_end_flag = ACCEPT_END_FLAG_ON;
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[502]--------------------------------------------- " << endl;
    // unit_test[502] 送信継続データあり list 2件 1件あり場合,終了フラグをON,status = SORRYSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[503]--------------------------------------------- " << endl;
    // unit_test[503] 送信継続データなし list 3件の場合,realserver切替中,sorry状態をOFF,status = SORRYSERVER_DISCONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_OK;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_switch_flag, REALSERVER_SWITCH_FLAG_ON);
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_OFF);
    BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[504]--------------------------------------------- " << endl;
    // unit_test[504] sorry状態以外,送信可能データあり list 1件の場合,status = REALSERVER_CONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[505]--------------------------------------------- " << endl;
    // unit_test[505] 送信可能データあり list 2件 1件ありの場合,status = REALSERVER_CONNECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, REALSERVER_CONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[506]--------------------------------------------- " << endl;
    // unit_test[506] 送信可能データなし list 3件の場合,status = REALSERVER_SELECT
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    // 下りスレッドの場合
    this->session_thread_data_map.clear();
    thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
    thread_data->thread_id = boost::this_thread::get_id();
    this->session_thread_data_map.insert(pair<boost::thread::id,
                thread_data_ptr> (boost::this_thread::get_id(),
                thread_data));
    this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint
                    = string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.109:8800");


    cout << "[507]--------------------------------------------- " << endl;
    // unit_test[507] 終了フラグをON,sorry状態をOFF,realserver切替中,status = SORRYSERVER_DISCONNECT
    // unit_test[507] test data:sorry状態の場合,送信不可データあり list 1件
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                std::pair<boost::asio::ip::tcp::endpoint, receive_data>(
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_OFF);
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->realserver_switch_flag, REALSERVER_SWITCH_FLAG_ON);
    BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[508]--------------------------------------------- " << endl;
    // unit_test[508] 終了フラグをON,status = SORRYSERVER_DISCONNECT
    // unit_test[508] test data:送信データ残サイズ ＞ ０　が存在する場合 list 2件
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 20;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
    BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[509]--------------------------------------------- " << endl;
    // unit_test[509] sorry状態をOFF
    // unit_test[509] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合 list 1件
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_OFF);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[510]--------------------------------------------- " << endl;
    // unit_test[510] sorry状態をOFF,送信可能データなし status = REALSERVER_RECV
    // unit_test[510] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合,送信可能データなし list 1件
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[511]--------------------------------------------- " << endl;
    // unit_test[511] status = CLIENT_CONNECTION_CHECK
    // unit_test[511] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合,送信可能データあり list 3件
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_CONTINUE;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_END;
    sendstatus.send_rest_size = 0;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[512]--------------------------------------------- " << endl;
    // unit_test[512] sorry状態以外,送信可能データあり list 1件の場合,status = CLIENT_CONNECTION_CHECK
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[513]--------------------------------------------- " << endl;
    // unit_test[513] 送信可能データあり list 2件 1件ありの場合,status = CLIENT_CONNECTION_CHECK
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[514]--------------------------------------------- " << endl;
    // unit_test[514] 送信可能データなし list 3件の場合,status =REALSERVER_RECV
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_END;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    sendstatus.status = SEND_NG;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[boost::this_thread::get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[515]--------------------------------------------- " << endl;
    // unit_test[515] 下りスレッドのendpoint対応のreceive_dataなし場合,sorry状態をOFF,status = REALSERVER_RECV
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.1:8800"),
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->sorry_flag, SORRY_FLAG_OFF);
    BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
    this->session_thread_data_map[boost::this_thread::get_id()]->receive_data_map.clear();

    cout << "[516]--------------------------------------------- " << endl;
    // unit_test[516] session_thread_data NULLの場合,status = FINALIZE
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    thread_data_ptr thread_data_null;
    this->session_thread_data_map[boost::this_thread::get_id()] = thread_data_null;
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);

    cout << "[517]--------------------------------------------- " << endl;
    // unit_test[517] thread_id対応のsession_thread_dataなし場合,status =FINALIZE
    this->session_thread_data_map.clear();
    thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
    thread_data->thread_id = thread_down.get_id();
    this->session_thread_data_map.insert(pair<boost::thread::id,
                thread_data_ptr> (thread_down.get_id(),
                thread_data));
    this->session_thread_data_map[thread_down.get_id()]->target_endpoint
                        = string_to_endpoint<boost::asio::ip::tcp> ("192.168.120.109:8800");
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    sendstatus.status = SEND_OK;
    receivedata.send_status_list.push_back(sendstatus);
    this->session_thread_data_map[thread_down.get_id()]->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        this->session_thread_data_map[thread_down.get_id()]->target_endpoint,
                        receivedata));
    status = this->handle_sorry_disable(thread_up.get_id());
    BOOST_CHECK_EQUAL(status, FINALIZE);
    this->session_thread_data_map[thread_down.get_id()]->receive_data_map.clear();

    cout << "[518]--------------------------------------------- " << endl;
    // unit_test[518] 上りスレッドの戻り値が「ACCEPT」を設定する、下りスレッドの戻り値が「CLIENT_CONNECTION_CHECK」を設定する
    // unit_test[518] test data:上りスレッドと下りスレッドの場合
    init_send_status(sendstatus);
    init_receive_data(receivedata);
    thread_data_ptr thread_data_up(new session_thread_data_sessionless);
    // 上りスレッド,accept完了フラグOFFの場合
    thread_data_up->thread_division = THREAD_DIVISION_UP_STREAM;
    thread_data_up->accept_end_flag = ACCEPT_END_FLAG_OFF;
    thread_data_up->thread_id = thread_up.get_id();
    thread_data_up->pair_thread_id = thread_down.get_id();
    thread_data_up->receive_data_map[thread_data_up->client_endpoint_tcp];
    // 下りスレッド,sorry状態以外の場合,送信可能データありの場合
    thread_data_ptr thread_data_down(new session_thread_data_sessionless);
    thread_data_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
    thread_data_down->thread_id = thread_down.get_id();
    thread_data_down->pair_thread_id = thread_up.get_id();
    thread_data_down->sorry_flag = SORRY_FLAG_OFF;
    sendstatus.status = SEND_OK;
    sendstatus.send_possible_size = 10;
    thread_data_down->receive_data_map[thread_data_down->target_endpoint].send_status_list.push_back(sendstatus);

    this->session_thread_data_map[thread_up.get_id()] = thread_data_up;
    this->session_thread_data_map[thread_down.get_id()] = thread_data_down;
    boost::thread_group threads;
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorry_disable_test_thread_func,
                                   this,
                                   thread_up.get_id(),
                                   ACCEPT));
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorry_disable_test_thread_func,
                                   this,
                                   thread_down.get_id(),
                                   CLIENT_CONNECTION_CHECK));
    threads.join_all();
    // sorry状態をOFF
    BOOST_CHECK_EQUAL(thread_data_up->sorry_flag, SORRY_FLAG_OFF);
}


void handle_sorry_disable_test_thread_func(const boost::thread::id thread_id,
                            EVENT_TAG check_value){
    EVENT_TAG ret = this->handle_sorry_disable(thread_id);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, check_value);
    }
}

void handle_sorry_disable_test_thread(){

    EVENT_TAG status;
    send_status sendstatus;
    receive_data receivedata;
    thread_data_ptr thread_data(new session_thread_data_sessionless);

    // 上りスレッドの場合
    thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
    thread_data->thread_id = boost::this_thread::get_id();
    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
    }

    cout << "[519]--------------------------------------------- " << endl;
    // unit_test[519] 終了フラグをON,status = SORRYSERVER_DISCONNECT
    // unit_test[519] test data:accept完了フラグONの場合,sorry状態の場合,送信継続データあり list 1件
    thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
    thread_data->sorry_flag = SORRY_FLAG_ON;
    sendstatus.status = SEND_CONTINUE;
    receivedata.send_status_list.push_back(sendstatus);
    thread_data->receive_data_map.insert(
                pair<boost::asio::ip::tcp::endpoint, receive_data> (
                        thread_data->client_endpoint_tcp,
                        receivedata));
    status = this->handle_sorry_disable(boost::this_thread::get_id());
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->end_flag, END_FLAG_ON);
        BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
    }
}

//handle_realserver_disconnect(tcp) 馮家純
void handle_realserver_disconnect_tcp_test(){
    EVENT_TAG ret;
    cout << "[520]--------------------------------------------- " << endl;
    //unit_test[520] 異常系 上りスレッドsession_thread_data_map中にThreadID対応のデータがない
    this->session_thread_data_map[boost::this_thread::get_id()].reset();
    boost::asio::ip::tcp::endpoint ep_err;
    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[521]--------------------------------------------- " << endl;
    //unit_test[521] 異常系 上りスレッドsession_thread_data_map中にThreadIDなしの場合のテスト
    this->session_thread_data_map.clear();
    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[522]--------------------------------------------- " << endl;
    //unit_test[522] 異常系 下りスレッドsession_thread_data_map中にThreadID対応のデータがない
    boost::thread t_err(down_thread_func);
    this->session_thread_data_map[t_err.get_id()].reset();
    ret = this->handle_realserver_disconnect(t_err.get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[523]--------------------------------------------- " << endl;
    //unit_test[523] 異常系 下りスレッドsession_thread_data_map中にThreadIDなしの場合のテスト
    this->session_thread_data_map.clear();
    ret = this->handle_realserver_disconnect(t_err.get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[524]--------------------------------------------- " << endl;
    //unit_test[524] 上りスレッドの場合->終了フラグがONの場合
    boost::asio::ip::tcp::endpoint ep1, ep_d1;

    thread_data_ptr data1(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data1;
    //make 上りスレッドの場合 0->上りスレッド
    data1->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがONの場合 1->on
    data1->end_flag = END_FLAG_ON;

    data1->receive_data_map[ep_d1];

    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep1);
    //遷移先ステータスを設定する status = CLIENT_RECV
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[525]--------------------------------------------- " << endl;
    //unit_test[525] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中の場合->送信可能データあり
    this->session_thread_data_map.clear();

    boost::asio::ip::tcp::endpoint ep2;

    thread_data_ptr data2(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data2;
    //make 上りスレッドの場合 0->上りスレッド
    data2->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data2->end_flag = END_FLAG_OFF;
    //make sorryserver切替中の場合 1->切替中
    data2->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
    //make 送信可能データあり
    send_status st2;
    st2.status = SEND_OK;
    st2.send_possible_size = 100u;
    data2->receive_data_map[ep2].send_status_list.push_back(st2);


    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep2);
    //遷移先ステータスを設定する status = SORRYSERVER_SELECT
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);

    cout << "[526]--------------------------------------------- " << endl;
    //unit_test[526] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中の場合->送信可能データなし
    this->session_thread_data_map.clear();

    boost::asio::ip::tcp::endpoint ep3;

    thread_data_ptr data3(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data3;
    //make 上りスレッドの場合 0->上りスレッド
    data3->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data3->end_flag = END_FLAG_OFF;
    //make sorryserver切替中の場合 1->切替中
    data3->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
    //make 送信可能データなし
    send_status st3;
    //send_possible=0
    st3.send_possible_size = 0u;
    data3->receive_data_map[ep3].send_status_list.push_back(st3);

    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep3);
    //遷移先ステータスを設定する status = SORRYSERVER_SELECT
    BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);

    cout << "[527]--------------------------------------------- " << endl;
    //unit_test[527] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中でない場合
    this->session_thread_data_map.clear();

    boost::asio::ip::tcp::endpoint ep4, ep_d4;

    thread_data_ptr data4(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data4;
    //make 上りスレッドの場合 0->上りスレッド
    data4->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data4->end_flag = END_FLAG_OFF;
    //make sorryserver切替中でない場合 0->切替中でない
    data4->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;

    data4->receive_data_map[ep_d4];

    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep4);
    //終了フラグをON
    BOOST_CHECK_EQUAL(data4->end_flag, END_FLAG_ON);
    //遷移先ステータスを設定する status = CLIENT_RECV
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[528]--------------------------------------------- " << endl;
    //unit_test[528] 下りスレッドの場合->送信可能データあり
    this->session_thread_data_map.clear();

    boost::thread t5(down_thread_func);
    boost::asio::ip::tcp::endpoint ep5;

    thread_data_ptr data5(new session_thread_data_sessionless);
    this->session_thread_data_map[t5.get_id()] = data5;
    //make 下りスレッドの場合 1->下りスレッド
    data5->thread_division = THREAD_DIVISION_DOWN_STREAM;
    data5->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
    //make 送信可能データあり
    send_status st5;
    st5.status = SEND_OK;
    //send_possible>0
    st5.send_possible_size = 100u;
    data5->receive_data_map[ep5].send_status_list.push_back(st5);

    ret = this->handle_realserver_disconnect(t5.get_id(), ep5);
    //終了フラグをON
    BOOST_CHECK_EQUAL(data5->end_flag, END_FLAG_ON);
    //遷移先ステータスを設定する status = CLIENT_CONNECTION_CHECK
    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    cout << "[529]--------------------------------------------- " << endl;
    //unit_test[529] 下りスレッドの場合->送信可能データなし
    this->session_thread_data_map.clear();

    boost::thread t6(down_thread_func);
    boost::asio::ip::tcp::endpoint ep6;

    thread_data_ptr data6(new session_thread_data_sessionless);
    this->session_thread_data_map[t6.get_id()] = data6;
    //make 下りスレッドの場合 1->下りスレッド
    data6->thread_division = THREAD_DIVISION_DOWN_STREAM;
    //make 送信可能データなし
    send_status st6;
    //send_possible>0
    st6.send_possible_size = 0u;
    data6->receive_data_map[ep6].send_status_list.push_back(st6);

    ret = this->handle_realserver_disconnect(t6.get_id(), ep6);
    //終了フラグをON
    BOOST_CHECK_EQUAL(data6->end_flag, END_FLAG_ON);
    //遷移先ステータスを設定する status = CLIENT_DISCONNECT
    BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);

    cout << "[530]--------------------------------------------- " << endl;
    //unit_test[530] 上りスレッドと下りスレッドの場合
    this->session_thread_data_map.clear();

    boost::thread tdown_for_get_id(down_thread_func);

    boost::asio::ip::tcp::endpoint epup;
    boost::asio::ip::tcp::endpoint epdown;

    //上りスレッドの場合->終了フラグがONの場合
    thread_data_ptr dataup(new session_thread_data_sessionless);
    dataup->thread_id = boost::this_thread::get_id();
    dataup->thread_division = THREAD_DIVISION_UP_STREAM;
    dataup->pair_thread_id = tdown_for_get_id.get_id();
    dataup->end_flag = END_FLAG_ON;
    dataup->receive_data_map[epup];

    // 下りスレッドの場合->送信可能データあり
    thread_data_ptr datadown(new session_thread_data_sessionless);
    datadown->thread_id = tdown_for_get_id.get_id();
    datadown->thread_division = THREAD_DIVISION_DOWN_STREAM;
    datadown->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_OFF;
    datadown->pair_thread_id = boost::this_thread::get_id();
    //make 送信可能データあり
    send_status st;
    st.status = SEND_OK;
    st.send_possible_size = 100u;
    datadown->receive_data_map[epdown].send_status_list.push_back(st);

    this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
    this->session_thread_data_map[tdown_for_get_id.get_id()] = datadown;

    boost::thread_group threads;
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_disconnect_test_thread_func,
                                   this,
                                   boost::this_thread::get_id(),
                                   epup,
                                   CLIENT_RECV));
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_disconnect_test_thread_func,
                                   this,
                                   tdown_for_get_id.get_id(),
                                   epdown,
                                   CLIENT_CONNECTION_CHECK));
    threads.join_all();

    //終了フラグをON
    BOOST_CHECK_EQUAL(datadown->end_flag, END_FLAG_ON);

}

void handle_realserver_disconnect_test_thread_func(const boost::thread::id thread_id,
                            const boost::asio::ip::tcp::endpoint & rs_endpoint,
                            EVENT_TAG check_value){
    EVENT_TAG ret = this->handle_realserver_disconnect(thread_id, rs_endpoint);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, check_value);
    }
}

void handle_realserver_disconnect_tcp_test_thread(){
    EVENT_TAG ret;
    cout << "[531]--------------------------------------------- " << endl;
    //unit_test[531] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中の場合->送信可能データなし

    boost::asio::ip::tcp::endpoint ep3;

    thread_data_ptr data3(new session_thread_data_sessionless);

    //make 上りスレッドの場合 0->上りスレッド
    data3->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data3->end_flag = END_FLAG_OFF;
    //make sorryserver切替中の場合 1->切替中
    data3->sorryserver_switch_flag = SORRYSERVER_SWITCH_FLAG_ON;
    //make 送信可能データなし
    send_status st3;
    //send_possible=0
    st3.send_possible_size = 0u;
    data3->receive_data_map[ep3].send_status_list.push_back(st3);

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data3;
    }

    ret = this->handle_realserver_disconnect(boost::this_thread::get_id(), ep3);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        //遷移先ステータスを設定する status = SORRYSERVER_SELECT
        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
    }
}

//handle_sorryserver_disconnect 馮家純
void handle_sorryserver_disconnect_test(){
    EVENT_TAG ret;
    cout << "[532]--------------------------------------------- " << endl;
    //unit_test[532] 異常系 上りスレッドsession_thread_data_map中にThreadID対応のデータなし
    this->session_thread_data_map[boost::this_thread::get_id()].reset();
    boost::asio::ip::tcp::endpoint ep_err;
    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[533]--------------------------------------------- " << endl;
    //unit_test[533] 異常系 上りスレッドsession_thread_data_map中にThreadIDなしの場合のテスト
    this->session_thread_data_map.clear();
    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[534]--------------------------------------------- " << endl;
    //unit_test[534] 異常系 下りスレッドsession_thread_data_map中にThreadID対応のデータなし
    boost::thread t_err(down_thread_func);
    this->session_thread_data_map[t_err.get_id()].reset();
    ret = this->handle_sorryserver_disconnect(t_err.get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[535]--------------------------------------------- " << endl;
    //unit_test[535] 異常系 下りスレッドsession_thread_data_map中にThreadIDなしの場合のテスト
    this->session_thread_data_map.clear();
    ret = this->handle_sorryserver_disconnect(t_err.get_id(), ep_err);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[536]--------------------------------------------- " << endl;
    //unit_test[536] 異常系 receive_data_mapにendpointなしの場合のテスト
    boost::asio::ip::tcp::endpoint ep1, ep_d1;

    thread_data_ptr data1(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data1;
    //make 上りスレッドの場合 0->上りスレッド
    data1->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがONの場合 1->on
    data1->end_flag = END_FLAG_ON;

    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep1);
    //遷移先ステータスを設定する status = FINALIZE
    BOOST_CHECK_EQUAL(ret, FINALIZE);

    cout << "[537]--------------------------------------------- " << endl;
    //unit_test[537] 上りスレッドの場合->終了フラグがONの場合
    data1->receive_data_map[ep_d1];
    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep1);
    //遷移先ステータスを設定する status = CLIENT_RECV
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[538]--------------------------------------------- " << endl;
    //unit_test[538] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中の場合->送信可能データあり
    this->session_thread_data_map.clear();

    boost::asio::ip::tcp::endpoint ep2;

    thread_data_ptr data2(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data2;
    //make 上りスレッドの場合 0->上りスレッド
    data2->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data2->end_flag = END_FLAG_OFF;
    //make realserver切替中の場合 1->切替中
    data2->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
    //make 送信可能データあり
    send_status st2;
    st2.status = SEND_OK;
    //send_possible>0
    st2.send_possible_size = 100u;
    data2->receive_data_map[ep2].send_status_list.push_back(st2);

    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep2);
    //遷移先ステータスを設定する status = REALSERVER_SELECT
    BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);

    cout << "[539]--------------------------------------------- " << endl;
    //unit_test[539] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中の場合->送信可能データなし
    this->session_thread_data_map.clear();

    boost::asio::ip::tcp::endpoint ep3;

    thread_data_ptr data3(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data3;
    //make 上りスレッドの場合 0->上りスレッド
    data3->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data3->end_flag = END_FLAG_OFF;
    //make realserver切替中の場合 1->切替中
    data3->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
    //make 送信可能データなし
    send_status st3;
    //send_possible=0
    st3.send_possible_size = 0u;
    data3->receive_data_map[ep3].send_status_list.push_back(st3);

    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep3);
    //遷移先ステータスを設定する status = REALSERVER_SELECT
    BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);

    cout << "[540]--------------------------------------------- " << endl;
    //unit_test[540] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中でない場合
    this->session_thread_data_map.clear();

    boost::asio::ip::tcp::endpoint ep4, ep_d4;

    thread_data_ptr data4(new session_thread_data_sessionless);
    this->session_thread_data_map[boost::this_thread::get_id()] = data4;
    //make 上りスレッドの場合 0->上りスレッド
    data4->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data4->end_flag = 0;
    //make realserver切替中でない場合 0->切替中でない
    data4->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;

    data4->receive_data_map[ep_d4];

    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep4);
    //終了フラグをON
    BOOST_CHECK_EQUAL(data4->end_flag, END_FLAG_ON);
    //遷移先ステータスを設定する status = CLIENT_RECV
    BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

    cout << "[541]--------------------------------------------- " << endl;
    //unit_test[541] 下りスレッドの場合->送信可能データあり
    this->session_thread_data_map.clear();

    boost::thread t5(down_thread_func);
    boost::asio::ip::tcp::endpoint ep5;

    thread_data_ptr data5(new session_thread_data_sessionless);
    this->session_thread_data_map[t5.get_id()] = data5;
    //make 下りスレッドの場合 1->下りスレッド
    data5->thread_division = THREAD_DIVISION_DOWN_STREAM;
    data5->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
    data5->end_flag = END_FLAG_OFF;
    //make 送信可能データあり
    send_status st5;
    st5.status = SEND_OK;
    //send_possible>0
    st5.send_possible_size = 100u;
    data5->receive_data_map[ep5].send_status_list.push_back(st5);

    ret = this->handle_sorryserver_disconnect(t5.get_id(), ep5);
    //終了フラグをON
    BOOST_CHECK_EQUAL(data5->end_flag, END_FLAG_ON);
    //遷移先ステータスを設定する status = CLIENT_CONNECTION_CHECK
    BOOST_CHECK_EQUAL(ret, CLIENT_CONNECTION_CHECK);

    cout << "[542]--------------------------------------------- " << endl;
    //unit_test[542] 下りスレッドの場合->送信可能データなし
    this->session_thread_data_map.clear();

    boost::thread t6(down_thread_func);
    boost::asio::ip::tcp::endpoint ep6;

    thread_data_ptr data6(new session_thread_data_sessionless);
    this->session_thread_data_map[t6.get_id()] = data6;
    //make 下りスレッドの場合 1->下りスレッド
    data6->thread_division = THREAD_DIVISION_DOWN_STREAM;
    data6->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
    data6->end_flag = END_FLAG_OFF;
    //make 送信可能データなし
    send_status st6;
    //send_possible=0
    st6.send_possible_size = 0u;
    data6->receive_data_map[ep6].send_status_list.push_back(st6);

    ret = this->handle_sorryserver_disconnect(t6.get_id(), ep6);
    //終了フラグをON
    BOOST_CHECK_EQUAL(data6->end_flag, END_FLAG_ON);
    //遷移先ステータスを設定する status = CLIENT_DISCONNECT
    BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);

    cout << "[543]--------------------------------------------- " << endl;
    //unit_test[543] 上りスレッドと下りスレッドの場合
    this->session_thread_data_map.clear();

    boost::thread tdown_for_get_id(down_thread_func);

    boost::asio::ip::tcp::endpoint epup;
    boost::asio::ip::tcp::endpoint epdown;

    //上りスレッドの場合->終了フラグがONの場合
    thread_data_ptr dataup(new session_thread_data_sessionless);
    dataup->thread_id = boost::this_thread::get_id();
    dataup->thread_division = THREAD_DIVISION_UP_STREAM;
    dataup->pair_thread_id = tdown_for_get_id.get_id();
    dataup->end_flag = END_FLAG_ON;
    dataup->receive_data_map[epup];

    // 下りスレッドの場合->送信可能データあり
    thread_data_ptr datadown(new session_thread_data_sessionless);
    datadown->thread_id = tdown_for_get_id.get_id();
    datadown->thread_division = THREAD_DIVISION_DOWN_STREAM;
    datadown->realserver_switch_flag = REALSERVER_SWITCH_FLAG_OFF;
    datadown->pair_thread_id = boost::this_thread::get_id();
    //make 送信可能データあり
    send_status st;
    st.status = SEND_OK;
    st.send_possible_size = 100u;
    datadown->receive_data_map[epdown].send_status_list.push_back(st);

    this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
    this->session_thread_data_map[tdown_for_get_id.get_id()] = datadown;

    boost::thread_group threads;
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_disconnect_test_thread_func,
                                   this,
                                   boost::this_thread::get_id(),
                                   epup,
                                   CLIENT_RECV));
    threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_disconnect_test_thread_func,
                                   this,
                                   tdown_for_get_id.get_id(),
                                   epdown,
                                   CLIENT_CONNECTION_CHECK));
    threads.join_all();

    //終了フラグをON
    BOOST_CHECK_EQUAL(datadown->end_flag, END_FLAG_ON);

}

void handle_sorryserver_disconnect_test_thread_func(const boost::thread::id thread_id,
                                const boost::asio::ip::tcp::endpoint & rs_endpoint, EVENT_TAG check_value){
    EVENT_TAG ret = this->handle_sorryserver_disconnect(thread_id, rs_endpoint);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        BOOST_CHECK_EQUAL(ret, check_value);
    }
}

void handle_sorryserver_disconnect_test_thread(){
    cout << "[544]--------------------------------------------- " << endl;
    //unit_test[544] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中の場合->送信可能データなし
    EVENT_TAG ret;
    boost::asio::ip::tcp::endpoint ep3;

    thread_data_ptr data3(new session_thread_data_sessionless);

    //make 上りスレッドの場合 0->上りスレッド
    data3->thread_division = THREAD_DIVISION_UP_STREAM;
    //make 終了フラグがOFFの場合 0->off
    data3->end_flag = END_FLAG_OFF;
    //make realserver切替中の場合 1->切替中
    data3->realserver_switch_flag = REALSERVER_SWITCH_FLAG_ON;
    //make 送信可能データなし
    send_status st3;
    //send_possible=0
    st3.send_possible_size = 0u;
    data3->receive_data_map[ep3].send_status_list.push_back(st3);

    {
        boost::mutex::scoped_lock sclock(this->session_thread_data_map_mutex);
        this->session_thread_data_map[boost::this_thread::get_id()] = data3;
    }

    ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep3);
    {
        boost::mutex::scoped_lock sclock(check_mutex);
        //遷移先ステータスを設定する status = REALSERVER_SELECT
        BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);
    }
}

//handle_realserver_close 馮家純
void handle_realserver_close_test(){
    cout << "[545]--------------------------------------------- " << endl;
    //unit_test[545] handle_realserver_close 戻り値が「STOP」に設定する。
    boost::asio::ip::udp::endpoint ep;
    EVENT_TAG ret = this->handle_realserver_close(boost::this_thread::get_id(), ep);
    BOOST_CHECK_EQUAL(ret, STOP);
}

void get_option_info_test(){
    {
        cout << "[546]--------------------------------------------- " << endl;
        //unit_test[546] forwarded_forが0、sorry_uriが""の場合
        this->forwarded_for = 0;
        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
        std::string option;
        this->get_option_info(option);
        BOOST_CHECK_EQUAL(strcmp(option.c_str(), "--sorry-uri ''"), 0);
    }
    {
        cout << "[547]--------------------------------------------- " << endl;
        //unit_test[547] forwarded_forが1、sorry_uriが"/sorry"の場合
        this->forwarded_for = 1;
        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
        strcpy(this->sorry_uri.c_array(), "/sorry");
        std::string option;
        this->get_option_info(option);
        BOOST_CHECK_EQUAL(strcmp(option.c_str(), "--forwarded-for --sorry-uri '/sorry'"), 0);
    }
}

};

/***********************************************************************************
 *以下はBOOST_TEST_SUITEに入れる関数
 ***********************************************************************************/
//protocol_module_sessionless 馮家純
void protocol_module_sessionless_test(){
    protocol_module_sessionless_test_class obj;
    obj.protocol_module_sessionless_test();
}

//is_tcp 馮家純
void is_tcp_test(){
    protocol_module_sessionless_test_class obj;
    obj.is_tcp_test();
}

//is_udp 馮家純
void is_udp_test(){
    protocol_module_sessionless_test_class obj;
    obj.is_udp_test();
}

//get_name 馮家純
void get_name_test(){
    protocol_module_sessionless_test_class obj;
    obj.get_name_test();
}

//initialize 董作方
void initialize_test() {
    protocol_module_sessionless_test_class obj;
    obj.initialize_test();
}

//finalize 董作方
void finalize_test() {
    protocol_module_sessionless_test_class obj;
    obj.finalize_test();
}

//is_use_sorry 馬翠翠
void is_use_sorry_test() {
    protocol_module_sessionless_test_class obj;
    obj.is_use_sorry_test();
}

//check_parameter 馬翠翠
void check_parameter_test() {
    protocol_module_sessionless_test_class obj;
    obj.check_parameter_test();
}

//set_parameter 馬翠翠
void set_parameter_test() {
    protocol_module_sessionless_test_class obj;
    obj.set_parameter_test();
}

//add_parameter 馬翠翠
void add_parameter_test() {
    protocol_module_sessionless_test_class obj;
    obj.add_parameter_test();
}

//register_schedule(tcp) 董作方
void register_schedule_tcp_test() {
    protocol_module_sessionless_test_class obj;
    obj.register_schedule_tcp_test();
}

//handle_session_initialize 董作方
void handle_session_initialize_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_session_initialize_test();
}

//handle_session_initialize 董作方
void handle_session_initialize_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_session_initialize_test_thread,&obj));
    }
    threads.join_all();
}

//handle_session_finalize 董作方
void handle_session_finalize_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_session_finalize_test();
}

//handle_session_finalize 董作方
void handle_session_finalize_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_session_finalize_test_thread,&obj));
    }
    threads.join_all();
}

//handle_accept 董作方
void handle_accept_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_accept_test();
}

//handle_accept 董作方
void handle_accept_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_accept_test_thread,&obj));
    }
    threads.join_all();
}

//handle_client_recv 郎希倹
void handle_client_recv_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_client_recv_test();
}

//handle_client_recv_thread 郎希倹
void handle_client_recv_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_client_recv_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_select(tcp) 馮家純
void handle_realserver_select_tcp_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_select_tcp_test();
}

//handle_realserver_select(tcp)_thread 馮家純
void handle_realserver_select_tcp_test_thread(){
    protocol_module_sessionless_test_class obj;

    //register function
    protocol_module_sessionless_test_class::tcp_schedule_func_type func = &schedule_tcp_determinate;

    obj.register_schedule(func);

    boost::thread_group threads;
    for(int i = 0; i < THREAD_COUNT; ++i){
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_select_tcp_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_select(udp) 馮家純
void handle_realserver_select_udp_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_select_udp_test();
}

//handle_realserver_connect 郎希倹
void handle_realserver_connect_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_connect_test();
}

//handle_realserver_connect_thread 郎希倹
void handle_realserver_connect_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_connect_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_connection_fail 馮家純
void handle_realserver_connection_fail_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_connection_fail_test();
}

void handle_realserver_connection_fail_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i = 0; i < THREAD_COUNT; ++i){
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_connection_fail_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_send 董作方
void handle_realserver_send_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_send_test();
}

//handle_realserver_send 董作方
void handle_realserver_send_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_send_test_thread,&obj));
    }
    threads.join_all();
}

//handle_sorryserver_select 馮家純
void handle_sorryserver_select_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_sorryserver_select_test();
}

void handle_sorryserver_select_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i = 0; i < THREAD_COUNT; ++i){
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_select_test_thread, &obj));
    }
    threads.join_all();
}

//handle_sorryserver_connect 郎希倹
void handle_sorryserver_connect_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_sorryserver_connect_test();
}

//handle_sorryserver_connect_thread 郎希倹
void handle_sorryserver_connect_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_connect_test_thread, &obj));
    }
    threads.join_all();
}

//handle_sorryserver_connection_fail 董作方
void handle_sorryserver_connection_fail_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_sorryserver_connection_fail_test();
}

//handle_sorryserver_connection_fail 董作方
void handle_sorryserver_connection_fail_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_connection_fail_test_thread,&obj));
    }
    threads.join_all();
}

//handle_sorryserver_send 郎希倹
void handle_sorryserver_send_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_sorryserver_send_test();
}

//handle_sorryserver_send 郎希倹
void handle_sorryserver_send_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_send_test_thread,&obj));
    }
    threads.join_all();
}

//handle_realserver_recv(tcp) 郎希倹
void handle_realserver_recv_tcp_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_recv_tcp_test();
}

//handle_realserver_recv_tcp_test_thread 郎希倹
void handle_realserver_recv_tcp_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_recv_tcp_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_recv(udp) 郎希倹
void handle_realserver_recv_udp_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_recv_udp_test();
}

//handle_sorryserver_recv 郎希倹
void handle_sorryserver_recv_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_sorryserver_recv_test();
}

//handle_sorryserver_recv_test_thread 郎希倹
void handle_sorryserver_recv_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_recv_test_thread, &obj));
    }
    threads.join_all();
}

//handle_response_send_inform 馮家純
void handle_response_send_inform_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_response_send_inform_test();
}

//handle_client_connection_check 馮家純
void handle_client_connection_check_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_client_connection_check_test();
}

void handle_client_connection_check_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i = 0; i < THREAD_COUNT; ++i){
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_client_connection_check_test_thread, &obj));
    }
    threads.join_all();
}

//handle_client_select 馮家純
void handle_client_select_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_client_select_test();
}

//handle_client_send 郎希倹
void handle_client_send_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_client_send_test();
}

//handle_client_send 郎希倹
void handle_client_send_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i=0; i<THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_client_send_test_thread,&obj));
    }
    threads.join_all();
}


//handle_client_disconnect 馮家純
void handle_client_disconnect_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_client_disconnect_test();
}

//handle_sorry_enable 馬翠翠
void handle_sorry_enable_test() {
    protocol_module_sessionless_test_class obj;
    obj.handle_sorry_enable_test();
}

//handle_sorry_enable_test_thread 馬翠翠
void handle_sorry_enable_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorry_enable_test_thread, &obj));
    }
    threads.join_all();
}

//handle_sorry_disable 馬翠翠
void handle_sorry_disable_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_sorry_disable_test();
}

//handle_sorry_disable_test_thread 馬翠翠
void handle_sorry_disable_test_thread() {
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for (int i = 0; i < THREAD_COUNT; i++) {
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorry_disable_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_disconnect(tcp) 馮家純
void handle_realserver_disconnect_tcp_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_disconnect_tcp_test();
}


void handle_realserver_disconnect_tcp_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i = 0; i < THREAD_COUNT; ++i){
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_realserver_disconnect_tcp_test_thread, &obj));
    }
    threads.join_all();
}

//handle_sorryserver_disconnect 馮家純
void handle_sorryserver_disconnect_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_sorryserver_disconnect_test();
}

void handle_sorryserver_disconnect_test_thread(){
    protocol_module_sessionless_test_class obj;
    boost::thread_group threads;
    for(int i = 0; i < THREAD_COUNT; ++i){
        threads.create_thread(bind(&protocol_module_sessionless_test_class::handle_sorryserver_disconnect_test_thread, &obj));
    }
    threads.join_all();
}

//handle_realserver_close 馮家純
void handle_realserver_close_test(){
    protocol_module_sessionless_test_class obj;
    obj.handle_realserver_close_test();
}

void get_option_info_test(){
    protocol_module_sessionless_test_class obj;
    obj.get_option_info_test();
}


test_suite*    init_unit_test_suite( int argc, char* argv[] ){

    test_suite* ts = BOOST_TEST_SUITE( "sessionless test" );

    ts->add(BOOST_TEST_CASE(&protocol_module_sessionless_test));
    ts->add(BOOST_TEST_CASE(&is_tcp_test));
    ts->add(BOOST_TEST_CASE(&is_udp_test));
    ts->add(BOOST_TEST_CASE(&get_name_test));
    ts->add(BOOST_TEST_CASE(&initialize_test));
    ts->add(BOOST_TEST_CASE(&finalize_test));
    ts->add(BOOST_TEST_CASE(&is_use_sorry_test));
    ts->add(BOOST_TEST_CASE(&check_parameter_test));
    ts->add(BOOST_TEST_CASE(&set_parameter_test));
    ts->add(BOOST_TEST_CASE(&add_parameter_test));
    ts->add(BOOST_TEST_CASE(&register_schedule_tcp_test));
    ts->add(BOOST_TEST_CASE(&handle_session_initialize_test));
    ts->add(BOOST_TEST_CASE(&handle_session_initialize_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_session_finalize_test));
    ts->add(BOOST_TEST_CASE(&handle_session_finalize_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_accept_test));
    ts->add(BOOST_TEST_CASE(&handle_accept_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_client_recv_test));
    ts->add(BOOST_TEST_CASE(&handle_client_recv_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_select_tcp_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_select_tcp_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_select_udp_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_connect_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_connect_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_connection_fail_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_connection_fail_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_send_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_send_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_select_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_select_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_connect_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_connect_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_connection_fail_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_connection_fail_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_send_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_send_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_recv_tcp_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_recv_tcp_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_recv_udp_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_recv_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_recv_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_response_send_inform_test));
    ts->add(BOOST_TEST_CASE(&handle_client_connection_check_test));
    ts->add(BOOST_TEST_CASE(&handle_client_connection_check_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_client_select_test));
    ts->add(BOOST_TEST_CASE(&handle_client_send_test));
    ts->add(BOOST_TEST_CASE(&handle_client_send_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_client_disconnect_test));
    ts->add(BOOST_TEST_CASE(&handle_sorry_enable_test));
    ts->add(BOOST_TEST_CASE(&handle_sorry_enable_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_sorry_disable_test));
    ts->add(BOOST_TEST_CASE(&handle_sorry_disable_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_disconnect_tcp_test));
    ts->add(BOOST_TEST_CASE(&handle_realserver_disconnect_tcp_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_disconnect_test));
    ts->add(BOOST_TEST_CASE(&handle_sorryserver_disconnect_test_thread));
    ts->add(BOOST_TEST_CASE(&handle_realserver_close_test));
    ts->add(BOOST_TEST_CASE(&get_option_info_test));

    framework::master_test_suite().add(ts);
    return 0;
}
