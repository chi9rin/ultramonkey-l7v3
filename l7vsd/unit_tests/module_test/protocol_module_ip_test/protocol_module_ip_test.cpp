/*
 * @file  protocol_module_ip_test.cpp
 * @brief protocol module ip test file.
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
#include "../../../module/protocol/http_utility.cpp"
#include "../../../module/protocol/protocol_module_ip.cpp"
#include "../../../include/replication.h"

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

LOG_LEVEL_TAG    stb_getloglevel()
{
        return LOG_LV_DEBUG;
}
LOG_LEVEL_TAG    stb_getloglevel_is_none()
{
        return LOG_LV_NONE;
}

void    stb_putLogFatal(const unsigned int message_id, const std::string &message, const char *file, int line)
{
//        cout << boost::format( "%s%d%06d %s %s" )
//                    % "PM"
//                    % LOG_LV_FATAL
//                    % message_id
//                    % message.c_str()
//                    % hostname;
//        cout << endl;
}
void    stb_putLogError(const unsigned int message_id, const std::string &message, const char *file, int line)
{
//        cout << boost::format( "%s%d%06d %s %s" )
//                    % "PM"
//                    % LOG_LV_ERROR
//                    % message_id
//                    % message.c_str()
//                    % hostname;
//        cout << endl;
}
void    stb_putLogWarn(const unsigned int message_id, const std::string &message, const char *file, int line)
{
//        cout << boost::format( "%s%d%06d %s %s" )
//                    % "PM"
//                    % LOG_LV_WARN
//                    % message_id
//                    % message.c_str()
//                    % hostname;
//        cout << endl;
}
void    stb_putLogInfo(const unsigned int message_id, const std::string &message, const char *file, int line)
{
//        cout << boost::format( "%s%d%06d %s %s" )
//                    % "PM"
//                    % LOG_LV_INFO
//                    % message_id
//                    % message.c_str()
//                    % hostname;
//        cout << endl;
}
void    stb_putLogDebug(const unsigned int message_id, const std::string &message, const char *file, int line)
{
//        cout << boost::format( "%s%d%06d %s %s" )
//                    % "PM"
//                    % LOG_LV_DEBUG
//                    % message_id
//                    % message.c_str()
//                    % hostname;
//        cout << endl;
}
static int calc_hash_2(const boost::asio::ip::tcp::endpoint &cl_endpoint)
{
        unsigned int hash = 0;
        if (cl_endpoint.address().is_v4()) {
                hash = cl_endpoint.address().to_v4().to_ulong() * GOLDEN_RATIO_PRIME;
        } else {
                boost::asio::ip::address_v6::bytes_type v6_bytes = cl_endpoint.address().to_v6().to_bytes();
                boost::asio::ip::address_v4::bytes_type v4_bytes = {{v6_bytes[12], v6_bytes[13], v6_bytes[14], v6_bytes[15]}};
                boost::asio::ip::address_v4::address_v4 v4_address = boost::asio::ip::address_v4::address_v4(v4_bytes);
                hash = v4_address.to_ulong() * GOLDEN_RATIO_PRIME;
        }
        return hash >> (32 - HASH_TABLE_BITS);
}
//new operator function
static bool new_flg = false;
void new_install()
{
        new_flg = true;
}
void new_uninstall()
{
        new_flg = false;
}
void *operator new[](size_t size)
{
        if (new_flg)
                throw std::bad_alloc();
        else {
                void *p = malloc(size);
                return p;
        }
}
void operator delete[](void *p)
{
        free(p);
}

protocol_module_ip::realserverlist_type::iterator get_rs_list_begin(protocol_module_ip::realserverlist_type *p_rs_list)
{
        return p_rs_list->begin();
}

protocol_module_ip::realserverlist_type::iterator get_rs_list_end(protocol_module_ip::realserverlist_type *p_rs_list)
{
        return p_rs_list->end();
}

protocol_module_ip::realserverlist_type::iterator get_rs_list_next(protocol_module_ip::realserverlist_type::iterator in_itr)
{
        return ++in_itr;
}



//RealServerリストの各操作関数
//RealServerリスト先頭取得関数
protocol_module_ip::realserverlist_type::iterator rslist_begin()
{
        return protocol_module_ip::realserverlist_type::iterator();
}
//RealServerリスト末端取得関数
protocol_module_ip::realserverlist_type::iterator rslist_end()
{
        return protocol_module_ip::realserverlist_type::iterator();
}
//RealServerリスト次要素取得関数
protocol_module_ip::realserverlist_type::iterator rslist_next(
        protocol_module_ip::realserverlist_type::iterator iter)
{
        return protocol_module_ip::realserverlist_type::iterator();
}
//RealServerリストロック関数
void rslist_lock()
{
}
//RealServerリストアンロック関数
void rslist_unlock()
{
}
//コンポーネント領域割り当て関数
void *pay_memory(const std::string &str, unsigned int *pro)
{
        return 0 ;
}
//コンポーネント領域ロック関数
void area_lock() {}
//コンポーネント領域アンロック関数
void area_unlock() {}
//Client振り分け処理関数
void schedule_tcp_stb(const boost::thread::id id,
protocol_module_base::rs_list_itr_func_type func_type1,
protocol_module_base::rs_list_itr_func_type func_type2,
protocol_module_base::rs_list_itr_next_func_type func_type3,
boost::asio::ip::tcp::endpoint &)
{
}

bool check_replication_area_lock = false;
bool check_replication_area_unlock = false;

void replication_area_lock_stb()
{
        check_replication_area_lock = true;
}

void replication_area_unlock_stb()
{
        check_replication_area_unlock = true;
}
//Thread function
void up_thread_func() {}
void down_thread_func() {}

class ip_session_data_processor_stub : public ip_session_data_processor
{
public:
        ip_session_data_processor_stub(
                int timeout,
                ip_replication_data_processor *replication_data_processor,
                getloglevel_func_type    ingetloglevel,
                logger_func_type inputLogFatal,
                logger_func_type inputLogError,
                logger_func_type inputLogWarn,
                logger_func_type inputLogInfo,
                logger_func_type inputLogDebug):
                ip_session_data_processor(
                        timeout,
                        replication_data_processor,
                        ingetloglevel,
                        inputLogFatal,
                        inputLogError,
                        inputLogWarn,
                        inputLogInfo,
                        inputLogDebug) {}

        ~ip_session_data_processor_stub() {}

        ip_session_table_entry get_session_data_stub(int ip_hash) {
                return session_table[ip_hash];
        }
        void write_session_data_stub(int ip_hash, const ip_session_table_entry &data) {

                session_table[ip_hash] = data;
        }
};

class ip_replication_data_processor_stub : public ip_replication_data_processor
{
public:
        ip_replication_data_processor_stub(
                char *ip_replication_area_begain,
                int ip_replication_area_size,
                const boost::asio::ip::tcp::endpoint &virtual_service_endpoint,
                getloglevel_func_type    ingetloglevel,
                logger_func_type inputLogFatal,
                logger_func_type inputLogError,
                logger_func_type inputLogWarn,
                logger_func_type inputLogInfo,
                logger_func_type inputLogDebug):
                ip_replication_data_processor(
                        ip_replication_area_begain,
                        ip_replication_area_size,
                        virtual_service_endpoint,
                        ingetloglevel,
                        inputLogFatal,
                        inputLogError,
                        inputLogWarn,
                        inputLogInfo,
                        inputLogDebug) {}

        ~ip_replication_data_processor_stub() {}

        ip_replication_data*& get_replication_area() {
                return replication_area;
        }

        std::deque<ip_replication_temp_data>& get_temp_list() {
                return temp_list;
        }
};
// using string to endpoint template function
template<typename InternetProtocol>
boost::asio::ip::basic_endpoint<InternetProtocol> string_to_endpoint(std::string str)
{
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
        boost::asio::ip::tcp::endpoint &rs_endpoint)
{
        rs_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888");
}

void schedule_tcp_determinate_config_result(//endpoint = 決定
        const boost::thread::id thread_id,
        protocol_module_base::rs_list_itr_func_type rs_list_begin,
        protocol_module_base::rs_list_itr_func_type rs_list_end,
        protocol_module_base::rs_list_itr_next_func_type rs_list_next,
        boost::asio::ip::tcp::endpoint &rs_endpoint,
        const boost::asio::ip::tcp::endpoint &result)
{
        rs_endpoint = result;
}


void schedule_tcp_nodeterminate(//endpoint = 未決定
        const boost::thread::id thread_id,
        protocol_module_base::rs_list_itr_func_type rs_list_begin,
        protocol_module_base::rs_list_itr_func_type rs_list_end,
        protocol_module_base::rs_list_itr_next_func_type rs_list_next,
        boost::asio::ip::tcp::endpoint &rs_endpoint)
{
}

void *replication_pay_memory_stb(const std::string &module_name, unsigned int *data_size)
{
        *data_size = 400;
        void *data_addr = malloc(*data_size * DATA_SIZE);
        memset(data_addr, 0, *data_size * DATA_SIZE);
        ip_replication_data_header *replication_header;
        replication_header = (ip_replication_data_header *)data_addr;
        strcpy(replication_header->virtualserver_ip, "192.168.120.246");
        replication_header->virtualserver_port = 8888;
        replication_header->offset = 128 * sizeof(ip_replication_data_header);
        replication_header->size = 1024 * sizeof(ip_replication_data);
        ip_replication_data *replication_data = (ip_replication_data *)((ip_replication_data_header *)data_addr + 128);
        strcpy(replication_data->realserver_ip, "192.168.120.246");
        replication_data->realserver_port = 8888;
        replication_data->last_time = time(0);
        replication_data->valid = 1;

        return data_addr;
}


//test class
class protocol_module_ip_test_class : public protocol_module_ip
{
public:
        boost::mutex check_mutex;

        //stub log function install
        void install_stb_log_func() {
                typedef boost::function< void (const unsigned int, const std::string &, const char *, int) > log_func_type;

                boost::function< LOG_LEVEL_TAG(void) >    getloglevel = &stb_getloglevel;
                log_func_type    putLogFatal    = &stb_putLogFatal;
                log_func_type    putLogError    = &stb_putLogError;
                log_func_type    putLogWarn    = &stb_putLogWarn;
                log_func_type    putLogInfo    = &stb_putLogInfo;
                log_func_type    putLogDebug    = &stb_putLogDebug;
                this->init_logger_functions(getloglevel, putLogFatal, putLogError,
                putLogWarn, putLogInfo, putLogDebug);

        }
        void install_stb_replication_func() {
                boost::function< void*(const std::string &, unsigned int *) >  inreplication_pay_memory = &replication_pay_memory_stb;
                boost::function<void (void)> inlock_func = &replication_area_lock_stb;
                boost::function<void (void)> inunlock_func = &replication_area_unlock_stb;
                boost::asio::ip::tcp::endpoint tcp_ep;
                boost::asio::ip::udp::endpoint udp_ep;
                this->init_replication_functions(inreplication_pay_memory, inlock_func, inunlock_func, tcp_ep, udp_ep);
        }
        //initinalize session_thread_data_ip
        void init_session_thread_data_ip(thread_data_ptr data,
        boost::thread::id thread_id) {
        }

        void set_replication_data_processor(ip_replication_data_processor *p) {
                replication_data_processor = p;
        }

        void set_ip_data_processor(ip_session_data_processor *p) {
                ip_data_processor = p;
        }

        ip_replication_data_processor *get_replication_data_processor() const {
                return replication_data_processor;
        }

        void release_replication_data_processor() {
                delete replication_data_processor;
                replication_data_processor = NULL;
        }

        void release_ip_data_processor() {
                delete ip_data_processor;
                ip_data_processor = NULL;
        }


        protocol_module_ip_test_class() {
                install_stb_log_func();
        }

        //protocol_module_ip
        void protocol_module_ip_test() {

                cout << "[1]--------------------------------------------- " << endl;
                //unit_test[1] モジュール名が"ip"に設定する。
                protocol_module_ip obj;
                BOOST_CHECK_EQUAL(obj.get_name(), "ip");

                cout << "[2]--------------------------------------------- " << endl;
                //unit_test[2] ipのインスタンスを生成する。
                protocol_module_ip *p = new protocol_module_ip();
                BOOST_CHECK_EQUAL(p->get_name(), "ip");
                delete p;
        }

        //is_tcp
        void is_tcp_test() {
                cout << "[3]--------------------------------------------- " << endl;
                //unit_test[3] is_tcp TRUEを返却する
                BOOST_CHECK(this->is_tcp());
        }

        //is_udp
        void is_udp_test() {
                cout << "[4]--------------------------------------------- " << endl;
                //unit_test[4] is_udp FALSEを返却する
                BOOST_CHECK_EQUAL(this->is_udp(), false);
        }

        //get_name
        void get_name_test() {
        }

        //initialize
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
                this->initialize(inrslist_begin, inrslist_end, inrslist_next, inlist_lock, inlist_unlock);

                BOOST_CHECK_EQUAL(this->rs_list_begin.empty(), true);
                BOOST_CHECK_EQUAL(this->rs_list_end.empty(), true);
                BOOST_CHECK_EQUAL(this->rs_list_next.empty(), true);
                BOOST_CHECK_EQUAL(this->rs_list_lock.empty(), true);
                BOOST_CHECK_EQUAL(this->rs_list_unlock.empty(), true);
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
                char *ip_replication_area_begain = NULL;
                boost::asio::ip::tcp::endpoint virtual_service_endpoint;
                getloglevel_func_type ingetloglevel = stb_getloglevel;
                logger_func_type inputLogFatal = stb_putLogFatal;
                logger_func_type inputLogError = stb_putLogError;
                logger_func_type inputLogWarn = stb_putLogWarn;
                logger_func_type inputLogInfo = stb_putLogInfo;
                logger_func_type inputLogDebug = stb_putLogDebug;
                this->forwarded_for = FORWARDED_FOR_ON;
                this->sorry_uri.assign('a');

                cout << "[7]--------------------------------------------- " << endl;
                //unit_test[7] 初期値がある場合
                int ip_replication_area_size = 10;

                this->initialize(inrslist_begin, inrslist_end, inrslist_next,
                inlist_lock, inlist_unlock);
                this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                inputLogWarn, inputLogInfo, inputLogDebug);
                this->ip_data_processor = new ip_session_data_processor_stub(
                        3600, this->replication_data_processor, ingetloglevel,
                        inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                        inputLogDebug);
                this->schedule_tcp = schedule_tcp_stb;
                this->replication_pay_memory = replication_pay_memory_stb;
                this->replication_area_lock = replication_area_lock_stb;
                this->replication_area_unlock = replication_area_unlock_stb;
                this->forwarded_for = FORWARDED_FOR_OFF;
                char *sorryuri = "/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
                memcpy(this->sorry_uri.c_array(), sorryuri, strlen(sorryuri));
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
                BOOST_CHECK_EQUAL(this->timeout, 0);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK(this->ip_data_processor == NULL);
                BOOST_CHECK(this->replication_data_processor == NULL);
                BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_OFF);
                BOOST_CHECK_EQUAL(this->sorry_uri[0], '\0');

                cout << "[8]--------------------------------------------- " << endl;
                //unit_test[8] 初期値が空の場合
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
                this->reschedule = 10;
                this->forwarded_for = FORWARDED_FOR_ON;
                this->sorry_uri.assign('a');
                this->replication_data_processor = NULL;

                this->ip_data_processor = NULL;
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
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK(this->ip_data_processor == NULL);
                BOOST_CHECK(this->replication_data_processor == NULL);
                BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_OFF);
                BOOST_CHECK_EQUAL(this->sorry_uri[0], '\0');
        }

        //is_use_sorry
        void is_use_sorry_test() {
                cout << "[9]--------------------------------------------- " << endl;
                //unit_test[9] 正常系で必ずtrueを返す
                BOOST_CHECK_EQUAL(this->is_use_sorry(), true);
        }

        //check_parameter
        void check_parameter_test() {
                std::vector<std::string> args;

                cout << "[10]--------------------------------------------- " << endl;
                //unit_test[10] オプション文字列が存在しない場合, チェック結果フラグにTRUEを設定する
                protocol_module_base::check_message_result check_message;
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[11]--------------------------------------------- " << endl;
                //unit_test[11] オプション文字列 = "-T", チェック結果フラグにTRUEを設定する
                args.push_back("-T");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[12]--------------------------------------------- " << endl;
                //unit_test[12] オプション文字列 = "--T", チェック結果フラグにFALSEを設定する、チェック結果メッセージに"Option error."を設定する
                args.clear();
                args.push_back("--T");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "Option error.");

                cout << "[13]--------------------------------------------- " << endl;
                //unit_test[13] オプション文字列 = "-T -T",チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("-T");
                args.push_back("-T");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[14]--------------------------------------------- " << endl;
                //unit_test[14] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する
                //unit_test[14] data test:オプション文字列 = "-T 2ewqt"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2ewqt");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2ewqt' is not numeric character.");

                cout << "[15]--------------------------------------------- " << endl;
                //unit_test[15] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '$@#' is not numeric character."を設定する
                //unit_test[15] data test:オプション文字列 = "-T $@#"
                args.clear();
                args.push_back("-T");
                args.push_back("$@#");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '$@#' is not numeric character.");

                cout << "[16]--------------------------------------------- " << endl;
                //unit_test[16] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2147483648' is too large."を設定する
                //unit_test[16] test data:オプション文字列 = "-T 2147483648"場合
                args.clear();
                args.push_back("-T");
                args.push_back("2147483648");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2147483648' is too large.");

                cout << "[17]--------------------------------------------- " << endl;
                //unit_test[17] チェック結果フラグにTRUEを設定する
                //unit_test[17] test data:オプション文字列 = "-T 2000"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[18]--------------------------------------------- " << endl;
                //unit_test[18] チェック結果フラグにTRUEを設定する
                //unit_test[18] test data:オプション文字列 = "-T 2147483647"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2147483647");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[19]--------------------------------------------- " << endl;
                //unit_test[19] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
                //unit_test[19] test data:オプション文字列 = "-T 2000 -T"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-T");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);

                cout << "[20]--------------------------------------------- " << endl;
                //unit_test[20] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
                //unit_test[20] test data:オプション文字列 = "-T 2000 -T 2000"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-T");
                args.push_back("2000");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);

                cout << "[21]--------------------------------------------- " << endl;
                //unit_test[21] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
                //unit_test[21] test data:オプション文字列 = "-T -T 20 -T"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("-T");
                args.push_back("20");
                args.push_back("-T");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);

                cout << "[22]--------------------------------------------- " << endl;
                //unit_test[22] オプション文字列 = "--timeout"次要素が存在しない場合,チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("--timeout");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[23]--------------------------------------------- " << endl;
                //unit_test[23] オプション文字列 = "--timeout --timeout" の場合,チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("--timeout");
                args.push_back("--timeout");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[24]--------------------------------------------- " << endl;
                //unit_test[24] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する
                //unit_test[24] test data:オプション文字列 = "--timeout 2ewqt"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2ewqt");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2ewqt' is not numeric character.");

                cout << "[25]--------------------------------------------- " << endl;
                //unit_test[25] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2147483648' is too large."を設定する
                //unit_test[25] test data:オプション文字列 = "--timeout 2147483648"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2147483648");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2147483648' is too large.");

                cout << "[26]--------------------------------------------- " << endl;
                //unit_test[26] チェック結果フラグにTRUEを設定する
                //unit_test[26] test data:オプション文字列 = "--timeout 2000"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[27]--------------------------------------------- " << endl;
                //unit_test[27] チェック結果フラグにTRUEを設定する
                //unit_test[27] test data:オプション文字列 = "--timeout 2147483647"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2147483647");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[28]--------------------------------------------- " << endl;
                //unit_test[28] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
                //unit_test[28] test data:オプション文字列 = "--timeout 2000 --timeout"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("--timeout");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);

                cout << "[29]--------------------------------------------- " << endl;
                //unit_test[29] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
                //unit_test[29] test data:オプション文字列 = "--timeout 2000 --timeout 2000"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("--timeout");
                args.push_back("2000");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);

                cout << "[30]--------------------------------------------- " << endl;
                //unit_test[30] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
                //unit_test[30] test data:オプション文字列 = "--timeout --timeout 2000 --timeout"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("--timeout");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);

                cout << "[31]--------------------------------------------- " << endl;
                //unit_test[31] チェック結果に"true"を設定する
                //unit_test[31] test data:オプション文字列 = "-F"の場合
                args.clear();
                args.push_back("-F");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[32]--------------------------------------------- " << endl;
                //unit_test[32] チェック結果に"false"を設定する,チェック結果メッセージに"You have to set  option value ''-S/--sorryURI"を設定する
                //unit_test[32] test data:オプション文字列 = "-S"の場合
                args.clear();
                args.push_back("-S");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to set option value '-S/--sorry-uri'.", check_message.message);

                cout << "[33]--------------------------------------------- " << endl;
                //unit_test[33] チェック結果に"false"を設定する,チェック結果メッセージに"-S/--sorryURI' option value '%s' is too long."を設定する
                //unit_test[33] test data:オプション文字列 = "-S ",次要素の文字列長 > 127の場合
                args.clear();
                args.push_back("-S");
                args.push_back("/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("'-S/--sorry-uri' option value '/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.", check_message.message);

                cout << "[34]--------------------------------------------- " << endl;
                //unit_test[34] チェック結果に"true"を設定する
                //unit_test[34] test data:オプション文字列 = "-S "の場合,次要素の文字列長 < 127,チェックOKの場合
                args.clear();
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[35]--------------------------------------------- " << endl;
                //unit_test[35] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is not a valid URI."（%sは次要素）を設定する
                //unit_test[35] test data:オプション文字列 = "-S"、次要素の文字列長 = 127 チェックNGの場合
                args.clear();
                args.push_back("-S");
                args.push_back("/a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("'-S/--sorry-uri' option value '/a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/' is not a valid URI.", check_message.message);

                cout << "[36]--------------------------------------------- " << endl;
                //unit_test[36] sorry-uri設定フラグをON、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
                //unit_test[36] test data:オプション文字列 = "-S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                //unit_test[36] "--sorry-uri /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"の場合
                args.clear();
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                args.push_back("--sorry-uri");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-S/--sorry-uri'.", check_message.message);

                cout << "[37]--------------------------------------------- " << endl;
                //unit_test[37] チェック結果フラグにTRUEを設定する
                //unit_test[37] test data:オプション文字列 ="-R"の場合
                args.clear();
                args.push_back("-R");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[38]--------------------------------------------- " << endl;
                //unit_test[38] チェック結果フラグにTRUEを設定する
                //unit_test[38] test data:オプション文字列 ="-N"の場合
                args.clear();
                args.push_back("-N");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[39]--------------------------------------------- " << endl;
                //unit_test[39] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
                //unit_test[39] test data:オプション文字列 ="-N -R"の場合
                args.clear();
                args.push_back("-N");
                args.push_back("-R");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);

                cout << "[40]--------------------------------------------- " << endl;
                //unit_test[40] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
                //unit_test[40] test data:オプション文字列 ="-R -N"の場合
                args.clear();
                args.push_back("-R");
                args.push_back("-N");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);

                cout << "[41]--------------------------------------------- " << endl;
                //unit_test[41] オプション文字列 ="-T -R -N -F -S"の場合,チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
                args.clear();
                args.push_back("-T");
                args.push_back("-R");
                args.push_back("-N");
                args.push_back("-F");
                args.push_back("-S");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);

                cout << "[42]--------------------------------------------- " << endl;
                //unit_test[42] オプション文字列 ="-T -N -R -S -F"の場合,チェック結果フラグにFALSEを設定する、チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
                args.clear();
                args.push_back("-T");
                args.push_back("-N");
                args.push_back("-R");
                args.push_back("-S");
                args.push_back("-F");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);

                cout << "[43]--------------------------------------------- " << endl;
                //unit_test[43] オプション文字列 ="-T 2000 -R -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/ -F"の場合,チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                args.push_back("-F");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[44]--------------------------------------------- " << endl;
                //unit_test[44] オプション文字列 ="--timeout 2000 -R -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/ -N"の場合,チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                args.push_back("-N");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);

                cout << "[45]--------------------------------------------- " << endl;
                //unit_test[45] オプション文字列 ="-T 2000 -R"の場合,チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[46]--------------------------------------------- " << endl;
                //unit_test[46] オプション文字列 ="--timeout 2000 -R -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/ -F"の場合,チェック結果フラグにTRUEを設定する
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                args.push_back("-F");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);

                cout << "[47]--------------------------------------------- " << endl;
                //unit_test[47] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
                //unit_test[47] test data:オプション文字列 ="-T 2000 -R -N -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/ -F"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-N");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                args.push_back("-F");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);

                cout << "[48]--------------------------------------------- " << endl;
                //unit_test[48] オプション文字列 = 上記以外の場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error"を設定する
                args.clear();
                args.push_back("-A");
                check_message = this->check_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Option error.", check_message.message);
        }

        //set_parameter
        void set_parameter_test() {
                cout << "[49]--------------------------------------------- " << endl;
                //unit_test[49] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[49] test data:オプション文字列が存在しないの場合
                protocol_module_base::check_message_result check_message;
                vector<string> args;
                install_stb_replication_func();
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[50]--------------------------------------------- " << endl;
                //unit_test[50] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[50] test data:オプション文字列 = "-T"の場合
                args.clear();
                args.push_back("-T");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[51]--------------------------------------------- " << endl;
                //unit_test[51] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error"を設定する,ipデータをreplication_areaから回復する
                //unit_test[51] test data:オプション文字列 = "--T"の場合
                args.clear();
                args.push_back("--T");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Option error.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[52]--------------------------------------------- " << endl;
                //unit_test[52] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[52] test data:オプション文字列 = "-T -T"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("-T");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[53]--------------------------------------------- " << endl;
                //unit_test[53] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する,ipデータをreplication_areaから回復する
                //unit_test[53] test data:オプション文字列 = "-T 2ewqt"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2ewqt");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2ewqt' is not numeric character.");
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[54]--------------------------------------------- " << endl;
                //unit_test[54] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout option' value '$@#' is not numeric character."を設定する,ipデータをreplication_areaから回復する
                //unit_test[54] test data:オプション文字列 = "-T $@#"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("$@#");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '$@#' is not numeric character.");
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[55]--------------------------------------------- " << endl;
                //unit_test[55] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2147483648' is too large."を設定する,ipデータをreplication_areaから回復する
                //unit_test[55] test data:オプション文字列 = "-T 2147483648"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2147483648");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2147483648' is too large.");
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[56]--------------------------------------------- " << endl;
                //unit_test[56] チェック結果フラグにTRUEを設定する,timeoutに2000を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[56] test data:オプション文字列 = "-T 2000"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->timeout, 2000);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[57]--------------------------------------------- " << endl;
                //unit_test[57] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[57] test data:オプション文字列 = "-T 2147483647"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2147483647");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[58]--------------------------------------------- " << endl;
                //unit_test[58] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[58] test data:オプション文字列 = "-T -T 2147483647"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("-T");
                args.push_back("2147483647");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[59]--------------------------------------------- " << endl;
                //unit_test[59] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,ipデータをreplication_areaから回復する
                //unit_test[59] test data:オプション文字列 = "-T 2147483647 -T"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2147483647");
                args.push_back("-T");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[60]--------------------------------------------- " << endl;
                //unit_test[60] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,ipデータをreplication_areaから回復する
                //unit_test[60] test data:オプション文字列 = "-T 2147483647 -T 2147483647"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("2147483647");
                args.push_back("-T");
                args.push_back("2147483647");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[61]--------------------------------------------- " << endl;
                //unit_test[61] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,ipデータをreplication_areaから回復する
                //unit_test[61] test data:オプション文字列 = "-T -T 2147483647 -T"の場合
                args.clear();
                args.push_back("-T");
                args.push_back("-T");
                args.push_back("2147483647");
                args.push_back("-T");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[62]--------------------------------------------- " << endl;
                //unit_test[62] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[62] test data:オプション文字列 = "--timeout"の場合
                args.clear();
                args.push_back("--timeout");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[63]--------------------------------------------- " << endl;
                //unit_test[63] チェック結果フラグにTRUEを設定する,timeoutに3600を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[63] test data:オプション文字列 = "--timeout --timeout"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("--timeout");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[64]--------------------------------------------- " << endl;
                //unit_test[64] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2ewqt' is not numeric character."を設定する,ipデータをreplication_areaから回復する
                //unit_test[64] test data:オプション文字列 = "--timeout 2ewqt"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2ewqt");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2ewqt' is not numeric character.");
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[65]--------------------------------------------- " << endl;
                //unit_test[65] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value '2147483648' is too large."を設定する,ipデータをreplication_areaから回復する
                //unit_test[65] test data:オプション文字列 = "--timeout 2147483648"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2147483648");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(check_message.message, "'-T/--timeout' option value '2147483648' is too large.");
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[66]--------------------------------------------- " << endl;
                //unit_test[66] チェック結果フラグにTRUEを設定する,timeoutに2000を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[66] test data:オプション文字列 = "--timeout 2000"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->timeout, 2000);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[67]--------------------------------------------- " << endl;
                //unit_test[67] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[67] test data:オプション文字列 = "--timeout"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2147483647");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[68]--------------------------------------------- " << endl;
                //unit_test[68] チェック結果フラグにTRUEを設定する,timeoutに2147483647を設定する,rescheduleに0を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[68] test data:オプション文字列 = "--timeout --timeout 2147483647"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("--timeout");
                args.push_back("2147483647");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[69]--------------------------------------------- " << endl;
                //unit_test[69] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,ipデータをreplication_areaから回復する
                //unit_test[69] test data:オプション文字列 = "--timeout 2147483647 --timeout"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2147483647");
                args.push_back("--timeout");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[70]--------------------------------------------- " << endl;
                //unit_test[70] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,ipデータをreplication_areaから回復する
                //unit_test[70] test data:オプション文字列 = "--timeout 2147483647 --timeout 2147483647"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("2147483647");
                args.push_back("--timeout");
                args.push_back("2147483647");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[71]--------------------------------------------- " << endl;
                //unit_test[71] チェック結果フラグにFALSEを設定する,timeoutに2147483647を設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する,ipデータをreplication_areaから回復する
                //unit_test[71] test data:オプション文字列 = "--timeout --timeout 2147483647 --timeout"の場合
                args.clear();
                args.push_back("--timeout");
                args.push_back("--timeout");
                args.push_back("2147483647");
                args.push_back("--timeout");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-T/--timeout'.", check_message.message);
                BOOST_CHECK_EQUAL(this->timeout, INT_MAX);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[72]--------------------------------------------- " << endl;
                //unit_test[72] チェック結果に"true"を設定する,送信元設定指示設定に1を設定する
                //unit_test[72] test data:オプション文字列 = "-F"の場合
                args.clear();
                args.push_back("-F");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->forwarded_for, FORWARDED_FOR_ON);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;


                cout << "[73]--------------------------------------------- " << endl;
                //unit_test[73] チェック結果に"false"を設定する,チェック結果メッセージに"You have to set  option value ''-S/--sorryURI"を設定する
                //unit_test[73] test data:オプション文字列 = "-S"の場合
                args.clear();
                args.push_back("-S");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to set option value '-S/--sorry-uri'.", check_message.message);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[74]--------------------------------------------- " << endl;
                //unit_test[74] チェック結果に"false"を設定する,チェック結果メッセージに"-S/--sorryURI' option value '%s' is too long."を設定する
                //unit_test[74] test data:オプション文字列 = "-S /bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/"の場合、次要素の文字列長 > 127の場合
                args.clear();
                args.push_back("-S");
                args.push_back("/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("'-S/--sorry-uri' option value '/bcdef1234567890/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.", check_message.message);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[75]--------------------------------------------- " << endl;
                //unit_test[75] チェック結果フラグにFALSEを設定する、チェック結果メッセージに"'-S/--sorry-uri' option value '%s' is not a valid URI."（%sは次要素）を設定する
                //unit_test[75] test data:オプション文字列 = "-S /a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"、、次要素の文字列長 = 127 チェックNGの場合
                args.clear();
                args.push_back("-S");
                args.push_back("/a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("'-S/--sorry-uri' option value '/a\r\n/123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/' is not a valid URI.", check_message.message);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[76]--------------------------------------------- " << endl;
                //unit_test[76] sorry-uri設定フラグをON、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
                //unit_test[76] test data:オプション文字列 = "-S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                //unit_test[76] " --sorry-uri /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"次要素の文字列長 = 127 チェックOKの場合
                args.clear();
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                args.push_back("--sorry-uri");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Cannot set multiple option '-S/--sorry-uri'.", check_message.message);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[77]--------------------------------------------- " << endl;
                //unit_test[77] sorry-uri設定フラグをON、チェック結果メッセージに"Cannot set multiple option '-S/--sorry-uri'."を設定する
                //unit_test[77] test data:オプション文字列 = "-S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"、次要素の文字列長 = 127 チェックOKの場合
                args.clear();
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                char *sorryuri = "/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
                int ret = memcmp(this->sorry_uri.c_array(), sorryuri, strlen(sorryuri));
                BOOST_CHECK_EQUAL(ret, 0);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[78]--------------------------------------------- " << endl;
                //unit_test[78] チェック結果フラグにTRUEを設定する,rescheduleに1を設定する,timeoutに3600を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[78] test data:オプション文字列 = "-R"
                args.clear();
                args.push_back("-R");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->reschedule, 1);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK_EQUAL(this->forwarded_for, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[79]--------------------------------------------- " << endl;
                //unit_test[79] チェック結果フラグにFALSEを設定する,rescheduleに0を設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,ipデータをreplication_areaから回復する
                //unit_test[79] test data:オプション文字列 ="-N -R"
                args.clear();
                args.push_back("-N");
                args.push_back("-R");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[80]--------------------------------------------- " << endl;
                //unit_test[80] チェック結果フラグにTRUEを設定する,rescheduleに0を設定する,timeoutに3600を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[80] test data:オプション文字列 = "-N"
                args.clear();
                args.push_back("-N");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->reschedule, 0);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 3600);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[81]--------------------------------------------- " << endl;
                //unit_test[81] チェック結果フラグにFALSEを設定する,rescheduleに1を設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,ipデータをreplication_areaから回復する
                //unit_test[81] test data:オプション文字列 = "-R -N"
                args.clear();
                args.push_back("-R");
                args.push_back("-N");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(this->reschedule, 1);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[82]--------------------------------------------- " << endl;
                //unit_test[82] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,timeoutに2000を設定する,,rescheduleに1を設定する,ipデータをreplication_areaから回復する
                //unit_test[82] test data:オプション文字列 = "-T -R -N -F -S"
                args.clear();
                args.push_back("-T");
                args.push_back("-R");
                args.push_back("-N");
                args.push_back("-F");
                args.push_back("-S");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[83]--------------------------------------------- " << endl;
                //unit_test[83] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,timeoutに2000を設定する,,rescheduleに1を設定する,ipデータをreplication_areaから回復する
                //unit_test[83] test data:オプション文字列 = "-T -N -R -F -S"
                args.clear();
                args.push_back("-T");
                args.push_back("-N");
                args.push_back("-R");
                args.push_back("-F");
                args.push_back("-S");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[84]--------------------------------------------- " << endl;
                //unit_test[84] チェック結果フラグにTRUEを設定する,timeoutに2000を設定する,,rescheduleに1を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[84] test data:オプション文字列 = "-T 2000 -R -F -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-F");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 2000);
                BOOST_CHECK_EQUAL(this->reschedule, 1);
                BOOST_CHECK_EQUAL(this->forwarded_for, 1);
                char *sorryuri_1 = "/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
                int ret_1 = memcmp(this->sorry_uri.c_array(), sorryuri_1, strlen(sorryuri_1));
                BOOST_CHECK_EQUAL(ret_1, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[85]--------------------------------------------- " << endl;
                //unit_test[85] チェック結果フラグにTRUEを設定する,,timeoutに2000を設定する,,rescheduleに1を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[85] test data:オプション文字列 = "-T 2000 -R -F -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-F");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 2000);
                BOOST_CHECK_EQUAL(this->reschedule, 1);
                BOOST_CHECK_EQUAL(this->forwarded_for, 1);
                char *sorryuri_2 = "/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
                int ret_2 = memcmp(this->sorry_uri.c_array(), sorryuri_2, strlen(sorryuri_2));
                BOOST_CHECK_EQUAL(ret_2, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[86]--------------------------------------------- " << endl;
                //unit_test[86] チェック結果フラグにTRUEを設定する,,timeoutに2000を設定する,,rescheduleに1を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[86] test data:オプション文字列 = "--timeout 2000 -R -F -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-F");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 2000);
                BOOST_CHECK_EQUAL(this->reschedule, 1);
                BOOST_CHECK_EQUAL(this->forwarded_for, 1);
                char *sorryuri_3 = "/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
                int ret_3 = memcmp(this->sorry_uri.c_array(), sorryuri_3, strlen(sorryuri_3));
                BOOST_CHECK_EQUAL(ret_3, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[87]--------------------------------------------- " << endl;
                //unit_test[87] チェック結果フラグにTRUEを設定する,,timeoutに2000を設定する,,rescheduleに1を設定する,送信元設定指示に0を設定する,ipデータをreplication_areaから回復する
                //unit_test[87] test data:オプション文字列 = "--timeout 2000 -R -F --sorry-uri /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                args.clear();
                args.push_back("--timeout");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-F");
                args.push_back("--sorry-uri");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, true);
                BOOST_CHECK_EQUAL(this->timeout, 2000);
                BOOST_CHECK_EQUAL(this->reschedule, 1);
                BOOST_CHECK_EQUAL(this->forwarded_for, 1);
                char *sorryuri_4 = "/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
                int ret_4 = memcmp(this->sorry_uri.c_array(), sorryuri_4, strlen(sorryuri_4));
                BOOST_CHECK_EQUAL(ret_4, 0);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[88]--------------------------------------------- " << endl;
                //unit_test[88] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,ipデータをreplication_areaから回復する
                //unit_test[88] test data:オプション文字列 = "-T 2000 -R -N -F -S /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-N");
                args.push_back("-F");
                args.push_back("-S");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[89]--------------------------------------------- " << endl;
                //unit_test[89] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_reschedule"を設定する,ipデータをreplication_areaから回復する
                //unit_test[89] test data:オプション文字列 = "-T 2000 -R -N -F --sorry-uri /bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/"
                args.clear();
                args.push_back("-T");
                args.push_back("2000");
                args.push_back("-R");
                args.push_back("-N");
                args.push_back("-F");
                args.push_back("--sorry-uri");
                args.push_back("/bc78/0123456789/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("You have to choose either of reschedule or no-reschedule.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

                cout << "[90]--------------------------------------------- " << endl;
                //unit_test[90] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error"を設定する,ipデータをreplication_areaから回復する
                //unit_test[90] test data:オプション文字列 = 上記以外
                args.clear();
                args.push_back("-A");
                check_message = this->set_parameter(args);
                BOOST_CHECK_EQUAL(check_message.flag, false);
                BOOST_CHECK_EQUAL("Option error.", check_message.message);
                BOOST_CHECK(this->ip_data_processor != NULL);
                BOOST_CHECK(this->replication_data_processor != NULL);
                this->timeout = 111;
                this->reschedule = 111;
                this->forwarded_for = 111;

        }

        //add_parameter
        void add_parameter_test() {
                check_message_result result;
                vector<string> args;

                cout << "[91]--------------------------------------------- " << endl;
                //unit_test[91] オプション文字列が存在しない場合,正常系で必ずtrueを返す
                result = this->add_parameter(args);
                BOOST_CHECK_EQUAL(result.flag, true);

                cout << "[92]--------------------------------------------- " << endl;
                //unit_test[92] オプション文字列が存在する場合,チェック結果フラグにFALSEを設定する, チェック結果メッセージに"Cannot add option."を設定する
                args.push_back("-F");
                result = this->add_parameter(args);
                BOOST_CHECK_EQUAL(result.flag, false);
                BOOST_CHECK_EQUAL(result.message, "Cannot add option.");
        }

        //register_schedule(tcp)
        void register_schedule_tcp_test() {
                cout << "[93]--------------------------------------------- " << endl;
                //unit_test[93]  Client振り分け処理関数のテスト
                tcp_schedule_func_type  func = schedule_tcp_stb;
                this->register_schedule(func);
                BOOST_CHECK_EQUAL(this->schedule_tcp, schedule_tcp_stb);

                cout << "[94]--------------------------------------------- " << endl;
                //unit_test[94]  Client振り分け処理関数空のテスト
                func.clear();
                this->register_schedule(func);
                BOOST_CHECK_EQUAL(this->schedule_tcp.empty(), true);
        }

        //handle_session_initialize
        void handle_session_initialize_test() {

                boost::thread down_thread(down_thread_func);
                boost::asio::ip::tcp::endpoint ep;
                boost::asio::ip::tcp::endpoint client_endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
                boost::asio::ip::udp::endpoint client_endpoint_udp;

                cout << "[95]--------------------------------------------- " << endl;
                //unit_test[95] パラメータが正常に入力去れた場合、戻り値にACCEPTを設定する。
                EVENT_TAG status =
                this->handle_session_initialize(boost::this_thread::get_id(),
                down_thread.get_id(), client_endpoint,
                client_endpoint_udp);
                BOOST_CHECK_EQUAL(status, ACCEPT);
                std::map<const boost::thread::id, thread_data_ptr>::iterator
                itr;
                itr = this->session_thread_data_map.find(boost::this_thread::get_id());
                BOOST_CHECK(itr != this->session_thread_data_map.end());
                BOOST_CHECK_EQUAL(itr->second->thread_id, boost::this_thread::get_id());
                BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_UP_STREAM);
                BOOST_CHECK_EQUAL(itr->second->pair_thread_id, down_thread.get_id());
                BOOST_CHECK_EQUAL(itr->second->accept_end_flag, ACCEPT_END_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->sorry_flag, SORRY_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->switch_flag, SWITCH_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->client_endpoint, client_endpoint);
                BOOST_CHECK(itr->second->data_buffer != NULL);
                BOOST_CHECK_EQUAL(itr->second->data_buffer_size, MAX_BUFFER_SIZE);
                BOOST_CHECK_EQUAL(itr->second->data_length, 0u);
                BOOST_CHECK_EQUAL(itr->second->data_offset, 0u);
                BOOST_CHECK_EQUAL(itr->second->current_message_rest_size, 0u);
                BOOST_CHECK_EQUAL(itr->second->data_state, HTTP_START);
                BOOST_CHECK_EQUAL(itr->second->last_status, ACCEPT);
                BOOST_CHECK_EQUAL(itr->second->ip_hash, static_cast<int>(l7vs_ip_service_calc_hash(itr->second->client_endpoint)));
                itr = this->session_thread_data_map.find(down_thread.get_id());
                BOOST_CHECK(itr != this->session_thread_data_map.end());
                BOOST_CHECK_EQUAL(itr->second->thread_id, down_thread.get_id());
                BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_DOWN_STREAM);
                BOOST_CHECK_EQUAL(itr->second->pair_thread_id, boost::this_thread::get_id());
                BOOST_CHECK_EQUAL(itr->second->accept_end_flag, ACCEPT_END_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->sorry_flag, SORRY_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->switch_flag, SWITCH_FLAG_OFF);
                BOOST_CHECK_EQUAL(itr->second->client_endpoint, client_endpoint);
                BOOST_CHECK(itr->second->data_buffer != NULL);
                BOOST_CHECK_EQUAL(itr->second->data_buffer_size, MAX_BUFFER_SIZE);
                BOOST_CHECK_EQUAL(itr->second->data_length, 0u);
                BOOST_CHECK_EQUAL(itr->second->data_offset, 0u);
                BOOST_CHECK_EQUAL(itr->second->current_message_rest_size, 0u);
                BOOST_CHECK_EQUAL(itr->second->data_state, HTTP_START);
                BOOST_CHECK_EQUAL(itr->second->last_status, REALSERVER_RECV);
                BOOST_CHECK_EQUAL(itr->second->ip_hash, static_cast<int>(l7vs_ip_service_calc_hash(itr->second->client_endpoint)));
                BOOST_CHECK_EQUAL(itr->second->buffer_sequence.empty(), true);

                this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());
        }

        //handle_session_initialize
        void handle_session_initialize_test_thread() {
                boost::thread down_thread(down_thread_func);
                boost::asio::ip::tcp::endpoint ep;
                boost::asio::ip::tcp::endpoint client_endpoint = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
                boost::asio::ip::udp::endpoint client_endpoint_udp;

                cout << "[96]--------------------------------------------- " << endl;
                //unit_test[96] パラメータが正常に入力去れた場合、戻り値にACCEPTを設定する。
                EVENT_TAG status =
                this->handle_session_initialize(boost::this_thread::get_id(),
                down_thread.get_id(), client_endpoint,
                client_endpoint_udp);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(status, ACCEPT);
                        std::map<const boost::thread::id, thread_data_ptr>::iterator
                        itr;
                        itr = this->session_thread_data_map.find(boost::this_thread::get_id());
                        BOOST_CHECK(itr != this->session_thread_data_map.end());
                        BOOST_CHECK_EQUAL(itr->second->thread_id, boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_UP_STREAM);
                        BOOST_CHECK_EQUAL(itr->second->pair_thread_id, down_thread.get_id());
                        BOOST_CHECK_EQUAL(itr->second->accept_end_flag, ACCEPT_END_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->switch_flag, SWITCH_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->client_endpoint, client_endpoint);
                        BOOST_CHECK(itr->second->data_buffer != NULL);
                        BOOST_CHECK_EQUAL(itr->second->data_buffer_size, MAX_BUFFER_SIZE);
                        BOOST_CHECK_EQUAL(itr->second->data_length, 0u);
                        BOOST_CHECK_EQUAL(itr->second->data_offset, 0u);
                        BOOST_CHECK_EQUAL(itr->second->current_message_rest_size, 0u);
                        BOOST_CHECK_EQUAL(itr->second->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(itr->second->last_status, ACCEPT);
                        itr = this->session_thread_data_map.find(down_thread.get_id());
                        BOOST_CHECK(itr != this->session_thread_data_map.end());
                        BOOST_CHECK_EQUAL(itr->second->thread_id, down_thread.get_id());
                        BOOST_CHECK_EQUAL(itr->second->thread_division, THREAD_DIVISION_DOWN_STREAM);
                        BOOST_CHECK_EQUAL(itr->second->pair_thread_id, boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(itr->second->accept_end_flag, ACCEPT_END_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->end_flag, END_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->switch_flag, SWITCH_FLAG_OFF);
                        BOOST_CHECK_EQUAL(itr->second->client_endpoint, client_endpoint);
                        BOOST_CHECK(itr->second->data_buffer != NULL);
                        BOOST_CHECK_EQUAL(itr->second->data_buffer_size, MAX_BUFFER_SIZE);
                        BOOST_CHECK_EQUAL(itr->second->data_length, 0u);
                        BOOST_CHECK_EQUAL(itr->second->data_offset, 0u);
                        BOOST_CHECK_EQUAL(itr->second->current_message_rest_size, 0u);
                        BOOST_CHECK_EQUAL(itr->second->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(itr->second->last_status, REALSERVER_RECV);
                        BOOST_CHECK_EQUAL(itr->second->buffer_sequence.empty(), true);

                }
                this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());

        }

        //handle_session_finalize
        void handle_session_finalize_test() {
                {
                        cout << "[97]--------------------------------------------- " << endl;
                        //unit_test[97] セッションスレッドに対応する終了処理
                        boost::thread down_thread(down_thread_func);
                        boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
                        boost::asio::ip::udp::endpoint ep_udp = string_to_endpoint<boost::asio::ip::udp> ("10.10.100.100:8088");

                        this->handle_session_initialize(boost::this_thread::get_id(), down_thread.get_id(), ep_tcp, ep_udp);
                        protocol_module_base::EVENT_TAG ret =
                        this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());

                        //session_thread_data_map中にthis_thread無し
                        session_thread_data_map_it thread_map_iterator
                        = this->session_thread_data_map.find(boost::this_thread::get_id());
                        bool bret = (thread_map_iterator == this->session_thread_data_map.end());
                        BOOST_CHECK_EQUAL(bret, true);
                        //session_thread_data_map中にdown_thread無し
                        thread_map_iterator = this->session_thread_data_map.find(down_thread.get_id());
                        bret = (thread_map_iterator == this->session_thread_data_map.end());
                        BOOST_CHECK_EQUAL(bret, true);

                        //遷移先ステータスを設定する status = STOP
                        BOOST_CHECK_EQUAL(ret, STOP);
                }
                {
                        cout << "[98]--------------------------------------------- " << endl;
                        //unit_test[98] session_thread_data_mapが空
                        session_thread_data_map.clear();
                        boost::thread down_thread(down_thread_func);
                        protocol_module_base::EVENT_TAG ret =
                        this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());

                        //session_thread_data_map中にthis_thread無し
                        session_thread_data_map_it thread_map_iterator
                        = this->session_thread_data_map.find(boost::this_thread::get_id());
                        bool bret = (thread_map_iterator == this->session_thread_data_map.end());
                        BOOST_CHECK_EQUAL(bret, true);
                        //session_thread_data_map中にdown_thread無し
                        thread_map_iterator = this->session_thread_data_map.find(down_thread.get_id());
                        bret = (thread_map_iterator == this->session_thread_data_map.end());
                        BOOST_CHECK_EQUAL(bret, true);

                        //遷移先ステータスを設定する status = STOP
                        BOOST_CHECK_EQUAL(ret, STOP);
                }
        }

        //handle_session_finalize
        void handle_session_finalize_test_thread() {
                {
                        cout << "[99]--------------------------------------------- " << endl;
                        //unit_test[99] セッションスレッドに対応する終了処理
                        boost::thread down_thread(down_thread_func);
                        boost::asio::ip::tcp::endpoint ep_tcp = string_to_endpoint<boost::asio::ip::tcp> ("10.10.100.100:8800");
                        boost::asio::ip::udp::endpoint ep_udp = string_to_endpoint<boost::asio::ip::udp> ("10.10.100.100:8088");

                        this->handle_session_initialize(boost::this_thread::get_id(), down_thread.get_id(), ep_tcp, ep_udp);

                        protocol_module_base::EVENT_TAG ret =
                        this->handle_session_finalize(boost::this_thread::get_id(), down_thread.get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);

                                //session_thread_data_map中にthis_thread無し
                                session_thread_data_map_it thread_map_iterator
                                = this->session_thread_data_map.find(boost::this_thread::get_id());
                                bool bret = (thread_map_iterator == this->session_thread_data_map.end());
                                BOOST_CHECK_EQUAL(bret, true);
                                //session_thread_data_map中にdown_thread無し
                                thread_map_iterator = this->session_thread_data_map.find(down_thread.get_id());
                                bret = (thread_map_iterator == this->session_thread_data_map.end());
                                BOOST_CHECK_EQUAL(bret, true);

                                //遷移先ステータスを設定する status = STOP
                                BOOST_CHECK_EQUAL(ret, STOP);
                        }
                }
        }

        //handle_accept
        void handle_accept_test() {
                EVENT_TAG ret;

                cout << "[100]--------------------------------------------- " << endl;
                //unit_test[100] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {

                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        ret = this->handle_accept(boost::this_thread::get_id());

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[101]--------------------------------------------- " << endl;
                //unit_test[101] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {
                        this->session_thread_data_map.clear();
                        ret = this->handle_accept(boost::this_thread::get_id());

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[102]--------------------------------------------- " << endl;
                //unit_test[102] sorry状態の場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_OFF;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_accept(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->accept_end_flag, ACCEPT_END_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_SELECT);
                }
                cout << "[103]--------------------------------------------- " << endl;
                //unit_test[103] sorry状態以外の場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_OFF;
                        data_ptr->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_accept(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->accept_end_flag, ACCEPT_END_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_SELECT);

                }

        }

        //handle_accept
        void handle_accept_test_thread() {
                EVENT_TAG ret;

                cout << "[104]--------------------------------------------- " << endl;
                //unit_test[104] sorry状態以外の場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);

                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                                data_ptr->accept_end_flag = ACCEPT_END_FLAG_OFF;
                                data_ptr->sorry_flag = SORRY_FLAG_OFF;
                                data_ptr->last_status = INITIALIZE;
                        }
                        ret = this->handle_accept(boost::this_thread::get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);
                                BOOST_CHECK_EQUAL(data_ptr->accept_end_flag, ACCEPT_END_FLAG_ON);
                                BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_SELECT);
                        }
                }
        }

        //handle_client_recv
        void handle_client_recv_test() {
                boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                size_t recvlen = 0;
                EVENT_TAG ret;

                cout << "[105]--------------------------------------------- " << endl;
                //unit_test[105] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {
                        thread_data_ptr thread_data_null;
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data_null;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[106]--------------------------------------------- " << endl;
                //unit_test[106] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {
                        this->session_thread_data_map.clear();
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[107]--------------------------------------------- " << endl;
                //unit_test[107] recvデータサイズ>recvbufferサイズの場合 status = FINALIZE
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        recvlen = MAX_BUFFER_SIZE + 1;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                        session_thread_data_map.clear();
                }

                cout << "[108]--------------------------------------------- " << endl;
                //unit_test[108] 終了フラグがONの場合 status = CLIENT_RECV
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_ON;
                        recvlen = 10;
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(thread_data->last_status, CLIENT_RECV);
                        session_thread_data_map.clear();
                }

                cout << "[109]--------------------------------------------- " << endl;
                //unit_test[109] 終了フラグがOFFの場合 get_data_from_recvbuffer() 追加設定不可能の場合 status = FINALIZE
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - 1;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        recvlen = 10;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[110]--------------------------------------------- " << endl;
                //unit_test[110] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当なしの場合
                //unit_test[110] データサイズ　=　データバッファMAXサイズ　－　受信バッファサイズの場合 sorry状態の場合
                //unit_test[110] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size() - 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 10;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(thread_data->data_length, MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size());
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, thread_data->data_length);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[111]--------------------------------------------- " << endl;
                //unit_test[111] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当なしの場合
                //unit_test[111] データサイズ　>　データバッファMAXサイズ　－　受信バッファサイズの場合 sorry状態の場合
                //unit_test[111] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size() - 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 11;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(thread_data->data_length, MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size() + 1);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, thread_data->data_length);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[112]--------------------------------------------- " << endl;
                //unit_test[112] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当なしの場合
                //unit_test[112] データサイズ　=　データバッファMAXサイズ　－　受信バッファサイズの場合 sorry状態以外の場合
                //unit_test[112] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size() - 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 10;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(thread_data->data_length, MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size());
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, thread_data->data_length);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[113]--------------------------------------------- " << endl;
                //unit_test[113] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当なしの場合
                //unit_test[113] データサイズ　>　データバッファMAXサイズ　－　受信バッファサイズの場合 sorry状態以外の場合
                //unit_test[113] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size() - 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 11;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(thread_data->data_length, MAX_IP_MODULE_BUFFER_SIZE - recvbuffer.size() + 1);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, thread_data->data_length);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[114]--------------------------------------------- " << endl;
                //unit_test[114] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当なしの場合 データサイズ　<　データバッファMAXサイズ　－　受信バッファサイズの場合
                //unit_test[114] status = CLIENT_RECV
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 10;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(thread_data->last_status, CLIENT_RECV);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[115]--------------------------------------------- " << endl;
                //unit_test[115] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が異常の場合 sorry状態の場合
                //unit_test[115] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        recvbuffer.assign('0');
                        char *http_header_ptr =  "GET /index.html HTTP 1.3\r\nHost:172.16.58.240\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(recvbuffer.c_array(), http_header_ptr, http_header_len);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = http_header_len;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(thread_data->data_length, 10u + http_header_len);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 10u + http_header_len);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[116]--------------------------------------------- " << endl;
                //unit_test[116] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が異常の場合 sorry状態以外の場合
                //unit_test[116] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        recvbuffer.assign('0');
                        char *http_header_ptr =  "GET /index.html HTTP 1.3\r\nHost:172.16.58.240\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(recvbuffer.c_array(), http_header_ptr, http_header_len);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = http_header_len;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(thread_data->data_length, 10u + http_header_len);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 10u + http_header_len);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[117]--------------------------------------------- " << endl;
                //unit_test[117] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が正常の場合
                //unit_test[117] find_http_header_content_length content_length検索結果が該当なしの場合 sorry状態の場合
                //unit_test[117] 未送信データサイズにHTTPヘッダサイズを設定する データ状態にHTTP_HEADERを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_START;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "GET /index.html HTTP/1.1\r\nHost:172.16.58.240\r\n\r\n12345678", 56);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 48u);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }
                cout << "[118]--------------------------------------------- " << endl;
                //unit_test[118] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が正常の場合
                //unit_test[118] find_http_header_content_length content_length検索結果が該当なしの場合 sorry状態の場合
                //unit_test[118] 未送信データサイズにHTTPヘッダサイズを設定する データ状態にHTTP_HEADERを設定する status = SORRYSERVER_CONNECT
                //\r\n\r\n
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_START;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "GET /index.html HTTP/1.1\r\n\r\n12345678";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(recvbuffer.c_array(), http_header_ptr, http_header_len);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, http_header_len - 8);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[119]--------------------------------------------- " << endl;
                //unit_test[119] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が正常の場合
                //unit_test[119] find_http_header_content_length content_length検索結果が該当なしの場合 sorry状態以外の場合
                //unit_test[119] 未送信データサイズにHTTPヘッダサイズを設定する データ状態にHTTP_HEADERを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "GET /index.html HTTP/1.1\r\nHost:172.16.58.240\r\n\r\n12345678", 56);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 48u);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }
                cout << "[120]--------------------------------------------- " << endl;
                //unit_test[120] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が正常の場合
                //unit_test[120] find_http_header_content_length content_length検索結果が該当なしの場合 sorry状態以外の場合
                //unit_test[120] 未送信データサイズにHTTPヘッダサイズを設定する データ状態にHTTP_HEADERを設定する status = REALSERVER_CONNECT
                //\r\n\r\n
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "GET /index.html HTTP/1.1\r\n\r\n12345678";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(recvbuffer.c_array(), http_header_ptr, http_header_len);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, http_header_len - 8);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }


                cout << "[121]--------------------------------------------- " << endl;
                //unit_test[121] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が正常の場合
                //unit_test[121] find_http_header_content_length content_length検索結果が該当ありの場合 sorry状態の場合
                //unit_test[121] 未送信データサイズに「HTTPヘッダサイズ　＋　ContentLength」を設定する データ状態にHTTP_HEADERを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_START;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "GET /index.html HTTP/1.1\r\nContent-Length: 100rnHost:172.16.58.240\r\n\r\n12345678", 77);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 69u + 100u);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[122]--------------------------------------------- " << endl;
                //unit_test[122] データ状態がHTTP_STARTの場合 find_http_header_all検索結果が該当ありの場合 check_http_method_and_versionチェック結果が正常の場合
                //unit_test[122] find_http_header_content_length content_length検索結果が該当ありの場合 sorry状態以外の場合
                //unit_test[122] 未送信データサイズに「HTTPヘッダサイズ　＋　ContentLength」を設定する データ状態にHTTP_HEADERを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "GET /index.html HTTP/1.1\r\nContent-Length: 100rnHost:172.16.58.240\r\n\r\n12345678", 77);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 69u + 100u);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }


                cout << "[123]--------------------------------------------- " << endl;
                //unit_test[123] データ状態がUNKNOWN sorry状態の場合 未送信データサイズにデータサイズを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = UNKNOWN;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 110u);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[124]--------------------------------------------- " << endl;
                //unit_test[124] データ状態がUNKNOWN sorry状態以外の場合 未送信データサイズにデータサイズを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = UNKNOWN;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 110u);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }
                cout << "[125]--------------------------------------------- " << endl;
                //unit_test[125] データ状態がHTTP_HEADER sorry状態の場合 status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_HEADER;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[126]--------------------------------------------- " << endl;
                //unit_test[126] データ状態がHTTP_HEADER sorry状態以外の場合 status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_HEADER;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[127]--------------------------------------------- " << endl;
                //unit_test[127] データ状態がHTTP_BODY sorry状態の場合 status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_BODY;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }

                cout << "[128]--------------------------------------------- " << endl;
                //unit_test[128] データ状態がHTTP_BODY sorry状態以外の場合 status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->end_flag = END_FLAG_OFF;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_BODY;
                        recvbuffer.assign('0');
                        memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        memcpy(recvbuffer.c_array(), "1234567890", 10);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        recvlen = 100;
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                        delete [] thread_data->data_buffer;
                        session_thread_data_map.clear();
                }
        }

        //handle_client_recv_test_thread
        void handle_client_recv_test_thread() {
                boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                size_t recvlen = 0;
                EVENT_TAG ret;

                cout << "[129]--------------------------------------------- " << endl;
                //unit_test[129] データ状態がHTTP_BODY sorry状態以外の場合 status = REALSERVER_CONNECT
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                                thread_data->end_flag = END_FLAG_OFF;
                                thread_data->sorry_flag = SORRY_FLAG_OFF;
                                thread_data->data_state = HTTP_BODY;
                                recvbuffer.assign('0');
                                memset(recvbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                                memcpy(recvbuffer.c_array(), "1234567890", 10);
                                thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                                thread_data->data_length = 10;
                                thread_data->data_offset = 10;
                                thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                                recvlen = 100;
                        }
                        ret = this->handle_client_recv(boost::this_thread::get_id(), recvbuffer, recvlen);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                                BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_CONNECT);
                                delete [] thread_data->data_buffer;
                                session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_realserver_select(tcp)
        void handle_realserver_select_tcp_test() {
                EVENT_TAG ret;
                std::string ip_id;
                boost::asio::ip::tcp::endpoint rs_endpoint;
                boost::asio::ip::tcp::endpoint tmp_endpoint;
                boost::asio::ip::tcp::endpoint
                ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                12345);
                boost::asio::ip::tcp::endpoint ep2(boost::asio::ip::address::from_string("192.168.120.250"),
                12345);

                boost::function<LOG_LEVEL_TAG(void)> ingetloglevel = stb_getloglevel;
                boost::function < void(const unsigned int, const std::string &,
                const char *, int) > inputLogFatal = stb_putLogFatal;
                boost::function < void(const unsigned int, const std::string &,
                const char *, int) > inputLogError = stb_putLogError;
                boost::function < void(const unsigned int, const std::string &,
                const char *, int) > inputLogWarn = stb_putLogWarn;
                boost::function < void(const unsigned int, const std::string &,
                const char *, int) > inputLogInfo = stb_putLogInfo;
                boost::function < void(const unsigned int, const std::string &,
                const char *, int) > inputLogDebug = stb_putLogDebug;
                int ip_replication_area_size = 10;
                char *ip_replication_area_begain = NULL;

                boost::asio::ip::tcp::endpoint virtual_service_endpoint;
                ip_session_table_entry ip_session_entry_data;

                realserverlist_type rs_list;
                rs_list_begin = boost::bind(&get_rs_list_begin, &rs_list);
                rs_list_end = boost::bind(&get_rs_list_end, &rs_list);
                rs_list_next = boost::bind(&get_rs_list_next, _1);
                rs_list_lock = &rslist_lock;
                rs_list_unlock = &rslist_unlock;


                cout << "[130]--------------------------------------------- " << endl;
                //unit_test[130] 異常系 functionがなし場合遷移先ステータスにFINALIZEを設定する
                //register function
                {
                        tcp_schedule_func_type func_err1 = NULL;
                        this->register_schedule(func_err1);
                        boost::asio::ip::tcp::endpoint ep_err1;

                        ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_err1);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[131]--------------------------------------------- " << endl;
                //unit_test[131] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {
                        this->session_thread_data_map.clear();
                        //register function
                        tcp_schedule_func_type func_cerr2 = &schedule_tcp_determinate;
                        this->register_schedule(func_cerr2);
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        boost::asio::ip::tcp::endpoint ep_err2;
                        ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_err2);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[132]--------------------------------------------- " << endl;
                //unit_test[132] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {

                        boost::asio::ip::tcp::endpoint ep_err2;
                        this->session_thread_data_map.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(), ep_err2);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[133]--------------------------------------------- " << endl;
                //unit_test[133] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[133] schedule_moduleにて振り分け先realserverを未決定
                //unit_test[133] status = SORRYSERVER_SELECT, sorryフラグをON
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func_err = &schedule_tcp_nodeterminate;
                        this->register_schedule(func_err);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);

                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_ON);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[134]--------------------------------------------- " << endl;
                //unit_test[134] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[134] schedule_moduleにて振り分け先realserverを決定
                //unit_test[134] データ状態がHTTP_STARTの場合
                //unit_test[134] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->data_state = HTTP_START;
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);

                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);

                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;

                }

                cout << "[135]--------------------------------------------- " << endl;
                //unit_test[135] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[135] schedule_moduleにて振り分け先realserverを決定
                //unit_test[135] データ状態がHTTP_HEADERの場合
                //unit_test[135] status =REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->data_state = HTTP_HEADER;
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;

                }

                cout << "[136]--------------------------------------------- " << endl;
                //unit_test[136] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[136] schedule_moduleにて振り分け先realserverを決定
                //unit_test[136] データ状態がHTTP_BODYの場合
                //unit_test[136] status =REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->data_state = HTTP_BODY;
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;

                }

                cout << "[137]--------------------------------------------- " << endl;
                //unit_test[137] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[137] schedule_moduleにて振り分け先realserverを決定
                //unit_test[137] データ状態がUNKNOWNの場合
                //unit_test[137] status =REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->data_state = UNKNOWN;
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[138]--------------------------------------------- " << endl;
                //unit_test[138] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[138] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[138] データ状態がHTTP_STARTの場合
                //unit_test[138] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = HTTP_START;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        realserver server;
                        server.tcp_endpoint = ep2;
                        rs_list.clear();
                        rs_list.push_back(server);

                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[139]--------------------------------------------- " << endl;
                //unit_test[139] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[139] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[139] データ状態がHTTP_HEADERの場合
                //unit_test[139] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = HTTP_HEADER;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        realserver server;
                        server.tcp_endpoint = ep2;
                        rs_list.clear();
                        rs_list.push_back(server);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[140]--------------------------------------------- " << endl;
                //unit_test[140] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[140] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[140] データ状態がHTTP_BODYの場合
                //unit_test[140] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = HTTP_BODY;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        realserver server;
                        server.tcp_endpoint = ep2;
                        rs_list.clear();
                        rs_list.push_back(server);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }
                cout << "[141]--------------------------------------------- " << endl;
                //unit_test[141] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[141] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[141] データ状態がUNKNOWNの場合
                //unit_test[141] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = UNKNOWN;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        realserver server;
                        server.tcp_endpoint = ep2;
                        rs_list.clear();
                        rs_list.push_back(server);
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[142]--------------------------------------------- " << endl;
                //unit_test[142] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[142] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[142] no rescheduleモード
                //unit_test[142] status = CLIENT_DISCONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        this->reschedule = 0;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        rs_list.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[143]--------------------------------------------- " << endl;
                //unit_test[143] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[143] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[143] rescheduleモード
                //unit_test[143] schedule_moduleにて振り分け先realserverを未決定する
                //unit_test[143] status = SORRYSERVER_SELECT, sorryフラグをON
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_nodeterminate;
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        this->reschedule = 1;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        rs_list.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_ON);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[144]--------------------------------------------- " << endl;
                //unit_test[144] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[144] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[144] rescheduleモード
                //unit_test[144] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[144] データ状態がHTTP_STARTの場合
                //unit_test[144] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = HTTP_START;
                        this->reschedule = 1;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        rs_list.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[145]--------------------------------------------- " << endl;
                //unit_test[145] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[145] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[145] rescheduleモード
                //unit_test[145] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[145] データ状態がHTTP_HEADERの場合
                //unit_test[145] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = HTTP_HEADER;
                        this->reschedule = 1;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        rs_list.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[146]--------------------------------------------- " << endl;
                //unit_test[146] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[146] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[146] rescheduleモード
                //unit_test[146] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[146] データ状態がHTTP_BODYの場合
                //unit_test[146] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = HTTP_BODY;
                        this->reschedule = 1;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        rs_list.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }

                cout << "[147]--------------------------------------------- " << endl;
                //unit_test[147] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[147] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[147] rescheduleモード
                //unit_test[147] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[147] データ状態がUNKNOWNの場合
                //unit_test[147] status = REALSERVER_CONNECT
                {
                        this->session_thread_data_map.clear();
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        tcp_schedule_func_type func = &schedule_tcp_determinate;
                        this->register_schedule(func);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        data_ptr->data_state = UNKNOWN;
                        this->reschedule = 1;
                        this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);
                        ip_session_entry_data.rs_endpoint = ep2;
                        ip_session_entry_data.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
                        rs_list.clear();
                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
                        BOOST_CHECK_EQUAL(entry.last_time, 0);
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
                        this->session_thread_data_map.clear();
                        delete this->replication_data_processor;
                        delete this->ip_data_processor;
                        this->replication_data_processor = NULL;
                        this->ip_data_processor = NULL;
                }


                cout << "[148]--------------------------------------------- " << endl;
                //unit_test[148] realserver_connect_failed_count=-1の場合
                //unit_test[148] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[148] schedule_moduleにて振り分け先realserverを未決定
                //unit_test[148] status = CLIENT_DISCONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func_err = &schedule_tcp_nodeterminate;
//            this->register_schedule(func_err);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[149]--------------------------------------------- " << endl;
                //unit_test[149] realserver_connect_failed_count=-1の場合
                //unit_test[149] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[149] schedule_moduleにて振り分け先realserverを決定
                //unit_test[149] データ状態がHTTP_STARTの場合
                //unit_test[149] status = CLIENT_RECV
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->data_state = HTTP_START;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_RECV);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[150]--------------------------------------------- " << endl;
                //unit_test[150] realserver_connect_failed_count=-1の場合
                //unit_test[150] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[150] schedule_moduleにて振り分け先realserverを決定
                //unit_test[150] データ状態がHTTP_HEADERの場合
                //unit_test[150] status =REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->data_state = HTTP_HEADER;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[151]--------------------------------------------- " << endl;
                //unit_test[151] realserver_connect_failed_count=-1の場合
                //unit_test[151] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[151] schedule_moduleにて振り分け先realserverを決定
                //unit_test[151] データ状態がHTTP_BODYの場合
                //unit_test[151] status =REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->data_state = HTTP_BODY;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            //    install_stb_replication_func();
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[152]--------------------------------------------- " << endl;
                //unit_test[152] realserver_connect_failed_count=-1の場合
                //unit_test[152] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[152] schedule_moduleにて振り分け先realserverを決定
                //unit_test[152] データ状態がUNKNOWNの場合
                //unit_test[152] status =REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->data_state = UNKNOWN;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[153]--------------------------------------------- " << endl;
                //unit_test[153] realserver_connect_failed_count=-1の場合
                //unit_test[153] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[153] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[153] データ状態がHTTP_STARTの場合
                //unit_test[153] status = CLIENT_RECV
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = HTTP_START;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            realserver server;
//            server.tcp_endpoint = ep2;
//            rs_list.clear();
//            rs_list.push_back(server);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_RECV);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[154]--------------------------------------------- " << endl;
                //unit_test[154] realserver_connect_failed_count=-1の場合
                //unit_test[154] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[154] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[154] データ状態がHTTP_HEADERの場合
                //unit_test[154] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = HTTP_HEADER;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            realserver server;
//            server.tcp_endpoint = ep2;
//            rs_list.clear();
//            rs_list.push_back(server);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[155]--------------------------------------------- " << endl;
                //unit_test[155] realserver_connect_failed_count=-1の場合
                //unit_test[155] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[155] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[155] データ状態がHTTP_BODYの場合
                //unit_test[155] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = HTTP_BODY;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            realserver server;
//            server.tcp_endpoint = ep2;
//            rs_list.clear();
//            rs_list.push_back(server);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }
                cout << "[156]--------------------------------------------- " << endl;
                //unit_test[156] realserver_connect_failed_count=-1の場合
                //unit_test[156] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[156] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在する場合
                //unit_test[156] データ状態がUNKNOWNの場合
                //unit_test[156] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = UNKNOWN;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            realserver server;
//            server.tcp_endpoint = ep2;
//            rs_list.clear();
//            rs_list.push_back(server);
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[157]--------------------------------------------- " << endl;
                //unit_test[157] realserver_connect_failed_count=-1の場合
                //unit_test[157] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[157] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[157] no rescheduleモード
                //unit_test[157] status = CLIENT_DISCONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = bind(&schedule_tcp_determinate_config_result, _1, _2, _3, _4, _5, ep2);
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            this->reschedule = 0;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            rs_list.clear();
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[158]--------------------------------------------- " << endl;
                //unit_test[158] realserver_connect_failed_count=-1の場合
                //unit_test[158] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[158] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[158] rescheduleモード
                //unit_test[158] schedule_moduleにて振り分け先realserverを未決定する
                //unit_test[158] status = CLIENT_DISCONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_nodeterminate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            this->reschedule = 1;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            rs_list.clear();
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[159]--------------------------------------------- " << endl;
                //unit_test[159] realserver_connect_failed_count=-1の場合
                //unit_test[159] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[159] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[159] rescheduleモード
                //unit_test[159] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[159] データ状態がHTTP_STARTの場合
                //unit_test[159] status = CLIENT_RECV
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = HTTP_START;
//            this->reschedule = 1;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            rs_list.clear();
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_RECV);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[160]--------------------------------------------- " << endl;
                //unit_test[160] realserver_connect_failed_count=-1の場合
                //unit_test[160] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[160] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[160] rescheduleモード
                //unit_test[160] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[160] データ状態がHTTP_HEADERの場合
                //unit_test[160] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = HTTP_HEADER;
//            this->reschedule = 1;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            rs_list.clear();
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[161]--------------------------------------------- " << endl;
                //unit_test[161] realserver_connect_failed_count=-1の場合
                //unit_test[161] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[161] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[161] rescheduleモード
                //unit_test[161] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[161] データ状態がHTTP_BODYの場合
                //unit_test[161] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = HTTP_BODY;
//            this->reschedule = 1;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            rs_list.clear();
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[162]--------------------------------------------- " << endl;
                //unit_test[162] realserver_connect_failed_count=-1の場合
                //unit_test[162] get_endpoint_from_session_data後endpoint = 決定
                //unit_test[162] rsリストを検索し、realserver endpointの存在チェックしrealserver endpoint が存在しない場合
                //unit_test[162] rescheduleモード
                //unit_test[162] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[162] データ状態がUNKNOWNの場合
                //unit_test[162] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = -1;
//            data_ptr->client_endpoint = ep1;
//            data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
//            data_ptr->data_state = UNKNOWN;
//            this->reschedule = 1;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//            ip_session_entry_data.rs_endpoint = ep2;
//            ip_session_entry_data.last_time = 0;
//            (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->write_session_data_stub(data_ptr->ip_hash, ip_session_entry_data);
//            rs_list.clear();
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//            ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub*>(this->ip_data_processor))->get_session_data_stub(data_ptr->ip_hash);
//            BOOST_CHECK_EQUAL(entry.rs_endpoint, rs_endpoint);
//            BOOST_CHECK_EQUAL(entry.last_time, 0);
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }




                cout << "[163]--------------------------------------------- " << endl;
                //unit_test[163] realserver_connect_failed_count < realserver_connect_failed_max_countの場合
                //unit_test[163] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[163] データ状態がHTTP_STARTの場合
                //unit_test[163] status = CLIENT_RECV
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT - 1;
//            data_ptr->data_state = HTTP_START;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
//            BOOST_CHECK_EQUAL((rs_endpoint != tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_RECV);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[164]--------------------------------------------- " << endl;
                //unit_test[164] realserver_connect_failed_count < realserver_connect_failed_max_countの場合
                //unit_test[164] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[164] データ状態がHTTP_HEADERの場合
                //unit_test[164] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT - 1;
//            data_ptr->data_state = HTTP_HEADER;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL((rs_endpoint != tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[165]--------------------------------------------- " << endl;
                //unit_test[165] realserver_connect_failed_count < realserver_connect_failed_max_countの場合
                //unit_test[165] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[165] データ状態がHTTP_BODYの場合
                //unit_test[165] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT - 1;
//            data_ptr->data_state = HTTP_BODY;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL((rs_endpoint != tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }


                cout << "[166]--------------------------------------------- " << endl;
                //unit_test[166] realserver_connect_failed_count < realserver_connect_failed_max_countの場合
                //unit_test[166] schedule_moduleにて振り分け先realserverを決定する
                //unit_test[166] データ状態がHTTP_UNKNOWNの場合
                //unit_test[166] status = REALSERVER_CONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_determinate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT - 1;
//            data_ptr->data_state = UNKNOWN;
//            this->replication_data_processor = new ip_replication_data_processor(ip_replication_area_begain,
//                    ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
//                    inputLogWarn, inputLogInfo, inputLogDebug);
//            this->ip_data_processor = new ip_session_data_processor_stub(
//                3600, this->replication_data_processor, ingetloglevel,
//                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
//                inputLogDebug);
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
//            BOOST_CHECK_EQUAL((rs_endpoint != tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_CONNECT);
//            this->session_thread_data_map.clear();
//            delete this->replication_data_processor;
//            delete this->ip_data_processor;
//            this->replication_data_processor = NULL;
//            this->ip_data_processor = NULL;
                }

                cout << "[167]--------------------------------------------- " << endl;
                //unit_test[167] realserver_connect_failed_count < realserver_connect_failed_max_countの場合
                //unit_test[167] schedule_moduleにて振り分け先realserverを未決定
                //unit_test[167] status = CLIENT_DISCONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            tcp_schedule_func_type func = &schedule_tcp_nodeterminate;
//            this->register_schedule(func);
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT - 1;
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
//            BOOST_CHECK_EQUAL((rs_endpoint == tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
//            this->session_thread_data_map.clear();
                }



                cout << "[168]--------------------------------------------- " << endl;
                //unit_test[168] realserver_connect_failed_count == realserver_connect_failed_max_countの場合
                //unit_test[168] status = CLIENT_DISCONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT;
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
//            BOOST_CHECK_EQUAL((rs_endpoint == tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
//            this->session_thread_data_map.clear();

                }

                cout << "[169]--------------------------------------------- " << endl;
                //unit_test[169] realserver_connect_failed_count = realserver_connect_failed_max_count+1の場合
                //unit_test[169] status = CLIENT_DISCONNECT
                {
//            this->session_thread_data_map.clear();
//            thread_data_ptr data_ptr(new session_thread_data_ip);
//            this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
//            //data_ptr->realserver_connect_failed_count = REALSERVER_CONNECT_FAILED_COUNT +1;
//
//            ret = this->handle_realserver_select(boost::this_thread::get_id(),
//                                                 rs_endpoint);
//
//            BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
//            BOOST_CHECK_EQUAL((rs_endpoint == tmp_endpoint), true);
//            BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_DISCONNECT);
//            this->session_thread_data_map.clear();

                }

        }

        //handle_realserver_select(tcp)_thread
        void handle_realserver_select_tcp_test_thread() {
                EVENT_TAG ret;
                std::string ip_id;
                boost::asio::ip::tcp::endpoint rs_endpoint;
                boost::asio::ip::tcp::endpoint tmp_endpoint;
                boost::asio::ip::tcp::endpoint
                ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                12345);
                boost::asio::ip::tcp::endpoint ep2(boost::asio::ip::address::from_string("192.168.120.250"),
                12345);


                boost::asio::ip::tcp::endpoint virtual_service_endpoint;
                ip_session_table_entry ip_session_entry_data;

                realserverlist_type rs_list;
                rs_list_begin = boost::bind(&get_rs_list_begin, &rs_list);
                rs_list_end = boost::bind(&get_rs_list_end, &rs_list);
                rs_list_next = boost::bind(&get_rs_list_next, _1);
                rs_list_lock = &rslist_lock;
                rs_list_unlock = &rslist_unlock;

                cout << "[170]--------------------------------------------- " << endl;
                //unit_test[170] get_endpoint_from_session_data後endpoint = 未決定
                //unit_test[170] schedule_moduleにて振り分け先realserverを未決定
                //unit_test[170] status = SORRYSERVER_SELECT, sorryフラグをON


                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        data_ptr->client_endpoint = ep1;
                        data_ptr->ip_hash = l7vs_ip_service_calc_hash(data_ptr->client_endpoint);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                                tcp_schedule_func_type func_err = &schedule_tcp_nodeterminate;
                                this->register_schedule(func_err);
                        }

                        ret = this->handle_realserver_select(boost::this_thread::get_id(),
                        rs_endpoint);
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL((rs_endpoint == tmp_endpoint), true);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_ON);

                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }

        }

        //handle_realserver_select(udp)
        void handle_realserver_select_udp_test() {
                cout << "[171]--------------------------------------------- " << endl;
                //unit_test[171] handle_realserver_select_udp_test 戻り値が「STOP」に設定する。
                boost::asio::ip::udp::endpoint ep;
                boost::array<char, MAX_BUFFER_SIZE> sbf;
                std::size_t d;

                EVENT_TAG ret = this->handle_realserver_select(boost::this_thread::get_id(), ep, sbf, d);

                BOOST_CHECK_EQUAL(ret, STOP);
        }

        //handle_realserver_connect
        void handle_realserver_connect_test() {
                size_t send_buffer_len = 0;
                boost::array<char, MAX_BUFFER_SIZE> send_buffer;
                EVENT_TAG ret;
                int cmp_ret = 0;
                std::pair<char *, size_t> buffer_element;

                cout << "[172]--------------------------------------------- " << endl;
                //unit_test[172] session_thread_dataがNULLの場合 status = FINALIZE
                thread_data_ptr session_data;
                this->session_thread_data_map[boost::this_thread::get_id()] = session_data;
                ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[173]--------------------------------------------- " << endl;
                //unit_test[173] thread_id対応のデータなしの場合 status = FINALIZE
                this->session_thread_data_map.clear();
                ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[174]--------------------------------------------- " << endl;
                //unit_test[174] 送信データリストが空の場合
                //unit_test[174] データ状態がHTTP_HEADER
                //unit_test[174] 送信元設定指示設定フラグをONの場合
                //unit_test[174] データ状態にHTTP_BODYを設定する
                //unit_test[174] status = REALSERVER_SEND
                //unit_test[174] 送信バッファにデータをコピーし、X-Forworded-For文字列を添加する
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->client_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888");
                        thread_data->data_state = HTTP_HEADER;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "POST / HTTP/1.0\r\nContent-Length: 65493\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 0u;
                        thread_data->current_message_rest_size = http_header_len + 65493u;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);
                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                        char *send_buffer_context = "POST / HTTP/1.0\r\nX-Forwarded-For: 10.10.10.10\r\nContent-Length: 65493\r\n\r\n";
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, strlen(send_buffer_context));
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータをコピーし、X-Forworded-For文字列を添加する
                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_BODY);// データ状態にHTTP_BODYを設定する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        delete[] thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }

                cout << "[175]--------------------------------------------- " << endl;
                //unit_test[175] 送信データリストが空の場合
                //unit_test[175] データ状態がHTTP_HEADER
                //unit_test[175] 送信元設定指示設定フラグをOFFの場合
                //unit_test[175] データ状態にHTTP_BODYを設定する
                //unit_test[175] status = REALSERVER_SEND
                //unit_test[175] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->data_state = HTTP_HEADER;
                        this->forwarded_for = FORWARDED_FOR_OFF;
                        send_buffer.assign('0');
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "POST / HTTP/1.0\r\nContent-Length: 65493\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 0u;
                        thread_data->current_message_rest_size = http_header_len + 65493u;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_BODY);// データ状態にHTTP_BODYを設定する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len);// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), thread_data->data_buffer, http_header_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 65493u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[176]--------------------------------------------- " << endl;
                //unit_test[176] 送信データリストが空の場合
                //unit_test[176] データ状態がHTTP_BODY
                //unit_test[176] status = REALSERVER_SEND
                //unit_test[176] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->data_state = HTTP_HEADER;
                        this->forwarded_for = FORWARDED_FOR_OFF;
                        send_buffer.assign('0');
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "1234567890";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 0u;
                        thread_data->current_message_rest_size = 100;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len);
                        cmp_ret = memcmp(send_buffer.c_array(), http_header_ptr, http_header_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 90u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;

                }


                cout << "[177]--------------------------------------------- " << endl;
                //unit_test[177] 送信データリストが空の場合
                //unit_test[177] データ状態がUNKNOWN
                //unit_test[177] status = REALSERVER_SEND
                //unit_test[177] 送信バッファにデータをコピーし
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->data_state = UNKNOWN;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];

                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "AAA / HTTP/1.0\r\nContent-Length: 15\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = http_header_len;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        memset(send_buffer.c_array(), 0, MAX_BUFFER_SIZE);

                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len);// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), thread_data->data_buffer + 0u, http_header_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[178]--------------------------------------------- " << endl;
                //unit_test[178] 送信データリストが空ではないの場合(size = 1, forwarded_for = 0)
                //unit_test[178] status = REALSERVER_SEND
                //unit_test[178] 送信バッファにデータをコピーし
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 15\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - 6; //ship "Get / "
                        thread_data->data_offset = 6;//"Get / " is already sent
                        thread_data->current_message_rest_size = http_header_len - 6 + 15;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        this->forwarded_for = FORWARDED_FOR_OFF;
                        buffer_element = std::make_pair(thread_data->data_buffer + 6, thread_data->data_length);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len - 6);// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), thread_data->data_buffer + 6u, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 15u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[179]--------------------------------------------- " << endl;
                //unit_test[179] 送信データリストが空ではないの場合(size = 2, forwarded_for = 1)
                //unit_test[179] status = REALSERVER_SEND
                //unit_test[179] 送信バッファにデータをコピーし
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 15\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - 16; //ship "Get / HTTP/1.0\r\n"
                        thread_data->data_offset = 16;//"Get / HTTP/1.0\r\n"is already sent
                        thread_data->current_message_rest_size = http_header_len - 16 + 15;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        thread_data->forwarded_for_buffer.assign('0');
                        strcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n");

                        buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element = std::make_pair(thread_data->data_buffer + 16, thread_data->data_length);
                        thread_data->buffer_sequence.push_back(buffer_element);



                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "X-Forwarded-For: 172.16.58.24\r\nContent-Length: 15\r\n\r\n";
                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 15u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }
                cout << "[180]--------------------------------------------- " << endl;
                //unit_test[180] 送信データリストが空ではないの場合(size = 3, forwarded_for = 1)
                //unit_test[180] status = REALSERVER_SEND
                //unit_test[180] 送信バッファにデータをコピーし
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 15\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - 6; //ship "Get / "
                        thread_data->data_offset = 6;//"Get / "is already sent
                        thread_data->current_message_rest_size = http_header_len - 6 + 15;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        buffer_element = std::make_pair(thread_data->data_buffer + 6, 10);
                        thread_data->buffer_sequence.push_back(buffer_element);
                        thread_data->forwarded_for_buffer.assign('0');
                        strcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n");
                        buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                        thread_data->buffer_sequence.push_back(buffer_element);
                        buffer_element = std::make_pair(thread_data->data_buffer + 16, 22);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "HTTP/1.0\r\nX-Forwarded-For: 172.16.58.24\r\nContent-Length: 15\r\n\r\n";
                        BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 15u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

        }

        //handle_realserver_connect_test_thread
        void handle_realserver_connect_test_thread() {
                size_t send_buffer_len = 0;
                boost::array<char, MAX_BUFFER_SIZE> send_buffer;
                EVENT_TAG ret;
                int cmp_ret = 0;
                size_t http_header_len;
                std::pair<char *, size_t> buffer_element;
                cout << "[181]--------------------------------------------- " << endl;
                //unit_test[181] 送信データリストが空ではないの場合(size = 3, forwarded_for = 1)
                //unit_test[181] status = REALSERVER_SEND
                //unit_test[181] 送信バッファにデータをコピーし
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                                send_buffer.assign('0');
                                send_buffer_len = 0;
                                thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                                char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 15\r\n\r\n";
                                http_header_len = strlen(http_header_ptr);
                                memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                                thread_data->data_length = http_header_len - 6; //ship "Get / "
                                thread_data->data_offset = 6;//"Get / "is already sent
                                thread_data->current_message_rest_size = http_header_len - 6 + 15;
                                thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                                this->forwarded_for = FORWARDED_FOR_ON;
                                buffer_element = std::make_pair(thread_data->data_buffer + 6, 10);
                                thread_data->buffer_sequence.push_back(buffer_element);
                                thread_data->forwarded_for_buffer.assign('0');

                                buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                                thread_data->buffer_sequence.push_back(buffer_element);

                                strcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n");
                                buffer_element = std::make_pair(thread_data->data_buffer + 16, 22);
                                thread_data->buffer_sequence.push_back(buffer_element);
                        }

                        ret = handle_realserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                char *send_buffer_context = "HTTP/1.0\r\nX-Forwarded-For: 172.16.58.24\r\nContent-Length: 15\r\n\r\n";
                                BOOST_CHECK_EQUAL(ret, REALSERVER_SEND);// 遷移先ステータスを設定する
                                BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);// 遷移ステータスを保存する
                                BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                                cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                                BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                                BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 15u);// 未送信データサイズに送信可能サイズを減算する
                                BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                                BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                                BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                                delete[] thread_data->data_buffer;
                        }
                }
        }

        //handle_realserver_connection_fail
        void handle_realserver_connection_fail_test() {
                EVENT_TAG ret;
                boost::asio::ip::tcp::endpoint ep_err1;

                cout << "[182]--------------------------------------------- " << endl;
                //unit_test[182] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {
                        this->session_thread_data_map.clear();

                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep_err1);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[183]--------------------------------------------- " << endl;
                //unit_test[183] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {
                        this->session_thread_data_map.clear();

                        ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep_err1);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[184]--------------------------------------------- " << endl;
                //unit_test[184] rescheduleモード  遷移先ステータスをREALSERVER_SELECT設定する
                {
//            boost::asio::ip::tcp::endpoint ep;
//
//            thread_data_ptr data(new session_thread_data_ip);
//            data->thread_id = boost::this_thread::get_id();
//            this->reschedule = 1;
//            this->session_thread_data_map[boost::this_thread::get_id()] = data;
//
//            ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
//
//            //遷移先ステータスを設定する
//            //status = REALSERVER_SELECT
//            BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);
//            BOOST_CHECK_EQUAL(data->last_status, REALSERVER_SELECT);
                }

                cout << "[185]--------------------------------------------- " << endl;
                //unit_test[185] 遷移先ステータスをCLIENT_DISCONNECT設定する
                {
                        boost::asio::ip::tcp::endpoint ep;

                        thread_data_ptr data(new session_thread_data_ip);
                        data->thread_id = boost::this_thread::get_id();
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);

                        //遷移先ステータスを設定する
                        //CLIENT_DISCONNECT
                        BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_DISCONNECT);
                }

        }

        //handle_realserver_connection_fail_thread
        void handle_realserver_connection_fail_test_thread() {
                EVENT_TAG ret;

                cout << "[186]--------------------------------------------- " << endl;
                //unit_test[186] 遷移先ステータスをCLIENT_DISCONNECT設定する
                {
                        boost::asio::ip::tcp::endpoint ep;

                        thread_data_ptr data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        }
                        ret = this->handle_realserver_connection_fail(boost::this_thread::get_id(), ep);
                        {
                                //遷移先ステータスを設定する
                                //CLIENT_DISCONNECT
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);
                                BOOST_CHECK_EQUAL(data->last_status, CLIENT_DISCONNECT);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_realserver_send
        void handle_realserver_send_test() {
                char *data_buffer = new char[MAX_BUFFER_SIZE];
                EVENT_TAG ret;

                char *buffer = NULL;
                cout << "[187]--------------------------------------------- " << endl;
                //unit_test[187] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {
                        thread_data_ptr thread_data;
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, FINALIZE);//遷移先ステータスを設定する status = FINALIZE
                }

                cout << "[188]--------------------------------------------- " << endl;
                //unit_test[188] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {
                        this->session_thread_data_map.clear();
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, FINALIZE);//遷移先ステータスを設定する status = FINALIZE
                }

                cout << "[189]--------------------------------------------- " << endl;
                //unit_test[189] 未送信データサイズ　＞　０ データサイズ　＞　０の場合 status = REALSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        data->current_message_rest_size  = 1;
                        data->data_length = 1;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);//遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(data->last_status, REALSERVER_CONNECT);//遷移ステータスを保存する
                        this->session_thread_data_map.clear();
                }

                cout << "[190]--------------------------------------------- " << endl;
                //unit_test[190] 未送信データサイズ　＞　０データサイズが０の場合 データ開始位置が０を設定する status = CLIENT_RECV
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        data->current_message_rest_size = 1;
                        data->data_length = 0;
                        data->data_offset = 5;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);//遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);//遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);// データ開始位置が０を設定する
                }

                cout << "[191]--------------------------------------------- " << endl;
                //unit_test[191] 未送信データサイズが０ データサイズが０の場合 且つ　データ状態がHTTP_BODYの場合
                //unit_test[191] データ状態にHTTP_STARTを設定する status = CLIENT_RECV データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        data->data_state = HTTP_BODY;
                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);
                }

                cout << "[192]--------------------------------------------- " << endl;
                //unit_test[192] 未送信データサイズが０ データサイズ  ＞　０の場合 データ状態がHTTP_BODYの場合
                //unit_test[192] 検索結果が該当なしの場合
                //unit_test[192] データ状態がHTTP_STARTを設定する status = CLIENT_RECV
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        char *buffer = "GET /index.html HTTP/1.1\r\nHost:172.16.58.234\r\n";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->current_message_rest_size = 0;
                        data->data_offset = 0;
                        data->data_length = strlen(buffer);
                        data->data_state = HTTP_BODY;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_START);//データ状態がHTTP_STARTを設定する
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);// 遷移ステータスを保存する
                }

                cout << "[193]--------------------------------------------- " << endl;
                //unit_test[193] 未送信データサイズが０ データサイズ  ＞　０の場合 データ状態がHTTP_BODYの場合
                //unit_test[193] 検索結果が該当あり チェック結果が異常の場合
                //unit_test[193] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/5.5\r\nHost:172.16.58.234\r\n\r\n123455668";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        data->data_state = HTTP_BODY;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, data->data_length);
                        BOOST_CHECK_EQUAL(data->last_status, REALSERVER_CONNECT);
                }

                cout << "[194]--------------------------------------------- " << endl;
                //unit_test[194] 未送信データサイズが０ データサイズ  ＞　０の場合 データ状態がHTTP_BODYの場合
                //unit_test[194] 検索結果が該当あり チェック結果が異常の場合
                //unit_test[194] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = REALSERVER_CONNECT
                //\r\n\r\n
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/5.5\r\n\r\n123455668";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, data->data_length);
                        BOOST_CHECK_EQUAL(data->last_status, REALSERVER_CONNECT);
                }


                cout << "[195]--------------------------------------------- " << endl;
                //unit_test[195] 未送信データサイズが０ データサイズ  ＞　０の場合 データ状態がHTTP_BODYの場合
                //unit_test[195] 検索結果が該当あり チェック結果が正常の場合 ContentLengthがなし
                //unit_test[195] データ状態にHTTP_HEADERを設定する 未送信データサイズにHTTPヘッダサイズを設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/1.1\r\nHost:172.16.58.234\r\n\r\n12345678";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, 48u);
                        BOOST_CHECK_EQUAL(data->last_status, REALSERVER_CONNECT);
                }

                cout << "[196]--------------------------------------------- " << endl;
                //unit_test[196] 未送信データサイズが０ データサイズ  ＞　０の場合 データ状態がHTTP_BODYの場合
                //unit_test[196] 検索結果が該当あり チェック結果が正常の場合 ContentLengthがなし
                //unit_test[196] データ状態にHTTP_HEADERを設定する 未送信データサイズにHTTPヘッダサイズを設定する status = REALSERVER_CONNECT
                //\r\n\r\n Only
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/1.1\r\n\r\n12345678";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, 28u);
                        BOOST_CHECK_EQUAL(data->last_status, REALSERVER_CONNECT);
                }

                cout << "[197]--------------------------------------------- " << endl;
                //unit_test[197] 未送信データサイズが０ データサイズ  ＞　０の場合 データ状態がHTTP_BODYの場合
                //unit_test[197] 検索結果が該当あり チェック結果が正常の場合 ContentLengthがあり
                //unit_test[197] データ状態にHTTP_HEADERを設定する 未送信データサイズに「HTTPヘッダサイズ　＋　ContentLength」を設定する status = REALSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/1.1\r\nContent-Length: 100\r\nHost:172.16.58.234\r\n\r\n12345678";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, REALSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, (69u + 100u));
                        BOOST_CHECK_EQUAL(data->last_status, REALSERVER_CONNECT);
                }

                cout << "[198]--------------------------------------------- " << endl;
                //unit_test[198]未送信データサイズが０の場合
                //unit_test[198] データサイズが０の場合
                //unit_test[198] データ状態がHTTP_BODYの場合
                //unit_test[198] データ状態にHTTP_STARTを設定する
                //unit_test[198] status = CLIENT_RECV
                //unit_test[198] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        data->data_state = HTTP_BODY;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);

                }
                cout << "[199]--------------------------------------------- " << endl;
                //unit_test[199] 未送信データサイズが０の場合
                //unit_test[199] データサイズが０の場合
                //unit_test[199] データ状態がHTTP_HEADERの場合
                //unit_test[199] status = CLIENT_RECV
                //unit_test[199] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        data->data_state = HTTP_HEADER;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);

                }

                cout << "[200]--------------------------------------------- " << endl;
                //unit_test[200] 未送信データサイズが０の場合
                //unit_test[200] データサイズが０の場合
                //unit_test[200] データ状態がUNKNOWNの場合
                //unit_test[200] status = CLIENT_RECV
                //unit_test[200] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        data->data_state = UNKNOWN;
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);

                }


                delete [] data_buffer;
        }

        //handle_realserver_send
        void handle_realserver_send_test_thread() {
                EVENT_TAG ret;

                cout << "[201]--------------------------------------------- " << endl;
                //unit_test[201] 未送信データサイズが０の場合
                //unit_test[201] データサイズが０の場合
                //unit_test[201] データ状態がUNKNOWNの場合
                //unit_test[201] status = CLIENT_RECV
                //unit_test[201] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = data;

                                data->current_message_rest_size = 0;
                                data->data_length = 0;
                                data->data_offset = 5;
                                data->data_state = UNKNOWN;
                        }
                        ret = this->handle_realserver_send(boost::this_thread::get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                                BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                                BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                                BOOST_CHECK_EQUAL(data->data_offset, 0u);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_sorryserver_select
        void handle_sorryserver_select_test() {
                boost::asio::ip::tcp::endpoint ep;
                EVENT_TAG ret;
                cout << "[202]--------------------------------------------- " << endl;
                //unit_test[202] データ状態がHTTP_STARTの場合
                {
                        //up,down thread map
                        thread_data_ptr dataup(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
                        dataup->thread_id = boost::this_thread::get_id();
                        dataup->data_state = HTTP_START;

                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

                        //遷移先ステータス status = CLIENT_RECV
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        //遷移ステータスを保存する last_status = CLIENT_RECV;
                        BOOST_CHECK_EQUAL(dataup->last_status, CLIENT_RECV);
                }

                cout << "[203]--------------------------------------------- " << endl;
                //unit_test[203] データ状態がHTTP_HEADERの場合
                {
                        thread_data_ptr dataup(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
                        dataup->thread_id = boost::this_thread::get_id();
                        dataup->data_state = HTTP_HEADER;
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);
                        //遷移先ステータス status = SORRYSERVER_CONNECT
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        //遷移ステータスを保存する last_status = SORRYSERVER_CONNECT;
                        BOOST_CHECK_EQUAL(dataup->last_status, SORRYSERVER_CONNECT);
                }

                cout << "[204]--------------------------------------------- " << endl;
                //unit_test[204] データ状態がHTTP_BODYの場合
                {
                        thread_data_ptr dataup(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
                        dataup->thread_id = boost::this_thread::get_id();
                        dataup->data_state = HTTP_BODY;
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);
                        //遷移先ステータス status = SORRYSERVER_CONNECT
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        //遷移ステータスを保存する last_status = SORRYSERVER_CONNECT;
                        BOOST_CHECK_EQUAL(dataup->last_status, SORRYSERVER_CONNECT);
                }

                cout << "[205]--------------------------------------------- " << endl;
                //unit_test[205] データ状態がUNKNOWNの場合
                {
                        thread_data_ptr dataup(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
                        dataup->thread_id = boost::this_thread::get_id();
                        dataup->data_state = UNKNOWN;
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);
                        //遷移先ステータス status = SORRYSERVER_CONNECT
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        //遷移ステータスを保存する last_status = SORRYSERVER_CONNECT;
                        BOOST_CHECK_EQUAL(dataup->last_status, SORRYSERVER_CONNECT);
                }


                {
                        cout << "[206]--------------------------------------------- " << endl;
                        //unit_test[206]     session_thread_data_map中に上りスレッド中にThreadID対応のデータがない
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                {
                        cout << "[207]--------------------------------------------- " << endl;
                        //unit_test[207]     session_thread_data_map中に上りスレッドのデータ無し場合のテスト
                        this->session_thread_data_map.erase(boost::this_thread::get_id());
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[208]--------------------------------------------- " << endl;
                //unit_test[208]     session_thread_data_map中に上りスレッドと下りスレッドのデータ無し場合のテスト
                {
                        this->session_thread_data_map.clear();
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }
        }


        void handle_sorryserver_select_test_thread() {
                boost::asio::ip::tcp::endpoint ep;
                EVENT_TAG ret;
                cout << "[209]--------------------------------------------- " << endl;
                //unit_test[209] データ状態がHTTP_STARTの場合
                {
                        //up,down thread map
                        thread_data_ptr dataup(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
                                dataup->thread_id = boost::this_thread::get_id();
                                dataup->data_state = HTTP_START;
                        }
                        ret = this->handle_sorryserver_select(boost::this_thread::get_id(), ep);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                //遷移先ステータス status = CLIENT_RECV
                                BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                                //遷移ステータスを保存する last_status = CLIENT_RECV;
                                BOOST_CHECK_EQUAL(dataup->last_status, CLIENT_RECV);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_sorryserver_connect
        void handle_sorryserver_connect_test() {
                size_t send_buffer_len = USE_BUFFER_SIZE;
                boost::array<char, MAX_BUFFER_SIZE> send_buffer;
                EVENT_TAG ret;
                int cmp_ret = 0;

                cout << "[210]--------------------------------------------- " << endl;
                //unit_test[210] session_thread_dataがNULLの場合 status = FINALIZE
                thread_data_ptr session_data;
                this->session_thread_data_map[boost::this_thread::get_id()] = session_data;
                ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[211]--------------------------------------------- " << endl;
                //unit_test[211] thread_id対応のデータなしの場合 status = FINALIZE
                this->session_thread_data_map.clear();
                ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[212]--------------------------------------------- " << endl;
                //unit_test[212] 送信データリストが空の場合
                //unit_test[212] データ状態がHTTP_HEADER
                //unit_test[212] 送信元設定指示設定フラグをONの場合
                //unit_test[212] データ状態にHTTP_BODYを設定する
                //unit_test[212] status = SORRYSERVER_SEND
                //unit_test[212] uriを置き換える
                //unit_test[212] X-Forworded-For文字列を添加する
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->client_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("10.10.10.10:8888");
                        thread_data->data_state = HTTP_HEADER;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        send_buffer.assign('0');
                        strcpy(this->sorry_uri.data(),  "/sorry.html");
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "POST / HTTP/1.0\r\nContent-Length: 65493\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 0u;
                        thread_data->current_message_rest_size = http_header_len + 65493u;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "POST /sorry.html HTTP/1.0\r\nX-Forwarded-For: 10.10.10.10\r\nContent-Length: 65493\r\n\r\n";
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, strlen(send_buffer_context));
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// uriを置き換えし、X-Forworded-For文字列を添加する
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_BODY);// データ状態にHTTP_BODYを設定する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 65493u);
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);
                        delete [] thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }

                cout << "[213]--------------------------------------------- " << endl;
                //unit_test[213] 送信データリストが空の場合
                //unit_test[213] データ状態がHTTP_HEADER
                //unit_test[213] 送信元設定指示設定フラグをOFFの場合
                //unit_test[213] データ状態にHTTP_BODYを設定する
                //unit_test[213] status = SORRYSERVER_SEND
                //unit_test[213] uriを置き換える
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->data_state = HTTP_HEADER;
                        this->forwarded_for = FORWARDED_FOR_OFF;
                        send_buffer.assign('0');
                        strcpy(this->sorry_uri.data(),  "/sorry.html");
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "POST / HTTP/1.0\r\nContent-Length: 65493\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 0u;
                        thread_data->current_message_rest_size = http_header_len + 65493u;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "POST /sorry.html HTTP/1.0\r\nContent-Length: 65493\r\n\r\n";
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, strlen(send_buffer_context));
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(thread_data->data_state, HTTP_BODY);// データ状態にHTTP_BODYを設定する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len + strlen("sorry.html")); //sorry_uri's length
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 65493u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        delete[] thread_data->data_buffer;

                        this->session_thread_data_map.clear();

                }

                cout << "[214]--------------------------------------------- " << endl;
                //unit_test[214] 送信データリストが空の場合
                //unit_test[214] データ状態がHTTP_BODY
                //unit_test[214] status = SORRYSERVER_SEND
                //unit_test[214] uriを置き換える
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->data_state = HTTP_BODY;
                        send_buffer.assign('0');
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "1234567890";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer + 50u, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 50u;
                        thread_data->current_message_rest_size = 100;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len);
                        cmp_ret = memcmp(send_buffer.c_array(), http_header_ptr, http_header_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 90u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, 50u + http_header_len);// データ開始位置に送信可能サイズを加算する
                        delete[] thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }


                cout << "[215]--------------------------------------------- " << endl;
                //unit_test[215] 送信データリストが空の場合
                //unit_test[215] データ状態がUNKNOWN
                //unit_test[215] status = SORRYSERVER_SEND
                //unit_test[215] uriを置き換える
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        thread_data->data_state = UNKNOWN;
                        send_buffer.assign('0');
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "1234567890";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer + 50u, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len;
                        thread_data->data_offset = 50u;
                        thread_data->current_message_rest_size = 100;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len);
                        cmp_ret = memcmp(send_buffer.c_array(), http_header_ptr, http_header_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 90u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, 50u + http_header_len);// データ開始位置に送信可能サイズを加算する
                        delete[] thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }

                cout << "[216]--------------------------------------------- " << endl;
                //unit_test[216] 送信データリストが空ではないの場合(size = 1)
                //unit_test[216] status = SORRYSERVER_SEND
                //unit_test[216] 送信バッファにデータをコピーし
                {

                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 200\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - 15; //ship "Get / HTTP/1.0\r\n"
                        thread_data->data_offset = 15;//"Get / HTTP/1.0\r\n" is already sent
                        thread_data->current_message_rest_size = http_header_len - 15 + 200;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        std::pair<char * , size_t> buffer_element = std::make_pair(thread_data->data_buffer + 15, thread_data->data_length);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, http_header_len - 15);
                        cmp_ret = memcmp(send_buffer.c_array(), thread_data->data_buffer + 15u, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 200u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[217]--------------------------------------------- " << endl;
                //unit_test[217] 送信データリストが空ではないの場合(size = 2)
                //unit_test[217] status = SORRYSERVER_SEND
                //unit_test[217] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 200\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - strlen("Get / HTTP/1.0\r\n"); //ship "Get / HTTP/1.0\r\n"
                        thread_data->data_offset = strlen("Get / HTTP/1.0\r\n");//"Get / HTTP/1.0\r\n"is already sent
                        thread_data->current_message_rest_size = http_header_len - strlen("Get / HTTP/1.0\r\n") + 200;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        thread_data->forwarded_for_buffer.assign('0');
                        memcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n", 31);
                        std::pair<char * , size_t>   buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element = std::make_pair(thread_data->data_buffer + 16, thread_data->data_length);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "X-Forwarded-For: 172.16.58.24\r\nContent-Length: 200\r\n\r\n";
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 200u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[218]--------------------------------------------- " << endl;
                //unit_test[218] 送信データリストが空ではないの場合(size = 3)
                //unit_test[218] status = SORRYSERVER_SEND
                //unit_test[218] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 200\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - strlen("Get / "); //ship "Get / "
                        thread_data->data_offset = strlen("Get / ");//"Get / "is already sent
                        thread_data->current_message_rest_size = http_header_len - 6 + 200;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        //HTTP/1.0 \r\n
                        std::pair<char * , size_t> buffer_element = std::make_pair(thread_data->data_buffer + thread_data->data_offset, 10);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        this->forwarded_for = FORWARDED_FOR_ON;
                        thread_data->forwarded_for_buffer.assign('0');
                        memcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n", 31);
                        buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element = std::make_pair(thread_data->data_buffer + strlen("Get / HTTP/1.0\r\n"), strlen("Content-Length: 200\r\n\r\n"));
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "HTTP/1.0\r\nX-Forwarded-For: 172.16.58.24\r\nContent-Length: 200\r\n\r\n";
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 200u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[219]--------------------------------------------- " << endl;
                //unit_test[219] 送信データリストが空ではないの場合(size = 4)
                //unit_test[219] status = SORRYSERVER_SEND
                //unit_test[219] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 200\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - 4; //ship "Get "
                        thread_data->data_offset = 4;//"Get /"is already sent
                        thread_data->current_message_rest_size = http_header_len - 4 + 200;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        //sorry_uri
                        this->sorry_uri.assign(0);
                        strcpy(this->sorry_uri.data(), "/sorry.html");
                        std::pair<char * , size_t> buffer_element = std::make_pair(this->sorry_uri.data(), 11);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        // HTTP/1.0\r\n
                        buffer_element = std::make_pair(thread_data->data_buffer + 5, 11);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        this->forwarded_for = FORWARDED_FOR_ON;
                        thread_data->forwarded_for_buffer.assign('0');
                        memcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n", 31);
                        buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element = std::make_pair(thread_data->data_buffer + 16, 23);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "/sorry.html HTTP/1.0\r\nX-Forwarded-For: 172.16.58.24\r\nContent-Length: 200\r\n\r\n";
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 200u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }

                cout << "[220]--------------------------------------------- " << endl;
                //unit_test[220] 送信データリストが空ではないの場合(size = 5)
                //unit_test[220] status = SORRYSERVER_SEND
                //unit_test[220] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        send_buffer.assign('0');
                        send_buffer_len = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 200\r\n\r\n";
                        size_t http_header_len = strlen(http_header_ptr);
                        memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                        thread_data->data_length = http_header_len - 2; //ship "Ge"
                        thread_data->data_offset = 2;//"Ge"is already sent
                        thread_data->current_message_rest_size = http_header_len - 2 + 200;
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                        std::pair<char * , size_t> buffer_element = std::make_pair(thread_data->data_buffer + thread_data->data_offset, 2);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        //sorry_uri
                        this->sorry_uri.assign(0);
                        strcpy(this->sorry_uri.data(), "/sorry.html");
                        buffer_element = std::make_pair(this->sorry_uri.data(), 11);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        // HTTP/1.0 \r\n
                        buffer_element = std::make_pair(thread_data->data_buffer + 5, 11);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        this->forwarded_for = FORWARDED_FOR_ON;
                        thread_data->forwarded_for_buffer.assign('0');
                        memcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n", 31);
                        buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element = std::make_pair(thread_data->data_buffer + 16, 23);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);

                        char *send_buffer_context = "t /sorry.html HTTP/1.0\r\nX-Forwarded-For: 172.16.58.24\r\nContent-Length: 200\r\n\r\n";
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                        cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 200u);// 未送信データサイズに送信可能サイズを減算する
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                        BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                        this->session_thread_data_map.clear();
                        delete[] thread_data->data_buffer;
                }


        }

        //handle_sorryserver_connect_test_thread
        void handle_sorryserver_connect_test_thread() {
                size_t send_buffer_len = USE_BUFFER_SIZE;
                boost::array<char, MAX_BUFFER_SIZE> send_buffer;
                EVENT_TAG ret;
                int cmp_ret = 0;
                size_t http_header_len;
                cout << "[221]--------------------------------------------- " << endl;
                //unit_test[221] 送信データリストが空ではないの場合(size = 5)
                //unit_test[221] status = SORRYSERVER_SEND
                //unit_test[221] 送信バッファにデータをコピーし
                {
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                                send_buffer.assign('0');
                                send_buffer_len = 0;
                                thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                                char *http_header_ptr = "Get / HTTP/1.0\r\nContent-Length: 200\r\n\r\n";
                                http_header_len = strlen(http_header_ptr);
                                memcpy(thread_data->data_buffer, http_header_ptr, http_header_len);
                                thread_data->data_length = http_header_len - 2; //ship "Ge"
                                thread_data->data_offset = 2;//"Ge"is already sent
                                thread_data->current_message_rest_size = http_header_len - 2 + 200;
                                thread_data->data_buffer_size = MAX_BUFFER_SIZE;

                                std::pair<char * , size_t> buffer_element = std::make_pair(thread_data->data_buffer + thread_data->data_offset, 2);
                                thread_data->buffer_sequence.push_back(buffer_element);

                                //sorry_uri
                                this->sorry_uri.assign(0);
                                strcpy(this->sorry_uri.data(), "/sorry.html");
                                buffer_element = std::make_pair(this->sorry_uri.data(), 11);
                                thread_data->buffer_sequence.push_back(buffer_element);

                                // HTTP/1.0\r\n
                                buffer_element = std::make_pair(thread_data->data_buffer + 5, 11);
                                thread_data->buffer_sequence.push_back(buffer_element);

                                this->forwarded_for = FORWARDED_FOR_ON;
                                thread_data->forwarded_for_buffer.assign('0');
                                memcpy(thread_data->forwarded_for_buffer.data(), "X-Forwarded-For: 172.16.58.24\r\n", 31);
                                buffer_element = std::make_pair(thread_data->forwarded_for_buffer.data(), 31);
                                thread_data->buffer_sequence.push_back(buffer_element);

                                buffer_element = std::make_pair(thread_data->data_buffer + 16, 23);
                                thread_data->buffer_sequence.push_back(buffer_element);
                        }

                        ret = handle_sorryserver_connect(boost::this_thread::get_id(), send_buffer, send_buffer_len);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                char *send_buffer_context = "t /sorry.html HTTP/1.0\r\nX-Forwarded-For: 172.16.58.24\r\nContent-Length: 200\r\n\r\n";
                                BOOST_CHECK_EQUAL(ret, SORRYSERVER_SEND);// 遷移先ステータスを設定する
                                BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_SEND);// 遷移ステータスを保存する
                                BOOST_CHECK_EQUAL(send_buffer_len, strlen(send_buffer_context));// 送信データサイズに送信可能サイズを設定する
                                cmp_ret = memcmp(send_buffer.c_array(), send_buffer_context, send_buffer_len);
                                BOOST_CHECK_EQUAL(cmp_ret, 0);// 送信バッファにデータを送信可能サイズ分コピーする
                                BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 200u);// 未送信データサイズに送信可能サイズを減算する
                                BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                                BOOST_CHECK_EQUAL(thread_data->data_offset, http_header_len);// データ開始位置に送信可能サイズを加算する
                                BOOST_CHECK_EQUAL(thread_data->buffer_sequence.empty(), true);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                                delete[] thread_data->data_buffer;
                        }
                }
        }

        //handle_sorryserver_connection_fail
        void handle_sorryserver_connection_fail_test() {
                EVENT_TAG ret;

                cout << "[222]--------------------------------------------- " << endl;
                //unit_test[222] 異常系 session_thread_data_map中にThreadID対応のデータがない
                //unit_test[222] 遷移先ステータスを設定する status = FINALIZE
                this->session_thread_data_map.clear();
                this->session_thread_data_map[boost::this_thread::get_id()].reset();
                boost::asio::ip::tcp::endpoint ep_err1;
                ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(), ep_err1);
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[223]--------------------------------------------- " << endl;
                //unit_test[223] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                //unit_test[223] 遷移先ステータスを設定する status = FINALIZE
                this->session_thread_data_map.clear();
                ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(), ep_err1);
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[224]--------------------------------------------- " << endl;
                //unit_test[224] 終了フラグをON status = CLIENT_DISCONNECT
                boost::asio::ip::tcp::endpoint ep;
                thread_data_ptr data(new session_thread_data_ip);
                data->thread_id = boost::this_thread::get_id();
                this->session_thread_data_map[boost::this_thread::get_id()] = data;
                ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(), ep);
                BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT); // 遷移先ステータス status = CLIENT_DISCONNECT
                BOOST_CHECK_EQUAL(data->last_status, CLIENT_DISCONNECT);// 遷移ステータスを保存する
        }

        //handle_sorryserver_connection_fail
        void handle_sorryserver_connection_fail_test_thread() {
                EVENT_TAG ret;
                cout << "[225]--------------------------------------------- " << endl;
                //unit_test[225] 終了フラグをON status = CLIENT_DISCONNECT
                boost::asio::ip::tcp::endpoint ep;
                thread_data_ptr data(new session_thread_data_ip);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        data->thread_id = boost::this_thread::get_id();
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                }
                ret = this->handle_sorryserver_connection_fail(boost::this_thread::get_id(), ep);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT); // 遷移先ステータス status = CLIENT_DISCONNECT
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_DISCONNECT);// 遷移ステータスを保存する
                        this->session_thread_data_map.erase(boost::this_thread::get_id());
                }
        }

        //handle_sorryserver_send
        void handle_sorryserver_send_test() {
                char *data_buffer = new char[MAX_BUFFER_SIZE];
                EVENT_TAG ret;

                char *buffer = NULL;
                cout << "[226]--------------------------------------------- " << endl;
                //unit_test[226] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {
                        thread_data_ptr thread_data;
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, FINALIZE);//遷移先ステータスを設定する status = FINALIZE
                }

                cout << "[227]--------------------------------------------- " << endl;
                //unit_test[227] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {
                        this->session_thread_data_map.clear();
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, FINALIZE);//遷移先ステータスを設定する status = FINALIZE
                }

                cout << "[228]--------------------------------------------- " << endl;
                //unit_test[228] 未送信データサイズ　＞　０ データサイズ　＞　０の場合 status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        data->current_message_rest_size  = 1;
                        data->data_length = 1;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);//遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(data->last_status, SORRYSERVER_CONNECT);//遷移ステータスを保存する
                        this->session_thread_data_map.clear();
                }

                cout << "[229]--------------------------------------------- " << endl;
                //unit_test[229] 未送信データサイズ　＞　０データサイズが０の場合 データ開始位置が０を設定する status = CLIENT_RECV
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        data->current_message_rest_size = 1;
                        data->data_length = 0;
                        data->data_offset = 5;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);//遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);//遷移ステータスを保存する
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);// データ開始位置が０を設定する
                }

                cout << "[230]--------------------------------------------- " << endl;
                //unit_test[230] 未送信データサイズが０ データサイズが０の場合
                //unit_test[230] データ状態がHTTP_BODYの場合
                //unit_test[230] データ状態にHTTP_STARTを設定する status = CLIENT_RECV データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;
                        data->data_state = HTTP_BODY;
                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);
                }

                cout << "[231]--------------------------------------------- " << endl;
                //unit_test[231] 未送信データサイズが０ データサイズ  ＞　０の場合
                //unit_test[231] データ状態がHTTP_BODYの場合
                //unit_test[231] find_http_header_all検索結果が該当なしの場合
                //unit_test[231] データ状態がHTTP_STARTを設定する status = CLIENT_RECV
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        char *buffer = "GET /index.html HTTP/1.1\r\nHost:172.16.58.234\r\n";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->current_message_rest_size = 0;
                        data->data_offset = 0;
                        data->data_state = HTTP_BODY;
                        data->data_length = strlen(buffer);
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);// 遷移先ステータスを設定する
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_START);//データ状態がHTTP_STARTを設定する
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);// 遷移ステータスを保存する
                }

                cout << "[232]--------------------------------------------- " << endl;
                //unit_test[232] 未送信データサイズが０ データサイズ  ＞　０の場合
                //unit_test[232] データ状態がHTTP_BODYの場合
                //unit_test[232] find_http_header_all検索結果が該当あり
                //unit_test[232] check_http_method_and_versionチェック結果が異常の場合
                //unit_test[232] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/5.5\r\nHost:172.16.58.234\r\n\r\n123455668";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, data->data_length);
                        BOOST_CHECK_EQUAL(data->last_status, SORRYSERVER_CONNECT);
                }

                cout << "[233]--------------------------------------------- " << endl;
                //unit_test[233] 未送信データサイズが０ データサイズ  ＞　０の場合
                //unit_test[233] データ状態がHTTP_BODYの場合
                //unit_test[233] find_http_header_all検索結果が該当あり
                //unit_test[233] check_http_method_and_versionチェック結果が異常の場合
                //unit_test[233] データ状態にUNKNOWNを設定する 未送信データサイズにデータサイズを設定する status = SORRYSERVER_CONNECT
                //\r\n\r\n only
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/5.5\r\n\r\n123455668";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, data->data_length);
                        BOOST_CHECK_EQUAL(data->last_status, SORRYSERVER_CONNECT);
                }


                cout << "[234]--------------------------------------------- " << endl;
                //unit_test[234] 未送信データサイズが０ データサイズ  ＞　０の場合
                //unit_test[234] データ状態がHTTP_BODYの場合
                //unit_test[234] find_http_header_all検索結果が該当あり check_http_method_and_versionチェック結果が正常の場合 ContentLengthがなし
                //unit_test[234] データ状態にHTTP_HEADERを設定する 未送信データサイズにHTTPヘッダサイズを設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/1.1\r\nHost:172.16.58.234\r\n\r\n12345678";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, 48u);
                        BOOST_CHECK_EQUAL(data->last_status, SORRYSERVER_CONNECT);
                }

                cout << "[235]--------------------------------------------- " << endl;
                //unit_test[235] 未送信データサイズが０ データサイズ  ＞　０の場合
                //unit_test[235] データ状態がHTTP_BODYの場合
                //unit_test[235] find_http_header_all検索結果が該当あり check_http_method_and_versionチェック結果が正常の場合 ContentLengthがなし
                //unit_test[235] データ状態にHTTP_HEADERを設定する 未送信データサイズにHTTPヘッダサイズを設定する status = SORRYSERVER_CONNECT
                //\r\n\r\n Only
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/1.1\r\n\r\n12345678";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, 28u);
                        BOOST_CHECK_EQUAL(data->last_status, SORRYSERVER_CONNECT);
                }

                cout << "[236]--------------------------------------------- " << endl;
                //unit_test[236] 未送信データサイズが０ データサイズ  ＞　０の場合
                //unit_test[236] find_http_header_all検索結果が該当あり check_http_method_and_versionチェック結果が正常の場合 ContentLengthがあり
                //unit_test[236] データ状態にHTTP_HEADERを設定する 未送信データサイズに「HTTPヘッダサイズ　＋　ContentLength」を設定する status = SORRYSERVER_CONNECT
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        buffer = "GET /index.html HTTP/1.1\r\nContent-Length: 100rnHost:172.16.58.234\r\n\r\n12345678";
                        data->data_buffer = data_buffer;
                        memset(data->data_buffer, 0, MAX_BUFFER_SIZE);
                        memcpy(data->data_buffer, buffer, strlen(buffer));
                        data->data_state = HTTP_BODY;
                        data->data_offset = 0;
                        data->current_message_rest_size = 0;
                        data->data_length = strlen(buffer);
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_CONNECT);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->current_message_rest_size, (69u + 100u));
                        BOOST_CHECK_EQUAL(data->last_status, SORRYSERVER_CONNECT);
                }

                cout << "[237]--------------------------------------------- " << endl;
                //unit_test[237] 未送信データサイズが０の場合
                //unit_test[237] データサイズが０の場合
                //unit_test[237] データ状態がHTTP_BODYの場合
                //unit_test[237] データ状態にHTTP_STARTを設定する
                //unit_test[237] status = CLIENT_RECV
                //unit_test[237] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        data->data_state = HTTP_BODY;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);

                }
                cout << "[238]--------------------------------------------- " << endl;
                //unit_test[238] 未送信データサイズが０の場合
                //unit_test[238] データサイズが０の場合
                //unit_test[238] データ状態がHTTP_HEADERの場合
                //unit_test[238] status = CLIENT_RECV
                //unit_test[238] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        data->data_state = HTTP_HEADER;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);

                }

                cout << "[239]--------------------------------------------- " << endl;
                //unit_test[239] 未送信データサイズが０の場合
                //unit_test[239] データサイズが０の場合
                //unit_test[239] データ状態がUNKNOWNの場合
                //unit_test[239] status = CLIENT_RECV
                //unit_test[239] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data;

                        data->current_message_rest_size = 0;
                        data->data_length = 0;
                        data->data_offset = 5;
                        data->data_state = UNKNOWN;
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data->data_offset, 0u);

                }


                delete [] data_buffer;
        }

        //handle_sorryserver_send
        void handle_sorryserver_send_test_thread() {
                EVENT_TAG ret;

                cout << "[240]--------------------------------------------- " << endl;
                //unit_test[240] 未送信データサイズが０の場合
                //unit_test[240] データサイズが０の場合
                //unit_test[240] データ状態がUNKNOWNの場合
                //unit_test[240] status = CLIENT_RECV
                //unit_test[240] データ開始位置が０を設定する
                {
                        thread_data_ptr data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = data;

                                data->current_message_rest_size = 0;
                                data->data_length = 0;
                                data->data_offset = 5;
                                data->data_state = UNKNOWN;
                        }
                        ret = this->handle_sorryserver_send(boost::this_thread::get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                                BOOST_CHECK_EQUAL(data->data_state, UNKNOWN);
                                BOOST_CHECK_EQUAL(data->last_status, CLIENT_RECV);
                                BOOST_CHECK_EQUAL(data->data_offset, 0u);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_realserver_recv(tcp)
        void handle_realserver_recv_tcp_test() {
                {
                        cout << "[241]--------------------------------------------- " << endl;
                        //unit_test[241] recvデータサイズ>recvbufferサイズの場合 status = FINALIZE
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_length = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = MAX_BUFFER_SIZE + 1;
                        this->session_thread_data_map.clear();
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[242]--------------------------------------------- " << endl;
                        //unit_test[242]異常系 session_thread_data_map中にThreadIDなし場合のテスト
                        EVENT_TAG status;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        this->session_thread_data_map.clear();
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[243]--------------------------------------------- " << endl;
                        //unit_test[243] 異常系 session_thread_data_map中にThreadID対応のデータがない
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        EVENT_TAG status;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[244]--------------------------------------------- " << endl;
                        //unit_test[244] get_data_from_recvbuffer() 追加設定不可能の場合
                        //unit_test[244] status = FINALIZE
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - 1;
                        down_thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->switch_flag, SWITCH_FLAG_OFF);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[245]--------------------------------------------- " << endl;
                        //unit_test[245] get_data_from_recvbuffer() 追加設定可能,データ状態がUNKNOWNの場合
                        //unit_test[245] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "HTTP/1.1 e", 10);
                        down_thread_data->data_state = UNKNOWN;
                        memset((down_thread_data->data_buffer + down_thread_data->data_offset), 1, down_thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 20u);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[246]--------------------------------------------- " << endl;
                        //unit_test[246] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_HEADERの場合
                        //unit_test[246] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "HTTP/1.1 e", 10);
                        down_thread_data->data_state = HTTP_HEADER;
                        memset((down_thread_data->data_buffer + down_thread_data->data_offset), 1, down_thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[247]--------------------------------------------- " << endl;
                        //unit_test[247] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_BODYの場合
                        //unit_test[247] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "HTTP/1.1 e", 10);
                        down_thread_data->data_state = HTTP_BODY;
                        memset((down_thread_data->data_buffer + down_thread_data->data_offset), 1, down_thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[248]--------------------------------------------- " << endl;
                        //unit_test[248] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当なし,データサイズ　<　データバッファMAXサイズ　－　受信バッファサイズの場合
                        //unit_test[248] 遷移先ステータスを設定するstatus = REALSERVER_RECV
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = 50;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "abcdefghij", 10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 ert\r\nHIJKLMNrn" } };
                        size_t recvlen = 23;
                        boost::asio::ip::tcp::endpoint ep;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 33u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[249]--------------------------------------------- " << endl;
                        //unit_test[249] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当なし,データサイズ　>　データバッファMAXサイズ　－　受信バッファサイズ
                        //unit_test[249] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE - 10;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "ABCDEFG\r\nHIJKLMN\r\n" } };
                        size_t recvlen = 18;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE + 8u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE + 8u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[250]--------------------------------------------- " << endl;
                        //unit_test[250] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当なし,データサイズ　=　データバッファMAXサイズ　－　受信バッファサイズ
                        //unit_test[250] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE - 10;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "ABCDEFG\r\nH" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }

                {
                        cout << "[251]--------------------------------------------- " << endl;
                        //unit_test[251] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[251] check_http_version_and_status_code()チェック結果が異常の場合,
                        //unit_test[251] status=CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = 50;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "abcdefghij", 10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 ttt\r\nHIJKLMN\r\n\r\n" } };
                        size_t recvlen = 25;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 35u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 10u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 35u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }

                {
                        cout << "[252]--------------------------------------------- " << endl;
                        //unit_test[252]get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[252]check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当なしの場合,
                        //unit_test[252] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n"
                                }
                        };
                        size_t recvlen = 183;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 183u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 183u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[253]--------------------------------------------- " << endl;
                        //unit_test[253] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[253] check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当ありの場合,
                        //unit_test[253] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: 21887\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n"
                                }
                        };
                        size_t recvlen = 206;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 206u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 22093u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[254]--------------------------------------------- " << endl;
                        //unit_test[254] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[254] check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当ありの場合,
                        //unit_test[254] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: 3\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n1234567"
                                }
                        };
                        size_t recvlen = strlen(recvbuffer.data());
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, strlen(recvbuffer.data()));
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, strlen(recvbuffer.data()) - 7 + 3);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }

        }

        //handle_realserver_recv_tcp_test_thread
        void handle_realserver_recv_tcp_test_thread() {
                {
                        cout << "[255]--------------------------------------------- " << endl;
                        //unit_test[255] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[255] check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当ありの場合,
                        //unit_test[255] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                down_thread_data->current_message_rest_size = 0;
                                down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                                down_thread_data->data_length = 0;
                                down_thread_data->data_offset = 0;
                                down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                                down_thread_data->data_state = HTTP_START;
                                this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        }
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: 3\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n1234567"
                                }
                        };
                        size_t recvlen = strlen(recvbuffer.data());
                        boost::asio::ip::tcp::endpoint ep;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        status = this->handle_realserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, strlen(recvbuffer.data()));
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, strlen(recvbuffer.data()) - 7 + 3);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                                BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                                BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                                delete[] down_thread_data->data_buffer;
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_realserver_recv(udp)
        void handle_realserver_recv_udp_test() {
                boost::asio::ip::udp::endpoint rs_endpoint;
                boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                size_t recvlen = 0;

                cout << "[256]--------------------------------------------- " << endl;
                //unit_test[256] 異常系で必ずSTOPを返す
                BOOST_CHECK_EQUAL(this->handle_realserver_recv(boost::this_thread::get_id(), rs_endpoint, recvbuffer, recvlen), STOP);
        }

        //handle_sorryserver_recv
        void handle_sorryserver_recv_test() {
                {
                        cout << "[257]--------------------------------------------- " << endl;
                        //unit_test[257] recvデータサイズ>recvbufferサイズの場合 status = FINALIZE
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghijabcdefghij" } };
                        size_t recvlen = MAX_BUFFER_SIZE + 1;
                        this->session_thread_data_map.clear();
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[258]--------------------------------------------- " << endl;
                        //unit_test[258]異常系 session_thread_data_map中にThreadIDなし場合のテスト
                        EVENT_TAG status;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        this->session_thread_data_map.clear();
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[259]--------------------------------------------- " << endl;
                        //unit_test[259] 異常系 session_thread_data_map中にThreadID対応のデータがない
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        EVENT_TAG status;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[260]--------------------------------------------- " << endl;
                        //unit_test[260] get_data_from_recvbuffer() 追加設定不可能の場合
                        //unit_test[260] status = FINALIZE
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - 1;
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->switch_flag, SWITCH_FLAG_OFF);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[261]--------------------------------------------- " << endl;
                        //unit_test[261] get_data_from_recvbuffer() 追加設定可能,データ状態がUNKNOWNの場合
                        //unit_test[261] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "HTTP/1.1 e", 10);
                        down_thread_data->data_state = UNKNOWN;
                        memset((down_thread_data->data_buffer + down_thread_data->data_offset), 1, down_thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[262]--------------------------------------------- " << endl;
                        //unit_test[262] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_HEADERの場合
                        //unit_test[262] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "HTTP/1.1 e", 10);
                        down_thread_data->data_state = HTTP_HEADER;
                        memset((down_thread_data->data_buffer + down_thread_data->data_offset), 1, down_thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[263]--------------------------------------------- " << endl;
                        //unit_test[263] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_BODYの場合
                        //unit_test[263] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "HTTP/1.1 e", 10);
                        down_thread_data->data_state = HTTP_BODY;
                        memset((down_thread_data->data_buffer + down_thread_data->data_offset), 1, down_thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[264]--------------------------------------------- " << endl;
                        //unit_test[264] get_data_from_recvbuffer() 追加設定可能の場合,データ状態がHTTP_STARTの場合,find_http_header_all()検索結果が該当なし,データサイズ　<　データバッファMAXサイズ　－　受信バッファサイズ
                        //unit_test[264] status=SORRYSERVER_RECV
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = 50;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "abcdefghij", 10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 ert\r\nHIJKLMNrn" } };
                        size_t recvlen = 23;
                        boost::asio::ip::tcp::endpoint ep;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 33u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_START);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[265]--------------------------------------------- " << endl;
                        //unit_test[265] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当なし,データサイズ　>　データバッファMAXサイズ　－　受信バッファサイズ
                        //unit_test[265] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        down_thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE - 10;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "ABCDEFG\r\nHIJKLMNrn" } };
                        size_t recvlen = 18;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE + 8u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE + 8u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[266]--------------------------------------------- " << endl;
                        //unit_test[266] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当なし,データサイズ　=　データバッファMAXサイズ　－　受信バッファサイズ
                        //unit_test[266] 遷移先ステータスを設定するstatus = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        down_thread_data->data_length = MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE - 18;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "ABCDEFG\r\nHIJKLMNrn" } };
                        size_t recvlen = 18;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, MAX_IP_MODULE_BUFFER_SIZE - MAX_BUFFER_SIZE);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }

                {
                        cout << "[267]--------------------------------------------- " << endl;
                        //unit_test[267] get_data_from_recvbuffer()追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[267] check_http_version_and_status_code()チェック結果が異常の場合,
                        //unit_test[267] status=CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = 50;
                        down_thread_data->data_length = 10;
                        down_thread_data->data_offset = 10;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memcpy(down_thread_data->data_buffer, "abcdefghij", 10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 ttt\r\nHIJKLMN\r\n\r\n" } };
                        size_t recvlen = 25;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 35u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 10u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 35u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, UNKNOWN);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[268]--------------------------------------------- " << endl;
                        //unit_test[268]get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[268]check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当なしの場合,
                        //unit_test[268] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n"
                                }
                        };
                        size_t recvlen = 183;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 183u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 183u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[269]--------------------------------------------- " << endl;
                        //unit_test[269] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[269] check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当ありの場合,
                        //unit_test[269] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: 21887\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n"
                                }
                        };
                        size_t recvlen = 206;
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 206u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 22093u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[270]--------------------------------------------- " << endl;
                        //unit_test[270] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[270] check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当ありの場合,
                        //unit_test[270] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_offset = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                        down_thread_data->data_state = HTTP_START;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: 3\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n1234567"
                                }
                        };
                        size_t recvlen = strlen(recvbuffer.data());
                        boost::asio::ip::tcp::endpoint ep;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, recvlen);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, strlen(recvbuffer.data()) - 7 + 3);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }

        }

        //handle_sorryserver_recv_test_thread
        void handle_sorryserver_recv_test_thread() {
                {
                        cout << "[271]--------------------------------------------- " << endl;
                        //unit_test[271] get_data_from_recvbuffer() 追加設定可能,データ状態がHTTP_START,find_http_header_all()検索結果が該当ありの場合
                        //unit_test[271] check_http_version_and_status_code()チェック結果が正常の場合,find_http_header_content_length()検索結果が該当ありの場合,
                        //unit_test[271] status=CLIENT_CONNECTION_CHECK,データ状態にHTTP_HEADERを設定する
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                down_thread_data->current_message_rest_size = 0;
                                down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                                down_thread_data->data_length = 0;
                                down_thread_data->data_offset = 0;
                                down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                //memcpy(down_thread_data->data_buffer,"abcdefghij",10);
                                down_thread_data->data_state = HTTP_START;
                                this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        }
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "HTTP/1.1 200 OK\r\n"
                                        "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                                        "Accept-Ranges: bytes\r\n"
                                        "Content-Length: 3\r\n"
                                        "Content-Type: text/css\r\n"
                                        "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                                        "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                                        "\r\n1234567"
                                }
                        };
                        size_t recvlen = strlen(recvbuffer.data());
                        boost::asio::ip::tcp::endpoint ep;
                        this->forwarded_for = FORWARDED_FOR_ON;
                        status = this->handle_sorryserver_recv(boost::this_thread::get_id(), ep, recvbuffer, recvlen);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, recvlen);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 0u);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, strlen(recvbuffer.data()) - 7 + 3);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_state, HTTP_HEADER);
                                BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                                BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                                delete[] down_thread_data->data_buffer;
                        }
                }
        }

        //handle_response_send_inform
        void handle_response_send_inform_test() {
                cout << "[272]--------------------------------------------- " << endl;
                //unit_test[272] 正常系で必ずSTOPを返す
                boost::asio::ip::tcp::endpoint ep;
                BOOST_CHECK_EQUAL(this->handle_response_send_inform(boost::this_thread::get_id()), STOP);
        }

        //handle_client_connection_check
        void handle_client_connection_check_test() {
                boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                size_t datalen = 0;
                size_t datalen_t = 0;
                EVENT_TAG status;
                char *ip_replication_area_begain = NULL;
                int ip_replication_area_size = 100;
                std::string session_id;
                boost::asio::ip::tcp::endpoint virtual_service_endpoint;
                getloglevel_func_type ingetloglevel = stb_getloglevel;
                logger_func_type inputLogFatal = stb_putLogFatal;
                logger_func_type inputLogError = stb_putLogError;
                logger_func_type inputLogWarn = stb_putLogWarn;
                logger_func_type inputLogInfo = stb_putLogInfo;
                logger_func_type inputLogDebug = stb_putLogDebug;
                boost::asio::ip::tcp::endpoint rs_endpoint(boost::asio::ip::address::from_string("172.16.58.239"), 12345);

                cout << "[273]--------------------------------------------- " << endl;
                //unit_test[273] 異常系 session_thread_data_mapにスレッドIDが存在しない場合(map空)
                //unit_test[273] 戻り値がFINALIZEで設定する。
                {

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                        this->session_thread_data_map.clear();
                }

                cout << "[274]--------------------------------------------- " << endl;
                //unit_test[274] 異常系 session_thread_data_mapにスレッドIDが存在しない場合(map空ではない)
                //unit_test[274] 戻り値がFINALIZEで設定する。
                {
                        boost::thread dumy_thread(up_thread_func);
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        this->session_thread_data_map[dumy_thread.get_id()] = down_thread_data;

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                        this->session_thread_data_map.clear();
                }

                cout << "[275]--------------------------------------------- " << endl;
                //unit_test[275] 異常系 session_thread_data_map中にスレッドID対応のデータがない
                //unit_test[275] 戻り値がFINALIZEで設定する。
                {

                        thread_data_ptr down_thread_data;
                        boost::thread thread_down(&down_thread_func);

                        this->session_thread_data_map[thread_down.get_id()] = down_thread_data;
                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                        this->session_thread_data_map.clear();
                }
                cout << "[276]--------------------------------------------- " << endl;
                //unit_test[276] データ状態がUNKNOWNの場合
                //unit_test[276] 未送信データサイズを0になるの場合
                //unit_test[276] session_tableのlast_timeをnow_timeで更新する
                //unit_test[276] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 5u;
                        down_thread_data->data_state = UNKNOWN;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );
                        sendbuffer.assign('0');
                        datalen = 0;

                        ip_session_table_entry entry_input;
                        entry_input.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                        ->write_session_data_stub(down_thread_data->ip_hash, entry_input);

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 15u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 5u);

                        ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                                                       ->get_session_data_stub(down_thread_data->ip_hash);
                        BOOST_CHECK_EQUAL(entry.rs_endpoint, init_endpoint);
                        BOOST_CHECK(entry.last_time != 0);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor = NULL;
                        this->session_thread_data_map.clear();
                }

                cout << "[277]--------------------------------------------- " << endl;
                //unit_test[277] データ状態がUNKNOWNの場合
                //unit_test[277] 未送信データサイズを0になってない場合
                //unit_test[277] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 15u;
                        down_thread_data->data_state = UNKNOWN;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );
                        sendbuffer.assign('0');
                        datalen = 0;

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 5u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 20u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 0u);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor = NULL;
                        this->session_thread_data_map.clear();
                }
                cout << "[278]--------------------------------------------- " << endl;
                //unit_test[278] データ状態がHTTP_STARTの場合
                //unit_test[278] 未送信データサイズを0になるの場合
                //unit_test[278] session_tableのlast_timeをnow_timeで更新する
                //unit_test[278] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 5u;
                        down_thread_data->data_state = UNKNOWN;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );
                        sendbuffer.assign('0');
                        datalen = 0;

                        ip_session_table_entry entry_input;
                        entry_input.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                        ->write_session_data_stub(down_thread_data->ip_hash, entry_input);
                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 15u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 5u);

                        ip_session_table_entry entry_out = (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                                                           ->get_session_data_stub(down_thread_data->ip_hash);
                        BOOST_CHECK_EQUAL(entry_out.rs_endpoint, init_endpoint);
                        BOOST_CHECK(entry_out.last_time != 0);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor  = NULL;
                        this->session_thread_data_map.clear();
                }
                cout << "[279]--------------------------------------------- " << endl;
                //unit_test[279] データ状態がHTTP_STARTの場合
                //unit_test[279] 未送信データサイズを0になってない場合
                //unit_test[279] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 15u;
                        down_thread_data->data_state = HTTP_START;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );

                        sendbuffer.assign('0');
                        datalen = 0;

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 5u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 20u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 0u);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor = NULL;
                        this->session_thread_data_map.clear();
                }
                cout << "[280]--------------------------------------------- " << endl;
                //unit_test[280] データ状態がHTTP_BODYの場合
                //unit_test[280] 未送信データサイズを0になるの場合
                //unit_test[280] session_tableのlast_timeをnow_timeで更新する
                //unit_test[280] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 5u;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );
                        sendbuffer.assign('0');
                        datalen = 0;

                        ip_session_table_entry entry_input;
                        entry_input.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                        ->write_session_data_stub(down_thread_data->ip_hash, entry_input);
                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 15u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 5u);

                        ip_session_table_entry entry_out = (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                                                           ->get_session_data_stub(down_thread_data->ip_hash);
                        BOOST_CHECK_EQUAL(entry_out.rs_endpoint, init_endpoint);
                        BOOST_CHECK(entry_out.last_time != 0);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor  = NULL;
                        this->session_thread_data_map.clear();
                }
                cout << "[281]--------------------------------------------- " << endl;
                //unit_test[281] データ状態がHTTP_BODYの場合
                //unit_test[281] 未送信データサイズを0になってない場合
                //unit_test[281] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 15u;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );

                        sendbuffer.assign('0');
                        datalen = 0;

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 5u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 20u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 0u);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor = NULL;
                        this->session_thread_data_map.clear();
                }
                cout << "[282]--------------------------------------------- " << endl;
                //unit_test[282] データ状態がHTTP_HEADERの場合
                //unit_test[282] 未送信データサイズを0になるの場合
                //unit_test[282] session_tableのlast_timeをnow_timeで更新する
                //unit_test[282] データ状態にHTTP_BODYを設定する
                //unit_test[282] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 5u;
                        down_thread_data->data_state = HTTP_HEADER;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );
                        sendbuffer.assign('0');
                        datalen = 0;

                        ip_session_table_entry entry_input;
                        entry_input.last_time = 0;
                        (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                        ->write_session_data_stub(down_thread_data->ip_hash, entry_input);
                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        BOOST_CHECK_EQUAL(down_thread_data->data_state, HTTP_BODY);
                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 0u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 15u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 5u);

                        ip_session_table_entry entry_out = (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                                                           ->get_session_data_stub(down_thread_data->ip_hash);
                        BOOST_CHECK_EQUAL(entry_out.rs_endpoint, init_endpoint);
                        BOOST_CHECK(entry_out.last_time != 0);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor  = NULL;
                        this->session_thread_data_map.clear();
                }
                cout << "[283]--------------------------------------------- " << endl;
                //unit_test[283] データ状態がHTTP_HEADERの場合
                //unit_test[283] 未送信データサイズを0になってない場合
                //unit_test[283] データ状態にHTTP_BODYを設定する
                //unit_test[283] 遷移先ステータスを設定するstatus = CLIENT_SEND

                {
                        boost::asio::ip::tcp::endpoint
                        ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                            12345);
                        boost::asio::ip::tcp::endpoint init_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        down_thread_data->current_message_rest_size = 15u;
                        down_thread_data->data_state = HTTP_HEADER;
                        down_thread_data->data_length = 10u;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_offset = 10u;
                        down_thread_data->client_endpoint = ep1;
                        down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                        memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                        this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                        ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                        inputLogWarn, inputLogInfo, inputLogDebug);
                        this->ip_data_processor = new ip_session_data_processor_stub(
                                3600, this->replication_data_processor, ingetloglevel,
                                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                inputLogDebug);

                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                             down_thread_data->data_length
                                            );

                        sendbuffer.assign('0');
                        datalen = 0;

                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);

                        BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                        BOOST_CHECK_EQUAL(datalen, datalen_t);
                        BOOST_CHECK_EQUAL(down_thread_data->data_state, HTTP_BODY);

                        int cmp_ret = memcmp(sendbuffer.data(),
                                             down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                        BOOST_CHECK_EQUAL(cmp_ret, 0);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 5u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 20u);
                        BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 0u);

                        delete down_thread_data->data_buffer;
                        delete this->ip_data_processor;
                        delete this->replication_data_processor;
                        this->ip_data_processor = NULL;
                        this->replication_data_processor = NULL;
                        this->session_thread_data_map.clear();
                }

        }

        void handle_client_connection_check_test_thread() {
                boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                size_t datalen = 0;
                size_t datalen_t = 0;
                EVENT_TAG status;
                char *ip_replication_area_begain = NULL;
                int ip_replication_area_size = 100;
                std::string session_id;
                boost::asio::ip::tcp::endpoint virtual_service_endpoint;
                getloglevel_func_type ingetloglevel = stb_getloglevel;
                logger_func_type inputLogFatal = stb_putLogFatal;
                logger_func_type inputLogError = stb_putLogError;
                logger_func_type inputLogWarn = stb_putLogWarn;
                logger_func_type inputLogInfo = stb_putLogInfo;
                logger_func_type inputLogDebug = stb_putLogDebug;
                boost::asio::ip::tcp::endpoint rs_endpoint(boost::asio::ip::address::from_string("172.16.58.239"), 12345);
                boost::asio::ip::tcp::endpoint init_endpoint;
                {
                        cout << "[284]--------------------------------------------- " << endl;
                        //unit_test[284] データ状態がUNKNOWNの場合
                        //unit_test[284] 未送信データサイズを0になるの場合
                        //unit_test[284] session_tableのlast_timeをnow_timeで更新する
                        //unit_test[284] 遷移先ステータスを設定するstatus = CLIENT_SEND
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                boost::asio::ip::tcp::endpoint
                                ep1(boost::asio::ip::address::from_string("192.168.120.249"),
                                    12345);


                                down_thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                                down_thread_data->current_message_rest_size = 5u;
                                down_thread_data->data_state = UNKNOWN;
                                down_thread_data->data_length = 10u;
                                down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                down_thread_data->data_offset = 10u;
                                down_thread_data->client_endpoint = ep1;
                                down_thread_data->ip_hash = l7vs_ip_service_calc_hash(ep1);

                                memset(down_thread_data->data_buffer + down_thread_data->data_offset, 1, down_thread_data->data_length);

                                this->replication_data_processor = new ip_replication_data_processor_stub(ip_replication_area_begain,
                                                ip_replication_area_size, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
                                                inputLogWarn, inputLogInfo, inputLogDebug);
                                this->ip_data_processor = new ip_session_data_processor_stub(
                                        3600, this->replication_data_processor, ingetloglevel,
                                        inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                                        inputLogDebug);

                                this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                                datalen_t = std::min(std::min(sendbuffer.size(), down_thread_data->current_message_rest_size),
                                                     down_thread_data->data_length
                                                    );
                                sendbuffer.assign('0');
                                datalen = 0;

                        }
                        {
                                boost::mutex::scoped_lock sclock(session_data_mutex);
                                ip_session_table_entry entry_input;
                                entry_input.last_time = 0;
                                (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                                ->write_session_data_stub(down_thread_data->ip_hash, entry_input);
                        }


                        status = this->handle_client_connection_check(boost::this_thread::get_id(),
                                        sendbuffer, datalen);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(status, CLIENT_SEND);
                                BOOST_CHECK_EQUAL(datalen, datalen_t);
                                int cmp_ret = memcmp(sendbuffer.data(),
                                                     down_thread_data->data_buffer + down_thread_data->data_offset - datalen_t, datalen_t);
                                BOOST_CHECK_EQUAL(cmp_ret, 0);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size, 0u);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_offset, 15u);
                                BOOST_CHECK_EQUAL(this->session_thread_data_map[boost::this_thread::get_id()]->data_length, 5u);

                                ip_session_table_entry entry = (dynamic_cast<ip_session_data_processor_stub *>(ip_data_processor))
                                                               ->get_session_data_stub(down_thread_data->ip_hash);
                                //BOOST_CHECK_EQUAL(entry.rs_endpoint, init_endpoint);
                                //BOOST_CHECK(entry.last_time != 0);

                                delete [] down_thread_data->data_buffer;
                                this->session_thread_data_map.erase(boost::this_thread::get_id());

                        }
                }
        }

        //handle_client_select
        void handle_client_select_test() {
                cout << "[285]--------------------------------------------- " << endl;
                //unit_test[285] 戻り値がSTOPで設定する。
                boost::asio::ip::udp::endpoint cl_endpoint;
                boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                size_t datalen;
                EVENT_TAG status;
                status = this->handle_client_select(boost::this_thread::get_id(), cl_endpoint, sendbuffer, datalen);
                BOOST_CHECK_EQUAL(status, STOP);
        }

        //handle_client_send
        void handle_client_send_test() {
                {
                        cout << "[286]--------------------------------------------- " << endl;
                        //unit_test[286] 未送信データサイズ>０, データサイズ>０, 切替中フラグON, SORRYフラグONの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[287]--------------------------------------------- " << endl;
                        //unit_test[287] 未送信データサイズ>０, データサイズが０, 切替中フラグON, SORRYフラグONの場合, status = CLIENT_DISCONNECT
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[288]--------------------------------------------- " << endl;
                        //unit_test[288] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当なし, 切替中フラグON, SORRYフラグONの場合
                        //unit_test[288] status = CLIENT_DISCONNECT
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 14;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[289]--------------------------------------------- " << endl;
                        //unit_test[289] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[289] check_http_version_and_status_code, チェック結果が異常, 切替中フラグON, SORRYフラグONの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "ABCDEFG\r\nHIJKLMN\r\n\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(UNKNOWN, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[290]--------------------------------------------- " << endl;
                        //unit_test[290] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[290] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[290] 検索結果が該当あり, 切替中フラグON, SORRYフラグONの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "Content-Length: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)(21887 + 206), this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[291]--------------------------------------------- " << endl;
                        //unit_test[291] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[291] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[291] 検索結果が該当なしり, 切替中フラグON, SORRYフラグONの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "XXXXXXX-XXXXXX: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)206, this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[292]--------------------------------------------- " << endl;
                        //unit_test[292] 未送信データサイズが０, データサイズが０,切替中フラグON, SORRYフラグONの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        delete[] down_thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[293]--------------------------------------------- " << endl;
                        //unit_test[293] 未送信データサイズ>０, データサイズ>０, 切替中フラグOFF, SORRYフラグONの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[294]--------------------------------------------- " << endl;
                        //unit_test[294] 未送信データサイズ>０, データサイズが０, 切替中フラグOFF, SORRYフラグONの場合, status = SORRYSERVER_RECV
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[295]--------------------------------------------- " << endl;
                        //unit_test[295] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当なし, 切替中フラグOFF, SORRYフラグONの場合
                        //unit_test[295] status = SORRYSERVER_RECV
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 14;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[296]--------------------------------------------- " << endl;
                        //unit_test[296] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[296] check_http_version_and_status_code, チェック結果が異常, 切替中フラグOFF, SORRYフラグONの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "ABCDEFG\r\nHIJKLMN\r\n\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(UNKNOWN, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[297]--------------------------------------------- " << endl;
                        //unit_test[297] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[297] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[297] 検索結果が該当あり, 切替中フラグOFF, SORRYフラグONの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "Content-Length: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)(21887 + 206), this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[298]--------------------------------------------- " << endl;
                        //unit_test[298] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[298] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[298] 検索結果が該当なしり, 切替中フラグOFF, SORRYフラグONの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "XXXXXXX-XXXXXX: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)206, this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[299]--------------------------------------------- " << endl;
                        //unit_test[299] 未送信データサイズが０, データサイズが０,切替中フラグOFF, SORRYフラグONの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_ON;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        delete[] down_thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[300]--------------------------------------------- " << endl;
                        //unit_test[300] 未送信データサイズ>０, データサイズ>０, 切替中フラグON, SORRYフラグOFFの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[301]--------------------------------------------- " << endl;
                        //unit_test[301] 未送信データサイズ>０, データサイズが０, 切替中フラグON, SORRYフラグOFFの場合, status = CLIENT_DISCONNECT
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[302]--------------------------------------------- " << endl;
                        //unit_test[302] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当なし, 切替中フラグON, SORRYフラグOFFの場合
                        //unit_test[302] status = CLIENT_DISCONNECT
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 14;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[303]--------------------------------------------- " << endl;
                        //unit_test[303] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[303] check_http_version_and_status_code, チェック結果が異常, 切替中フラグON, SORRYフラグOFFの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "ABCDEFG\r\nHIJKLMN\r\n\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(UNKNOWN, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[304]--------------------------------------------- " << endl;
                        //unit_test[304] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[304] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[304] 検索結果が該当あり, 切替中フラグON, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "Content-Length: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)(21887 + 206), this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[305]--------------------------------------------- " << endl;
                        //unit_test[305] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[305] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[305] 検索結果が該当なしり, 切替中フラグON, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "XXXXXXX-XXXXXX: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)206, this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[306]--------------------------------------------- " << endl;
                        //unit_test[306] 未送信データサイズが０, データサイズが０,切替中フラグON, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_ON;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        delete[] down_thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[307]--------------------------------------------- " << endl;
                        //unit_test[307] 未送信データサイズ>０, データサイズ>０, 切替中フラグOFF, SORRYフラグOFFの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[308]--------------------------------------------- " << endl;
                        //unit_test[308] 未送信データサイズ>０, データサイズが０, 切替中フラグOFF, SORRYフラグOFFの場合, status = REALSERVER_RECV
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 10;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[309]--------------------------------------------- " << endl;
                        //unit_test[309] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当なし, 切替中フラグOFF, SORRYフラグOFFの場合
                        //unit_test[309] status = REALSERVER_RECV
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 14;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200\r\n");
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[310]--------------------------------------------- " << endl;
                        //unit_test[310] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[310] check_http_version_and_status_code, チェック結果が異常, 切替中フラグOFF, SORRYフラグOFFの場合, status = CLIENT_CONNECTION_CHECK
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 20;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "ABCDEFG\r\nHIJKLMN\r\n\r\n");
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL((size_t)20, this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        BOOST_CHECK_EQUAL(UNKNOWN, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[311]--------------------------------------------- " << endl;
                        //unit_test[311] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[311] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[311] 検索結果が該当あり, 切替中フラグOFF, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "Content-Length: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)(21887 + 206), this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[312]--------------------------------------------- " << endl;
                        //unit_test[312] 未送信データサイズが０, データサイズ>０, find_http_header_all(), 検索結果が該当あり
                        //unit_test[312] check_http_version_and_status_code, チェック結果が正常, find_http_header_content_length
                        //unit_test[312] 検索結果が該当なしり, 切替中フラグOFF, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 206;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(down_thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        strcpy(down_thread_data->data_buffer, "HTTP/1.1 200 OK\r\n"
                               "Date: Thu, 29 Oct 2009 03:03:48 GMT\r\n"
                               "Accept-Ranges: bytes\r\n"
                               "XXXXXXX-XXXXXX: 21887\r\n"
                               "Content-Type: text/css\r\n"
                               "Last-Modified: Fri, 09 Jan 2009 19:16:12 GMT\r\n"
                               "X-Powered-By: Servlet/2.4 JSP/2.0\r\n"
                               "\r\n");
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, CLIENT_CONNECTION_CHECK);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_HEADER, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)206, this->session_thread_data_map[boost::this_thread::get_id()]->current_message_rest_size);
                        this->session_thread_data_map.clear();
                        delete[] down_thread_data->data_buffer;
                }
                {
                        cout << "[313]--------------------------------------------- " << endl;
                        //unit_test[313] 未送信データサイズが０, データサイズが０,切替中フラグOFF, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->current_message_rest_size = 0;
                        down_thread_data->data_length = 0;
                        down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        down_thread_data->data_state = HTTP_BODY;
                        down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        down_thread_data->data_offset = 0;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        delete[] down_thread_data->data_buffer;
                        this->session_thread_data_map.clear();
                }

                {
                        cout << "[314]--------------------------------------------- " << endl;
                        //unit_test[314] 異常の場合、status = FINALIZE
                        EVENT_TAG status;
                        this->session_thread_data_map.clear();
                        status = this->handle_client_send(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
        }

        //handle_client_send
        void handle_client_send_test_thread() {
                {
                        cout << "[315]--------------------------------------------- " << endl;
                        //unit_test[315] 未送信データサイズが０, データサイズが０,切替中フラグOFF, SORRYフラグOFFの場合
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                down_thread_data->current_message_rest_size = 0;
                                down_thread_data->data_length = 0;
                                down_thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                                down_thread_data->switch_flag = SWITCH_FLAG_OFF;
                                down_thread_data->data_state = HTTP_BODY;
                                down_thread_data->sorry_flag = SORRY_FLAG_OFF;
                                down_thread_data->data_offset = 0;
                                this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        }
                        status = this->handle_client_send(boost::this_thread::get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(status, REALSERVER_RECV);
                                BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                                BOOST_CHECK_EQUAL(HTTP_START, this->session_thread_data_map[boost::this_thread::get_id()]->data_state);
                                BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                                delete[] down_thread_data->data_buffer;
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_client_disconnect
        void handle_client_disconnect_test() {
                cout << "[316]--------------------------------------------- " << endl;
                //unit_test[316] 正常系で必ずFINALIZEを返す
                EVENT_TAG status;
                thread_data_ptr down_thread_data(new session_thread_data_ip);
                this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                status = this->handle_client_disconnect(boost::this_thread::get_id());
                BOOST_CHECK_EQUAL(status, FINALIZE);
                this->session_thread_data_map.clear();
        }

        void handle_client_disconnect_test_thread() {
                cout << "[317]--------------------------------------------- " << endl;
                //unit_test[317] 正常系で必ずFINALIZEを返す
                EVENT_TAG status;
                thread_data_ptr down_thread_data(new session_thread_data_ip);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                }
                status = this->handle_client_disconnect(boost::this_thread::get_id());
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.erase(boost::this_thread::get_id());
                }
        }

        //handle_sorry_enable
        void handle_sorry_enable_test() {
                EVENT_TAG status;
                thread_data_ptr thread_data(new session_thread_data_ip);
                boost::thread thread_up(&up_thread_func);
                boost::thread thread_down(&down_thread_func);

                // 上りスレッドの場合
                thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;

                cout << "[318]--------------------------------------------- " << endl;
                //unit_test[318] accept完了フラグOFFの場合,sorry状態をON,status = ACCEPT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, ACCEPT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, ACCEPT);
                }

                cout << "[319]--------------------------------------------- " << endl;
                //unit_test[319] accept完了フラグON,sorry状態,テータ状態がHTTP_STARTの場合,切替中フラグON status = SORRYSERVER_DISCONNECT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_START;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_DISCONNECT);
                }

                cout << "[320]--------------------------------------------- " << endl;
                //unit_test[320] accept完了フラグON,sorry状態,テータ状態がHTTP_HEADERの場合,切替中フラグON status = SORRYSERVER_DISCONNECT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_HEADER;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_DISCONNECT);
                }

                cout << "[321]--------------------------------------------- " << endl;
                //unit_test[321] accept完了フラグON,sorry状態,テータ状態がHTTP_BODYの場合, status = last_status
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = HTTP_BODY;
                        thread_data->last_status = CLIENT_RECV;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(thread_data->last_status, CLIENT_RECV);
                }

                cout << "[322]--------------------------------------------- " << endl;
                //unit_test[322] 上りスレッドの場合,accept完了フラグON,sorry状態,テータ状態がUNKNOWNの場合
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->data_state = UNKNOWN;
                        thread_data->last_status = CLIENT_RECV;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(thread_data->last_status, CLIENT_RECV);
                }

                cout << "[323]--------------------------------------------- " << endl;
                //unit_test[323] accept完了フラグON sorry状態以外,テータ状態がHTTP_STARTの場合,切替中フラグON,status = REALSERVER_DISCONNECT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_DISCONNECT);
                }

                cout << "[324]--------------------------------------------- " << endl;
                //unit_test[324] accept完了フラグON sorry状態以外,テータ状態がHTTP_HEADERの場合,切替中フラグON,status = REALSERVER_DISCONNECT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_HEADER;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_DISCONNECT);
                }

                cout << "[325]--------------------------------------------- " << endl;
                //unit_test[325] accept完了フラグON sorry状態以外,テータ状態がHTTP_BODYの場合,終了フラグをON,status = REALSERVER_DISCONNECT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_BODY;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->end_flag, END_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_DISCONNECT);
                }

                cout << "[326]--------------------------------------------- " << endl;
                //unit_test[326] accept完了フラグON sorry状態以外,テータ状態がUNKNOWNの場合,終了フラグをON,status = REALSERVER_DISCONNECT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_ON;
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = UNKNOWN;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->end_flag, END_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_DISCONNECT);
                }

                // 下りスレッドの場合
                thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
                {
                        cout << "[327]--------------------------------------------- " << endl;
                        //unit_test[327] sorry状態の場合 status = last_status
                        thread_data->sorry_flag = SORRY_FLAG_ON;
                        thread_data->last_status = SORRYSERVER_RECV;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_RECV);
                }

                cout << "[328]--------------------------------------------- " << endl;
                //unit_test[328] sorry状態以外の場合 テータ状態がHTTP_START　かつ　データサイズが０の場合,status = SORRYSERVER_RECV
                {
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->data_length = 0;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(thread_data->last_status, SORRYSERVER_RECV);
                }

                cout << "[329]--------------------------------------------- " << endl;
                //unit_test[329] sorry状態以外の場合 テータ状態がHTTP_START　かつ　データサイズ ＞０　の場合,status = REALSERVER_DISCONNECT
                {
                        thread_data->sorry_flag    = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_START;
                        thread_data->data_length = 5;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_DISCONNECT);
                }

                cout << "[330]--------------------------------------------- " << endl;
                //unit_test[330] sorry状態以外の場合 テータ状態がHTTP_HEADER の場合,切替中フラグON status = last_status
                {
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_HEADER;
                        thread_data->switch_flag = SWITCH_FLAG_OFF;
                        thread_data->last_status = REALSERVER_SEND;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);
                }

                cout << "[331]--------------------------------------------- " << endl;
                //unit_test[331] sorry状態以外の場合 テータ状態がHTTP_BODY の場合,切替中フラグON status = last_status
                {
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = HTTP_BODY;
                        thread_data->switch_flag = SWITCH_FLAG_OFF;
                        thread_data->last_status = REALSERVER_SEND;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_SEND);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_SEND);
                }

                cout << "[332]--------------------------------------------- " << endl;
                //unit_test[332] sorry状態以外の場合 テータ状態がUNKNOWN の場合,status = REALSERVER_DISCONNECT
                {
                        thread_data->sorry_flag = SORRY_FLAG_OFF;
                        thread_data->data_state = UNKNOWN;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(thread_data->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(status, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(thread_data->last_status, REALSERVER_DISCONNECT);
                }

                cout << "[333]--------------------------------------------- " << endl;
                //unit_test[333] session_thread_data NULLの場合,status = FINALIZE
                {
                        thread_data_ptr thread_data_null;
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data_null;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }

                cout << "[334]--------------------------------------------- " << endl;
                //unit_test[334] thread_id対応のsession_thread_dataなし場合,status = FINALIZE
                {
                        this->session_thread_data_map.clear();
                        status = this->handle_sorry_enable(thread_up.get_id());
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }

                cout << "[335]--------------------------------------------- " << endl;
                //unit_test[335] session_thread_dataにthread_idなし場合,status = FINALIZE
                {
                        thread_data_ptr thread_data_threadid_err(new session_thread_data_ip);

                        this->session_thread_data_map.clear();
                        this->session_thread_data_map[thread_down.get_id()] = thread_data_threadid_err;
                        status = this->handle_sorry_enable(thread_up.get_id());
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }

                cout << "[336]--------------------------------------------- " << endl;
                //unit_test[336] 上りスレッドの戻り値が「ACCEPT」を設定する、下りスレッドの戻り値が「SORRYSERVER_RECV」を設定する
                //unit_test[336] test data:上りスレッドと下りスレッドの場合
                {
                        thread_data_ptr thread_data_up(new session_thread_data_ip);
                        // 上りスレッド,accept完了フラグOFFの場合
                        thread_data_up->thread_division = THREAD_DIVISION_UP_STREAM;
                        thread_data_up->thread_id = thread_up.get_id();
                        thread_data_up->accept_end_flag = ACCEPT_END_FLAG_OFF;

                        // 下りスレッド,sorry状態以外の場合 テータ状態がHTTP_START　かつ　データサイズが０の場合,status = SORRYSERVER_RECV
                        thread_data_ptr thread_data_down(new session_thread_data_ip);
                        thread_data_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        thread_data_down->thread_id = thread_down.get_id();
                        thread_data_down->sorry_flag = SORRY_FLAG_OFF;
                        thread_data_down->data_state = HTTP_START;
                        thread_data_down->data_length = 0;
                        this->session_thread_data_map[thread_up.get_id()] = thread_data_up;
                        this->session_thread_data_map[thread_down.get_id()] = thread_data_down;
                        boost::thread_group threads;
                        threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorry_enable_test_thread_func,
                                                   this,
                                                   thread_up.get_id(),
                                                   ACCEPT));
                        threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorry_enable_test_thread_func,
                                                   this,
                                                   thread_down.get_id(),
                                                   SORRYSERVER_RECV));
                        threads.join_all();
                        // sorry状態をON
                        BOOST_CHECK_EQUAL(thread_data_up->sorry_flag, SORRY_FLAG_ON);
                        BOOST_CHECK_EQUAL(thread_data_down->sorry_flag, SORRY_FLAG_ON);
                }
        }

        void handle_sorry_enable_test_thread_func(const boost::thread::id thread_id,
                        EVENT_TAG check_value) {
                EVENT_TAG ret = this->handle_sorry_enable(thread_id);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(ret, check_value);
                }
        }
        void handle_sorry_enable_test_thread() {
                EVENT_TAG status;
                thread_data_ptr thread_data(new session_thread_data_ip);
                boost::thread thread_up(&up_thread_func);
                boost::thread thread_down(&down_thread_func);

                // 上りスレッドの場合
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                }
                cout << "[337]--------------------------------------------- " << endl;
                //unit_test[337] accept完了フラグOFFの場合,sorry状態をON,status = ACCEPT
                {
                        thread_data->accept_end_flag = ACCEPT_END_FLAG_OFF;
                        status = this->handle_sorry_enable(boost::this_thread::get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(thread_data->sorry_flag, SORRY_FLAG_ON);
                                BOOST_CHECK_EQUAL(status, ACCEPT);
                                BOOST_CHECK_EQUAL(thread_data->last_status, ACCEPT);
                        }
                }
        }

        //handle_sorry_disable
        void handle_sorry_disable_test() {

                EVENT_TAG ret;

                cout << "[338]--------------------------------------------- " << endl;
                //unit_test[338] 異常系 session_thread_data_map中にThreadID対応のデータがない
                {
                        //assign the map a value before reset
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[339]--------------------------------------------- " << endl;
                //unit_test[339] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                {
                        this->session_thread_data_map.clear();
                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }

                cout << "[340]--------------------------------------------- " << endl;
                //unit_test[340] 上りスレッド accept完了フラグOFFの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_OFF;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, ACCEPT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, ACCEPT);
                }
                cout << "[341]--------------------------------------------- " << endl;
                //unit_test[341] 上りスレッド accept完了フラグONの場合 sorry状態の場合 テータ状態がHTTP_STARTの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->data_state = HTTP_START;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);

                }


                cout << "[342]--------------------------------------------- " << endl;
                //unit_test[342] 上りスレッド accept完了フラグONの場合 sorry状態の場合 テータ状態がHTTP_HEADERの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->data_state = HTTP_HEADER;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);

                }


                cout << "[343]--------------------------------------------- " << endl;
                //unit_test[343] 上りスレッド accept完了フラグONの場合 sorry状態の場合 テータ状態がHTTP_BODYの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->end_flag = END_FLAG_OFF;
                        data_ptr->data_state = HTTP_BODY;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->end_flag, END_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);
                }


                cout << "[344]--------------------------------------------- " << endl;
                //unit_test[344] 上りスレッド accept完了フラグONの場合 sorry状態の場合 テータ状態がUNKNOWNの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->end_flag = END_FLAG_OFF;
                        data_ptr->data_state = UNKNOWN;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->end_flag, END_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);
                }

                cout << "[345]--------------------------------------------- " << endl;
                //unit_test[345] 上りスレッド accept完了フラグONの場合 sorry状態以外の場合 テータ状態がHTTP_STARTの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr->data_state = HTTP_START;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_DISCONNECT);
                }

                cout << "[346]--------------------------------------------- " << endl;
                //unit_test[346] 上りスレッド accept完了フラグONの場合 sorry状態以外の場合 テータ状態がHTTP_HEADERの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr->data_state = HTTP_HEADER;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = INITIALIZE;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, REALSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->switch_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_DISCONNECT);
                }


                cout << "[347]--------------------------------------------- " << endl;
                //unit_test[347] 上りスレッド accept完了フラグONの場合 sorry状態以外の場合 テータ状態がHTTP_BODYの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr->data_state = HTTP_BODY;
                        data_ptr->last_status = CLIENT_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_RECV);
                }



                cout << "[348]--------------------------------------------- " << endl;
                //unit_test[348] 上りスレッド accept完了フラグONの場合 sorry状態以外の場合 テータ状態がUNKNOWNの場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr->accept_end_flag = ACCEPT_END_FLAG_ON;
                        data_ptr->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr->data_state = UNKNOWN;
                        data_ptr->last_status = CLIENT_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, CLIENT_RECV);
                }


                cout << "[349]--------------------------------------------- " << endl;
                //unit_test[349] 下りスレッド  sorry状態以外の場合
                //unit_test[349] 遷移先ステータスを設定するstatus = last_status
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr->last_status = REALSERVER_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_RECV);
                }


                cout << "[350]--------------------------------------------- " << endl;
                //unit_test[350] 下りスレッド  sorry状態の場合 テータ状態がHTTP_START　かつ　データサイズが０の場合
                //unit_test[350] 遷移先ステータスを設定する status = REALSERVER_RECV
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr->data_state = HTTP_START;
                        data_ptr->data_length = 0;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->last_status = SORRYSERVER_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, REALSERVER_RECV);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, REALSERVER_RECV);
                }

                cout << "[351]--------------------------------------------- " << endl;
                //unit_test[351] 下りスレッド  sorry状態の場合 テータ状態がHTTP_START　かつ　データサイズ>０の場合
                //unit_test[351] 遷移先ステータスを設定する status = SORRYSERVER_DISCONNECT
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr->data_state = HTTP_START;
                        data_ptr->data_length = 100;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->last_status = SORRYSERVER_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);
                }

                cout << "[352]--------------------------------------------- " << endl;
                //unit_test[352] 下りスレッド  sorry状態の場合 テータ状態がHTTP_HEADER　の場合
                //unit_test[352] 切替中フラグON 遷移先ステータスを設定する`status = last_status
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr->data_state = HTTP_HEADER;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = SORRYSERVER_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        //BOOST_CHECK_EQUAL(data_ptr->swith_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_RECV);
                }

                cout << "[353]--------------------------------------------- " << endl;
                //unit_test[353] 下りスレッド  sorry状態の場合 テータ状態がHTTP_BODY　の場合
                //unit_test[353] 切替中フラグON 遷移先ステータスを設定する`status = last_status
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr->data_state = HTTP_BODY;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->switch_flag = SWITCH_FLAG_OFF;
                        data_ptr->last_status = SORRYSERVER_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_RECV);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        //BOOST_CHECK_EQUAL(data_ptr->swith_flag, SWITCH_FLAG_ON);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_RECV);
                }

                cout << "[354]--------------------------------------------- " << endl;
                //unit_test[354] 下りスレッド  sorry状態の場合 テータ状態がUNKNOWN　の場合
                //遷移先ステータスを設定する status = SORRYSERVER_DISCONNECT
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                        data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr->data_state = UNKNOWN;
                        data_ptr->sorry_flag = SORRY_FLAG_ON;
                        data_ptr->last_status = SORRYSERVER_RECV;

                        ret = this->handle_sorry_disable(boost::this_thread::get_id());

                        BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                        BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);
                }

                {
                        cout << "[355]--------------------------------------------- " << endl;
                        //unit_test[355]上りスレッドと下りスレッドの場合
                        boost::thread tdown_for_get_id(down_thread_func);
                        thread_data_ptr data_ptr_up(new session_thread_data_ip);
                        this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr_up;
                        data_ptr_up->thread_division = THREAD_DIVISION_UP_STREAM;
                        data_ptr_up->accept_end_flag = ACCEPT_END_FLAG_OFF;


                        thread_data_ptr data_ptr_down(new session_thread_data_ip);
                        this->session_thread_data_map[tdown_for_get_id.get_id()] = data_ptr_down;
                        data_ptr_down->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        data_ptr_down->sorry_flag = SORRY_FLAG_OFF;
                        data_ptr_down->last_status = REALSERVER_RECV;

                        boost::thread_group threads;
                        threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorry_disable_test_thread_func,
                                                   this,
                                                   boost::this_thread::get_id(),
                                                   ACCEPT));
                        threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorry_disable_test_thread_func,
                                                   this,
                                                   tdown_for_get_id.get_id(),
                                                   REALSERVER_RECV));
                        threads.join_all();
                        //sorryフラグをOFF
                        BOOST_CHECK_EQUAL(data_ptr_up->sorry_flag, SORRY_FLAG_OFF);
                        BOOST_CHECK_EQUAL(data_ptr_down->sorry_flag, SORRY_FLAG_OFF);
                }
        }

        void handle_sorry_disable_test_thread_func(const boost::thread::id thread_id,
                        EVENT_TAG check_value) {
                EVENT_TAG ret = this->handle_sorry_disable(thread_id);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(ret, check_value);
                }
        }

        void handle_sorry_disable_test_thread() {
                EVENT_TAG ret;
                cout << "[356]--------------------------------------------- " << endl;
                //unit_test[356] 下りスレッド  sorry状態の場合 テータ状態がUNKNOWN　の場合
                {
                        thread_data_ptr data_ptr(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                this->session_thread_data_map[boost::this_thread::get_id()] = data_ptr;
                                data_ptr->thread_division = THREAD_DIVISION_DOWN_STREAM;
                                data_ptr->data_state = UNKNOWN;
                                data_ptr->sorry_flag = SORRY_FLAG_ON;
                                data_ptr->last_status = SORRYSERVER_RECV;
                        }
                        ret = this->handle_sorry_disable(boost::this_thread::get_id());
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(ret, SORRYSERVER_DISCONNECT);
                                BOOST_CHECK_EQUAL(data_ptr->sorry_flag, SORRY_FLAG_OFF);
                                BOOST_CHECK_EQUAL(data_ptr->last_status, SORRYSERVER_DISCONNECT);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_realserver_disconnect(tcp)
        void handle_realserver_disconnect_tcp_test() {

                {
                        cout << "[357]--------------------------------------------- " << endl;
                        //unit_test[357] 異常系 session_thread_data_map中にThreadIDなし場合のテスト
                        EVENT_TAG status;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[358]--------------------------------------------- " << endl;
                        //unit_test[358] 異常系 session_thread_data_map中にThreadID対応のデータがない
                        //unit_test[358]assign the map a value before reset
                        this->session_thread_data_map[boost::this_thread::get_id()].reset();
                        EVENT_TAG status;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);

                        //遷移先ステータスを設定する status = FINALIZE
                        BOOST_CHECK_EQUAL(status, FINALIZE);
                }
                {
                        cout << "[359]--------------------------------------------- " << endl;
                        //unit_test[359]上りスレッド, 終了フラグがONの場合, status = CLIENT_RECV
                        EVENT_TAG status;
                        thread_data_ptr up_thread_data(new session_thread_data_ip);
                        up_thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                        up_thread_data->end_flag = END_FLAG_ON;
                        this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[360]--------------------------------------------- " << endl;
                        //unit_test[360]上りスレッド, 終了フラグがOFF, 切替中, sorryフラグがONの場合, status = SORRYSERVER_SELECT
                        EVENT_TAG status;
                        thread_data_ptr up_thread_data(new session_thread_data_ip);
                        up_thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                        up_thread_data->end_flag = END_FLAG_OFF;
                        up_thread_data->switch_flag = SWITCH_FLAG_ON;
                        up_thread_data->sorry_flag = SORRY_FLAG_ON;
                        this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        BOOST_CHECK_EQUAL(status, SORRYSERVER_SELECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(SWITCH_FLAG_OFF, this->session_thread_data_map[boost::this_thread::get_id()]->switch_flag);
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[361]--------------------------------------------- " << endl;
                        //unit_test[361]上りスレッド, 終了フラグがOFF, 切替中, sorryフラグがOFFの場合, status = REALSERVER_SELECT
                        EVENT_TAG status;
                        thread_data_ptr up_thread_data(new session_thread_data_ip);
                        up_thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                        up_thread_data->end_flag = END_FLAG_OFF;
                        up_thread_data->switch_flag = SWITCH_FLAG_ON;
                        up_thread_data->sorry_flag = SORRY_FLAG_OFF;
                        this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        BOOST_CHECK_EQUAL(status, REALSERVER_SELECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        BOOST_CHECK_EQUAL(SWITCH_FLAG_OFF, this->session_thread_data_map[boost::this_thread::get_id()]->switch_flag);
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[362]--------------------------------------------- " << endl;
                        //unit_test[362]上りスレッド, 終了フラグがOFF, 切替中でない場合, status = CLIENT_RECV
                        EVENT_TAG status;
                        thread_data_ptr up_thread_data(new session_thread_data_ip);
                        up_thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                        up_thread_data->end_flag = END_FLAG_OFF;
                        up_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        BOOST_CHECK_EQUAL(status, CLIENT_RECV);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[363]--------------------------------------------- " << endl;
                        //unit_test[363]下りスレッドの場合, status = CLIENT_DISCONNECT
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                        BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                        this->session_thread_data_map.clear();
                }
                {
                        cout << "[364]--------------------------------------------- " << endl;
                        //unit_test[364]上りスレッドと下りスレッドの場合
                        boost::thread tdown_for_get_id(down_thread_func);
                        thread_data_ptr up_thread_data(new session_thread_data_ip);
                        up_thread_data->thread_division = THREAD_DIVISION_UP_STREAM;
                        up_thread_data->end_flag = END_FLAG_OFF;
                        up_thread_data->switch_flag = SWITCH_FLAG_OFF;
                        this->session_thread_data_map[boost::this_thread::get_id()] = up_thread_data;
                        boost::asio::ip::tcp::endpoint rs_endpoint;

                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
                        this->session_thread_data_map[tdown_for_get_id.get_id()] = down_thread_data;

                        boost::thread_group threads;
                        threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_disconnect_test_thread_func,
                                                   this,
                                                   boost::this_thread::get_id(),
                                                   rs_endpoint,
                                                   CLIENT_RECV));
                        threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_disconnect_test_thread_func,
                                                   this,
                                                   tdown_for_get_id.get_id(),
                                                   rs_endpoint,
                                                   CLIENT_DISCONNECT));
                        threads.join_all();
                }
        }

        void handle_realserver_disconnect_test_thread_func(const boost::thread::id thread_id,
                        const boost::asio::ip::tcp::endpoint &rs_endpoint,
                        EVENT_TAG check_value) {
                EVENT_TAG ret = this->handle_realserver_disconnect(thread_id, rs_endpoint);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(ret, check_value);
                }
        }

        void handle_realserver_disconnect_tcp_test_thread() {
                {
                        cout << "[365]--------------------------------------------- " << endl;
                        //unit_test[365]下りスレッドの場合, status = CLIENT_DISCONNECT
                        EVENT_TAG status;
                        thread_data_ptr down_thread_data(new session_thread_data_ip);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                down_thread_data->thread_division = THREAD_DIVISION_DOWN_STREAM;
                                this->session_thread_data_map[boost::this_thread::get_id()] = down_thread_data;
                        }
                        boost::asio::ip::tcp::endpoint rs_endpoint;
                        status = this->handle_realserver_disconnect(boost::this_thread::get_id(), rs_endpoint);
                        {
                                boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                                BOOST_CHECK_EQUAL(status, CLIENT_DISCONNECT);
                                BOOST_CHECK_EQUAL(status, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                                this->session_thread_data_map.erase(boost::this_thread::get_id());
                        }
                }
        }

        //handle_sorryserver_disconnect
        void handle_sorryserver_disconnect_test() {
                EVENT_TAG ret;
                cout << "[366]--------------------------------------------- " << endl;
                //unit_test[366] 異常系 上りスレッドsession_thread_data_map中にThreadID対応のデータなし

                this->session_thread_data_map[boost::this_thread::get_id()].reset();
                boost::asio::ip::tcp::endpoint ep_err;
                ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep_err);
                //BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = FINALIZE
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[367]--------------------------------------------- " << endl;
                //unit_test[367] 異常系 上りスレッドsession_thread_data_map中にThreadIDなしの場合のテスト
                this->session_thread_data_map.clear();
                ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep_err);
                //BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = FINALIZE
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[368]--------------------------------------------- " << endl;
                //unit_test[368] 異常系 下りスレッドsession_thread_data_map中にThreadID対応のデータなし
                boost::thread t_err(down_thread_func);
                this->session_thread_data_map[t_err.get_id()].reset();
                ret = this->handle_sorryserver_disconnect(t_err.get_id(), ep_err);
                //BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = FINALIZE
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[369]--------------------------------------------- " << endl;
                //unit_test[369] 異常系 下りスレッドsession_thread_data_map中にThreadIDなしの場合のテスト
                this->session_thread_data_map.clear();
                ret = this->handle_sorryserver_disconnect(t_err.get_id(), ep_err);
                //BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = FINALIZE
                BOOST_CHECK_EQUAL(ret, FINALIZE);

                cout << "[370]--------------------------------------------- " << endl;
                //unit_test[370] 上りスレッドの場合,終了フラグがONの場合
                //unit_test[370] 遷移先ステータスを設定する status = CLIENT_RECV
                boost::asio::ip::tcp::endpoint ep1, ep_d1;
                thread_data_ptr data1(new session_thread_data_ip);
                this->session_thread_data_map[boost::this_thread::get_id()] = data1;
                data1->thread_division = THREAD_DIVISION_UP_STREAM;
                data1->end_flag = END_FLAG_ON;
                ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep1);
                BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = CLIENT_RECV
                BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

                cout << "[371]--------------------------------------------- " << endl;
                //unit_test[371] 上りスレッドの場合,終了フラグがOFF,sorryserver切替中の場合,sorryフラグがONの場合
                //unit_test[371] 遷移先ステータスを設定する status = SORRYSERVER_SELECT
                this->session_thread_data_map.clear();
                boost::asio::ip::tcp::endpoint ep2;
                thread_data_ptr data2(new session_thread_data_ip);
                this->session_thread_data_map[boost::this_thread::get_id()] = data2;
                data2->thread_division = THREAD_DIVISION_UP_STREAM;
                data2->end_flag = END_FLAG_OFF;
                data2->switch_flag = SWITCH_FLAG_ON;
                data2->sorry_flag = SORRY_FLAG_ON;
                ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep2);
                BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = SORRYSERVER_SELECT
                BOOST_CHECK_EQUAL(ret, SORRYSERVER_SELECT);
                BOOST_CHECK_EQUAL(data2->switch_flag, SWITCH_FLAG_OFF);

                cout << "[372]--------------------------------------------- " << endl;
                //unit_test[372] 上りスレッドの場合,終了フラグがOFF,sorryserver切替中の場合,sorryフラグがOFFの場合
                //unit_test[372] //遷移先ステータスを設定する status = REALSERVER_SELECT
                this->session_thread_data_map.clear();
                boost::asio::ip::tcp::endpoint ep3;
                thread_data_ptr data3(new session_thread_data_ip);
                this->session_thread_data_map[boost::this_thread::get_id()] = data3;
                data3->thread_division = THREAD_DIVISION_UP_STREAM;
                data3->end_flag = END_FLAG_OFF;
                data3->switch_flag = SWITCH_FLAG_ON;
                data3->sorry_flag = SORRY_FLAG_OFF;

                ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep3);
                BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = REALSERVER_SELECT
                BOOST_CHECK_EQUAL(ret, REALSERVER_SELECT);
                BOOST_CHECK_EQUAL(data2->switch_flag, SWITCH_FLAG_OFF);

                cout << "[373]--------------------------------------------- " << endl;
                //unit_test[373] 上りスレッドの場合,終了フラグがOFF,sorryserver切替中でない場合
                //unit_test[373] 遷移先ステータスを設定する status = CLIENT_RECV
                this->session_thread_data_map.clear();
                boost::asio::ip::tcp::endpoint ep4, ep_d4;
                thread_data_ptr data4(new session_thread_data_ip);
                this->session_thread_data_map[boost::this_thread::get_id()] = data4;
                data4->thread_division = THREAD_DIVISION_UP_STREAM;
                data4->end_flag = END_FLAG_OFF;
                data4->switch_flag = SWITCH_FLAG_OFF;

                ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep4);
                BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
                //遷移先ステータスを設定する status = CLIENT_RECV
                BOOST_CHECK_EQUAL(ret, CLIENT_RECV);

                cout << "[374]--------------------------------------------- " << endl;
                //unit_test[374] 下りスレッド
                //unit_test[374] status = CLIENT_DISCONNECT
                this->session_thread_data_map.clear();

                boost::thread t5(down_thread_func);
                boost::asio::ip::tcp::endpoint ep5;

                thread_data_ptr data5(new session_thread_data_ip);
                this->session_thread_data_map[t5.get_id()] = data5;
                //make 下りスレッドの場合 1->下りスレッド
                data5->thread_division = THREAD_DIVISION_DOWN_STREAM;

                ret = this->handle_sorryserver_disconnect(t5.get_id(), ep5);
                BOOST_CHECK_EQUAL(ret, this->session_thread_data_map[t5.get_id()]->last_status);
                BOOST_CHECK_EQUAL(ret, CLIENT_DISCONNECT);

                cout << "[375]--------------------------------------------- " << endl;
                //unit_test[375] 上りスレッドと下りスレッドの場合
                this->session_thread_data_map.clear();

                boost::thread tdown_for_get_id(down_thread_func);

                boost::asio::ip::tcp::endpoint epup;
                boost::asio::ip::tcp::endpoint epdown;

                //上りスレッドの場合,終了フラグがOFF,sorryserver切替中の場合,sorryフラグがONの場合
                thread_data_ptr dataup(new session_thread_data_ip);
                dataup->thread_id = boost::this_thread::get_id();
                dataup->thread_division = THREAD_DIVISION_UP_STREAM;
                dataup->pair_thread_id = tdown_for_get_id.get_id();
                dataup->end_flag = END_FLAG_OFF;
                dataup->switch_flag = SWITCH_FLAG_ON;
                dataup->sorry_flag = SORRY_FLAG_ON;

                //下りスレッド
                thread_data_ptr datadown(new session_thread_data_ip);
                datadown->thread_id = tdown_for_get_id.get_id();
                datadown->thread_division = THREAD_DIVISION_DOWN_STREAM;
                datadown->pair_thread_id = boost::this_thread::get_id();

                this->session_thread_data_map[boost::this_thread::get_id()] = dataup;
                this->session_thread_data_map[tdown_for_get_id.get_id()] = datadown;

                boost::thread_group threads;
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_disconnect_test_thread_func,
                                           this,
                                           boost::this_thread::get_id(),
                                           epup,
                                           SORRYSERVER_SELECT));
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_disconnect_test_thread_func,
                                           this,
                                           tdown_for_get_id.get_id(),
                                           epdown,
                                           CLIENT_DISCONNECT));
                threads.join_all();

                //終了フラグをON
                BOOST_CHECK_EQUAL(dataup->switch_flag, SWITCH_FLAG_OFF);
                BOOST_CHECK_EQUAL(SORRYSERVER_SELECT, this->session_thread_data_map[boost::this_thread::get_id()]->last_status);
        }

        void handle_sorryserver_disconnect_test_thread_func(const boost::thread::id thread_id,
                        const boost::asio::ip::tcp::endpoint &rs_endpoint, EVENT_TAG check_value) {
                EVENT_TAG ret = this->handle_sorryserver_disconnect(thread_id, rs_endpoint);
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(ret, check_value);
                }
        }

        void handle_sorryserver_disconnect_test_thread() {
                cout << "[376]--------------------------------------------- " << endl;
                //unit_test[376] 異常系 上りスレッドsession_thread_data_map中にThreadID対応のデータなし

                this->session_thread_data_map[boost::this_thread::get_id()].reset();
                boost::asio::ip::tcp::endpoint ep_err;
                EVENT_TAG ret = this->handle_sorryserver_disconnect(boost::this_thread::get_id(), ep_err);
                //遷移先ステータスを設定する status = FINALIZE
                {
                        boost::mutex::scoped_lock sclock(session_thread_data_map_mutex);
                        BOOST_CHECK_EQUAL(ret, FINALIZE);
                }
        }

        //create_x_forwarded_for
        void create_x_forwarded_for_test() {
                {
                        cout << "[377]--------------------------------------------- " << endl;
                        //unit_test[377] HTTPヘッダーでX-Forworded-Forを検索結果が該当なしの場合
                        bool ret = false;
                        std::string client_endpoint("172.16.58.64");
                        char buffer[] = "GET / HTTP/1.1\r\nHost: 172.16.58.66:80\r\n\r\n";
                        size_t buffer_len =  41;
                        size_t x_forwarded_for_insert_pos = 0;
                        std::string x_forwarded_for_context;
                        ret = this->create_x_forwarded_for(client_endpoint, buffer, buffer_len,
                                                           x_forwarded_for_insert_pos, x_forwarded_for_context);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL((size_t)16, x_forwarded_for_insert_pos);
                        BOOST_CHECK_EQUAL(0, x_forwarded_for_context.compare("X-Forwarded-For: 172.16.58.64\r\n"));
                }
                {
                        cout << "[378]--------------------------------------------- " << endl;
                        //unit_test[378] HTTPヘッダーでX-Forworded-Forを検索結果が該当ありの場合
                        bool ret = true;
                        std::string client_endpoint("172.16.58.64");
                        char buffer[] = "GET / HTTP/1.1\r\nHost: 172.16.58.66:80\r\nX-Forwarded-For: 172.16.58.69\r\n\r\n";
                        size_t buffer_len = 75;
                        size_t x_forwarded_for_insert_pos = 0;
                        std::string x_forwarded_for_context;
                        ret = this->create_x_forwarded_for(client_endpoint, buffer, buffer_len,
                                                           x_forwarded_for_insert_pos, x_forwarded_for_context);
                        BOOST_CHECK_EQUAL(false, ret);
                        BOOST_CHECK_EQUAL((size_t)68, x_forwarded_for_insert_pos);
                        BOOST_CHECK_EQUAL(0, x_forwarded_for_context.compare(", 172.16.58.64"));
                }
        }
        //handle_realserver_close
        void handle_realserver_close_test() {
                cout << "[379]--------------------------------------------- " << endl;
                //unit_test[379] handle_realserver_close 戻り値が「STOP」に設定する。
                boost::asio::ip::udp::endpoint ep;
                EVENT_TAG ret = this->handle_realserver_close(boost::this_thread::get_id(), ep);
                BOOST_CHECK_EQUAL(ret, STOP);
        }

        //get_data_from_recvbuffer
        void get_data_from_recvbuffer_test() {
                {
                        cout << "[380]--------------------------------------------- " << endl;
                        //unit_test[380] データバッファサイズ > 0, データサイズ > 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[380] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset + 10,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)20, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[381]--------------------------------------------- " << endl;
                        //unit_test[381] データバッファサイズ > 0, データサイズ = 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[381] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[382]--------------------------------------------- " << endl;
                        //unit_test[382] データバッファサイズ > 0, データサイズ > 0, データ開始位置 = 0, リクエストデータサイズ > 0
                        //unit_test[382] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset + 10,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)20, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[383]--------------------------------------------- " << endl;
                        //unit_test[383] データバッファサイズ > 0, データサイズ > 0, データ開始位置 > 0, リクエストデータサイズ = 0
                        //unit_test[383] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 0;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset + 10,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[384]--------------------------------------------- " << endl;
                        //unit_test[384] データバッファサイズ > 0, データサイズ = 0, データ開始位置 = 0, リクエストデータサイズ > 0
                        //unit_test[384] データバッファサイズ － データサイズ － データ開始位置 = リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 10;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[385]--------------------------------------------- " << endl;
                        //unit_test[385] データバッファサイズ > 0, データサイズ > 0, データ開始位置 = 0, リクエストデータサイズ = 0
                        //unit_test[385] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 0;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset + 10,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[386]--------------------------------------------- " << endl;
                        //unit_test[386] データバッファサイズ > 0, データサイズ = 0, データ開始位置 > 0, リクエストデータサイズ = 0
                        //unit_test[386] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 0;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)10, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[387]--------------------------------------------- " << endl;
                        //unit_test[387] データバッファサイズ > 0, データサイズ = 0, データ開始位置 = 0, リクエストデータサイズ = 0
                        //unit_test[387] データバッファサイズ － データサイズ － データ開始位置 > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer = { { "abcdefghij" } };
                        size_t recvlen = 0;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[388]--------------------------------------------- " << endl;
                        //unit_test[388] データバッファサイズ > 0, データサイズ > 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[388] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[388] データバッファサイズ － データサイズ > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_BUFFER_SIZE - 11;
                        memset(recvbuffer.c_array(), 2, recvlen);
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset + 10,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)(10 + MAX_BUFFER_SIZE - 11), this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[389]--------------------------------------------- " << endl;
                        //unit_test[389] データバッファサイズ > 0, データサイズ = 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[389] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[389] データバッファサイズ － データサイズ > リクエストデータサイズ
                        bool ret = false;
                        int cmpresult = -1;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset((thread_data->data_buffer + thread_data->data_offset), 1, thread_data->data_length);
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_BUFFER_SIZE;
                        memset(recvbuffer.c_array(), 2, recvlen);
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        cmpresult = memcmp(this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer +
                        this->session_thread_data_map[boost::this_thread::get_id()]->data_offset,
                        recvbuffer.c_array(), recvlen);
                        BOOST_CHECK_EQUAL(true, ret);
                        BOOST_CHECK_EQUAL(MAX_BUFFER_SIZE, this->session_thread_data_map[boost::this_thread::get_id()]->data_buffer_size);
                        BOOST_CHECK_EQUAL((size_t)(MAX_BUFFER_SIZE), this->session_thread_data_map[boost::this_thread::get_id()]->data_length);
                        BOOST_CHECK_EQUAL((size_t)0, this->session_thread_data_map[boost::this_thread::get_id()]->data_offset);
                        BOOST_CHECK_EQUAL(0, cmpresult);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[390]--------------------------------------------- " << endl;
                        //unit_test[390] データバッファサイズ > 0, データサイズ > 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[390] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[390] データバッファサイズ － データサイズ ＜ リクエストデータサイズ
                        //unit_test[390] データバッファサイズ > データバッファMAXサイズ　
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[391]--------------------------------------------- " << endl;
                        //unit_test[391] データバッファサイズ > 0, データサイズ > 0, データ開始位置 = 0, リクエストデータサイズ > 0
                        //unit_test[391] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[391] データバッファサイズ － データサイズ ＜ リクエストデータサイズ
                        //unit_test[391] データバッファサイズ > データバッファMAXサイズ　
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_IP_MODULE_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_IP_MODULE_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[392]--------------------------------------------- " << endl;
                        //unit_test[392] データバッファサイズ > 0, データサイズ > 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[392] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[392] データバッファサイズ － データサイズ ＜ リクエストデータサイズ
                        //unit_test[392] データバッファサイズ ＜ データバッファMAXサイズ
                        //unit_test[392] データサイズ ＋ リクエストデータサイズ ＞ データバッファMAXサイズ
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[393]--------------------------------------------- " << endl;
                        //unit_test[393] データバッファサイズ > 0, データサイズ = 0, データ開始位置 > 0, リクエストデータサイズ > 0
                        //unit_test[393] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[393] データバッファサイズ － データサイズ ＜ リクエストデータサイズ
                        //unit_test[393] データバッファサイズ ＜ データバッファMAXサイズ
                        //unit_test[393] データサイズ ＋ リクエストデータサイズ ＞ データバッファMAXサイズ
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 10;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE + 1;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[394]--------------------------------------------- " << endl;
                        //unit_test[394] データバッファサイズ > 0, データサイズ > 0, データ開始位置 = 0, リクエストデータサイズ > 0
                        //unit_test[394] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[394] データバッファサイズ － データサイズ ＜ リクエストデータサイズ
                        //unit_test[394] データバッファサイズ ＜ データバッファMAXサイズ
                        //unit_test[394] データサイズ ＋ リクエストデータサイズ ＞ データバッファMAXサイズ
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 10;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE + 1;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                        delete[] thread_data->data_buffer;
                }
                {
                        cout << "[395]--------------------------------------------- " << endl;
                        //unit_test[395] データバッファサイズ > 0, データサイズ = 0, データ開始位置 = 0, リクエストデータサイズ > 0
                        //unit_test[395] データバッファサイズ － データサイズ － データ開始位置 ＜ リクエストデータサイズ
                        //unit_test[395] データバッファサイズ － データサイズ ＜ リクエストデータサイズ
                        //unit_test[395] データバッファサイズ ＜ データバッファMAXサイズ
                        //unit_test[395] データサイズ ＋ リクエストデータサイズ ＞ データバッファMAXサイズ
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = 0;
                        thread_data->data_offset = 0;
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        this->session_thread_data_map[boost::this_thread::get_id()] = thread_data;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE + 1;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[396]--------------------------------------------- " << endl;
                        //unit_test[396]異常の場合、retu\r\n false
                        bool ret = true;
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = NULL;
                        boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
                        size_t recvlen = MAX_IP_MODULE_BUFFER_SIZE + 1;
                        ret = this->get_data_from_recvbuffer(thread_data, recvbuffer, recvlen);
                        BOOST_CHECK_EQUAL(false, ret);
                }
        }

        void put_data_into_sendbuffer_test() {
                char targetUrl_1[] = "GET / HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100\r\nHost: 172.16.58.66:80\r\n\r\n";
                char targetUrl_2[] = "GET / HTTP/1.1\r\nHost: 172.16.58.66:80\r\n\r\n";
                char forwardCtx_1[] = "X-Forwarded-For: 192.168.1.100\r\n";
                char forwardCtx_2[] = ", 192.168.1.101";
                char sorryUri_1[] = "/sorry.html";
                char result_1[] = "GET / HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100, 192.168.1.101\r\nHost: 172.16.58.66:80\r\n\r\n";
                char result_2[] = "GET / HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100\r\nHost: 172.16.58.66:80\r\n\r\n";
                char result_3[] = "GET /sorry.html HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100\r\nHost: 172.16.58.66:80\r\n\r\n";
                char result_4[] = "GET /sorry.html HTTP/1.1\r\nHost: 172.16.58.66:80\r\n\r\n";
                char result_5[] = "GET /sorry.html HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100, 192.168.1.101\r\nHost: 172.16.58.66:80\r\n\r\n";
                char result_6[] = "GET /sorry.html HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100\r\nHost: 172.16.58.66:80\r\n\r\n";
                std::pair<char *, size_t> buffer_element;

                {
                        cout << "[397]--------------------------------------------- " << endl;
                        //unit_test[397] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがOFF,forwarded_forがOFFの場合、データコピーが成功します。
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);
                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = thread_data->data_length;
                        thread_data->buffer_sequence.push_back(buffer_element);
                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), targetUrl_1, strlen(targetUrl_1)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(targetUrl_1));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[398]--------------------------------------------- " << endl;
                        //unit_test[398] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがOFF,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);
                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = thread_data->data_length;
                        thread_data->buffer_sequence.push_back(buffer_element);
                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), targetUrl_2, strlen(targetUrl_2)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(targetUrl_2));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[399]--------------------------------------------- " << endl;
                        //unit_test[399] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがOFF,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_1, "\r\nHost") - targetUrl_1;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_2);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_2);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "\r\nHost");
                        buffer_element.second = strlen("\r\nHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_1, strlen(result_1)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_1));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[400]--------------------------------------------- " << endl;
                        //unit_test[400] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがOFF,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_2, "Host") - targetUrl_2;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_1);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "Host");
                        buffer_element.second = strlen("Host: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_2, strlen(result_2)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_2));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[401]--------------------------------------------- " << endl;
                        //unit_test[401] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがON,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_1, "/ HTTP") - targetUrl_1;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100rnHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_3, strlen(result_3)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_3));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[402]--------------------------------------------- " << endl;
                        //unit_test[402] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがON,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_2, "/ HTTP") - targetUrl_2;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\nHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_4, strlen(result_4)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_4));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[403]--------------------------------------------- " << endl;
                        //unit_test[403] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがON,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_1, "/ HTTP") - targetUrl_1;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_2);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_2);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "\r\nHost");
                        buffer_element.second = strlen("\r\nHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_5, strlen(result_5)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_5));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[404]--------------------------------------------- " << endl;
                        //unit_test[404] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがOFF,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);
                        thread_data->buffer_sequence.clear();

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_2, "/ HTTP") - targetUrl_2;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_1);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "Host");
                        buffer_element.second = strlen("Host: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        memset(sendbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_6, strlen(result_6)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_6));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2));
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[405]--------------------------------------------- " << endl;
                        //unit_test[405] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがOFF,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);
                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = thread_data->data_length;
                        thread_data->buffer_sequence.push_back(buffer_element);
                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), targetUrl_1, strlen(targetUrl_1)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(targetUrl_1));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[406]--------------------------------------------- " << endl;
                        //unit_test[406] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがOFF,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);
                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = thread_data->data_length;
                        thread_data->buffer_sequence.push_back(buffer_element);
                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), targetUrl_2, strlen(targetUrl_2)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(targetUrl_2));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[407]--------------------------------------------- " << endl;
                        //unit_test[407] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがOFF,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_1, "\r\nHost") - targetUrl_1;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_2);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_2);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "\r\nHost");
                        buffer_element.second = strlen("\r\nHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_1, strlen(result_1)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_1));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[408]--------------------------------------------- " << endl;
                        //unit_test[408] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがOFF,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_2, "Host") - targetUrl_2;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_1);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "Host");
                        buffer_element.second = strlen("Host: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_2, strlen(result_2)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_2));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[409]--------------------------------------------- " << endl;
                        //unit_test[409] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがON,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_1, "/ HTTP") - targetUrl_1;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100rnHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_3, strlen(result_3)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_3));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[410]--------------------------------------------- " << endl;
                        //unit_test[410] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがON,forwarded_forがOFFの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_2, "/ HTTP") - targetUrl_2;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\nHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_4, strlen(result_4)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_4));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[411]--------------------------------------------- " << endl;
                        //unit_test[411] data_bufferにX-Forwarded-Forがあり,data_offsetが0,sorry_flagがON,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_1);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_1);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_1);

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_1, "/ HTTP") - targetUrl_1;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\nX-Forwarded-For: 192.168.1.100");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_2);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_2);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "\r\nHost");
                        buffer_element.second = strlen("\r\nHost: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_5, strlen(result_5)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_5));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_1) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[412]--------------------------------------------- " << endl;
                        //unit_test[412] data_bufferにX-Forwarded-Forが存在しない,data_offsetが0,sorry_flagがOFF,forwarded_forがONの場合、データコピーが成功します
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE];
                        memset(thread_data->data_buffer, 0, MAX_BUFFER_SIZE);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE;
                        thread_data->data_length = strlen(targetUrl_2);
                        thread_data->data_offset = 10;
                        thread_data->current_message_rest_size = strlen(targetUrl_2);
                        strcpy(thread_data->data_buffer + thread_data->data_offset, targetUrl_2);
                        thread_data->buffer_sequence.clear();

                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = strstr(targetUrl_2, "/ HTTP") - targetUrl_2;
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), sorryUri_1);
                        buffer_element.first = this->sorry_uri.c_array();
                        buffer_element.second = strlen(sorryUri_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, " HTTP");
                        buffer_element.second = strlen(" HTTP/1.1\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        memset(thread_data->forwarded_for_buffer.c_array(), 0, MAX_FORWARD_FOR_SIZE);
                        strcpy(thread_data->forwarded_for_buffer.c_array(), forwardCtx_1);
                        buffer_element.first = thread_data->forwarded_for_buffer.c_array();
                        buffer_element.second = strlen(forwardCtx_1);
                        thread_data->buffer_sequence.push_back(buffer_element);

                        buffer_element.first = strstr(thread_data->data_buffer + thread_data->data_offset, "Host");
                        buffer_element.second = strlen("Host: 172.16.58.66:80\r\n\r\n");
                        thread_data->buffer_sequence.push_back(buffer_element);

                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        memset(sendbuffer.c_array(), 0, MAX_BUFFER_SIZE);
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), result_6, strlen(result_6)), 0);
                        BOOST_CHECK_EQUAL(datalen, strlen(result_6));
                        BOOST_CHECK_EQUAL(thread_data->data_length, 0u);
                        BOOST_CHECK_EQUAL(thread_data->data_offset, strlen(targetUrl_2) + 10);
                        BOOST_CHECK_EQUAL(thread_data->current_message_rest_size, 0u);
                        delete[] thread_data->data_buffer;
                }

                {
                        cout << "[413]--------------------------------------------- " << endl;
                        //unit_test[413] data_buffer > sendbuffer の場合
                        thread_data_ptr thread_data(new session_thread_data_ip);
                        thread_data->data_buffer = new char[MAX_BUFFER_SIZE + 1];
                        memset(thread_data->data_buffer, 1, MAX_BUFFER_SIZE + 1);
                        thread_data->data_buffer_size = MAX_BUFFER_SIZE + 1;
                        thread_data->data_length = MAX_BUFFER_SIZE + 1;
                        thread_data->data_offset = 0;
                        thread_data->current_message_rest_size = MAX_BUFFER_SIZE + 1;
                        buffer_element.first = thread_data->data_buffer + thread_data->data_offset;
                        buffer_element.second = thread_data->data_length;
                        thread_data->buffer_sequence.push_back(buffer_element);
                        boost::array<char, MAX_BUFFER_SIZE> sendbuffer;
                        size_t datalen = 0;
                        this->put_data_into_sendbuffer(thread_data, sendbuffer, datalen);
                        BOOST_CHECK_EQUAL(memcmp(sendbuffer.c_array(), thread_data->data_buffer, MAX_BUFFER_SIZE), 0);
                        BOOST_CHECK_EQUAL(datalen, MAX_BUFFER_SIZE);
                        delete[] thread_data->data_buffer;
                }
        }

        void get_option_info_test() {
                {
                        cout << "[414]--------------------------------------------- " << endl;
                        //unit_test[414] timeoutが0、forwarded_forが0、rescheduleが0、sorry_uriが""の場合
                        this->timeout = 0;
                        this->forwarded_for = 0;
                        this->reschedule = 0;
                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        std::string option;
                        this->get_option_info(option);
                        BOOST_CHECK_EQUAL(strcmp(option.c_str(), "--timeout 0 --no-reschedule --sorry-uri ''"), 0);
                }
                {
                        cout << "[415]--------------------------------------------- " << endl;
                        //unit_test[415] timeoutが3600、forwarded_forが1、rescheduleが1、sorry_uriが"/sorry"の場合
                        this->timeout = 3600;
                        this->forwarded_for = 1;
                        this->reschedule = 1;
                        memset(this->sorry_uri.c_array(), 0, MAX_OPTION_SIZE);
                        strcpy(this->sorry_uri.c_array(), "/sorry");
                        std::string option;
                        this->get_option_info(option);
                        BOOST_CHECK_EQUAL(strcmp(option.c_str(), "--timeout 3600 --forwarded-for --reschedule --sorry-uri '/sorry'"), 0);
                }
        }

        //replication_interrupt
        void replication_interrupt_test() {
                install_stb_replication_func();
                unsigned int data_size = 0;
                void *data_addr = NULL;
                const boost::asio::ip::tcp::endpoint ep;
                data_addr = replication_pay_memory(get_name(), &data_size);
                this->replication_data_processor = new ip_replication_data_processor_stub(
                        (char *)data_addr, data_size, ep, stb_getloglevel,
                        stb_putLogFatal, stb_putLogError, stb_putLogWarn, stb_putLogInfo,
                        stb_putLogDebug);
                cout << "[416]--------------------------------------------- " << endl;
                //unit_test[416] op_codeが「A」の場合、replication_dataをreplication_areaに追加する。
                boost::asio::ip::tcp::endpoint cl_endpoint = string_to_endpoint <boost::asio::ip::tcp> ("172.16.58.239:1234");
                time_t last_time_A = time(0);
                int ip_hash_a = (int)calc_hash_2(cl_endpoint);
                boost::asio::ip::tcp::endpoint real_ep_A;
                struct ip_replication_temp_data replication_temp_data_add;
                replication_temp_data_add.op_code = 'A';
                replication_temp_data_add.ip_hash = (int)calc_hash_2(cl_endpoint);
                replication_temp_data_add.last_time = last_time_A;
                replication_temp_data_add.rs_endpoint = real_ep_A;
                ip_replication_data_processor_stub *pstub;
                try {
                        pstub = dynamic_cast<ip_replication_data_processor_stub *>(replication_data_processor);
                } catch (std::bad_cast &) {
                        std::cout << "bad_cast:::" << std::endl;
                }
                pstub->get_replication_area() = new ip_replication_data;
                pstub->register_replication_area_lock(replication_area_lock_stb);
                pstub->register_replication_area_unlock(replication_area_unlock_stb);
                pstub->get_temp_list().push_back(replication_temp_data_add);
                pstub->get_replication_area()->valid = 0;

                boost::thread test_thread1(boost::bind(&protocol_module_ip_test_class::replication_interrupt, this));
                sleep(5);
                BOOST_CHECK_EQUAL(pstub->get_replication_area()[ip_hash_a].valid, 1);
                BOOST_CHECK_EQUAL(pstub->get_replication_area()[ip_hash_a].last_time, last_time_A);
                BOOST_CHECK_EQUAL(pstub->get_replication_area()[ip_hash_a].realserver_ip, real_ep_A.address().to_string().c_str());
                BOOST_CHECK_EQUAL(pstub->get_replication_area()[ip_hash_a].realserver_port, real_ep_A.port());
                test_thread1.interrupt();

                cout << "[417]--------------------------------------------- " << endl;
                //unit_test[417] op_codeが「U」、且つip_hashが一致の場合、replication_area中の該当データを更新する。
                time_t last_time_U = time(0);
                boost::asio::ip::tcp::endpoint real_ep_U;
                struct ip_replication_temp_data replication_temp_data_update;
                replication_temp_data_update.op_code = 'U';
                replication_temp_data_update.ip_hash = (int)calc_hash_2(cl_endpoint);
                replication_temp_data_update.last_time = last_time_U;
                replication_temp_data_update.rs_endpoint = real_ep_U;
                pstub->get_temp_list().push_back(replication_temp_data_update);
                pstub->get_replication_area()->valid = 1;
                boost::thread test_thread2(boost::bind(&protocol_module_ip_test_class::replication_interrupt, this));
                sleep(5);
                BOOST_CHECK_EQUAL(pstub->get_replication_area()[ip_hash_a].valid, 1);
                BOOST_CHECK_EQUAL(pstub->get_replication_area()[ip_hash_a].last_time, last_time_U);
                test_thread2.interrupt();
        }
};

/***********************************************************************************
 *以下はBOOST_TEST_SUITEに入れる関数
 ***********************************************************************************/
