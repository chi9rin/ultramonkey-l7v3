
#include <iostream>
extern "C"
{
#include <malloc.h>
}
#include <boost/test/included/unit_test.hpp>
#include "snmpbridge.h"
#include "snmpbridge_test.h"

//using namespace std;
using namespace boost::unit_test;
using namespace l7vs;

extern "C"
{
extern void* _start;    //開始位置
extern void* _etext;    //終了位置

//mallocデバッグ
static void* malloc_dbg(size_t, const void*); 
static void  malloc_dbg_install();
static void  malloc_dbg_uninstall();
static void* (*old_malloc_hook)(size_t, const void*); 

int malloc_count = 0;

static void * malloc_dbg(size_t size, const void *caller)
{
	void *ret;
/*
	printf("mallocdbg_caller:%p\n", caller);
	printf("_start:%p\n", _start);
	printf("_etext:%p\n", _etext);
*/
	if((&_start < caller) && (caller < &_etext))
	{
		//printf("test code\n");
		if (0 < malloc_count)
		{
			--malloc_count;
			malloc_dbg_uninstall();
			void* ret = malloc(size);
			malloc_dbg_install();
			return ret;
		}
		else return NULL;
	}
	else{
		//printf("library code\n");
		malloc_dbg_uninstall();
		ret = malloc(size);
		malloc_dbg_install();
		return ret;
	}
}

static void
malloc_dbg_install ()
{
        old_malloc_hook = __malloc_hook;
        __malloc_hook = malloc_dbg;
}

static void
malloc_dbg_uninstall ()
{
        __malloc_hook = old_malloc_hook;
}

}// extern "C"

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
											snmp_param = snmp_param_in; }
	void								set_connection_state(bool in){ connection_state = in; }

	int									send_message2(){ return send_message(); }
	int									load_config2(){ return load_config(); }
	void								load_loglevel2(){ load_loglevel(); }
};

