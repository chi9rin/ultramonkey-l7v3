#include "sslid_to_be_test_file.h"
#include <boost/thread.hpp>

using namespace boost::unit_test;
using namespace l7vs;

// declaration
std::list<realserver> rs_list;
bool function_check_flag = false;
void rs_list_init(void);
void rs_list_fini(void);
std::list<realserver>::iterator rs_list_itr_begin(void);
std::list<realserver>::iterator rs_list_itr_end(void);
std::list<realserver>::iterator rs_list_itr_next(void);
void list_lock(void);
void list_unlock(void);
void thread_callback(void);
void rs_list_init(void)
{
	for(int i=0; i<5; i++)
	{
		realserver rs;
		rs_list.push_back(rs);
	}
}

void rs_list_fini(void)
{
	rs_list.clear();
}

std::list<realserver>::iterator rs_list_itr_begin(void)
{
	function_check_flag = true;
	return rs_list.begin();
}

std::list<realserver>::iterator rs_list_itr_end(void)
{
	function_check_flag = true;
	return rs_list.end();
}

std::list<realserver>::iterator rs_list_itr_next(void)
{
	function_check_flag = true;
	return ++rs_list.begin();
}

void list_lock(void)
{
	function_check_flag = true;
}

void list_unlock(void)
{
	function_check_flag = true;
}

void thread_callback(void){}

void thread_func(){}

void thread_func2(){}

bool check=false;


//protocol_module_sslid 鮑志飛
void protocol_module_sslid_test(){

	protocol_module_sslid sslid;

	// unit_test[1] check the module name
	BOOST_CHECK_EQUAL("sslid", sslid.name);
}

//is_tcp 許秋粉
void is_tcp_test(){
    protocol_module_sslid sslid;
    //unit_test[1] is_tcp()メソッドのテスト　 normal
    //unit_test[1] test data:NULL
    BOOST_CHECK(sslid.is_tcp());
}

//is_udp 許秋粉
void is_udp_test(){
	protocol_module_sslid sslid;
	//unit_test[2] is_udp()メソッドのテスト　 normal
	//unit_test[2] test data:NULL
	BOOST_CHECK(!sslid.is_udp());
}

//get_name 許秋粉
void get_name_test()
{
	protocol_module_sslid sslid;
	std::string name=sslid.get_name();
	std::string sslidname="sslid";
	//unit_test[3]　get_name()メソッドのテスト　 normal
	//unit_test[3] test data:NULL
	BOOST_CHECK_EQUAL(sslidname,name);
}

//initialize 鮑志飛
void initialize_test(){

	std::list<realserver> rs_list;
	rs_list_init();
	boost::function<std::list<realserver>::iterator(void)> inrslist_begin = rs_list_itr_begin;
	boost::function<std::list<realserver>::iterator(void)> inrslist_end = rs_list_itr_end;
	boost::function<std::list<realserver>::iterator(void)> inrslist_next = rs_list_itr_next;
	boost::function<void(void)> inlist_lock = list_lock;
	boost::function<void(void)> inlist_unlock = list_unlock;
	l7vs::protocol_module_sslid sslid;

	// unit_test[3] check the initial data
	sslid.initialize(inrslist_begin, inrslist_end, inrslist_next, inlist_lock, inlist_unlock);
	BOOST_CHECK_EQUAL(sslid.rs_list_begin, rs_list_itr_begin);
	function_check_flag = false;
	sslid.rs_list_begin();
	BOOST_CHECK(function_check_flag);
	BOOST_CHECK_EQUAL(sslid.rs_list_end, rs_list_itr_end);
	function_check_flag = false;
	sslid.rs_list_end();
	BOOST_CHECK(function_check_flag);
	BOOST_CHECK_EQUAL(sslid.rs_list_next, rs_list_itr_next);
	function_check_flag = false;
	sslid.rs_list_next();
	BOOST_CHECK(function_check_flag);
	BOOST_CHECK_EQUAL(sslid.rs_list_lock, list_lock);
	function_check_flag = false;
	sslid.rs_list_lock();
	BOOST_CHECK(function_check_flag);
	BOOST_CHECK_EQUAL(sslid.rs_list_unlock, list_unlock);
	function_check_flag = false;
	sslid.rs_list_unlock();
	BOOST_CHECK(function_check_flag);

	// unit_test[4] pass NULL to initialize(), check function object
	sslid.initialize(NULL, NULL, NULL, NULL, NULL);
	BOOST_CHECK(!sslid.rs_list_begin);
	BOOST_CHECK(!sslid.rs_list_end);
	BOOST_CHECK(!sslid.rs_list_next);
	BOOST_CHECK(!sslid.rs_list_lock);
	BOOST_CHECK(!sslid.rs_list_unlock);

	rs_list_fini();
}