//protocol_module_ip
void protocol_module_ip_test()
{
        protocol_module_ip_test_class obj;
        obj.protocol_module_ip_test();
}

//is_tcp
void is_tcp_test()
{
        protocol_module_ip_test_class obj;
        obj.is_tcp_test();
}

//is_udp
void is_udp_test()
{
        protocol_module_ip_test_class obj;
        obj.is_udp_test();
}

//get_name
void get_name_test()
{
        protocol_module_ip_test_class obj;
        obj.get_name_test();
}

//initialize
void initialize_test()
{
        protocol_module_ip_test_class obj;
        obj.initialize_test();
}

//finalize
void finalize_test()
{
        protocol_module_ip_test_class obj;
        obj.finalize_test();
}

//is_use_sorry
void is_use_sorry_test()
{
        protocol_module_ip_test_class obj;
        obj.is_use_sorry_test();
}

//check_parameter
void check_parameter_test()
{
        protocol_module_ip_test_class obj;
        obj.check_parameter_test();
}

//set_parameter
void set_parameter_test()
{
        protocol_module_ip_test_class obj;
        obj.set_parameter_test();
}

//add_parameter
void add_parameter_test()
{
        protocol_module_ip_test_class obj;
        obj.add_parameter_test();
}

//register_schedule(tcp)
void register_schedule_tcp_test()
{
        protocol_module_ip_test_class obj;
        obj.register_schedule_tcp_test();
}

