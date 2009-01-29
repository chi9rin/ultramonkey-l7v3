#include <boost/test/included/unit_test.hpp>
#include "../../module/protocol/protocol_module_sessionless.cpp"

using namespace std;
using namespace boost;
using namespace boost::unit_test;
using namespace l7vs;

typedef l7vs::protocol_module_sessionless::check_message_result
		check_message_result;

//protocol_module_sessionless 馮家純
void protocol_module_sessionless_test(){

	//unit_test[1] モジュール名（"sessionless"）
    protocol_module_sessionless obj1;
    BOOST_CHECK_EQUAL("sessionless",obj1.name);

    //unit_test[2] new
    protocol_module_sessionless *p1=new protocol_module_sessionless();
    BOOST_CHECK_EQUAL("sessionless", p1->name);
    delete p1;
}

//~protocol_module_sessionless 馮家純

//is_tcp 馮家純
void is_tcp_test(){

	//unit_test[4] TRUEを返却する
    protocol_module_sessionless obj;
    BOOST_CHECK(obj.is_tcp());
}

//is_udp 馮家純
void is_udp_test(){

	//unit_test[5] FALSEを返却する
    protocol_module_sessionless obj;
    BOOST_CHECK(!obj.is_udp());
}

//get_name 馮家純
void get_name_test(){

	//unit_test[5] モジュール名（"sessionless"）を返却する
    protocol_module_sessionless obj;
    BOOST_CHECK_EQUAL("sesslonless", obj.get_name());
}

//initialize 董作方
typedef protocol_module_sessionless::realserverlist_type real_server_list;
typedef	protocol_module_sessionless::rs_list_itr_func_type rs_list_itr_func_type;
real_server_list real_list;
real_server_list::iterator real_begin() {return real_list.begin();}
real_server_list::iterator real_end() {return real_list.end();}
real_server_list::iterator real_next() {return ++real_list.begin();}
void list_lock_for_test() {}
void list_unlock_for_test() {}
void initialize_test() {

	//unit_test[1] RealServerリストの各操作関数メソッドのテスト
	//unit_test[1] test data:RealServerリスト先頭取得関数,RealServerリスト末端取得関数,RealServerリスト次要素取得関数
	//unit_test[1] test data:RealServerリストロック関数,RealServerリストアンロック関数
	rs_list_itr_func_type inrslist_begin = real_begin;
	rs_list_itr_func_type inrslist_end = real_end;
	rs_list_itr_func_type inrslist_next = real_next;
	function<void(void)> inlist_lock = list_lock_for_test;
	function<void(void)> inlist_unlock = list_unlock_for_test;

	protocol_module_sessionless sessionless;
	sessionless.initialize(inrslist_begin,inrslist_end,inrslist_next,inlist_lock,inlist_unlock);

	BOOST_CHECK_EQUAL(sessionless.rs_list_begin, real_begin);
	BOOST_CHECK_EQUAL(sessionless.rs_list_end, real_end);
	BOOST_CHECK_EQUAL(sessionless.rs_list_next, real_next);
	BOOST_CHECK_EQUAL(sessionless.rs_list_lock, list_lock_for_test);
	BOOST_CHECK_EQUAL(sessionless.rs_list_unlock, list_unlock_for_test);

};

//finalize 董作方
void finalize_test() {

	//unit_test[2] 各操作関数を初期化する
	//unit_test[2] test data
	protocol_module_sessionless sessionless;
	sessionless.finalize();

	BOOST_CHECK(!sessionless.getloglevel);
	BOOST_CHECK(!sessionless.putLogFatal);
	BOOST_CHECK(!sessionless.putLogError);
	BOOST_CHECK(!sessionless.putLogWarn);
	BOOST_CHECK(!sessionless.putLogInfo);
	BOOST_CHECK(!sessionless.putLogDebug);

	BOOST_CHECK(!sessionless.rs_list_begin);
	BOOST_CHECK(!sessionless.rs_list_end);
	BOOST_CHECK(!sessionless.rs_list_next);
	BOOST_CHECK(!sessionless.rs_list_lock);
	BOOST_CHECK(!sessionless.rs_list_unlock);

	BOOST_CHECK(!sessionless.replication_pay_memory);
	BOOST_CHECK(!sessionless.replication_area_lock);
	BOOST_CHECK(!sessionless.replication_area_unlock);

	BOOST_CHECK(!sessionless.schedule_tcp);
	BOOST_CHECK(!sessionless.schedule_udp);

	BOOST_CHECK_EQUAL(sessionless.forwarded_for, 0);
    BOOST_CHECK_EQUAL( '\0', sessionless.sorry_uri[0] );

}

//is_use_sorry 馬翠翠
void is_use_sorry_test() {
	protocol_module_sessionless sessionless;

	// unit_test[1]  is_use_sorryメソッドのテスト
	bool is_return = sessionless.is_use_sorry();
	BOOST_CHECK_EQUAL(is_return,true);
}

//check_parameter 馬翠翠
void check_parameter_test() {
	protocol_module_sessionless sessionless;
	check_message_result result;
	vector<string> args(5);

	// unit_test[2] チェック結果フラグにTRUEを設定する
	// unit_test[2] test data:オプション文字列にデータなし
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[3] 送信元設定指示設定フラグをON,チェック結果フラグにTRUEを設定する
	// unit_test[3] test data:オプション文字列 = "-F"
	args[0] = "l7vsadmin";
	args[1] = "-F";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[4] 送信元設定指示設定フラグをON, チェック結果フラグにTRUEを設定する
	// unit_test[4] test data:オプション文字列 = "-F"
	args[0] = "l7vsadmin";
	args[1] = "--forwarded-for";
	sessionless.check_parameter(args);
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[5] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-S/--sorryURI' option value '%s' is too long."（%sは次要素）を設定する
	// unit_test[5] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 > 127
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.");

	// unit_test[6] sorryURI設定フラグをON チェック結果メッセージに"Cannot set multiple option ''-S/--sorryURI'."を設定する
	// unit_test[6] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 = 127 チェックOKの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	args[3] = "----sorryURI";
	args[4] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[7] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-S/--sorryURI' option value '%s' is not a valid URI."（%sは次要素）を設定する
	// unit_test[7] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 = 127 チェックNGの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/' is not a valid URI.");

	// unit_test[8] sorryURI設定フラグをON チェック結果メッセージに"Cannot set multiple option ''-S/--sorryURI'."を設定する
	// unit_test[8] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 < 127 チェックOKの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "----sorryURI";
	args[4] = "http://abcd.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[9] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-S/--sorryURI' option value '%s' is not a valid URI."（%sは次要素）を設定する
	// unit_test[9] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 < 127 チェックNGの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/aa+";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcd.com/aa+' is not a valid URI.");

	// unit_test[10] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to set  option value ''-S/--sorryURI'."を設定する
	// unit_test[10] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素が存在しない場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"You have to set  option value ''-S/--sorryURI'.");

	// unit_test[11] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option ''-S/--sorryURI'."を設定する
	// unit_test[11] test data:オプション文字列 = "-S" sorryURI設定フラグ = ON
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "-S";
	args[4] = "http://www.xyz.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI");

	// unit_test[12] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error."を設定する
	// unit_test[12] test data:オプション文字列"-F","-S" 以外の場合
	args[0] = "l7vsadmin";
	args[1] = "-D";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Option error.");

}