void create_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "create_snmpbridge_test" );
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
//	BOOST_TEST_MESSAGE( "initialize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[8] initialize 初期化前のカテゴリ名のマップのサイズのチェック
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[9] initialize 初期化前のログレベルのマップのサイズのチェック
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

	// unit_test[10] initialize 戻り値が0であることを確認（正常系）
	int ret = 0;
	int set_portno_val = 60162;
	int set_interval_val = 1000;
	set_portno(set_portno_val);
	set_interval(set_interval_val);

	ret = test.initialize();
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[11] initialize 初期化後のカテゴリ名のマップのサイズのチェック
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)7 );

	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
	// unit_test[12] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_START_STOP）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_start_stop" );

	// unit_test[13] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_MANAGER_RECEIVE）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_receive" );

	// unit_test[14] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_MANAGER_SEND）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_send" );

	// unit_test[15] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_L7VSD_RECEIVE）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_receive" );

	// unit_test[16] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_L7VSD_SEND）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_send" );

	// unit_test[17] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_LOGGER）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_logger" );

	// unit_test[18] initialize 初期化後のカテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_PARAMETER）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_parameter" );

	// unit_test[19] initialize 初期化後のログレベルのマップのサイズをチェック
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

	// unit_test[20] initialize 初期化後のログレベルのマップの値のチェック（debug）
	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
	levelstring_map_it = levelstring_map.find( "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_DEBUG );

	// unit_test[21] initialize 初期化後のログレベルのマップの値のチェック（info）
	levelstring_map_it = levelstring_map.find( "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_INFO );

	// unit_test[22] initialize 初期化後のログレベルのマップの値のチェック（warn）
	levelstring_map_it = levelstring_map.find( "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_WARN );

	// unit_test[23] initialize 初期化後のログレベルのマップの値のチェック（error）
	levelstring_map_it = levelstring_map.find( "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_ERROR );

	// unit_test[24] initialize 初期化後のログレベルのマップの値のチェック（fatal）
	levelstring_map_it = levelstring_map.find( "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_FATAL );

	// initialize snmp_paramの確認
	l7ag_parameter& snmpparam = test.get_snmp_param();
	// unit_test[25] initialize ポート番号の値のチェック
	BOOST_CHECK_EQUAL( snmpparam.portno, set_portno_val );
	// unit_test[26] initialize インターバルの値のチェック
	BOOST_CHECK_EQUAL( snmpparam.interval, set_interval_val );

	// unit_test[27] initialize snmp_acceptorのopenされていることのチェック
	boost::asio::ip::tcp::acceptor& acc = test.get_snmp_acceptor();
	BOOST_CHECK_EQUAL( acc.is_open(), true );
	// unit_test[28] initialize snmp_acceptorのreuse_addressの設定のチェック
	boost::asio::socket_base::reuse_address option(true);
	boost::asio::socket_base::reuse_address option1;
	acc.get_option(option1);
	BOOST_CHECK_EQUAL( option1, option );

	// unit_test[29] initialize 各パラメータが正常に取得できない場合の確認（異常系）
	get_int_errorflag = true;
	get_string_errorflag = true;
	ret = test.initialize();
	get_int_errorflag = false;
	get_string_errorflag = false;

#if 0
	// unit_test[] initialize 戻り値が−1であることを確認（異常系）
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif
	// unit_test[30] initialize カテゴリ名のマップのサイズと値のチェック
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)7 );

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

	// unit_test[31] initialize ログレベルのマップのサイズと値のチェック
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

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

	// initialize snmp_paramの確認
	snmpparam = test.get_snmp_param();
	// unit_test[32] initialize ポート番号のデフォルト設定値のチェック
	BOOST_CHECK_EQUAL( snmpparam.portno, PORT_DEFAULT );

	// unit_test[33] initialize インターバルのデフォルト設定値のチェック
	BOOST_CHECK_EQUAL( snmpparam.interval, INTERVAL_DEFAULT );

	test.finalize();
}

void finalize_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "finalize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[34] finalizeの呼び出しのチェック
	test.finalize();
	// unit_test[35] finalize connection_stateの値をチェック
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );
	// unit_test[36] finalize snmp_acceptorがcloseされていることのチェック
	BOOST_CHECK_EQUAL( test.get_snmp_acceptor().is_open(), false );
	// unit_test[37] finalize snmp_socketがcloseされていることのチェック
	BOOST_CHECK_EQUAL( test.get_snmp_socket().is_open(), false );
}

void send_trap_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "send_trap_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// send_trap
	std::string msg = "error message";
	int ret = 0;
	ret = test.send_trap(msg);
	// unit_test[38] send_trap 戻り値が0であることを確認（正常系）
	BOOST_CHECK_EQUAL( ret, 0 );
	// send_trap 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header        = (struct l7ag_message_header*) buffer;
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) (header + 1);
	struct l7ag_traprequest_message* trapmes  = (struct l7ag_traprequest_message*) (payloadheader + 1);

	// unit_test[] send_trap header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[] send_trap header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)1 );
	// unit_test[] send_trap 
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_TRAPREQUEST );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_traprequest_message ) );
	// unit_test[] send_trap 
	char oid[] = "1.3.6.1.4.1.60000.1.0.2";
	BOOST_CHECK_EQUAL( trapmes->oid, oid );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( trapmes->message, msg.c_str() );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( trapmes->magic[0], 0x54 );
	// unit_test[] send_trap 
	BOOST_CHECK_EQUAL( trapmes->magic[1], 0x52 );
	// unit_test[] send_trap 送信サイズのチェック
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_traprequest_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	ret = test.send_trap(msg);
	malloc_dbg_uninstall();
	// unit_test[] send_trap 戻り値が−1であることを確認（異常系）
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] send_trap 送信バッファのNULLチェック
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

#if 0
	// unit_test[] send_trap 送信サイズのチェック
//	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
#endif

