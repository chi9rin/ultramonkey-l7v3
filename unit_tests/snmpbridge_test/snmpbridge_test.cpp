
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "snmpbridge.h"
#include "snmpbridge_test.h"

using namespace boost::unit_test;
using namespace l7vs;

class snmpbridge_testclass : public snmpbridge{
public:
	l7ag_parameter&						get_snmp_param(){ return snmp_param; }
	char*								get_send_buffer(){ return send_buffer; }
	size_t&								get_send_buffer_size(){ return send_buffer_size; }
	boost::mutex&						get_send_buffer_mutex(){ return send_buffer_mutex; }
	boost::array<char, READBUF_SIZE>&	get_recv_buffer(){ return recv_buffer; }
	boost::asio::io_service&			get_snmp_io_service(){ return snmp_io_service; }
	boost::asio::ip::tcp::acceptor&		get_snmp_acceptor(){ return snmp_acceptor; }
	boost::asio::ip::tcp::socket&		get_snmp_socket(){ return snmp_socket; }
	bool&								get_connection_state(){ return connection_state; }
	snmp_logcategory_map_type&			get_snmp_loglevel_map(){ return snmp_loglevel_map; }
	string_loglevel_map_type&			get_levelstring_map(){ return levelstring_map; }
	l7vsd&								get_vsd(){ return vsd; }


	snmpbridge_testclass( l7vsd& l7vsd_in, boost::asio::io_service& io_service_in ) : snmpbridge(l7vsd_in, io_service_in){}

	void								set_snmp_param(l7ag_parameter& snmp_param_in){
//											memcpy(&snmp_param, snmp_param_in, sizeof(snmp_param)); }
											snmp_param = snmp_param_in; }
	void								set_connection_state(bool in){ connection_state = in; }

};

void create_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "create_snmpbridge_test" );
	// unit_test[1] snmpbridge クラスを生成する
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[2] snmpbridge 生成時に設定されるsend_buffer_sizeの値をチェック
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
	// unit_test[3] snmpbridge 生成時に設定されるconnection_stateの値をチェック
	BOOST_CHECK_EQUAL( test.get_connection_state(), false );
	// unit_test[4] snmpbridge 生成時に設定されるvsdの値をチェック
	BOOST_CHECK_EQUAL( &(test.get_vsd()), &vsd );
	// unit_test[5] snmpbridge 生成時に設定されるsnmp_io_serviceの値をチェック
	BOOST_CHECK_EQUAL( &(test.get_snmp_io_service()), &io_service );
	// unit_test[6] snmpbridge 生成時に設定されるsnmp_loglevel_mapのマップのサイズをチェック
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[7] snmpbridge 生成時に設定されるlevelstring_mapのマップのサイズをチェック
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

}

void initialize_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "initialize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[8] initialize 初期化前のカテゴリ名のマップのサイズのチェック
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[9] initialize 初期化前のログレベルのマップのサイズのチェック
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

	// unit_test[10] initialize 戻り値のチェック（正常系）
	int ret = 0;
	ret = test.initialize();
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのサイズのチェック
//	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)7 );

	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_START_STOP）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_start_stop" );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_MANAGER_RECEIVE）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_receive" );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_MANAGER_SEND）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_send" );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_L7VSD_RECEIVE）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_receive" );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_L7VSD_SEND）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_send" );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_LOGGER）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_logger" );

	// unit_test[] initialize 初期化後のカテゴリ名のマップのキーと値のチェック（LOG_CAT_SNMPAGENT_PARAMETER）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_parameter" );

	// unit_test[] initialize 初期化後のログレベルのマップのサイズをチェック