//set_parameter 馬翠翠
void set_parameter_test() {
	protocol_module_sessionless sessionless;
	check_message_result result;
	vector<string> args(5);

	// unit_test[13] チェック結果フラグにTRUEを設定する
	// unit_test[13] test data:オプション文字列にデータなし
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[14] 送信元設定指示に1を設定する
	args[0] = "l7vsadmin";
	sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,1);

	// unit_test[15] 送信元設定指示設定フラグをON,チェック結果フラグにTRUEを設定する
	// unit_test[15] test data:オプション文字列 = "-F"
	args[0] = "l7vsadmin";
	args[1] = "-F";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,1);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[16] 送信元設定指示設定フラグをON,チェック結果フラグにTRUEを設定する
	// unit_test[16] test data:オプション文字列 = "-F"
	args[0] = "l7vsadmin";
	args[1] = "--forwarded-for";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,1);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[17] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-S/--sorryURI' option value '%s' is too long."（%sは次要素）を設定する
	// unit_test[17] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 > 127
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/' is too long.");

	// unit_test[18] sorryURI設定フラグをON チェック結果メッセージに"Cannot set multiple option ''-S/--sorryURI'."を設定する
	// unit_test[18] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 = 127 チェックOKの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	args[3] = "--sorryURI";
	args[4] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/12345678/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[19] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-S/--sorryURI' option value '%s' is not a valid URI."（%sは次要素）を設定する
	// unit_test[19] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 = 127 チェックNGの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcde.com/0123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/123456789/1234567+/' is not a valid URI.");

	// unit_test[20] sorryURI設定フラグをON チェック結果メッセージに"Cannot set multiple option ''-S/--sorryURI'."を設定する
	// unit_test[20] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素の文字列長 < 127 チェックOKの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "--sorryURI";
	args[4] = "http://abcd.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI'.");
	BOOST_CHECK_EQUAL(result.flag,false);

	// unit_test[21] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"'-S/--sorryURI' option value '%s' is not a valid URI."（%sは次要素）を設定する
	// unit_test[21] test data:次要素の文字列長 < 127 チェックNGの場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/aa+";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"'-S/--sorryURI' option value 'http://abcd.com/aa+' is not a valid URI.");

	// unit_test[22] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"You have to set  option value ''-S/--sorryURI'."を設定する
	// unit_test[22] test data:オプション文字列 = "-S" sorryURI設定フラグ = OFF 次要素が存在しない場合
	args[0] = "l7vsadmin";
	args[1] = "-S";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"You have to set  option value ''-S/--sorryURI'.");

	// unit_test[23] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Cannot set multiple option ''-S/--sorryURI'."を設定する
	// unit_test[23] test data:オプション文字列 = "-S" sorryURI設定フラグ = ON
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	args[3] = "-S";
	args[4] = "http://www.xyz.com/";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Cannot set multiple option ''-S/--sorryURI");

	// unit_test[24] チェック結果フラグにFALSEを設定する,チェック結果メッセージに"Option error."を設定する
	// unit_test[24] test data:オプション文字列"-F","-S" 以外の場合
	args[0] = "l7vsadmin";
	args[1] = "-D";
	result = sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Option error.");

	// unit_test[25] 送信元設定指示に0を設定する
	// unit_test[25] test data:チェック結果フラグ = TRUEの場合、送信元設定指示設定フラグ = OFF
	args[0] = "l7vsadmin";
	args[1] = "-S";
	args[2] = "http://abcd.com/";
	sessionless.check_parameter(args);
	BOOST_CHECK_EQUAL(sessionless.forwarded_for,0);

}

//add_parameter 馬翠翠
void add_parameter_test() {
	protocol_module_sessionless sessionless;
	check_message_result result;
	vector<string> args(2);

	// unit_test[58] チェック結果フラグにTRUEを設定する
	// unit_test[58] test data:オプション文字列が存在しない場合
	result = sessionless.add_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,true);

	// unit_test[59] チェック結果フラグにFALSEを設定する, チェック結果メッセージに"Cannot add option."を設定する
	// unit_test[59] test data:オプション文字列が存在する場合
	args[0] = "-F";
	result = sessionless.add_parameter(args);
	BOOST_CHECK_EQUAL(result.flag,false);
	BOOST_CHECK_EQUAL(result.message,"Cannot add option.");
}

//handle_rslist_update 馬翠翠

//register_schedule(tcp) 董作方
void schedule_tcp( const boost::thread::id id,
		           rs_list_itr_func_type func_type1,
		           rs_list_itr_func_type func_type2,
		           rs_list_itr_func_type func_type3,
		           boost::asio::ip::tcp::endpoint& ) {};
void register_schedule_tcp_test() {

	// unit_test[3]  schedule_tcpメソッドのテスト
	// unit_test[3] test data:Client振り分け処理関数
	protocol_module_sessionless::tcp_schedule_func_type  func = schedule_tcp;

	protocol_module_sessionless sessionless;
	sessionless.register_schedule(func);

	BOOST_CHECK_EQUAL(sessionless.schedule_tcp,schedule_tcp);
}

//register_schedule(udp) 董作方