/*//////////////////////////////////
	// 送信サイズが0の場合
//	ret = test.send_trap(msg);
	// unit_test[] send_trap 戻り値が−1であることを確認（異常系）
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] send_trap 送信バッファのNULLチェック
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] send_trap 送信サイズのチェック
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
/*//////////////////////////////////
	test.finalize();
}

void reload_config_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "reload_config_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// reload_config
//	int ret = 0;
//	ret = test.reload_config();
	test.reload_config();
#if 0
	// unit_test[] reload_config 戻り値が0であることを確認（正常系）
//	BOOST_CHECK_EQUAL( ret, 0 );
#endif
	// reload_config 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
    struct l7ag_message_header* header             = (struct l7ag_message_header*) buffer;
    struct l7ag_payload_header* payloadheader      = (struct l7ag_payload_header*) (header + 1);
    struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);

	// unit_test[] reload_config header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[] reload_config header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)1 );
	// unit_test[] reload_config 
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_COMMANDREQUEST );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message ) );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( settingcmd->command_id, (unsigned long long)COMMAND_SETTINGFILE_RELOAD );
	// unit_test[] reload_config 
	unsigned char data[COMMANDDATASIZE];
	memset( data, 0, COMMANDDATASIZE );
//	BOOST_CHECK_EQUAL( settingcmd->data, data );
	for( int i = 0; i < COMMANDDATASIZE; i++ ){
		BOOST_CHECK_EQUAL( settingcmd->data[i], data[i] );
	}
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( settingcmd->magic[0], 0x53 );
	// unit_test[] reload_config 
	BOOST_CHECK_EQUAL( settingcmd->magic[1], 0x54 );

	// unit_test[] reload_config 送信サイズのチェック
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	test.reload_config();
	malloc_dbg_uninstall();
#if 0
	// unit_test[] reload_config 戻り値が−1であることを確認（異常系）
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif

	// unit_test[] reload_config 送信バッファのNULLチェック
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

#if 0
	// unit_test[] reload_config 送信サイズのチェック
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
#endif

/*//////////////////////////////////
	// 送信サイズが0の場合
//	ret = test.reload_config();
#if 0
	// unit_test[] reload_config 戻り値が−1であることを確認（異常系）
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif
	// unit_test[] reload_config 送信バッファのNULLチェック
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] reload_config 送信サイズのチェック
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
/*//////////////////////////////////
	test.finalize();
}

void change_loglevel_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "change_loglevel_snmpbridge_test" );
	// change_loglevel 
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// change_loglevel 
	int ret = 0;
	LOG_CATEGORY_TAG snmp_log_category = LOG_CAT_SNMPAGENT_START_STOP;
	LOG_LEVEL_TAG loglevel = LOG_LV_INFO;
	// unit_test[]  change_loglevel 戻り値が0であることを確認（正常系）
	ret = test.change_loglevel( snmp_log_category, loglevel );
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] change_loglevel 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header             = (struct l7ag_message_header*) buffer;
	struct l7ag_payload_header* payloadheader      = (struct l7ag_payload_header*) (header + 1);
	struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
	struct l7ag_changeloglevel_parameter* param    = (struct l7ag_changeloglevel_parameter*) settingcmd->data;

	// unit_test[] change_loglevel header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[] change_loglevel header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );

	// unit_test[] change_loglevel 
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_COMMANDREQUEST );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message ) );

	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( settingcmd->command_id, (unsigned long long)COMMAND_LOGLEVEL_CHANGE );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( settingcmd->magic[0], 0x4c );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( settingcmd->magic[1], 0x4c );

	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( param->log_category, (unsigned long long)snmp_log_category );
	// unit_test[] change_loglevel 
	BOOST_CHECK_EQUAL( param->log_level, (unsigned long long)loglevel );

	// unit_test[] change_loglevel 送信サイズは正しいか？
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	ret = test.change_loglevel( snmp_log_category, loglevel );
	malloc_dbg_uninstall();
	// unit_test[] change_loglevel 戻り値が−1であることを確認
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] change_loglevel 送信バッファのポインタはNULLか？
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