//finalize 鮑志飛
void finalize_test(){
	l7vs::protocol_module_sslid sslid;

	// unit_test[5] check finalize
	sslid.finalize();
	BOOST_CHECK(!sslid.getloglevel);
	BOOST_CHECK(!sslid.putLogFatal);
	BOOST_CHECK(!sslid.putLogError);
	BOOST_CHECK(!sslid.putLogWarn);
	BOOST_CHECK(!sslid.putLogInfo);
	BOOST_CHECK(!sslid.putLogDebug);
	BOOST_CHECK(!sslid.rs_list_begin);
	BOOST_CHECK(!sslid.rs_list_end);
	BOOST_CHECK(!sslid.rs_list_next);
	BOOST_CHECK(!sslid.rs_list_lock);
	BOOST_CHECK(!sslid.rs_list_unlock);
	BOOST_CHECK(!sslid.replication_pay_memory);
	BOOST_CHECK(!sslid.replication_area_lock);
	BOOST_CHECK(!sslid.replication_area_unlock);
	BOOST_CHECK(!sslid.schedule_tcp);
	BOOST_CHECK(!sslid.schedule_udp);
	BOOST_CHECK_EQUAL(sslid.timeout, 0);
	BOOST_CHECK_EQUAL(sslid.maxlist, 0);
	BOOST_CHECK_EQUAL(sslid.reschedule, 0);
	BOOST_CHECK(sslid.session_data_processor == NULL);
	BOOST_CHECK(sslid.replication_data_processor == NULL);
}

//is_use_sorry 許秋粉
void is_use_sorry_test()
{
	protocol_module_sslid  sslid;
	//unit_test[4] is_use_sorry()メソッドのテスト　 normal
	//unit_test[4] test data:NULL
	BOOST_CHECK(!sslid.is_use_sorry());
}