//	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

	// unit_test[] initialize 初期化後のログレベルのマップのキーと値のチェック（debug）
	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
	levelstring_map_it = levelstring_map.find( "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_DEBUG );

	// unit_test[] initialize 初期化後のログレベルのマップのキーと値のチェック（info）
	levelstring_map_it = levelstring_map.find( "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_INFO );

	// unit_test[] initialize 初期化後のログレベルのマップのキーと値のチェック（warn）
	levelstring_map_it = levelstring_map.find( "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_WARN );

	// unit_test[] initialize 初期化後のログレベルのマップのキーと値のチェック（error）
	levelstring_map_it = levelstring_map.find( "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_ERROR );

	// unit_test[] initialize 初期化後のログレベルのマップのキーと値のチェック（fatal）
	levelstring_map_it = levelstring_map.find( "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_FATAL );

	// initialize snmp_paramの確認
	l7ag_parameter& snmpparam = test.get_snmp_param();
	// unit_test[] initialize ポート番号の値のチェック
//	BOOST_CHECK_EQUAL( snmpparam.portno,  );
	// unit_test[] initialize インターバルの値のチェック
//	BOOST_CHECK_EQUAL( snmpparam.interval,  );

	// unit_test[] initialize アクセプタ、エンドポイントのチェック

	// パラメータが異常な場合の確認
	// set_parameter
	// ret = test.initialize();
#if 0
	// unit_test[] initialize 戻り値のチェック（異常系）
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif
	// unit_test[] initialize 初期化後のパラメータの確認
	// マップのサイズ、設定されている値
//	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)7 );

//	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_start_stop" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_receive" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_send" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_receive" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_send" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_logger" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_parameter" );

	// unit_test[] initialize 初期化後のパラメータの確認
	// マップのサイズ、設定されている値
//	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

//	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
	levelstring_map_it = levelstring_map.find( "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_DEBUG );

	levelstring_map_it = levelstring_map.find( "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_INFO );

	levelstring_map_it = levelstring_map.find( "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_WARN );

	levelstring_map_it = levelstring_map.find( "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_ERROR );

	levelstring_map_it = levelstring_map.find( "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_FATAL );

	// unit_test[] initialize snmp_paramの確認
	snmpparam = test.get_snmp_param();

	// unit_test[] initialize ポート番号がデフォルト設定値になっているか？
	BOOST_CHECK_EQUAL( snmpparam.portno, PORT_DEFAULT );

	// unit_test[] initialize インターバルがデフォルト設定値になっているか？
	BOOST_CHECK_EQUAL( snmpparam.interval, INTERVAL_DEFAULT );

	test.finalize();
}

void finalize_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "finalize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] finalizeの呼び出しのチェック
	test.finalize();
	// unit_test[] finalize connection_stateの値をチェック
	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );
	// unit_test[] finalize snmp_acceptorがcloseされていることのチェック
	BOOST_CHECK_EQUAL( test.get_snmp_acceptor().is_open(), false );
	// unit_test[] finalize snmp_socketがcloseされていることのチェック
	BOOST_CHECK_EQUAL( test.get_snmp_socket().is_open(), false );
}

void send_trap_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "send_trap_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
	int ret = 0;
//	ret = test.send_trap();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );
	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_traprequest_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// メモリが確保できなかった場合
//	ret = test.send_trap();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// 送信サイズが0の場合
//	ret = test.send_trap();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void reload_config_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "reload_config_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	int ret = 0;
	// 正常
//	ret = test.reload_config();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// メモリが確保できなかった場合
//	ret = test.reload_config();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );


	// 送信サイズが0の場合
//	ret = test.reload_config();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void change_loglevel_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "change_loglevel_snmpbridge_test" );
	// unit_test[]
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
	int ret = 0;
//	ret = test.change_loglevel();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// メモリが確保できなかった場合
//	ret = test.change_loglevel();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// 送信サイズが0の場合
//	ret = test.change_loglevel();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void change_loglevel_allcategory_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "change_loglevel_allcategory_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
	int ret = 0;
//	ret = test.change_loglevel_allcategory();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) * settingcmd_vec.size() +
			sizeof( struct l7ag_settingcommand_message ) * settingcmd_vec.size();
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// メモリが確保できなかった場合
//	ret = test.change_loglevel_allcategory();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

//	ret = test.change_loglevel_allcategory();
	// 送信サイズが0の場合
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void send_mibcollection_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "send_mibcollection_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
	int ret = 0;
	// virtualservice１つだけの場合
//	ret = test.send_mibcollection();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice１つ、realserver１つの場合
//	ret = test.send_mibcollection();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
//	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice２つの場合
//	ret = test.send_mibcollection();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
//	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice２つ、realserver２つの場合
//	ret = test.send_mibcollection();
	// unit_test[] 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] 送信データの設定は正しいか？

	// unit_test[] 送信サイズは正しいか？
//	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// メモリが確保できなかった場合
//	ret = test.send_mibcollection();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// 送信サイズが0の場合
//	ret = test.send_mibcollection();
	// unit_test[] 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void get_connectionstate_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "get_connectionstate_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] snmpbridge get_connectionstateのテスト
	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );

	// unit_test[] snmpbridge get_connectionstateのテスト
	test.set_connection_state(true);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), 0 );

	// unit_test[] snmpbridge get_connectionstateのテスト
	test.set_connection_state(false);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );

	test.finalize();
}

void get_loglevel_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "get_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevelのテスト
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), LOG_LV_NONE );


	// unit_test[] snmpbridge loglevelのテスト