#if 0
	// unit_test[] change_loglevel 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
#endif

/*//////////////////////////////////
	// 送信サイズが0の場合
//	ret = test.change_loglevel();
	// unit_test[] change_loglevel 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] change_loglevel 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] change_loglevel 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
/*//////////////////////////////////

	test.finalize();
}

void change_loglevel_allcategory_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "change_loglevel_allcategory_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	int ret = 0;
	ret = test.change_loglevel_allcategory(LOG_LV_INFO);
	// unit_test[] change_loglevel_allcategory 戻り値が0であることを確認
	BOOST_CHECK_EQUAL( ret, 0 );

	// change_loglevel_allcategory 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	std::vector<struct l7ag_settingcommand_message> settingcmd_vec;
	l7ag_parameter& snmpparam = test.get_snmp_param();
	for( std::map<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>::iterator it = snmpparam.loglevel.begin();
		it != snmpparam.loglevel.end(); ++it ){

		struct l7ag_settingcommand_message cmd;
		cmd.command_id = COMMAND_LOGLEVEL_CHANGE;
		settingcmd_vec.push_back( cmd );
	}
	struct l7ag_message_header* header = (struct l7ag_message_header*) buffer;
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) (header + 1);
	struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
	// unit_test[] change_loglevel_allcategory 
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[] change_loglevel_allcategory 
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[] change_loglevel_allcategory 
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[] change_loglevel_allcategory 
	BOOST_CHECK_EQUAL( header->payload_count, settingcmd_vec.size() );
	// unit_test[] change_loglevel_allcategory 
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

	for( std::vector<struct l7ag_settingcommand_message>::iterator itr = settingcmd_vec.begin();
		itr != settingcmd_vec.end(); ++itr ){

		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_COMMANDREQUEST );
		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message ) );
		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( settingcmd->command_id, (unsigned long long)COMMAND_LOGLEVEL_CHANGE );
		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( settingcmd->magic[0], 0x4c );
		// unit_test[] change_loglevel_allcategory 
		BOOST_CHECK_EQUAL( settingcmd->magic[1], 0x4c );
//		memcpy( settingcmd->data, &(*itr), sizeof( struct l7ag_changeloglevel_parameter) );

		payloadheader = (struct l7ag_payload_header*) (settingcmd + 1);
		settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
	}

	// unit_test[] change_loglevel_allcategory 送信サイズは正しいか？
	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) * settingcmd_vec.size() +
			sizeof( struct l7ag_settingcommand_message ) * settingcmd_vec.size();
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// change_loglevel_allcategory メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	ret = test.change_loglevel_allcategory(LOG_LV_INFO);
	malloc_dbg_uninstall();
	// unit_test[] change_loglevel_allcategory 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] change_loglevel_allcategory 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

#if 0
	// unit_test[] change_loglevel_allcategory 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
#endif

/*//////////////////////////////////
//	ret = test.change_loglevel( snmp_log_category, loglevel );
	// change_loglevel_allcategory 送信サイズが0の場合
	// unit_test[] change_loglevel_allcategory 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] change_loglevel_allcategory 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] change_loglevel_allcategory 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

/*//////////////////////////////////
	test.finalize();
}

void send_mibcollection_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "send_mibcollection_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// 正常
	int ret = 0;
	// virtualservice１つだけの場合
#if 0
	l7vs::error_code err;
	l7vs::realserver_element rs;
	l7vs::virtualservice_element vs;
	vs.realserver_vector.push_back(rs);
	vsd.add_virtual_service(&vs, err);
#endif
	struct l7ag_mibrequest_message payload;
	ret = test.send_mibcollection(&payload);
	// unit_test[] send_mibcollection 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