//handle_session_initialize 董作方
typedef protocol_module_sessionless::session_thread_data_sessionless session_thread_data;
typedef std::map< thread::id, session_thread_data*> thread_map;
void up_thread_func(){}
void down_thread_func(){}
void handle_session_initialize_test() {

	// unit_test[5] 遷移先ステータスを設定する status = ACCEPT
	// unit_test[5] test data:スレッド
	boost::thread up_thread(up_thread_func);
	boost::thread down_thread(down_thread_func);

	protocol_module_sessionless sessionless;
	protocol_module_base::EVENT_TAG ret =
		sessionless.handle_session_initialize(up_thread.get_id(),down_thread.get_id());

    BOOST_CHECK_EQUAL( protocol_module_base::ACCEPT, ret);

    thread_map::iterator thread_map_iterator;
    thread_map::iterator thread_map_iterator_end;

    //unit_test[6]  上りスレッドマプ空のテスト
    //unit_test[6] test data:マプ
    thread_map_iterator = sessionless.session_thread_data_map.find(up_thread.get_id());
    thread_map_iterator_end = sessionless.session_thread_data_map.end();
    BOOST_CHECK(thread_map_iterator != thread_map_iterator_end);

    //unit_test[7]  上りスレッドマプのテスト
    thread_map_iterator = sessionless.session_thread_data_map.find(up_thread.get_id());
    session_thread_data* data = thread_map_iterator->second;

    BOOST_CHECK(!data);
    BOOST_CHECK_EQUAL(up_thread.get_id(), data->thread_id);
    BOOST_CHECK_EQUAL(0, data->thread_division);
    BOOST_CHECK_EQUAL(down_thread.get_id(), data->pair_thread_id);
    BOOST_CHECK(!(data->recive_data_map.empty()));

    BOOST_CHECK_EQUAL(data->end_flag, 0);
    BOOST_CHECK_EQUAL(data->accept_end_flag, 0);
    BOOST_CHECK_EQUAL(data->sorry_flag, 0);
    BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, 0);
    BOOST_CHECK_EQUAL(data->realserver_switch_flag, 0);

    //unit_test[8]  下りスレッドマプ空のテスト
    thread_map_iterator = sessionless.session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(thread_map_iterator != thread_map_iterator_end);

	//unit_test[9]  下りスレッドマプのテスト
    thread_map_iterator = sessionless.session_thread_data_map.find(down_thread.get_id());
    data = thread_map_iterator->second;

    BOOST_CHECK(!data);
    BOOST_CHECK_EQUAL(down_thread.get_id(), data->thread_id);
    BOOST_CHECK_EQUAL(data->thread_division, 1);
    BOOST_CHECK_EQUAL(up_thread.get_id(), data->pair_thread_id);
    BOOST_CHECK(!(data->recive_data_map.empty()));

    BOOST_CHECK_EQUAL(data->end_flag, 0);
    BOOST_CHECK_EQUAL(data->accept_end_flag, 0);
    BOOST_CHECK_EQUAL(data->sorry_flag, 0);
    BOOST_CHECK_EQUAL(data->sorryserver_switch_flag, 0);
    BOOST_CHECK_EQUAL(data->realserver_switch_flag, 0);
}

//handle_session_finalize 董作方
void handle_session_finalize_test() {

	//unit_test[1] セッションスレッドに対応する終了処理
	//unit_test[1] test data:セッションスレッド初期化
	boost::thread up_thread(up_thread_func);
	boost::thread down_thread(down_thread_func);
	protocol_module_sessionless sessionless;
	sessionless.handle_session_initialize(up_thread.get_id(),down_thread.get_id());
	protocol_module_base::EVENT_TAG ret =
		sessionless.handle_session_finalize(up_thread.get_id(), down_thread.get_id());

    thread_map::iterator thread_map_iterator;
    thread_map::iterator thread_map_iterator_end;
    thread_map_iterator = sessionless.session_thread_data_map.find(up_thread.get_id());

    thread_map_iterator_end = sessionless.session_thread_data_map.end();
    BOOST_CHECK(thread_map_iterator == thread_map_iterator_end);

    thread_map_iterator = sessionless.session_thread_data_map.find(down_thread.get_id());
    BOOST_CHECK(thread_map_iterator == thread_map_iterator_end);

    BOOST_CHECK_EQUAL( protocol_module_base::STOP, ret);

}

//handle_accept 董作方
void thread_func_for_accept(){}
void handle_accept_test() {

	session_thread_data  data;
	boost::thread accept_thread(thread_func_for_accept);

	//unit_test[13] sorry状態の場合
	//unit_test[13] test data:accept_end_flag=0,sorry_flag=1
	data.accept_end_flag = 0;
	data.sorry_flag = 1;

	protocol_module_sessionless obj1;
	obj1.session_thread_data_map.insert(
			std::pair<boost::thread::id, session_thread_data*>(accept_thread.get_id(), &data));

	protocol_module_base::EVENT_TAG ret = obj1.handle_accept(accept_thread.get_id());

	BOOST_CHECK_EQUAL(data.accept_end_flag, 1);//accept完了フラグをON
	BOOST_CHECK_EQUAL(ret, protocol_module_base::SORRYSERVER_SELECT);//遷移先ステータス=SORRYSERVER_SELECT

	//unit_test[13] sorry状態以外の場合
	data.accept_end_flag = 0;
	data.sorry_flag = 0;

	protocol_module_sessionless obj2;
	obj2.session_thread_data_map.insert(
				std::pair<boost::thread::id, session_thread_data*>(accept_thread.get_id(), &data));

	ret = obj2.handle_accept(accept_thread.get_id());

	BOOST_CHECK_EQUAL(data.accept_end_flag, 1);//accept完了フラグをON
	BOOST_CHECK_EQUAL(ret, protocol_module_base::REALSERVER_SELECT);//遷移先ステータス=REALSERVER_SELECT
}

//handle_client_recv

//handle_realserver_select(tcp) 馮家純
void upthread_func_for_handle_realserver_select_test(){}
void downthread_func_for_handle_realserver_select_test(){}

void schedule_tcp_determinate(//endpoint = 決定
		const boost::thread::id thread_id,
		protocol_module_base::rs_list_itr_func_type rs_list_begin,
		protocol_module_base::rs_list_itr_func_type rs_list_end,
		protocol_module_base::rs_list_itr_func_type rs_list_next,
		boost::asio::ip::tcp::endpoint &rs_endpoint){
	rs_endpoint.resize(1234);
}