//check_parameter 許秋粉
void check_parameter_test()
{

	l7vs::protocol_module_sslid sslid;
	std::vector<std::string> args;

	//unit_test[13] オプション文字列が存在しない場合, チェック結果フラグにTRUEを設定する
	//unit_test[13] オプション文字列が存在しない場合
	args.push_back("l7vsdadm");
	protocol_module_base::check_message_result check_message;
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[14] オプション文字列 = "-T", timeout設定フラグ = OFF 次要素が存在しない場合,チェック結果フラグにTRUEを設定する
	//unit_test[14] data test:オプション文字列 = "-T", timeout設定フラグ = OFF 次要素が存在しない場合
	args.push_back("-T");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);


	//unit_test[15] オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列に数字以外が含まれている場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value 2ewqt is not numeric character."を設定する
	//unit_test[15] data test:オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列に数字以外が含まれている場合
	args.push_back("2ewqt");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout option' value 2ewqt is not numeric character.");

	//unit_test[16] オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 ＞ INT_MAX,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value 2148583647 is not numeric character."を設定する
	//unit_test[16] data test:オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 ＞ INT_MAX
	args.pop_back();
	args.push_back("2148583647");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout option' value 2148583647 is too large.");

	//unit_test[17] オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 < INT_MAX,チェック結果フラグにTRUEを設定する
	//unit_test[17] data test:オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 < INT_MAX
	args.pop_back();
	args.push_back("2000");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[18] オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 = INT_MAX,チェック結果フラグにTRUEを設定する
	//unit_test[18] data test:オプション文字列 = "-T" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 = INT_MAX
	args.pop_back();
	args.push_back("2147483674");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[19] オプション文字列 = "-T" timeout設定フラグ = ON 次要素の文字列が全て数字の場合 数値 <= INT_MAX,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[19] data test:オプション文字列 = "-T" timeout設定フラグ = ON 次要素の文字列が全て数字の場合 数値 = INT_MAX
	args.pop_back();
	args.push_back("2000");
	args.push_back("-T");
	args.push_back("3000");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/timeout'",check_message.message);

	//unit_test[20] オプション文字列 = "--timeout" timeout設定フラグ = ON 次要素の文字列が全て数字の場合 数値 <= INT_MAX,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[20] data test:オプション文字列 = "-T" timeout設定フラグ = ON 次要素の文字列が全て数字の場合 数値 = INT_MAX
	args.pop_back();
	args.pop_back();
	args.push_back("--timeout");
	args.push_back("100");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("Cannot set multiple option'-T/--timeout'",check_message.message);

	//unit_test[21] オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素が存在しない場合,チェック結果フラグにTRUEを設定する
	//unit_test[21] data test:オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素が存在しない場合
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.push_back("--timeout");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);


	//unit_test[22] オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列に数字以外が含まれている場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value 2ewqt is not numeric character."を設定する
	//unit_test[22] data test:オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列に数字以外が含まれている場合
	args.push_back("2ewqt");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout option' value 2ewqt is not numeric character.");

	//unit_test[23] オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 ＞ INT_MAX,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-T/--timeout' option value 2148583647 is not numeric character."を設定する
	//unit_test[23] data test:オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 ＞ INT_MAX
	args.pop_back();
	args.push_back("2148583647");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-T/--timeout option' value 2148583647 is too large.");

	//unit_test[24] オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 < INT_MAX,チェック結果フラグにTRUEを設定する
	//unit_test[24] data test:オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 < INT_MAX
	args.pop_back();
	args.push_back("2000");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[25] オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 = INT_MAX,チェック結果フラグにTRUEを設定する
	//unit_test[25] data test:オプション文字列 = "--timeout" timeout設定フラグ = OFF 次要素の文字列が全て数字の場合 数値 = INT_MAX
	args.pop_back();
	args.push_back("2147483674");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//数値 <= INT_MAX timeout設定フラグ = ON
	//unit_test[26] オプション文字列 = "--timeout" timeout設定フラグ = ON 次要素の文字列が全て数字の場合 数値 <= INT_MAX,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-T/timeout'"を設定する
	//unit_test[26] data test:オプション文字列 = "--timeout" timeout設定フラグ = ON 次要素の文字列が全て数字の場合 数値 <= INT_MAX
	args.pop_back();
	args.push_back("2000");
	args.push_back("-timeout");
	args.push_back("3000");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-T/timeout'",check_message.message);

	//maxlist
	//unit_test[27] オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素が存在しない場合,チェック結果フラグにTRUEを設定する
	//unit_test[27] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素が存在しない場合
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.push_back("-M");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[28] オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列に数字以外が含まれている場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist option' value 1st is not numeric character."を設定する
	//unit_test[28] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列に数字以外が含まれている
	args.push_back("1st");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist option' value 1st is not numeric character.");

	//unit_test[29] オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字 数値 ＞ INT_MAX場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist option' value 2148583647 is too large."を設定する
	//unit_test[29] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字 数値 ＞ INT_MAX
	args.pop_back();
	args.push_back("2148583647");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist option' value 2148583647 is too large.");

	//unit_test[30] オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字 数値 < INT_MAX場合,チェック結果フラグにTRUEを設定する
	//unit_test[30] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字 数値 < INT_MAX
	args.pop_back();
	args.push_back("100");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[31] オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字 数値 = INT_MAX場合,チェック結果フラグにTRUEを設定する
	//unit_test[31] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字 数値 = INT_MAX
	args.pop_back();
	args.push_back("2147483674");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);


	//unit_test[32] オプション文字列 = "-M" maxlist設定フラグ = ON 次要素の文字列が全て数字 数値 <= INT_MAX場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[32] data test:オプション文字列 = "-M" maxlist設定フラグ = ON 次要素の文字列が全て数字 数値 <= INT_MAX
	args.pop_back();
	args.push_back("100");
	args.push_back("-M");
	args.push_back("200");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/maxlist'",check_message.message);

	//unit_test[33] オプション文字列 = "--maxlist" maxlist設定フラグ = ON 次要素の文字列が全て数字 数値 <= INT_MAX場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[33] data test:オプション文字列 = "-M" maxlist設定フラグ = ON 次要素の文字列が全て数字 数値 <= INT_MAX
	args.pop_back();
	args.pop_back();
	args.push_back("--maxlist");
	args.push_back("200");
	check_message=sslid.check_parameter(args);
	//unit_test[21] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/maxlist'",check_message.message);

	//unit_test[34] オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素が存在しない場合,チェック結果フラグにTRUEを設定する
	//unit_test[34] data test:オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素が存在しない
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.push_back("--maxlist");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[35] オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素の文字列に数字以外が含まれている場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist option' value 1st is not numeric character."を設定する
	//unit_test[35] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列に数字以外が含まれている
	args.push_back("1st");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist option' value 1st is not numeric character.");

	//unit_test[36] オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素の文字列が全て数字,数値 ＞ INT_MAXの場合,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-M/--maxlist option' value 2148583647 is too large."を設定する
	//unit_test[36] data test:オプション文字列 = "-M" maxlist設定フラグ = OFF 次要素の文字列が全て数字,数値 ＞ INT_MAX
	args.pop_back();
	args.push_back("2148583647");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL(check_message.message,"'-M/--maxlist option' value 2148583647 is too large.");

	//unit_test[37] オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素の文字列が全て数字,数値 <= INT_MAX,チェック結果フラグにTRUEを設定する
	//unit_test[37] data test:オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素の文字列が全て数字,数値 <= INT_MAX
	args.pop_back();
	args.push_back("100");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[38] オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素の文字列が全て数字,数値 = INT_MAX,チェック結果フラグにTRUEを設定する
	//unit_test[38] data test:オプション文字列 = "--maxlist" maxlist設定フラグ = OFF 次要素の文字列が全て数字,数値 = INT_MAX
	args.pop_back();
	args.push_back("2147483674");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);


	//unit_test[39] オプション文字列 = "--maxlist",数値 <= INT_MAX maxlist設定フラグ = ON,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option '-M/maxlist'"を設定する
	//unit_test[39] data test:オプション文字列 = "--maxlist",数値 <= INT_MAX maxlist設定フラグ = ON
	args.pop_back();
	args.push_back("100");
	args.push_back("--maxlist");
	args.push_back("200");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("Cannot set multiple option '-M/maxlist'",check_message.message);

	//unit_test[40] オプション文字列 ="-R",no_reschedule設定フラグ = ON,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	//unit_test[40] data test:オプション文字列 ="-R",no_reschedule設定フラグ = ON
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.pop_back();
	args.push_back("-N");
	args.push_back("-R");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no_reschedule",check_message.message);

	//unit_test[41] オプション文字列 ="-N",reschedule設定フラグ = ON,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	//unit_test[41] data test:オプション文字列 ="-N",reschedule設定フラグ = ON
	args.pop_back();
	args.pop_back();
	args.push_back("-R");
	args.push_back("-N");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no_reschedule",check_message.message);

	//unit_test[42] オプション文字列 ="-T 2000 --maxlist 100 -R",チェック結果フラグにTRUEを設定する
	//unit_test[42] data test:オプション文字列 ="-T 2000 --maxlist 100 -R"
	args.pop_back();
	args.pop_back();
	args.push_back("-T");
	args.push_back("2000");
	args.push_back("--maxlist");
	args.push_back("100");
	args.push_back("-R");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(true,check_message.flag);

	//unit_test[43] オプション文字列 ="-T 2000 --maxlist 100 -R -N",チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to choose either of reschedule or no_rescheduleを設定する
	//unit_test[43] data test:オプション文字列 ="-T 2000 --maxlist 100 -R"
	args.push_back("-N");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("You have to choose either of reschedule or no_reschedule",check_message.message);

	//unit_test[44] オプション文字列 = 上記以外,チェック結果フラグにFALSEを設定する,チェック結果メッセージに"option error"を設定する
	//unit_test[44] data test:オプション文字列 = 上記以外
	args.pop_back();
	args.pop_back();
	args.push_back("-$");
	check_message=sslid.check_parameter(args);
	BOOST_CHECK_EQUAL(false,check_message.flag);
	BOOST_CHECK_EQUAL("option error",check_message.message);
}