//handle_session_initialize
void handle_session_initialize_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_session_initialize_test();
}

//handle_session_initialize
void handle_session_initialize_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_session_initialize_test_thread, &obj));
        }
        threads.join_all();
}

//handle_session_finalize
void handle_session_finalize_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_session_finalize_test();
}

//handle_session_finalize
void handle_session_finalize_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_session_finalize_test_thread, &obj));
        }
        threads.join_all();
}

//handle_accept
void handle_accept_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_accept_test();
}

//handle_accept
void handle_accept_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_accept_test_thread, &obj));
        }
        threads.join_all();
}

//handle_client_recv
void handle_client_recv_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_client_recv_test();
}

//handle_client_recv_thread
void handle_client_recv_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_client_recv_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_select(tcp)
void handle_realserver_select_tcp_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_select_tcp_test();
}

//handle_realserver_select(tcp)_thread
void handle_realserver_select_tcp_test_thread()
{
        protocol_module_ip_test_class obj;

        boost::asio::ip::tcp::endpoint virtual_service_endpoint;
        boost::function<LOG_LEVEL_TAG(void)> ingetloglevel = stb_getloglevel;
        boost::function < void(const unsigned int, const std::string &,
        const char *, int) > inputLogFatal = stb_putLogFatal;
        boost::function < void(const unsigned int, const std::string &,
        const char *, int) > inputLogError = stb_putLogError;
        boost::function < void(const unsigned int, const std::string &,
        const char *, int) > inputLogWarn = stb_putLogWarn;
        boost::function < void(const unsigned int, const std::string &,
        const char *, int) > inputLogInfo = stb_putLogInfo;
        boost::function < void(const unsigned int, const std::string &,
        const char *, int) > inputLogDebug = stb_putLogDebug;

        obj.set_replication_data_processor(new ip_replication_data_processor(NULL,
        10, virtual_service_endpoint, ingetloglevel, inputLogFatal, inputLogError,
        inputLogWarn, inputLogInfo, inputLogDebug));
        obj.set_ip_data_processor(new ip_session_data_processor_stub(
                3600, obj.get_replication_data_processor(), ingetloglevel,
                inputLogFatal, inputLogError, inputLogWarn, inputLogInfo,
                inputLogDebug));


        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; ++i) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_select_tcp_test_thread, &obj));
        }
        threads.join_all();

        obj.release_replication_data_processor();
        obj.release_ip_data_processor();
}