void schedule_tcp_nodeterminate(//endpoint = 未決定
		const boost::thread::id thread_id,
		protocol_module_base::rs_list_itr_func_type rs_list_begin,
		protocol_module_base::rs_list_itr_func_type rs_list_end,
		protocol_module_base::rs_list_itr_func_type rs_list_next,
		boost::asio::ip::tcp::endpoint &rs_endpoint){
}
void handle_realserver_select_test(){

	//unit_test[6] endpoint = 決定
	protocol_module_sessionless obj1;

	boost::thread upt1(upthread_func_for_handle_realserver_select_test);
	boost::thread downt1(downthread_func_for_handle_realserver_select_test);
	session_thread_data dataup;
	session_thread_data datadown;
	obj1.session_thread_data_map[upt1.get_id()] = &dataup;
	obj1.session_thread_data_map[downt1.get_id()] = &datadown;
	dataup.thread_division = 0;
	dataup.pair_thread_id = downt1.get_id();
	datadown.thread_division = 1;
	datadown.pair_thread_id = upt1.get_id();
	datadown.recive_data_map.clear();
	protocol_module_sessionless::tcp_schedule_func_type func1 = &schedule_tcp_determinate;

	obj1.register_schedule(func1);

	boost::asio::ip::tcp::endpoint ep1;
	protocol_module_base::EVENT_TAG ret=obj1.handle_realserver_select(upt1.get_id(), ep1);

	BOOST_CHECK_EQUAL(ep1.size(), static_cast<size_t>(1234));
	BOOST_CHECK(datadown.recive_data_map.find(ep1) != datadown.recive_data_map.end());
	BOOST_CHECK_EQUAL(ret, protocol_module_base::CLIENT_RECV);

	//unit_test[7] endpoint = 未決定
	protocol_module_sessionless obj2;

	boost::thread upt2(upthread_func_for_handle_realserver_select_test);
	session_thread_data dataup2;
	obj2.session_thread_data_map[upt2.get_id()] = &dataup2;
	dataup2.end_flag = 0;
	protocol_module_sessionless::tcp_schedule_func_type func2 = &schedule_tcp_nodeterminate;

	obj2.register_schedule(func2);

	boost::asio::ip::tcp::endpoint ep2;
	ret=obj2.handle_realserver_select(upt2.get_id(), ep2);

	BOOST_CHECK_EQUAL(dataup2.end_flag, 1);
	BOOST_CHECK_EQUAL(ret, protocol_module_base::CLIENT_DISCONNECT);
}

//handle_realserver_select(udp)

//handle_realserver_connect

//handle_realserver_connection_fail 馮家純
void handle_realserver_connection_fail_test(){
	//unit_test[8] 終了フラグ,遷移先ステータス
	protocol_module_sessionless obj;

	boost::thread t;
	boost::asio::ip::tcp::endpoint ep;
	protocol_module_sessionless::EVENT_TAG ret;

	protocol_module_sessionless::session_thread_data_sessionless data;
	data.thread_id = t.get_id();
	obj.session_thread_data_map[t.get_id()] = &data;

	ret = obj.handle_realserver_connection_fail(t.get_id(),ep);

	BOOST_CHECK_EQUAL(data.end_flag, 1);		//終了フラグをON
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT, ret);		//status = CLIENT_DISCONNECT
}

//handle_realserver_send 董作方

//handle_sorryserver_select 馮家純
void handle_sorryserver_select_test(){

	//unit_test[9] 該当スレッドの送信可能データを取得する,下りスレッドにsorryserver受信バッファを確保する,送信先endpoint,遷移先ステータス
	protocol_module_sessionless obj;

	boost::thread upt,downt;
	boost::asio::ip::tcp::endpoint ep;
	protocol_module_sessionless::EVENT_TAG ret;

	//up,down thread map
	protocol_module_sessionless::session_thread_data_sessionless dataup,datadown;
	obj.session_thread_data_map[upt.get_id()] = &dataup;
	obj.session_thread_data_map[downt.get_id()] = &datadown;
	dataup.thread_id = upt.get_id();
	dataup.thread_division = 0;
	dataup.pair_thread_id = downt.get_id();
	datadown.thread_id = downt.get_id();
	datadown.thread_division = 1;
	datadown.pair_thread_id = upt.get_id();
	datadown.recive_data_map.clear();

	//send_status送信可能データ
	protocol_module_sessionless::send_status st;
	st.status = protocol_module_sessionless::SEND_OK;
	st.send_possible_size = 2;
	dataup.recive_data_map[ep].send_status_list.push_back(st);

	ret = obj.handle_sorryserver_select(upt.get_id(), ep);

	BOOST_CHECK(!datadown.recive_data_map.empty());		//下りスレッドにsorryserver受信バッファを確保する
	//BOOST_CHECK_EQUAL(ep, dataup.sorry_endpoint);	// 送信先endpointにendpointを設定する
	BOOST_CHECK_EQUAL(protocol_module_base::SORRYSERVER_CONNECT, ret);	//status = SORRYSERVER_CONNECT
}

//handle_sorryserver_connect

//handle_sorryserver_connection_fail

//handle_sorryserver_send

//handle_realserver_recv(tcp)

//handle_realserver_recv(udp)

//handle_sorryserver_recv

//handle_response_send_inform