//	test.change_loglevel( LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG );
	l7ag_parameter& snmpparam = test.get_snmp_param();
	std::map<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>::iterator it = snmpparam.loglevel.find(LOG_CAT_SNMPAGENT_START_STOP);
//	BOOST_CHECK_EQUAL( it->second, LOG_LV_NONE );//debug
//	BOOST_CHECK_EQUAL( snmpparam.loglevel.find( LOG_CAT_SNMPAGENT_START_STOP )->second, LOG_LV_NONE );//debug
	it->second = LOG_LV_DEBUG;
//	BOOST_CHECK_EQUAL( it->second, LOG_LV_DEBUG );//debug
//	BOOST_CHECK_EQUAL( snmpparam.loglevel.find( LOG_CAT_SNMPAGENT_START_STOP )->second, LOG_LV_DEBUG );//debug
//	test.set_snmp_param(snmpparam);
//	BOOST_CHECK( &snmpparam == &(test.get_snmp_param()) );//debug
//	BOOST_CHECK_EQUAL( snmpparam.loglevel.find( LOG_CAT_SNMPAGENT_START_STOP )->second, LOG_LV_DEBUG );//debug
//	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_DEBUG );

//	test.change_loglevel_allcategory( LOG_LV_INFO );
//	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_INFO );
//	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_INFO );

	test.finalize();
}

void handle_accept_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "handle_accept_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
//	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );
//	test.handle_accept();

	// unit_test[] handle_acceptが呼ばれたときに状態が変わっていることの確認
//	BOOST_CHECK_EQUAL( test.get_connectionstate(), 0 );

	test.finalize();
}

void handle_receive_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "handle_receive_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
//	test.handle_receive();

	test.finalize();
}

void handle_send_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "handle_send_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
//	test.handle_send();

	test.finalize();
}

void load_config_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "load_config_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
//	test.load_config();

	test.finalize();
}

void load_loglevel_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "load_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
//	test.load_loglevel();
	// ロードした設定値の確認

	// unit_test[]
	// パラメータが異常な場合	
//	test.load_loglevel();
	// デフォルト値が設定されていることを確認

	test.finalize();
}

test_suite* init_unit_test_suite( int args, char* argv[]){

	// create unit test suite
	test_suite* ts = BOOST_TEST_SUITE( "snmpbridge" );

	// add test case to test suite
	ts->add( BOOST_TEST_CASE( &create_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &initialize_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &finalize_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &send_trap_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &reload_config_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &change_loglevel_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &change_loglevel_allcategory_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &send_mibcollection_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_connectionstate_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_loglevel_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_accept_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_receive_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_send_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_config_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_loglevel_snmpbridge_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