//handle_realserver_select(udp)
void handle_realserver_select_udp_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_select_udp_test();
}

//handle_realserver_connect
void handle_realserver_connect_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_connect_test();
}

//handle_realserver_connect_thread
void handle_realserver_connect_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_connect_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_connection_fail
void handle_realserver_connection_fail_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_connection_fail_test();
}

void handle_realserver_connection_fail_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; ++i) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_connection_fail_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_send
void handle_realserver_send_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_send_test();
}

//handle_realserver_send
void handle_realserver_send_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_send_test_thread, &obj));
        }
        threads.join_all();
}

//handle_sorryserver_select
void handle_sorryserver_select_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorryserver_select_test();
}

void handle_sorryserver_select_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; ++i) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_select_test_thread, &obj));
        }
        threads.join_all();
}

//handle_sorryserver_connect
void handle_sorryserver_connect_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorryserver_connect_test();
}

//handle_sorryserver_connect_thread
void handle_sorryserver_connect_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_connect_test_thread, &obj));
        }
        threads.join_all();
}

//handle_sorryserver_connection_fail
void handle_sorryserver_connection_fail_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorryserver_connection_fail_test();
}

//handle_sorryserver_connection_fail
void handle_sorryserver_connection_fail_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_connection_fail_test_thread, &obj));
        }
        threads.join_all();
}