//register_schedule 許秋粉
void t_schedule(const boost::thread::id,protocol_module_base::rs_list_itr_func_type,
		protocol_module_base::rs_list_itr_func_type,
		protocol_module_base::rs_list_itr_func_type,
		boost::asio::ip::tcp::endpoint&)
{
	check=true;
}
void register_schedule_test()
{
	protocol_module_sslid sslid;
	//unit_test[10] schedule_tcpにt_scheduleを設定する
	//unit_test[10] test data:NULL
	boost::thread thread1(thread_func);
	const boost::thread::id thread_id=thread1.get_id();
	protocol_module_base::rs_list_itr_func_type rs_1,rs_2,rs_3;
	boost::asio::ip::tcp::endpoint ep;
	protocol_module_base::tcp_schedule_func_type tcp_schedule;//initialize it
    tcp_schedule=t_schedule;
	sslid.register_schedule(tcp_schedule);
	sslid.schedule_tcp(thread_id,rs_1,rs_2,rs_3,ep);
	BOOST_CHECK_EQUAL(check,true);
	BOOST_CHECK_EQUAL(sslid.schedule_tcp,t_schedule);
}

//handle_session_finalize 鮑志飛
void handle_session_finalize_test(){
	boost::thread up_thread(thread_callback);
	boost::thread down_thread(thread_callback);
	boost::thread::id up_thread_id = up_thread.get_id();
	boost::thread::id down_thread_id = down_thread.get_id();
	protocol_module_base::EVENT_TAG status;
	protocol_module_sslid sslid;

	// unit_test[7]
	sslid.handle_session_initialize(up_thread_id, down_thread_id);
	status = sslid.handle_session_finalize(up_thread_id, down_thread_id);
	BOOST_CHECK_EQUAL(status, protocol_module_base::STOP);
	std::map<const boost::thread::id, session_thread_data_sslid*>::iterator itr;
	itr = sslid.session_thread_data_map.find(up_thread_id);
	BOOST_CHECK(itr == sslid.session_thread_data_map.end());
	itr = sslid.session_thread_data_map.find(down_thread_id);
	BOOST_CHECK(itr == sslid.session_thread_data_map.end());
}