//handle_client_connection_check
void handle_client_connection_check_test(){
	//unit_test[10] 送信バッファサイズ　>　送信可能データサイズ
	protocol_module_sessionless obj1;

	boost::thread t1;
	boost::array<char,MAX_BUFFER_SIZE> sbf1;
	std::size_t d1;
	protocol_module_sessionless::EVENT_TAG ret;
	boost::asio::ip::tcp::endpoint ep1;

	//session_thread_data_sessionless
	protocol_module_sessionless::session_thread_data_sessionless data1;
	obj1.session_thread_data_map[t1.get_id()] = &data1;
	//send_status
	protocol_module_sessionless::send_status sd1;
	sd1.status = protocol_module_sessionless::SEND_OK;
	sd1.send_possible_size = 2;
	sd1.send_offset = 1;
	//recive_data
	protocol_module_sessionless::recive_data rd1;
	rd1.recive_buffer = "abcd";
	//send_status->recive_data
	rd1.send_status_list.push_back(sd1);
	//recive_data->session_thread_data_sessionless
	data1.recive_data_map[ep1] = rd1;

	ret = obj1.handle_client_connection_check(t1.get_id(),sbf1,d1);
	//送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
	std::string chk1 = "bc";
	BOOST_CHECK_EQUAL(chk1.c_str(),sbf1.data());
	//送信済データサイズに送信可能データサイズを設定する
	BOOST_CHECK_EQUAL(data1.recive_data_map[ep1].send_status_list.rbegin()->send_end_size, 2);
	//送信可能データサイズに0を設定する
	BOOST_CHECK_EQUAL(data1.recive_data_map[ep1].send_status_list.rbegin()->send_possible_size, 0);
	//遷移先ステータスを設定する
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_SEND, ret);


	//unit_test[11] 送信バッファサイズ　<　送信可能データサイズ
	protocol_module_sessionless obj2;

	boost::thread t2;
	boost::array<char,MAX_BUFFER_SIZE> sbf2;
	std::size_t d2;
	boost::asio::ip::tcp::endpoint ep2;

	//session_thread_data_sessionless
	protocol_module_sessionless::session_thread_data_sessionless data2;
	obj2.session_thread_data_map[t2.get_id()] = &data2;
	//send_status
	protocol_module_sessionless::send_status sd2;
	sd2.status = protocol_module_sessionless::SEND_OK;
	sd2.send_possible_size = 5000;
	sd2.send_offset = 1;
	//recive_data
	protocol_module_sessionless::recive_data rd2;
	rd2.recive_buffer = "abcdefg";
	//send_status->recive_data
	rd2.send_status_list.push_back(sd2);
	//recive_data->session_thread_data_sessionless
	data2.recive_data_map[ep2] = rd2;

	ret = obj2.handle_client_connection_check(t2.get_id(),sbf2,d2);
	//送信可能データを先頭から送信バッファサイズ分、送信バッファにコピーする
	std::string chk2 = "bcdef";
	BOOST_CHECK_EQUAL(chk2.c_str(),sbf2.data());
	//送信済データサイズに送信バッファサイズを設定する
	int sbfsize = sbf2.max_size();
	BOOST_CHECK_EQUAL(data2.recive_data_map[ep2].send_status_list.rbegin()->send_end_size, sbfsize);
	//送信可能データサイズから送信バッファサイズを減算する
	BOOST_CHECK_EQUAL(5000-sbfsize,data2.recive_data_map[ep2].send_status_list.rbegin()->send_possible_size);
	//遷移先ステータスを設定する
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_SEND,ret);

	//unit_test[10] 送信バッファサイズ　=　送信可能データサイズ
	protocol_module_sessionless obj3;

	boost::thread t3;
	boost::array<char,MAX_BUFFER_SIZE> sbf3;
	std::size_t d3;
	boost::asio::ip::tcp::endpoint ep3;

	//session_thread_data_sessionless
	protocol_module_sessionless::session_thread_data_sessionless data3;
	obj1.session_thread_data_map[t3.get_id()] = &data3;
	//send_status
	protocol_module_sessionless::send_status sd3;
	sd3.status = protocol_module_sessionless::SEND_OK;
	sd3.send_possible_size = 4096;
	sd3.send_offset = 1;
	//recive_data
	protocol_module_sessionless::recive_data rd3;
	memset(rd3.recive_buffer,96,4096);
	//send_status->recive_data
	rd3.send_status_list.push_back(sd3);
	//recive_data->session_thread_data_sessionless
	data3.recive_data_map[ep3] = rd3;

	ret = obj3.handle_client_connection_check(t3.get_id(),sbf3,d3);
	//送信可能データを先頭から送信可能データサイズ分、送信バッファにコピーする
	char *chk3;
	memset(chk3,96,4095);
	BOOST_CHECK_EQUAL(chk3, sbf3.data());
	//送信済データサイズに送信可能データサイズを設定する
	BOOST_CHECK_EQUAL(data3.recive_data_map[ep3].send_status_list.rbegin()->send_end_size, 4096);
	//送信可能データサイズに0を設定する
	BOOST_CHECK_EQUAL(data3.recive_data_map[ep3].send_status_list.rbegin()->send_possible_size, 0);
	//遷移先ステータスを設定する
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_SEND, ret);

}

//handle_client_select
void handle_client_select_test(){
	//unit_test[1] STOP
	protocol_module_sessionless obj;

	boost::thread t;
	boost::asio::ip::udp::endpoint ep;
	boost::array<char,MAX_BUFFER_SIZE> sbf;
	std::size_t d;
	protocol_module_sessionless::EVENT_TAG ret;
	ret = obj.handle_client_select(t.get_id(),ep,sbf,d);

	BOOST_CHECK_EQUAL(protocol_module_base::STOP, ret);
}

//handle_client_send

//handle_client_disconnect 馮家純
void handle_client_disconnect_test(){
	//unit_test[12] 遷移先ステータスを設定する
	protocol_module_sessionless obj;

	protocol_module_sessionless::EVENT_TAG ret;
	boost::thread t;

	ret=obj.handle_client_disconnect(t.get_id());
	BOOST_CHECK_EQUAL(protocol_module_base::FINALIZE,ret);
}