/*//////////////////////////////////
	// unit_test[] send_mibcollection 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header = (struct l7ag_message_header*) buffer;
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );

	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)2 );//vs 1 rs 0
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

//VS 1
	void* pos = (void *) (header + 1);
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;

	struct l7ag_mibdata_payload_vs* vs = (struct l7ag_mibdata_payload_vs*) pos;
	BOOST_CHECK_EQUAL( vs->magic[0], 0x56 );
	BOOST_CHECK_EQUAL( vs->magic[1], 0x53 );
//	BOOST_CHECK_EQUAL( vs->index, 1 );
//		virtualservice_element& srv = (*vs_it)->get_element();
	BOOST_CHECK_EQUAL( vs->protocol, (uint8_t)IPPROTO_TCP );
//	BOOST_CHECK_EQUAL( vs->ipAddress, );
//	BOOST_CHECK_EQUAL( vs->portNumber, );
	BOOST_CHECK_EQUAL( vs->reschedule, 0 );
//	BOOST_CHECK_EQUAL( vs->sorryIpAddress, );
//	BOOST_CHECK_EQUAL( vs->sorryPortNumber, );
//	BOOST_CHECK_EQUAL( vs->sorryThreshold, );
//	BOOST_CHECK_EQUAL( vs->sorryForceFlag, );
//	BOOST_CHECK_EQUAL( vs->QoSThresholdUp, );
//	BOOST_CHECK_EQUAL( vs->QoSThresholdDown, );
	BOOST_CHECK_EQUAL( vs->throughputUp, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->throughputDown, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->vs_count, 1);
		strncpy(vs->scheduleModule, srv.schedule_module_name.c_str(), L7VS_MODNAME_LEN);
		strncpy(vs->protocolModule, srv.protocol_module_name.c_str(), L7VS_MODNAME_LEN);
		std::stringstream module_arg;
		for( std::vector<std::string>::iterator arg_it = srv.protocol_args.begin();
			arg_it != srv.protocol_args.end(); arg_it++ ){

			module_arg << *arg_it << " ";
		}
		std::string module_option;
		module_arg >> module_option;
		strncpy(vs->protocolModuleOption, module_option.c_str(), L7VS_PROTOMOD_OPT_LEN);


//RS 0
		struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
		BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
		BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
		BOOST_CHECK_EQUAL( payloadheader->message_id, MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
		BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
		pos = payloadheader + 1;
		struct l7ag_mibdata_payload_rs* rs = (struct l7ag_mibdata_payload_rs*) pos;
		BOOST_CHECK_EQUAL( rs->magic[0], 0x52 );
		BOOST_CHECK_EQUAL( rs->magic[1], 0x53 );
		BOOST_CHECK_EQUAL( rs->index, 0 );
		BOOST_CHECK_EQUAL( rs->rs_count, 0);
		pos = rs + 1;
/*//////////////////////////////////

	// unit_test[] send_mibcollection 送信サイズは正しいか？
	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 1;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

/*//////////////////////////////////
	// virtualservice１つ、realserver１つの場合
//	ret = test.send_mibcollection();
	// unit_test[] send_mibcollection 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] send_mibcollection 送信データの設定は正しいか？

	// unit_test[] send_mibcollection 送信サイズは正しいか？
//	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 1;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice２つの場合
//	ret = test.send_mibcollection();
	// unit_test[] send_mibcollection 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] send_mibcollection 送信データの設定は正しいか？

	// unit_test[] send_mibcollection 送信サイズは正しいか？
//	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 2 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 0;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice２つ、realserver２つの場合
//	ret = test.send_mibcollection();
	// unit_test[] send_mibcollection 戻り値は0か？
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] send_mibcollection 送信データの設定は正しいか？

	// unit_test[] send_mibcollection 送信サイズは正しいか？
//	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 2 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 2;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


/*//////////////////////////////////
	// send_mibcollection メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	ret = test.send_mibcollection(&payload);
	malloc_dbg_uninstall();
	// unit_test[] send_mibcollection 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] send_mibcollection 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

#if 0
	// unit_test[] send_mibcollection 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
#endif