//handle_accept 許秋粉
void handle_accept_test()
{
	protocol_module_sslid sslid;
	boost::thread thread1(thread_func);
	boost::thread::id thread1_id=thread1.get_id();
	//unit_test[5] handle_accpet()メソッドのテスト　 normal
	//unit_test[5] test data:NULL
	protocol_module_base::EVENT_TAG accept=sslid.handle_accept(thread1_id);
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, accept);
}

//handle_client_recv 鮑志飛
void handle_client_recv_test(){

	boost::thread up_thread(thread_callback);
	boost::thread down_thread(thread_callback);
	boost::thread::id up_thread_id = up_thread.get_id();
	boost::thread::id down_thread_id = down_thread.get_id();
	protocol_module_sslid sslid;
	session_thread_data_sslid* up_thread_data;
	boost::array<char, MAX_BUFFER_SIZE> recvbuffer;
	int recvlen = 0;
	protocol_module_base::EVENT_TAG status;
	unsigned short low_length = 0;
	unsigned short high_length = 0;
	unsigned short all_length = 0;
	int all_data_size = 0;

	// unit_test[8]
	// condition:
	//	end_flag = 1(ON)
	// check:
	//	handle_client_recv() return CLIENT_RECV
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 1;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(status, protocol_module_base::CLIENT_RECV);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[9]
	// condition:
	//	end_flag = 0 (OFF), data_size = 0, current_record_rest_size != 0
	// check:
	//  handle_client_recv() return REALSERVER_SELECT
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 0;
	up_thread_data->current_record_rest_size = 10;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[10]
	// condition:
	//  end_flag = 0 (OFF), data_size = 0, current_record_rest_size = 0, check_ssl_record_sendable() return -1 (異常)
	// check:
	//  handle_client_recv() return FINALIZE
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 0;
	up_thread_data->current_record_rest_size = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvbuffer[0] = 0x00;
	recvlen = 6;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(status, protocol_module_base::FINALIZE);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[11]
	// condition:
	//	end_flag = 0 (OFF), data_size = 0, current_record_rest_size = 0, check_ssl_record_sendable() return 1 (送信不可)
	// check:
	//  handle_client_recv() return CLIENT_RECV
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 0;
	up_thread_data->current_record_rest_size = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 1;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(status, protocol_module_base::CLIENT_RECV);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[12]
	// condition:
	//  end_flag = 0 (OFF), data_size = 0, current_record_rest_size = 0, check_ssl_record_sendable() return 0 (送信可能)
	//  is hello message
	// check:
	//  hello_message_flag = true, handle_client_recv() return REALSERVER_SELECT, current_record_rest_size
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 0;
	up_thread_data->current_record_rest_size = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 76;
	recvbuffer[0] = 0x16;
	recvbuffer[1] = 0x03;
	recvbuffer[2] = 0x01;
	recvbuffer[3] = 0x00;
	recvbuffer[4] = 0x9e;
	recvbuffer[5] = 0x01;
	recvbuffer[9] = 0x03;
	recvbuffer[10] = 0x01;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[4]));
	all_length = (high_length | low_length) + 5;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_length, sslid.session_thread_data_map[up_thread_id]->current_record_rest_size);
	BOOST_CHECK(sslid.session_thread_data_map[up_thread_id]->hello_message_flag);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[13]
	// condition:
	//  end_flag = 0 (OFF), data_size = 0, current_record_rest_size = 0, check_ssl_record_sendable() return 0 (送信可能)
	// is not hello message
	// check:
	// hello_message_flag = false, handle_client_recv() return REALSERVER_SELECT, current_record_rest_size
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 0;
	up_thread_data->current_record_rest_size = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 6;
	recvbuffer[0] = 0x17;
	recvbuffer[1] = 0x03;
	recvbuffer[2] = 0x01;
	recvbuffer[3] = 0x05;
	recvbuffer[4] = 0x8c;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[3]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[4]));
	all_length = (high_length | low_length) + 5;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_length, sslid.session_thread_data_map[up_thread_id]->current_record_rest_size);
	BOOST_CHECK(!sslid.session_thread_data_map[up_thread_id]->hello_message_flag);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[13]
	// condition:
	//  end_flag = 0, data_size > 0, current_record_rest_size != 0, data_begain_offset != 0
	// check:
	//  handle_client_recv() return REALSERVER_SELECT, data_size, data_begain_offset
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 10;
	up_thread_data->data_begain_offset = 36;
	up_thread_data->current_record_rest_size = 15;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 5;
	all_data_size = up_thread_data->data_size + recvlen;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[14]
	// condition:
	//  end_flag = 0, data_size > 0, current_record_rest_size != 0, data_begain_offset = 0
	// check:
	//  handle_client_recv() return REALSERVER_SELECT, data_size, data_begain_offset
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 10;
	up_thread_data->data_begain_offset = 0;
	up_thread_data->current_record_rest_size = 15;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 5;
	all_data_size = up_thread_data->data_size + recvlen;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[15]
	// condition:
	//  end_flag = 0, data_size > 0,current_record_rest_size = 0,data_begain_offset != 0
	//  check_ssl_record_sendable() return -1 (異常)
	// check:
	//  handle_client_recv() return FINALIZE, data_size, data_begain_offset
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 10;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_begain_offset = 12;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 5;
	all_data_size = up_thread_data->data_size + recvlen;
	recvbuffer[0] = 0x00;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(status, protocol_module_base::FINALIZE);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[16]
	// condition:
	//  end_flag = 0, data_size > 0,current_record_rest_size = 0,data_begain_offset = 0
	//  check_ssl_record_sendable() return -1 (異常)
	// check:
	//  handle_client_recv() return FINALIZE, data_size, data_begain_offset
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 10;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_begain_offset = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 5;
	all_data_size = up_thread_data->data_size + recvlen;
	recvbuffer[0] = 0x00;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(status, protocol_module_base::FINALIZE);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[17]
	// condition:
	//  end_flag = 0, data_size > 0,current_record_rest_size = 0,data_begain_offset != 0
	//  check_ssl_record_sendable() return 1 (送信不可)
	// check:
	//  handle_client_recv() return CLIENT_RECV, data_size, data_begain_offset
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 1;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_begain_offset = 13;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 2;
	all_data_size = up_thread_data->data_size + recvlen;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(status, protocol_module_base::CLIENT_RECV);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[18]
	// condition:
	//  end_flag = 0, data_size > 0,current_record_rest_size = 0,data_begain_offset = 0
	//  check_ssl_record_sendable() return 1 (送信不可)
	// check:
	//  handle_client_recv() return CLIENT_RECV, data_size, data_begain_offset
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 1;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_begain_offset = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 2;
	all_data_size = up_thread_data->data_size + recvlen;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(status, protocol_module_base::CLIENT_RECV);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[19]
	// condition:
	//  end_flag = 0, data_size > 0, current_record_rest_size = 0,data_begain_offset != 0
	//  check_ssl_record_sendable() return 0 (送信可能), is hello message
	// check:
	// handle_client_recv() return REALSERVER_SELECT, data_size, data_begain_offset,current_record_rest_size
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 3;
	up_thread_data->data_begain_offset = 12;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_buffer[12] = 0x16;
	up_thread_data->data_buffer[13] = 0x03;
	up_thread_data->data_buffer[14] = 0x01;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 76;
	all_data_size = up_thread_data->data_size + recvlen;
	recvbuffer[0] = 0x00;
	recvbuffer[1] = 0x9e;
	recvbuffer[2] = 0x01;
	recvbuffer[3] = 0x03;
	recvbuffer[4] = 0x01;
	recvbuffer[5] = 0x00;
	recvbuffer[6] = 0x03;
	recvbuffer[7] = 0x01;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[0]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[1]));
	all_length = (high_length | low_length) + 5;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK(sslid.session_thread_data_map[up_thread_id]->hello_message_flag);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(all_length, sslid.session_thread_data_map[up_thread_id]->current_record_rest_size);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[20]
	// condition:
	//  end_flag = 0, data_size > 0, current_record_rest_size = 0,data_begain_offset = 0
	//  check_ssl_record_sendable() return 0 (送信可能), is hello message
	// check:
	// handle_client_recv() return REALSERVER_SELECT, data_size, data_begain_offset,current_record_rest_size
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 3;
	up_thread_data->data_begain_offset = 0;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_buffer[12] = 0x16;
	up_thread_data->data_buffer[13] = 0x03;
	up_thread_data->data_buffer[14] = 0x01;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 76;
	all_data_size = up_thread_data->data_size + recvlen;
	recvbuffer[0] = 0x00;
	recvbuffer[1] = 0x9e;
	recvbuffer[2] = 0x01;
	recvbuffer[3] = 0x03;
	recvbuffer[4] = 0x01;
	recvbuffer[5] = 0x00;
	recvbuffer[6] = 0x03;
	recvbuffer[7] = 0x01;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[0]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[1]));
	all_length = (high_length | low_length) + 5;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK(sslid.session_thread_data_map[up_thread_id]->hello_message_flag);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(all_length, sslid.session_thread_data_map[up_thread_id]->current_record_rest_size);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[21]
	// condition:
	//  end_flag = 0,data_size>0,current_record_rest_size=0,data_begain_offset != 0
	//  check_ssl_record_sendable() return 0 (送信可能), is not hello message
	// check:
	// handle_client_recv() return REALSERVER_SELECT, data_size, data_begain_offset,current_record_rest_size
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 3;
	up_thread_data->data_begain_offset = 12;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_buffer[12] = 0x17;
	up_thread_data->data_buffer[13] = 0x03;
	up_thread_data->data_buffer[14] = 0x01;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 6;
	all_data_size = up_thread_data->data_size + recvlen;
	recvlen = 6;
	recvbuffer[0] = 0x00;
	recvbuffer[1] = 0x9e;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[0]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[1]));
	all_length = (high_length | low_length) + 5;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK(!sslid.session_thread_data_map[up_thread_id]->hello_message_flag);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(all_length, sslid.session_thread_data_map[up_thread_id]->current_record_rest_size);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[22]
	// condition:
	//  end_flag = 0,data_size>0,current_record_rest_size=0,data_begain_offset = 0
	//  check_ssl_record_sendable() return 0 (送信可能), is not hello message
	// check:
	// handle_client_recv() return REALSERVER_SELECT, data_size, data_begain_offset,current_record_rest_size
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 3;
	up_thread_data->data_begain_offset = 0;
	up_thread_data->current_record_rest_size = 0;
	up_thread_data->data_buffer[12] = 0x17;
	up_thread_data->data_buffer[13] = 0x03;
	up_thread_data->data_buffer[14] = 0x01;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = 6;
	all_data_size = up_thread_data->data_size + recvlen;
	recvlen = 6;
	recvbuffer[0] = 0x00;
	recvbuffer[1] = 0x9e;
	high_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[0]));
	high_length = high_length << 8;
	low_length = static_cast<unsigned short>(static_cast<unsigned char>(recvbuffer[1]));
	all_length = (high_length | low_length) + 5;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(all_data_size, sslid.session_thread_data_map[up_thread_id]->data_size);
	BOOST_CHECK(!sslid.session_thread_data_map[up_thread_id]->hello_message_flag);
	BOOST_CHECK_EQUAL(0, sslid.session_thread_data_map[up_thread_id]->data_begain_offset);
	BOOST_CHECK_EQUAL(all_length, sslid.session_thread_data_map[up_thread_id]->current_record_rest_size);
	BOOST_CHECK_EQUAL(status, protocol_module_base::REALSERVER_SELECT);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

	// unit_test[23]
	// condition:
	//  recvlen > recvbuffer.size()
	// check:
	//  handle_client_recv() return FINALIZE
	recvlen = recvbuffer.size() + 1;
	status = sslid.handle_client_recv(up_thread_id,recvbuffer,recvlen);
	BOOST_CHECK_EQUAL(status, protocol_module_base::FINALIZE);

	// unit_test[24]
	// condition:
	//  up_thread_id is not in map
	// check:
	//  handle_client_recv() return FINALIZE
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	BOOST_CHECK_EQUAL(status, protocol_module_base::FINALIZE);

	// unit_test[25]
	// condition:
	//  end_flag = 0, data_size+recvlen > MAX_BUFFER_SIZE
	// check:
	//  handle_client_recv() return FINALIZE
	up_thread_data = new session_thread_data_sslid;
	up_thread_data->end_flag = 0;
	up_thread_data->data_size = 10;
	up_thread_data->data_begain_offset = 0;
	sslid.session_thread_data_map[up_thread_id] = up_thread_data;
	recvlen = MAX_BUFFER_SIZE;
	status = sslid.handle_client_recv(up_thread_id, recvbuffer, recvlen);
	delete up_thread_data;
	sslid.session_thread_data_map.clear();

}