//handle_sorry_enable 馬翠翠
void thread_t(){}
void handle_sorry_enable_test() {
	protocol_module_sessionless sessionless;
	protocol_module_sessionless::EVENT_TAG status;
	protocol_module_sessionless::send_status sendstatus;
	protocol_module_sessionless::recive_data receivedata;
	session_thread_data *thread_data = new session_thread_data();
	boost::thread thread_up(&thread_t);
	boost::thread thread_down(&thread_t);
	boost::thread::id thread_id_up = thread_up.get_id();
	boost::thread::id thread_id_down = thread_down.get_id();
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::endpoint endpoint_down;

	// 上りスレッドの場合
	thread_data->thread_division = 0;
	thread_data->thread_id = thread_id_up;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_up,thread_data));

	// unit_test[26] sorry状態をON,status = protocol_module_sessionless::ACCEPT
	// unit_test[26] test data:accept完了フラグOFFの場合
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 0;
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::ACCEPT);

	// unit_test[27] status =SORRYSERVER_SELECT
	// unit_test[27] test data:accept完了フラグONの場合,sorry状態の場合,送信可能データありの場合 list 1件
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 1;
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[28] status =SORRYSERVER_SELECT
	// unit_test[28] test data:送信可能データありの場合 list 2件 1件あり
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[29] status =CLIENT_RECV
	// unit_test[29] test data:送信可能データなし list 3件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_RECV);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[30] 終了フラグをON,status = REALSERVER_DISCONNECT
	// unit_test[30] test data:sorry状態以外の場合,送信継続データあり list 1件
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[31] 終了フラグをON,status = REALSERVER_DISCONNECT
	// unit_test[31] test data:送信継続データあり list 2件 1件あり
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[32] sorryserver切替中フラグON,sorry状態をON,status = REALSERVER_DISCONNECT
	// unit_test[32] test data:送信継続データなし list 3件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorryserver_switch_flag,1);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// 下りスレッドの場合
	sessionless.session_thread_data_map.clear();
	thread_data->thread_division = 1;
	thread_data->thread_id = thread_id_down;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_down,thread_data));

	// unit_test[33] status =CLIENT_CONNECTION_CHECK
	// unit_test[33] test data:sorry状態の場合,送信可能データあり list 1件
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[34] status =CLIENT_CONNECTION_CHECK
	// unit_test[34] test data:送信可能データあり list 2件 1件あり
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_possible_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[35] status =SORRYSERVER_RECV
	// unit_test[35] test data:送信可能データなし list 3件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[36] 終了フラグをON,status = REALSERVER_DISCONNECT
	// unit_test[36] test data:sorry状態以外の場合,送信不可データあり list 1件
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[37] 終了フラグをON,status = REALSERVER_DISCONNECT
	// unit_test[37] test data:送信データ残サイズ ＞ ０　が存在する場合 list 2件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 20;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[38] sorry状態をON
	// unit_test[38] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合 list 1件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->sorry_flag,1);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[39]  status = SORRYSERVER_RECV
	// unit_test[39] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合,送信可能データなし list 1件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[40] 送信可能データあり status = CLIENT_CONNECTION_CHECK
	// unit_test[40] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合,送信可能データあり list 3件
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[41] status =FINALIZE
	// unit_test[41] test data:thread_id対応のsession_thread_dataなし
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_enable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::FINALIZE);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

}

//handle_sorry_disable 馬翠翠
void handle_sorry_disable_test(){
	protocol_module_sessionless sessionless;
	protocol_module_sessionless::EVENT_TAG status;
	protocol_module_sessionless::send_status sendstatus;
	protocol_module_sessionless::recive_data receivedata;
	session_thread_data *thread_data = new session_thread_data();
	boost::thread thread_up(&thread_t);
	boost::thread thread_down(&thread_t);
	boost::thread::id thread_id_up = thread_up.get_id();
	boost::thread::id thread_id_down = thread_down.get_id();
	boost::asio::ip::tcp::endpoint endpoint;
	boost::asio::ip::tcp::endpoint endpoint_down;

	// 上りスレッドの場合
	thread_data->thread_division = 0;
	thread_data->thread_id = thread_id_up;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_up,thread_data));

	// unit_test[42] sorry状態をOFF,status =ACCEPT
	// unit_test[42] test data:accept完了フラグOFFの場合
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 0;
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,0);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::ACCEPT);

	// unit_test[43] 終了フラグをON,status = SORRYSERVER_DISCONNECT
	// unit_test[43] test data:accept完了フラグONの場合,sorry状態の場合,送信継続データあり list 1件
	sessionless.session_thread_data_map[thread_id_up]->accept_end_flag = 1;
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[44] 終了フラグをON,status = SORRYSERVER_DISCONNECT
	// unit_test[44] test data:送信継続データあり list 2件 1件あり
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->end_flag,1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[45] realserver切替中,sorry状態をOFF,status = SORRYSERVER_DISCONNECT
	// unit_test[45] test data:送信継続データなし list 3件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->realserver_switch_flag,1);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_up]->sorry_flag,0);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[46] status =REALSERVER_SELECT
	// unit_test[46] test data:sorry状態以外の場合,送信可能データありの場合 list 1件
	sessionless.session_thread_data_map[thread_id_up]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[47] status =REALSERVER_SELECT
	// unit_test[47] test data:送信可能データありの場合 list 2件 1件あり
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_SELECT);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// unit_test[48] status =CLIENT_RECV
	// unit_test[48] test data:送信可能データなし list 3件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_RECV);
	sessionless.session_thread_data_map[thread_id_up]->recive_data_map.clear();

	// 下りスレッドの場合
	sessionless.session_thread_data_map.clear();
	thread_data->thread_division = 1;
	thread_data->thread_id = thread_id_down;
	sessionless.session_thread_data_map.insert(std::pair<boost::thread::id, session_thread_data*>(thread_id_down,thread_data));

	// unit_test[49] 終了フラグをON,status = SORRYSERVER_DISCONNECT
	// unit_test[49] test data:sorry状態の場合,送信不可データあり list 1件
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 1;
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[50] 終了フラグをON,status = SORRYSERVER_DISCONNECT
	// unit_test[50] test data:送信データ残サイズ ＞ ０　が存在する場合 list 2件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 10;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 20;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->end_flag , 1);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::SORRYSERVER_DISCONNECT);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[51] sorry状態をOFF
	// unit_test[51] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合 list 1件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(sessionless.session_thread_data_map[thread_id_down]->sorry_flag,0);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[52] sorry状態をOFF,送信可能データなし status = REALSERVER_RECV
	// unit_test[52] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合,送信可能データなし list 1件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[53] status = CLIENT_CONNECTION_CHECK
	// unit_test[53] test data:送信不可データなし　かつ　送信データ残サイズ ＞ ０　が存在しない場合,送信可能データあり list 3件
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_END;
	sendstatus.send_rest_size = 0;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[54] status =CLIENT_CONNECTION_CHECK
	// unit_test[54] test data:sorry状態以外の場合,送信可能データあり list 1件
	sessionless.session_thread_data_map[thread_id_down]->sorry_flag = 0;
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[55] status =CLIENT_CONNECTION_CHECK
	// unit_test[55] test data:送信可能データあり list 2件 1件あり
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::CLIENT_CONNECTION_CHECK);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[56] status =REALSERVER_RECV
	// unit_test[56] test data:送信可能データなし list 3件
	sendstatus.status = protocol_module_sessionless::SEND_END;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_CONTINUE;
	receivedata.send_status_list.push_back(sendstatus);
	sendstatus.status = protocol_module_sessionless::SEND_NG;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_down);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::REALSERVER_RECV);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();

	// unit_test[57] status =FINALIZE
	// unit_test[57] test data:thread_id対応のsession_thread_dataなし
	sendstatus.status = protocol_module_sessionless::SEND_OK;
	receivedata.send_status_list.push_back(sendstatus);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.insert(std::pair<boost::asio::ip::tcp::endpoint, protocol_module_sessionless::recive_data>(endpoint_down,receivedata));
	status = sessionless.handle_sorry_disable(thread_id_up);
	BOOST_CHECK_EQUAL(status,protocol_module_sessionless::FINALIZE);
	sessionless.session_thread_data_map[thread_id_down]->recive_data_map.clear();
}