/*//////////////////////////////////
	// send_mibcollection 送信サイズが0の場合
//	ret = test.send_mibcollection();
	// unit_test[] send_mibcollection 戻り値は-1か？
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] send_mibcollection 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] send_mibcollection 送信サイズは0か？
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

/*//////////////////////////////////
	test.finalize();
}

void get_connectionstate_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_connectionstate_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] snmpbridge get_connectionstate 戻り値のチェック(初期状態)
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );

	// unit_test[] snmpbridge get_connectionstate 戻り値(true)のチェック
	test.set_connection_state(true);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	// unit_test[] snmpbridge get_connectionstate 戻り値(false)のチェック
	test.set_connection_state(false);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );

	test.finalize();
}

void get_loglevel_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_START_STOP)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_INFO );
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_MANAGER_RECEIVE)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_INFO );
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_MANAGER_SEND)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), LOG_LV_INFO );
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), LOG_LV_INFO );
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_L7VSD_SEND)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), LOG_LV_INFO );
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_LOGGER)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), LOG_LV_INFO );
	// unit_test[] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_PARAMETER)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), LOG_LV_INFO );


	// unit_test[] snmpbridge get_loglevelの各カテゴリのログレベルをチェック
	test.change_loglevel( LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_LOGGER , LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_PARAMETER, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), LOG_LV_DEBUG );

	test.finalize();
}

void get_loglevel_allcategory_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_loglevel_allcategory_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] get_loglevel_allcategory 全カテゴリのログレベルが取得出来ることを確認
	test.change_loglevel_allcategory( LOG_LV_INFO );
	std::map<l7vs::LOG_CATEGORY_TAG, l7vs::LOG_LEVEL_TAG>	loglevelmap;
	test.get_loglevel_allcategory(loglevelmap);
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), loglevelmap.find( LOG_CAT_SNMPAGENT_START_STOP )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), loglevelmap.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), loglevelmap.find( LOG_CAT_SNMPAGENT_MANAGER_SEND )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), loglevelmap.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), loglevelmap.find( LOG_CAT_SNMPAGENT_L7VSD_SEND )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), loglevelmap.find( LOG_CAT_SNMPAGENT_LOGGER )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), loglevelmap.find( LOG_CAT_SNMPAGENT_PARAMETER )->second );

	test.finalize();
}

void handle_accept_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_accept_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] handle_acceptの呼び出しのチェック
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );
//	test.handle_accept();

	// unit_test[] handle_acceptが呼ばれたときにconnection_stateの値が変わっていることの確認
	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	test.finalize();
}

void handle_receive_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_receive_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] handle_receiveの呼び出しのチェック
//	test.handle_receive();

	test.finalize();
}

void handle_send_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_send_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] handle_sendの呼び出しのチェック
//	test.handle_send();

	test.finalize();
}

void load_config_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "load_config_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] load_configの呼び出しのチェック（正常系）
	test.load_config2();

	// unit_test[] load_configの呼び出しのチェック（パラメータ取得異常の場合）	
	get_int_errorflag = true;
	get_string_errorflag = true;
	test.load_config2();
	get_int_errorflag = false;
	get_string_errorflag = false;
	// デフォルト値が設定されていることを確認

	test.finalize();
}

void load_loglevel_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "load_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] load_loglevelの呼び出しのチェック（正常系）
	test.load_loglevel2();
	// ロードした設定値の確認

	// unit_test[] load_loglevelの呼び出しのチェック（パラメータ取得異常の場合）	
	get_int_errorflag = true;
	get_string_errorflag = true;
	test.load_loglevel2();
	get_int_errorflag = false;
	get_string_errorflag = false;
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
	ts->add( BOOST_TEST_CASE( &send_trap_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &reload_config_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &change_loglevel_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &change_loglevel_allcategory_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &send_mibcollection_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_connectionstate_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_loglevel_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_loglevel_allcategory_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_accept_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_receive_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_send_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_config_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_loglevel_snmpbridge_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