//handle_realserver_connection_fail 許秋粉
void handle_realserver_connection_fail_test()
{
	protocol_module_sslid sslid;
	boost::thread thread_up(thread_func);
	const boost::thread::id thread_up_id=thread_up.get_id();
	boost::asio::ip::tcp::endpoint ep;
	session_thread_data_sslid thread_up_data_value;
	thread_up_data_value.thread_division=0;
	thread_up_data_value.realserver_connect_failed_count=0;
	thread_up_data_value.data_begain_offset=0;
	thread_up_data_value.data_size=0;
	thread_up_data_value.current_record_rest_size=0;
	thread_up_data_value.hello_message_flag=false;
	session_thread_data_sslid* thread_up_data=&thread_up_data_value;
	sslid.session_thread_data_map.insert(std::pair<const boost::thread::id,session_thread_data_sslid*>(thread_up_id,thread_up_data));
	std::map<const boost::thread::id,session_thread_data_sslid*>::iterator iter;
	iter=sslid.session_thread_data_map.find(thread_up_id);
	session_thread_data_sslid* data=iter->second;
	if(iter == sslid.session_thread_data_map.end())
	{
		BOOST_MESSAGE("end of the map");
	}

	//unit_test[7] rescheduleモードの場合,realserver_connect_failed_count で1を加算する,遷移先ステータスを設定する,status = REALSERVER_SELECT
	//unit_test[7] test data:rescheduleモード,the first fail
	sslid.reschedule=1;
	protocol_module_base::EVENT_TAG schedule1=sslid.handle_realserver_connection_fail(thread_up_id,ep);
	int fail_count_add=data->realserver_connect_failed_count;
	BOOST_CHECK_EQUAL(fail_count_add,1);
	BOOST_CHECK_EQUAL(protocol_module_base::REALSERVER_SELECT,schedule1);

	//unit_test[8] rescheduleモードの場合,realserver_connect_failed_count で1を加算する,遷移先ステータスを設定する,status = REALSERVER_SELECT
	//unit_test[8] test data:reschedule, the third fail
	sslid.reschedule=1;
	protocol_module_base::EVENT_TAG schedule3=sslid.handle_realserver_connection_fail(thread_up_id,ep);
	schedule3=sslid.handle_realserver_connection_fail(thread_up_id,ep);
	int fail_count_add3=data->realserver_connect_failed_count;
	BOOST_CHECK_EQUAL(fail_count_add3,3);
	BOOST_CHECK_EQUAL(protocol_module_base::REALSERVER_SELECT,schedule3);

	//unit_test[9] no rescheduleモードの場合,終了フラグをON,遷移先ステータスを設定する,status = CLIENT_DISCONNECT
	//unit_test[9] test data:no rescheduleモード
	sslid.reschedule=0;
	protocol_module_base::EVENT_TAG schedule=sslid.handle_realserver_connection_fail(thread_up_id,ep);
	BOOST_CHECK_EQUAL(data->end_flag,1);
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT,schedule);
}