//handle_realserver_disconnect(tcp)
void handle_realserver_disconnect_test(){

	//unit_test[13] 上りスレッドの場合->終了フラグがONの場合
	protocol_module_sessionless obj1;

	boost::thread t1;
	boost::asio::ip::tcp::endpoint ep1;
	protocol_module_sessionless::EVENT_TAG ret;

	protocol_module_sessionless::session_thread_data_sessionless data1;
	obj1.session_thread_data_map[t1.get_id()] = &data1;
	//make 上りスレッドの場合 0->上りスレッド
	data1.thread_division = 0;
	//make 終了フラグがONの場合 1->on
	data1.end_flag = 1;

	ret = obj1.handle_realserver_disconnect(t1.get_id(), ep1);
	//遷移先ステータスを設定する status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[14] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中の場合->送信可能データあり
	protocol_module_sessionless obj2;

	boost::thread t2;
	boost::asio::ip::tcp::endpoint ep2;

	protocol_module_sessionless::session_thread_data_sessionless data2;
	obj2.session_thread_data_map[t2.get_id()] = &data2;
	//make 上りスレッドの場合 0->上りスレッド
	data2.thread_division = 0;
	//make 終了フラグがOFFの場合 0->off
	data2.end_flag = 0;
	//make sorryserver切替中の場合 1->切替中
	data2.sorryserver_switch_flag = 1;
	//make 送信可能データあり
	protocol_module_sessionless::send_status st2;
	st2.status = protocol_module_sessionless::SEND_OK;
	st2.send_possible_size = 100;
	data2.recive_data_map[ep2].send_status_list.push_back(st2);


	ret = obj2.handle_realserver_disconnect(t2.get_id(),ep2);
	//遷移先ステータスを設定する status = SORRYSERVER_SELECT
	BOOST_CHECK_EQUAL(protocol_module_base::SORRYSERVER_SELECT, ret);


	//unit_test[15] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中の場合->送信可能データなし
	protocol_module_sessionless obj3;

	boost::thread t3;
	boost::asio::ip::tcp::endpoint ep3;

	protocol_module_sessionless::session_thread_data_sessionless data3;
	obj3.session_thread_data_map[t3.get_id()] = &data3;
	//make 上りスレッドの場合 0->上りスレッド
	data3.thread_division = 0;
	//make 終了フラグがOFFの場合 0->off
	data3.end_flag = 0;
	//make sorryserver切替中の場合 1->切替中
	data3.sorryserver_switch_flag = 1;
	//make 送信可能データなし
	protocol_module_sessionless::send_status st3;
	//send_possible=0
	st3.send_possible_size = 0;
	data3.recive_data_map[ep3].send_status_list.push_back(st3);

	ret = obj3.handle_realserver_disconnect(t3.get_id(), ep3);
	//遷移先ステータスを設定する status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[16] 上りスレッドの場合->終了フラグがOFFの場合->sorryserver切替中でない場合
	protocol_module_sessionless obj4;

	boost::thread t4;
	boost::asio::ip::tcp::endpoint ep4;

	protocol_module_sessionless::session_thread_data_sessionless data4;
	obj4.session_thread_data_map[t4.get_id()] = &data4;
	//make 上りスレッドの場合 0->上りスレッド
	data4.thread_division = 0;
	//make 終了フラグがOFFの場合 0->off
	data4.end_flag = 0;
	//make sorryserver切替中でない場合 0->切替中でない
	data4.sorryserver_switch_flag = 0;

	ret = obj4.handle_realserver_disconnect(t4.get_id(),ep4);
	//終了フラグをON
	BOOST_CHECK_EQUAL(data4.end_flag, 1);
	//遷移先ステータスを設定する status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[17] 下りスレッドの場合->送信可能データあり
	protocol_module_sessionless obj5;

	boost::thread t5;
	boost::asio::ip::tcp::endpoint ep5;

	protocol_module_sessionless::session_thread_data_sessionless data5;
	obj5.session_thread_data_map[t5.get_id()] = &data5;
	//make 下りスレッドの場合 1->下りスレッド
	data5.thread_division = 1;
	//make 送信可能データあり
	protocol_module_sessionless::send_status st5;
	st5.status = protocol_module_sessionless::SEND_OK;
	//send_possible>0
	st5.send_possible_size = 100;
	data5.recive_data_map[ep5].send_status_list.push_back(st5);

	ret = obj5.handle_realserver_disconnect(t5.get_id(), ep5);
	//終了フラグをON
	BOOST_CHECK_EQUAL(data5.end_flag, 1);
	//遷移先ステータスを設定する status = CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_CONNECTION_CHECK, ret);


	//unit_test[18] 下りスレッドの場合->送信可能データなし
	protocol_module_sessionless obj6;

	boost::thread t6;
	boost::asio::ip::tcp::endpoint ep6;

	protocol_module_sessionless::session_thread_data_sessionless data6;
	obj6.session_thread_data_map[t6.get_id()] = &data6;
	//make 下りスレッドの場合 1->下りスレッド
	data6.thread_division = 1;
	//make 送信可能データなし
	protocol_module_sessionless::send_status st6;
	//send_possible>0
	st5.send_possible_size = 0;
	data6.recive_data_map[ep6].send_status_list.push_back(st6);

	ret = obj6.handle_realserver_disconnect(t6.get_id(), ep6);
	//終了フラグをON
	BOOST_CHECK_EQUAL(data6.end_flag, 1);
	//遷移先ステータスを設定する status = CLIENT_DISCONNECT
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT, ret);
}

//handle_realserver_disconnect(udp)