//handle_sorryserver_send
void handle_sorryserver_send_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorryserver_send_test();
}


//handle_sorryserver_send
void handle_sorryserver_send_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_send_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_recv(tcp)
void handle_realserver_recv_tcp_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_recv_tcp_test();
}

//handle_realserver_recv_tcp_test_thread
void handle_realserver_recv_tcp_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_recv_tcp_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_recv(udp)
void handle_realserver_recv_udp_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_recv_udp_test();
}

//handle_sorryserver_recv
void handle_sorryserver_recv_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorryserver_recv_test();
}

//handle_sorryserver_recv_test_thread
void handle_sorryserver_recv_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_recv_test_thread, &obj));
        }
        threads.join_all();
}

//handle_response_send_inform
void handle_response_send_inform_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_response_send_inform_test();
}

//handle_client_connection_check
void handle_client_connection_check_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_client_connection_check_test();
}

void handle_client_connection_check_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; ++i) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_client_connection_check_test_thread, &obj));
        }
        threads.join_all();
}

//handle_client_select
void handle_client_select_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_client_select_test();
}

//handle_client_send
void handle_client_send_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_client_send_test();
}

//handle_client_send
void handle_client_send_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_client_send_test_thread, &obj));
        }
        threads.join_all();
}


//handle_client_disconnect
void handle_client_disconnect_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_client_disconnect_test();
}