//handle_client_disconnect 許秋粉
void handle_client_disconnect_test()
{
	protocol_module_sslid sslid;
	boost::thread thread1(thread_func);
	boost::thread::id thread_id=thread1.get_id();
	//unit_test[6] handle_client_disconnect()メソッドのテスト　 normal
	//unit_test[6] test data:NULL
	protocol_module_base::EVENT_TAG disconn=sslid.handle_client_disconnect(thread_id);
	BOOST_CHECK(protocol_module_base::FINALIZE==disconn);
}

void protocol_module_sslid_test_main()
{
	test_suite* ts = BOOST_TEST_SUITE( "protocol_module_sslid_ut" );

	ts->add( BOOST_TEST_CASE( &protocol_module_sslid_test) );
	ts->add( BOOST_TEST_CASE( &is_tcp_test) );
	ts->add( BOOST_TEST_CASE( &is_udp_test) );
	ts->add( BOOST_TEST_CASE( &get_name_test) );
	ts->add( BOOST_TEST_CASE( &initialize_test) );
	ts->add( BOOST_TEST_CASE( &finalize_test) );
	ts->add( BOOST_TEST_CASE( &is_use_sorry_test) );
	ts->add( BOOST_TEST_CASE( &check_parameter_test) );
	ts->add( BOOST_TEST_CASE( &register_schedule_test) );
	ts->add( BOOST_TEST_CASE( &handle_session_finalize_test) );
	ts->add( BOOST_TEST_CASE( &handle_accept_test) );
	ts->add( BOOST_TEST_CASE( &handle_client_recv_test) );
	ts->add( BOOST_TEST_CASE( &handle_realserver_connection_fail_test) );
	ts->add( BOOST_TEST_CASE( &handle_client_disconnect_test) );
	framework::master_test_suite().add( ts );
}