//handle_sorryserver_disconnect 馮家純
void handle_sorryserver_disconnect_test(){

	//unit_test[19] 上りスレッドの場合->終了フラグがONの場合
	protocol_module_sessionless obj1;

	boost::thread t1;
	boost::asio::ip::tcp::endpoint ep1;
	protocol_module_sessionless::EVENT_TAG ret;

	protocol_module_sessionless::session_thread_data_sessionless data1;
	obj1.session_thread_data_map[t1.get_id()] = &data1;
	//make 上りスレッドの場合 0->上りスレッド
	data1.thread_division = 0;
	//make 終了フラグがONの場合 1->on
	data1.end_flag = 1;

	ret = obj1.handle_realserver_disconnect(t1.get_id(), ep1);
	//遷移先ステータスを設定する status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[20] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中の場合->送信可能データあり
	protocol_module_sessionless obj2;

	boost::thread t2;
	boost::asio::ip::tcp::endpoint ep2;

	protocol_module_sessionless::session_thread_data_sessionless data2;
	obj2.session_thread_data_map[t2.get_id()] = &data2;
	//make 上りスレッドの場合 0->上りスレッド
	data2.thread_division = 0;
	//make 終了フラグがOFFの場合 0->off
	data2.end_flag = 0;
	//make realserver切替中の場合 1->切替中
	data2.realserver_switch_flag = 1;
	//make 送信可能データあり
	protocol_module_sessionless::send_status st2;
	st2.status = protocol_module_sessionless::SEND_OK;
	//send_possible>0
	st2.send_possible_size = 100;
	data2.recive_data_map[ep2].send_status_list.push_back(st2);

	ret = obj2.handle_realserver_disconnect(t2.get_id(), ep2);
	//遷移先ステータスを設定する status = REALSERVER_SELECT
	BOOST_CHECK_EQUAL(protocol_module_base::REALSERVER_SELECT, ret);


	//unit_test[21] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中の場合->送信可能データなし
	protocol_module_sessionless obj3;

	boost::thread t3;
	boost::asio::ip::tcp::endpoint ep3;

	protocol_module_sessionless::session_thread_data_sessionless data3;
	obj3.session_thread_data_map[t3.get_id()] = &data3;
	//make 上りスレッドの場合 0->上りスレッド
	data3.thread_division = 0;
	//make 終了フラグがOFFの場合 0->off
	data3.end_flag = 0;
	//make realserver切替中の場合 1->切替中
	data3.realserver_switch_flag = 1;
	//make 送信可能データなし
	protocol_module_sessionless::send_status st3;
	//send_possible=0
	st3.send_possible_size = 0;
	data3.recive_data_map[ep3].send_status_list.push_back(st3);

	ret = obj3.handle_realserver_disconnect(t3.get_id(), ep3);
	//遷移先ステータスを設定する status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[22] 上りスレッドの場合->終了フラグがOFFの場合->realserver切替中でない場合
	protocol_module_sessionless obj4;

	boost::thread t4;
	boost::asio::ip::tcp::endpoint ep4;

	protocol_module_sessionless::session_thread_data_sessionless data4;
	obj4.session_thread_data_map[t4.get_id()] = &data4;
	//make 上りスレッドの場合 0->上りスレッド
	data4.thread_division = 0;
	//make 終了フラグがOFFの場合 0->off
	data4.end_flag = 0;
	//make realserver切替中でない場合 0->切替中でない
	data4.realserver_switch_flag = 0;

	ret = obj4.handle_realserver_disconnect(t4.get_id(),ep4);
	//終了フラグをON
	BOOST_CHECK_EQUAL(data4.end_flag,1);
	//遷移先ステータスを設定する status = CLIENT_RECV
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_RECV, ret);


	//unit_test[23] 下りスレッドの場合->送信可能データあり
	protocol_module_sessionless obj5;

	boost::thread t5;
	boost::asio::ip::tcp::endpoint ep5;

	protocol_module_sessionless::session_thread_data_sessionless data5;
	obj5.session_thread_data_map[t5.get_id()] = &data5;
	//make 下りスレッドの場合 1->下りスレッド
	data5.thread_division = 1;
	//make 送信可能データあり
	protocol_module_sessionless::send_status st5;
	st5.status = protocol_module_sessionless::SEND_OK;
	//send_possible>0
	st5.send_possible_size = 100;
	data5.recive_data_map[ep5].send_status_list.push_back(st5);

	ret = obj5.handle_realserver_disconnect(t5.get_id(),ep5);
	//終了フラグをON
	BOOST_CHECK_EQUAL(data5.end_flag, 1);
	//遷移先ステータスを設定する status = CLIENT_CONNECTION_CHECK
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_CONNECTION_CHECK, ret);


	//unit_test[24] 下りスレッドの場合->送信可能データなし
	protocol_module_sessionless obj6;

	boost::thread t6;
	boost::asio::ip::tcp::endpoint ep6;

	protocol_module_sessionless::session_thread_data_sessionless data6;
	obj6.session_thread_data_map[t6.get_id()] = &data6;
	//make 下りスレッドの場合 1->下りスレッド
	data6.thread_division = 1;
	//make 送信可能データなし
	protocol_module_sessionless::send_status st6;
	//send_possible=0
	st6.send_possible_size = 0;
	data6.recive_data_map[ep6].send_status_list.push_back(st6);

	ret = obj6.handle_realserver_disconnect(t6.get_id(),ep6);
	//終了フラグをON
	BOOST_CHECK_EQUAL(data6.end_flag,1);
	//遷移先ステータスを設定する status = CLIENT_DISCONNECT
	BOOST_CHECK_EQUAL(protocol_module_base::CLIENT_DISCONNECT,ret);
}


test_suite*	init_unit_test_suite( int argc, char* argv[] ){

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
	ts->add(BOOST_TEST_CASE(&handle_session_finalize_test));
	ts->add(BOOST_TEST_CASE(&handle_accept_test));
	ts->add(BOOST_TEST_CASE(&handle_realserver_select_test));
	ts->add(BOOST_TEST_CASE(&handle_realserver_connection_fail_test));
	ts->add(BOOST_TEST_CASE(&handle_sorryserver_select_test));
	ts->add(BOOST_TEST_CASE(&handle_client_connection_check_test));
	ts->add(BOOST_TEST_CASE(&handle_client_select_test));
	ts->add(BOOST_TEST_CASE(&handle_client_disconnect_test));
	ts->add(BOOST_TEST_CASE(&handle_sorry_enable_test));
	ts->add(BOOST_TEST_CASE(&handle_sorry_disable_test));
	ts->add(BOOST_TEST_CASE(&handle_realserver_disconnect_test));
	ts->add(BOOST_TEST_CASE(&handle_sorryserver_disconnect_test));

	framework::master_test_suite().add(ts);
	return 0;
}