//handle_sorry_enable
void handle_sorry_enable_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorry_enable_test();
}

//handle_sorry_enable_test_thread
void handle_sorry_enable_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorry_enable_test_thread, &obj));
        }
        threads.join_all();
}

//handle_sorry_disable
void handle_sorry_disable_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorry_disable_test();
}

//handle_sorry_disable_test_thread
void handle_sorry_disable_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; i++) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorry_disable_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_disconnect(tcp)
void handle_realserver_disconnect_tcp_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_disconnect_tcp_test();
}


void handle_realserver_disconnect_tcp_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; ++i) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_realserver_disconnect_tcp_test_thread, &obj));
        }
        threads.join_all();
}

//handle_sorryserver_disconnect
void handle_sorryserver_disconnect_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_sorryserver_disconnect_test();
}

void handle_sorryserver_disconnect_test_thread()
{
        protocol_module_ip_test_class obj;
        boost::thread_group threads;
        for (int i = 0; i < THREAD_COUNT; ++i) {
                threads.create_thread(bind(&protocol_module_ip_test_class::handle_sorryserver_disconnect_test_thread, &obj));
        }
        threads.join_all();
}

//handle_realserver_close
void handle_realserver_close_test()
{
        protocol_module_ip_test_class obj;
        obj.handle_realserver_close_test();
}
void create_x_forwarded_for_test()
{
        protocol_module_ip_test_class obj;
        obj.create_x_forwarded_for_test();
}
void get_data_from_recvbuffer_test()
{
        protocol_module_ip_test_class obj;
        obj.get_data_from_recvbuffer_test();
}
void put_data_into_sendbuffer_test()
{
        protocol_module_ip_test_class obj;
        obj.put_data_into_sendbuffer_test();
}
void get_option_info_test()
{
        protocol_module_ip_test_class obj;
        obj.get_option_info_test();
}
void replication_interrupt_test()
{
        protocol_module_ip_test_class obj;
        obj.replication_interrupt_test();
}


test_suite    *protocol_module_ip_test_main()
{

        test_suite *ts = BOOST_TEST_SUITE("ip test");

        ts->add(BOOST_TEST_CASE(&protocol_module_ip_test));
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
        ts->add(BOOST_TEST_CASE(&create_x_forwarded_for_test));
        ts->add(BOOST_TEST_CASE(&get_data_from_recvbuffer_test));
        ts->add(BOOST_TEST_CASE(&put_data_into_sendbuffer_test));
        ts->add(BOOST_TEST_CASE(&get_option_info_test));
        ts->add(BOOST_TEST_CASE(&replication_interrupt_test));

        framework::master_test_suite().add(ts);
        return 0;
}
