
#include <iostream>
extern "C"
{
#include <malloc.h>
}
#include <boost/test/included/unit_test.hpp>
#include "snmpbridge.h"
#include "snmpbridge_test.h"

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

class snmpbridge_testclass : public l7vs::snmpbridge{
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
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)12 );
	// unit_test[12] initialize 初期化後のログレベルのマップのサイズのチェック
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

	// unit_test[13] initialize snmp_acceptorのopenされていることのチェック
	boost::asio::ip::tcp::acceptor& acc = test.get_snmp_acceptor();
	BOOST_CHECK_EQUAL( acc.is_open(), true );
	// unit_test[14] initialize snmp_acceptorのreuse_addressの設定のチェック
	boost::asio::socket_base::reuse_address option(true);
	boost::asio::socket_base::reuse_address option1;
	acc.get_option(option1);
	BOOST_CHECK_EQUAL( option1, option );

	// unit_test[15] initialize 各パラメータが正常に取得できない場合の確認（異常系）
	get_int_errorflag = true;
	get_string_errorflag = true;
	ret = test.initialize();
	get_int_errorflag = false;
	get_string_errorflag = false;

#if 0
	// unit_test[] initialize 戻り値が−1であることを確認（異常系）
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif

	test.finalize();
}

void finalize_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "finalize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[16] finalizeの呼び出しのチェック
	test.finalize();
	// unit_test[17] finalize connection_stateの値をチェック
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );
	// unit_test[18] finalize snmp_acceptorがcloseされていることのチェック
	BOOST_CHECK_EQUAL( test.get_snmp_acceptor().is_open(), false );
	// unit_test[19] finalize snmp_socketがcloseされていることのチェック
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
	// unit_test[20] send_trap 戻り値が0であることを確認（正常系）
	BOOST_CHECK_EQUAL( ret, 0 );
	// send_trap 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header        = (struct l7ag_message_header*) buffer;
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) (header + 1);
	struct l7ag_traprequest_message* trapmes  = (struct l7ag_traprequest_message*) (payloadheader + 1);

	// unit_test[21] send_trap header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[22] send_trap header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[23] send_trap header->versionのチェック
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[24] send_trap header->payload_countのチェック
	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)1 );
	// unit_test[25] send_trap header->sizeのチェック
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );
	// unit_test[26] send_trap payloadheader->magic[0]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[27] send_trap payloadheader->magic[1]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[28] send_trap payloadheader->message_idのチェック
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_TRAPREQUEST );
	// unit_test[29] send_trap payloadheader->payload_datasizeのチェック
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_traprequest_message ) );
	// unit_test[30] send_trap trapmes->oidのチェック
	char oid[] = "1.3.6.1.4.1.60000.1.0.2";
	BOOST_CHECK_EQUAL( trapmes->oid, oid );
	// unit_test[31] send_trap trapmes->messageのチェック
	BOOST_CHECK_EQUAL( trapmes->message, msg.c_str() );
	// unit_test[32] send_trap trapmes->magic[0]のチェック
	BOOST_CHECK_EQUAL( trapmes->magic[0], 0x54 );
	// unit_test[33] send_trap trapmes->magic[1]のチェック
	BOOST_CHECK_EQUAL( trapmes->magic[1], 0x52 );
	// unit_test[34] send_trap 送信サイズのチェック
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
	// unit_test[35] send_trap 戻り値が−1であることを確認（異常系、mallocに失敗した場合）
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[36] send_trap 送信バッファのNULLチェック
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

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
	// unit_test[37] reload_config 呼び出しのチェック（正常系）
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

	// unit_test[38] reload_config header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[39] reload_config header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[40] reload_config header->versionのチェック
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[41] reload_config header->payload_countのチェック
	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)1 );
	// unit_test[42] reload_config header->sizeのチェック
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );
	// unit_test[43] reload_config payloadheader->magic[0]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[44] reload_config payloadheader->magic[1]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[45] reload_config payloadheader->message_idのチェック
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_COMMANDREQUEST );
	// unit_test[46] reload_config payloadheader->payload_datasizeのチェック
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message ) );
	// unit_test[47] reload_config settingcmd->command_idのチェック
	BOOST_CHECK_EQUAL( settingcmd->command_id, (unsigned long long)COMMAND_SETTINGFILE_RELOAD );
	// unit_test[48] reload_config settingcmd->dataのチェック
	unsigned char data[COMMANDDATASIZE];
	memset( data, 0, COMMANDDATASIZE );
//	BOOST_CHECK_EQUAL( settingcmd->data, data );
	for( int i = 0; i < COMMANDDATASIZE; i++ ){
		BOOST_CHECK_EQUAL( settingcmd->data[i], data[i] );
	}
	// unit_test[49] reload_config settingcmd->magic[0]のチェック
	BOOST_CHECK_EQUAL( settingcmd->magic[0], 0x53 );
	// unit_test[50] reload_config settingcmd->magic[1]のチェック
	BOOST_CHECK_EQUAL( settingcmd->magic[1], 0x54 );

	// unit_test[51] reload_config 送信サイズのチェック
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	// unit_test[52] reload_config 呼び出しのチェック（異常系、mallocに失敗した場合）
	test.reload_config();
	malloc_dbg_uninstall();
#if 0
	// unit_test[] reload_config 戻り値が−1であることを確認（異常系、mallocに失敗した場合）
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif

	// unit_test[53] reload_config 送信バッファのNULLチェック
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

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
	// unit_test[54]  change_loglevel 戻り値が0であることを確認（正常系）
	ret = test.change_loglevel( snmp_log_category, loglevel );
	BOOST_CHECK_EQUAL( ret, 0 );

	// change_loglevel 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header             = (struct l7ag_message_header*) buffer;
	struct l7ag_payload_header* payloadheader      = (struct l7ag_payload_header*) (header + 1);
	struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
	struct l7ag_changeloglevel_parameter* param    = (struct l7ag_changeloglevel_parameter*) settingcmd->data;

	// unit_test[55] change_loglevel header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[56] change_loglevel header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[57] change_loglevel header->versionのチェック
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );

	// unit_test[58] change_loglevel header->sizeのチェック
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

	// unit_test[59] change_loglevel payloadheader->magic[0]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[60] change_loglevel payloadheader->magic[1]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[61] change_loglevel payloadheader->message_idのチェック
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_COMMANDREQUEST );
	// unit_test[62] change_loglevel payloadheader->payload_datasizeのチェック
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message ) );

	// unit_test[63] change_loglevel settingcmd->command_idのチェック
	BOOST_CHECK_EQUAL( settingcmd->command_id, (unsigned long long)COMMAND_LOGLEVEL_CHANGE );
	// unit_test[64] change_loglevel settingcmd->magic[0]のチェック
	BOOST_CHECK_EQUAL( settingcmd->magic[0], 0x4c );
	// unit_test[65] change_loglevel settingcmd->magic[1]のチェック
	BOOST_CHECK_EQUAL( settingcmd->magic[1], 0x4c );

	// unit_test[66] change_loglevel param->log_categoryのチェック
	BOOST_CHECK_EQUAL( param->log_category, (unsigned long long)snmp_log_category );
	// unit_test[67] change_loglevel param->log_levelのチェック
	BOOST_CHECK_EQUAL( param->log_level, (unsigned long long)loglevel );

	// unit_test[68] change_loglevel 送信サイズは正しいか？
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
	// unit_test[69] change_loglevel 戻り値が−1であることを確認（異常系、mallocに失敗した場合）
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[70] change_loglevel 送信バッファのポインタはNULLか？
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

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
	// unit_test[71] change_loglevel_allcategory 戻り値が0であることを確認（正常系）
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
	// unit_test[72] change_loglevel_allcategory header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[73] change_loglevel_allcategory header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[74] change_loglevel_allcategory header->versionのチェック
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[75] change_loglevel_allcategory header->payload_countのチェック
	BOOST_CHECK_EQUAL( header->payload_count, settingcmd_vec.size() );
	// unit_test[76] change_loglevel_allcategory header->sizeのチェック
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

	for( std::vector<struct l7ag_settingcommand_message>::iterator itr = settingcmd_vec.begin();
		itr != settingcmd_vec.end(); ++itr ){

		// unit_test[77] change_loglevel_allcategory payloadheader->magic[0]のチェック
		BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
		// unit_test[78] change_loglevel_allcategory payloadheader->magic[1]のチェック
		BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
		// unit_test[79] change_loglevel_allcategory payloadheader->message_idのチェック
		BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_COMMANDREQUEST );
		// unit_test[80] change_loglevel_allcategory payloadheader->payload_datasizeのチェック
		BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_settingcommand_message ) );
		// unit_test[81] change_loglevel_allcategory settingcmd->command_idのチェック
		BOOST_CHECK_EQUAL( settingcmd->command_id, (unsigned long long)COMMAND_LOGLEVEL_CHANGE );
		// unit_test[82] change_loglevel_allcategory settingcmd->magic[0]のチェック
		BOOST_CHECK_EQUAL( settingcmd->magic[0], 0x4c );
		// unit_test[83] change_loglevel_allcategory settingcmd->magic[1]のチェック
		BOOST_CHECK_EQUAL( settingcmd->magic[1], 0x4c );
//		memcpy( settingcmd->data, &(*itr), sizeof( struct l7ag_changeloglevel_parameter) );

		payloadheader = (struct l7ag_payload_header*) (settingcmd + 1);
		settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);
	}

	// unit_test[84] change_loglevel_allcategory 送信サイズは正しいか？
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
	// unit_test[85] change_loglevel_allcategory 戻り値が-1であることを確認（異常系、mallocに失敗した場合）
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[86] change_loglevel_allcategory 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	test.finalize();
}

void send_mibcollection_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "send_mibcollection_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	int ret = 0;
#if 1
	boost::asio::io_service		dispatcher;
	l7vs::replication			rep( dispatcher );
	l7vs::error_code err;
	l7vs::virtualservice_element element;
	boost::asio::ip::tcp::endpoint vs_endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 10001);
	boost::asio::ip::tcp::endpoint sorry_endpoint(boost::asio::ip::address::from_string("127.0.0.2"), 10002);
	element.udpmode = false;
	element.tcp_accept_endpoint	= vs_endpoint;
	element.protocol_module_name = "protocol1";
	element.schedule_module_name = "schedule1";
	element.sorry_maxconnection = 1000;
	element.sorry_endpoint = sorry_endpoint;
	element.sorry_flag = 0;
	element.qos_upstream = 100;
	element.qos_downstream = 100;
	vsd.add_virtual_service(&element, err);
#endif
	struct l7ag_mibrequest_message payload;
	ret = test.send_mibcollection(&payload);
	// unit_test[87] send_mibcollection 戻り値が0であることをチェック（正常系、virtualservice１つだけの場合）
	BOOST_CHECK_EQUAL( ret, 0 );

	// send_mibcollection 送信データの設定は正しいか？
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header = (struct l7ag_message_header*) buffer;
	// unit_test[88] send_mibcollection header->magic[0]のチェック
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[89] send_mibcollection header->magic[1]のチェック
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	// unit_test[90] send_mibcollection header->versionのチェック
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );
	// unit_test[91] send_mibcollection header->payload_countのチェック
	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)2 );//vs 1 rs 0
	// unit_test[92] send_mibcollection header->sizeのチェック
	size_t send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

//VS 1
	void* pos = (void *) (header + 1);
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) pos;
	// unit_test[93] send_mibcollection payloadheader->magic[0]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[94] send_mibcollection payloadheader->magic[1]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[95] send_mibcollection payloadheader->message_idのチェック
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	// unit_test[96] send_mibcollection payloadheader->payload_datasizeのチェック
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;

	struct l7ag_mibdata_payload_vs* vs = (struct l7ag_mibdata_payload_vs*) pos;
	// unit_test[97] send_mibcollection vs->magic[0]のチェック
	BOOST_CHECK_EQUAL( vs->magic[0], 0x56 );
	// unit_test[98] send_mibcollection vs->magic[1]のチェック
	BOOST_CHECK_EQUAL( vs->magic[1], 0x53 );
	// unit_test[99] send_mibcollection vs->indexのチェック
	BOOST_CHECK_EQUAL( vs->index, 1 );//
	// unit_test[100] send_mibcollection vs->protocolのチェック
	BOOST_CHECK_EQUAL( vs->protocol, (uint8_t)IPPROTO_TCP );
	// unit_test[101] send_mibcollection vs->ipAddressのチェック
	if( vs_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->ipAddress, vs_endpoint.address().to_v4().to_ulong() );
	}
	// unit_test[102] send_mibcollection vs->portNumberのチェック
	BOOST_CHECK_EQUAL( vs->portNumber, vs_endpoint.port() );
	// unit_test[103] send_mibcollection vs->rescheduleのチェック
	BOOST_CHECK_EQUAL( vs->reschedule, 0 );
	// unit_test[104] send_mibcollection vs->sorryIpAddressのチェック
	if( sorry_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->sorryIpAddress, sorry_endpoint.address().to_v4().to_ulong());
	}
	// unit_test[105] send_mibcollection vs->sorryPortNumberのチェック
	BOOST_CHECK_EQUAL( vs->sorryPortNumber, sorry_endpoint.port() );
	// unit_test[106] send_mibcollection vs->sorryThresholdのチェック
	BOOST_CHECK_EQUAL( vs->sorryThreshold, element.sorry_maxconnection );
	// unit_test[107] send_mibcollection vs->sorryForceFlagのチェック
	BOOST_CHECK_EQUAL( vs->sorryForceFlag, element.sorry_flag );
	// unit_test[108] send_mibcollection vs->QoSThresholdUpのチェック
	BOOST_CHECK_EQUAL( vs->QoSThresholdUp, element.qos_upstream );
	// unit_test[109] send_mibcollection vs->QoSThresholdDownのチェック
	BOOST_CHECK_EQUAL( vs->QoSThresholdDown, element.qos_downstream );
	// unit_test[110] send_mibcollection vs->throughputUpのチェック
	BOOST_CHECK_EQUAL( vs->throughputUp, (unsigned long long)10);
	// unit_test[111] send_mibcollection vs->throughputDownのチェック
	BOOST_CHECK_EQUAL( vs->throughputDown, (unsigned long long)10);
	// unit_test[112] send_mibcollection vs->vs_countのチェック
	BOOST_CHECK_EQUAL( vs->vs_count, 1);
	// unit_test[113] send_mibcollection vs->scheduleModuleのチェック
	BOOST_CHECK_EQUAL( vs->scheduleModule, element.schedule_module_name );
	// unit_test[114] send_mibcollection vs->protocolModuleのチェック
	BOOST_CHECK_EQUAL( vs->protocolModule, element.protocol_module_name );

	pos = vs + 1;
//RS 0
	payloadheader = (struct l7ag_payload_header*) pos;
	// unit_test[115] send_mibcollection payloadheader->magic[0]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	// unit_test[116] send_mibcollection payloadheader->magic[1]のチェック
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	// unit_test[117] send_mibcollection payloadheader->message_idのチェック
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	// unit_test[118] send_mibcollection payloadheader->payload_datasizeのチェック
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;
	struct l7ag_mibdata_payload_rs* rs = (struct l7ag_mibdata_payload_rs*) pos;
	// unit_test[119] send_mibcollection rs->magic[0]のチェック
	BOOST_CHECK_EQUAL( rs->magic[0], 0x52 );
	// unit_test[120] send_mibcollection rs->magic[1]のチェック
	BOOST_CHECK_EQUAL( rs->magic[1], 0x53 );
	// unit_test[121] send_mibcollection rs->indexのチェック
	BOOST_CHECK_EQUAL( rs->index, 0 );
	// unit_test[122] send_mibcollection rs->rs_countのチェック
	BOOST_CHECK_EQUAL( rs->rs_count, 0);
	pos = rs + 1;

	// unit_test[123] send_mibcollection 送信サイズは正しいか？
	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 1;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

#if 1
	vsd.del_virtual_service(&element, err);

	l7vs::realserver_element rs_elem;
	boost::asio::ip::tcp::endpoint rs_endpoint(boost::asio::ip::address::from_string("127.0.0.3"), 10003);
	rs_elem.tcp_endpoint = rs_endpoint;
	rs_elem.weight = 0;

	element.realserver_vector.push_back(rs_elem);
	vsd.add_virtual_service(&element, err);
#endif
	ret = test.send_mibcollection(&payload);
	// unit_test[124] send_mibcollection の戻り値が0であることをチェック（正常系、virtualservice１つ、realserver１つの場合）
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[125] send_mibcollection 送信データの設定のチェック
	buffer = test.get_send_buffer();
	header = (struct l7ag_message_header*) buffer;
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );

	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)2 );//vs 1 rs 0
	send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

//VS 1
	pos = (void *) (header + 1);
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;

	vs = (struct l7ag_mibdata_payload_vs*) pos;
	BOOST_CHECK_EQUAL( vs->magic[0], 0x56 );
	BOOST_CHECK_EQUAL( vs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( vs->index, 1 );//
	BOOST_CHECK_EQUAL( vs->protocol, (uint8_t)IPPROTO_TCP );
	if( vs_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->ipAddress, vs_endpoint.address().to_v4().to_ulong() );
	}
	BOOST_CHECK_EQUAL( vs->portNumber, vs_endpoint.port() );
	BOOST_CHECK_EQUAL( vs->reschedule, 0 );
	if( sorry_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->sorryIpAddress, sorry_endpoint.address().to_v4().to_ulong());
	}
	BOOST_CHECK_EQUAL( vs->sorryPortNumber, sorry_endpoint.port() );
	BOOST_CHECK_EQUAL( vs->sorryThreshold, element.sorry_maxconnection );
	BOOST_CHECK_EQUAL( vs->sorryForceFlag, element.sorry_flag );
	BOOST_CHECK_EQUAL( vs->QoSThresholdUp, element.qos_upstream );
	BOOST_CHECK_EQUAL( vs->QoSThresholdDown, element.qos_downstream );
	BOOST_CHECK_EQUAL( vs->throughputUp, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->throughputDown, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->vs_count, 1);
	BOOST_CHECK_EQUAL( vs->scheduleModule, element.schedule_module_name );
	BOOST_CHECK_EQUAL( vs->protocolModule, element.protocol_module_name );

	pos = vs + 1;
//RS 1
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_RS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_rs) );
	pos = payloadheader + 1;
	rs = (struct l7ag_mibdata_payload_rs*) pos;
	BOOST_CHECK_EQUAL( rs->magic[0], 0x52 );
	BOOST_CHECK_EQUAL( rs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( rs->index, 1 );//
	BOOST_CHECK_EQUAL( rs->virtualServiceIndex, 1);//
	pos = rs + 1;

	if( rs_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( rs->ipAddress, rs_endpoint.address().to_v4().to_ulong() );
	}
	BOOST_CHECK_EQUAL( rs->portNumber, rs_endpoint.port() );
	BOOST_CHECK_EQUAL( rs->forwardMode, 1 );
	BOOST_CHECK_EQUAL( rs->weight, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->activeConn, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->inactiveConn, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->rs_count, 1 );//

	// unit_test[126] send_mibcollection 送信サイズは正しいか？
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 1;//
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

#if 1
	l7vs::realserver_element rs_elem2;
	boost::asio::ip::tcp::endpoint rs_endpoint2(boost::asio::ip::address::from_string("127.0.0.4"), 10004);
	rs_elem2.tcp_endpoint = rs_endpoint2;
	rs_elem2.weight = 0;

	l7vs::virtualservice_element element2;
	boost::asio::ip::tcp::endpoint vs_endpoint2(boost::asio::ip::address::from_string("127.0.0.5"), 10005);
	boost::asio::ip::tcp::endpoint sorry_endpoint2(boost::asio::ip::address::from_string("127.0.0.6"), 10006);
	element2.udpmode = false;
	element2.tcp_accept_endpoint	= vs_endpoint2;
	element2.protocol_module_name = "protocol2";
	element2.schedule_module_name = "schedule2";
	element2.sorry_maxconnection = 2000;
	element2.sorry_endpoint = sorry_endpoint2;
	element2.sorry_flag = 0;
	element2.qos_upstream = 200;
	element2.qos_downstream = 200;
	element2.realserver_vector.push_back(rs_elem2);
	vsd.add_virtual_service(&element2, err);
#endif
	ret = test.send_mibcollection(&payload);
	// send_mibcollection 戻り値が0であることをチェック（正常系、virtualservice２つ、realserver２つの場合）
	BOOST_CHECK_EQUAL( ret, 0 );

	// send_mibcollection 送信データの設定のチェック
	buffer = test.get_send_buffer();
	header = (struct l7ag_message_header*) buffer;
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	BOOST_CHECK_EQUAL( header->magic[1], 0x47 );
	BOOST_CHECK_EQUAL( header->version, (unsigned long long)1 );

	BOOST_CHECK_EQUAL( header->payload_count, (unsigned long long)4 );//vs 2 rs 2
	send_buffer_size = test.get_send_buffer_size();
	BOOST_CHECK_EQUAL( header->size, send_buffer_size );

//VS 1
	pos = (void *) (header + 1);
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;

	vs = (struct l7ag_mibdata_payload_vs*) pos;
	BOOST_CHECK_EQUAL( vs->magic[0], 0x56 );
	BOOST_CHECK_EQUAL( vs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( vs->index, 1 );//vs 1
	BOOST_CHECK_EQUAL( vs->protocol, (uint8_t)IPPROTO_TCP );
	if( vs_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->ipAddress, vs_endpoint.address().to_v4().to_ulong() );
	}
	BOOST_CHECK_EQUAL( vs->portNumber, vs_endpoint.port() );
	BOOST_CHECK_EQUAL( vs->reschedule, 0 );
	if( sorry_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->sorryIpAddress, sorry_endpoint.address().to_v4().to_ulong());
	}
	BOOST_CHECK_EQUAL( vs->sorryPortNumber, sorry_endpoint.port() );
	BOOST_CHECK_EQUAL( vs->sorryThreshold, element.sorry_maxconnection );
	BOOST_CHECK_EQUAL( vs->sorryForceFlag, element.sorry_flag );
	BOOST_CHECK_EQUAL( vs->QoSThresholdUp, element.qos_upstream );
	BOOST_CHECK_EQUAL( vs->QoSThresholdDown, element.qos_downstream );
	BOOST_CHECK_EQUAL( vs->throughputUp, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->throughputDown, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->vs_count, 2);//
	BOOST_CHECK_EQUAL( vs->scheduleModule, element.schedule_module_name );
	BOOST_CHECK_EQUAL( vs->protocolModule, element.protocol_module_name );

	pos = vs + 1;
//RS 1
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_RS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_rs) );
	pos = payloadheader + 1;
	rs = (struct l7ag_mibdata_payload_rs*) pos;
	BOOST_CHECK_EQUAL( rs->magic[0], 0x52 );
	BOOST_CHECK_EQUAL( rs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( rs->index, 1 );//rs 1
	BOOST_CHECK_EQUAL( rs->virtualServiceIndex, 1);//
	pos = rs + 1;

	if( rs_endpoint.address().is_v4() ){
		BOOST_CHECK_EQUAL( rs->ipAddress, rs_endpoint.address().to_v4().to_ulong() );
	}
	BOOST_CHECK_EQUAL( rs->portNumber, rs_endpoint.port() );
	BOOST_CHECK_EQUAL( rs->forwardMode, 1 );
	BOOST_CHECK_EQUAL( rs->weight, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->activeConn, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->inactiveConn, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->rs_count, 2 );//

//VS 2
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_VS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_vs) );
	pos = payloadheader + 1;

	vs = (struct l7ag_mibdata_payload_vs*) pos;
	BOOST_CHECK_EQUAL( vs->magic[0], 0x56 );
	BOOST_CHECK_EQUAL( vs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( vs->index, 2 );//vs 2
	BOOST_CHECK_EQUAL( vs->protocol, (uint8_t)IPPROTO_TCP );
	if( vs_endpoint2.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->ipAddress, vs_endpoint2.address().to_v4().to_ulong() );
	}
	BOOST_CHECK_EQUAL( vs->portNumber, vs_endpoint2.port() );
	BOOST_CHECK_EQUAL( vs->reschedule, 0 );
	if( sorry_endpoint2.address().is_v4() ){
		BOOST_CHECK_EQUAL( vs->sorryIpAddress, sorry_endpoint2.address().to_v4().to_ulong());
	}
	BOOST_CHECK_EQUAL( vs->sorryPortNumber, sorry_endpoint2.port() );
	BOOST_CHECK_EQUAL( vs->sorryThreshold, element2.sorry_maxconnection );
	BOOST_CHECK_EQUAL( vs->sorryForceFlag, element2.sorry_flag );
	BOOST_CHECK_EQUAL( vs->QoSThresholdUp, element2.qos_upstream );
	BOOST_CHECK_EQUAL( vs->QoSThresholdDown, element2.qos_downstream );
	BOOST_CHECK_EQUAL( vs->throughputUp, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->throughputDown, (unsigned long long)10);
	BOOST_CHECK_EQUAL( vs->vs_count, 2);//
	BOOST_CHECK_EQUAL( vs->scheduleModule, element2.schedule_module_name );
	BOOST_CHECK_EQUAL( vs->protocolModule, element2.protocol_module_name );

	pos = vs + 1;

//RS 2
	payloadheader = (struct l7ag_payload_header*) pos;
	BOOST_CHECK_EQUAL( payloadheader->magic[0], 0x50 );
	BOOST_CHECK_EQUAL( payloadheader->magic[1], 0x59 );
	BOOST_CHECK_EQUAL( payloadheader->message_id, (unsigned long long)MESSAGE_ID_MIBCOLLECTRESPONSE_RS );
	BOOST_CHECK_EQUAL( payloadheader->payload_datasize, sizeof(struct l7ag_payload_header) + sizeof(struct l7ag_mibdata_payload_rs) );
	pos = payloadheader + 1;
	rs = (struct l7ag_mibdata_payload_rs*) pos;
	BOOST_CHECK_EQUAL( rs->magic[0], 0x52 );
	BOOST_CHECK_EQUAL( rs->magic[1], 0x53 );
	BOOST_CHECK_EQUAL( rs->index, 2 );//rs 2
	BOOST_CHECK_EQUAL( rs->virtualServiceIndex, 2);//vs 2
	pos = rs + 1;

	if( rs_endpoint2.address().is_v4() ){
		BOOST_CHECK_EQUAL( rs->ipAddress, rs_endpoint2.address().to_v4().to_ulong() );
	}
	BOOST_CHECK_EQUAL( rs->portNumber, rs_endpoint2.port() );
	BOOST_CHECK_EQUAL( rs->forwardMode, 1 );
	BOOST_CHECK_EQUAL( rs->weight, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->activeConn, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->inactiveConn, (unsigned long long)0 );//
	BOOST_CHECK_EQUAL( rs->rs_count, 2 );//

	// send_mibcollection 送信サイズは正しいか？
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 2 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 2;//
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// send_mibcollection メモリが確保できなかった場合
	malloc_count = 0;
	malloc_dbg_install();
	ret = test.send_mibcollection(&payload);
	malloc_dbg_uninstall();
	// unit_test[127] send_mibcollection 戻り値は-1か？（異常系、mallocに失敗した場合）
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[128] send_mibcollection 送信バッファのポインタはNULLか？
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	test.finalize();
}

void get_connectionstate_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_connectionstate_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[129] snmpbridge get_connectionstate 戻り値のチェック(初期状態)
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );

	// unit_test[130] snmpbridge get_connectionstate 戻り値(true)のチェック
	test.set_connection_state(true);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	// unit_test[131] snmpbridge get_connectionstate 戻り値(false)のチェック
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
	// unit_test[132] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_START_STOP)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_INFO );
	// unit_test[133] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_MANAGER_RECEIVE)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_INFO );
	// unit_test[134] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_MANAGER_SEND)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), LOG_LV_INFO );
	// unit_test[135] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_L7VSD_RECEIVE)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), LOG_LV_INFO );
	// unit_test[136] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_L7VSD_SEND)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), LOG_LV_INFO );
	// unit_test[137] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_LOGGER)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), LOG_LV_INFO );
	// unit_test[138] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_PARAMETER)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), LOG_LV_INFO );
	// unit_test[139] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_SYSTEM)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM ), LOG_LV_INFO );
	// unit_test[140] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_SYSTEM_MEMORY)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY ), LOG_LV_INFO );
	// unit_test[141] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT ), LOG_LV_INFO );
	// unit_test[142] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL ), LOG_LV_INFO );
	// unit_test[143] snmpbridge get_loglevelのチェック(LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT)
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT ), LOG_LV_INFO );

	// unit_test[144] snmpbridge get_loglevelの各カテゴリのログレベルをチェック
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
	test.change_loglevel( LOG_CAT_SNMPAGENT_SYSTEM, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL ), LOG_LV_DEBUG );
	test.change_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT ), LOG_LV_DEBUG );

	test.finalize();
}

void get_loglevel_allcategory_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_loglevel_allcategory_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[145] get_loglevel_allcategory 全カテゴリのログレベルが取得出来ることを確認
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
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM ), loglevelmap.find( LOG_CAT_SNMPAGENT_SYSTEM )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY ), loglevelmap.find( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT ), loglevelmap.find( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL ), loglevelmap.find( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL )->second );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT ), loglevelmap.find( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT )->second );

	test.finalize();
}

void handle_accept_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_accept_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[146] handle_acceptの呼び出しのチェック
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );
	boost::system::error_code error;
	test.handle_accept(error);

	// unit_test[147] handle_acceptが呼ばれたときにconnection_stateの値が変わっていることの確認
	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	test.finalize();
}

void handle_receive_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_receive_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[148] handle_receiveの呼び出しのチェック
	boost::system::error_code error;
	size_t bytes_transferred;
	test.handle_receive( error, bytes_transferred );

	test.finalize();
}

void handle_send_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_send_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[149] handle_sendの呼び出しのチェック
	boost::system::error_code error;
	size_t bytes_transferred;
	test.handle_send( error, bytes_transferred );

	test.finalize();
}

void load_config_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "load_config_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[150] load_configの呼び出しのチェック（正常系）
	test.load_config2();

	// load_config snmp_paramの確認
	l7ag_parameter& snmpparam = test.get_snmp_param();

	// unit_test[151] load_config nicの値のチェック
	BOOST_CHECK_EQUAL( snmpparam.nic, nic );
	// unit_test[152] load_config addressの値のチェック
	BOOST_CHECK_EQUAL( snmpparam.address, ip_addr );
	// unit_test[153] load_config portnoの値のチェック
	BOOST_CHECK_EQUAL( snmpparam.portno, portno );
	// unit_test[154] load_config intervalの値のチェック
	BOOST_CHECK_EQUAL( snmpparam.interval, interval );
	// unit_test[155] load_config statusの値のチェック
	BOOST_CHECK_EQUAL( snmpparam.status, status );

	// unit_test[156] load_configの呼び出しのチェック（パラメータ取得異常の場合）	
	get_int_errorflag = true;
	get_string_errorflag = true;
	test.load_config2();
	get_int_errorflag = false;
	get_string_errorflag = false;
	// unit_test[157] load_config デフォルト値が設定されていることを確認
	BOOST_CHECK_EQUAL( snmpparam.nic, NIC_DEFAULT );
	BOOST_CHECK_EQUAL( snmpparam.address, ADDR_DEFAULT );
	BOOST_CHECK_EQUAL( snmpparam.portno, PORT_DEFAULT );
	BOOST_CHECK_EQUAL( snmpparam.interval, INTERVAL_DEFAULT );
	BOOST_CHECK_EQUAL( snmpparam.status, 0 );

	test.finalize();
}

void load_loglevel_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "load_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[158] load_loglevelの呼び出しのチェック（正常系）
	test.load_loglevel2();
	// ロードした設定値の確認
	// unit_test[159] load_loglevel カテゴリ名のマップのサイズのチェック
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)12 );

	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
	// unit_test[160] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_START_STOP）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_start_stop" );

	// unit_test[161] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_MANAGER_RECEIVE）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_receive" );

	// unit_test[162] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_MANAGER_SEND）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_send" );

	// unit_test[163] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_L7VSD_RECEIVE）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_receive" );

	// unit_test[164] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_L7VSD_SEND）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_send" );

	// unit_test[165] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_LOGGER）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_logger" );

	// unit_test[166] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_PARAMETER）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_parameter" );

	// unit_test[167] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_SYSTEM）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system" );

	// unit_test[168] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_SYSTEM_MEMORY）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_MEMORY );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_memory" );

	// unit_test[169] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_endpoint" );

	// unit_test[170] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_signal" );

	// unit_test[171] load_loglevel カテゴリ名のマップの値のチェック（LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT）
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_environment" );

	// unit_test[172] load_loglevel ログレベルのマップのサイズをチェック
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

	// unit_test[173] load_loglevel ログレベルのマップの値のチェック（debug）
	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
	levelstring_map_it = levelstring_map.find( "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_DEBUG );

	// unit_test[174] load_loglevel ログレベルのマップの値のチェック（info）
	levelstring_map_it = levelstring_map.find( "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_INFO );

	// unit_test[175] load_loglevel ログレベルのマップの値のチェック（warn）
	levelstring_map_it = levelstring_map.find( "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_WARN );

	// unit_test[176] load_loglevel ログレベルのマップの値のチェック（error）
	levelstring_map_it = levelstring_map.find( "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_ERROR );

	// unit_test[177] load_loglevel ログレベルのマップの値のチェック（fatal）
	levelstring_map_it = levelstring_map.find( "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_FATAL );

	// unit_test[178] load_loglevelの呼び出しのチェック（パラメータ取得異常の場合）	
	get_int_errorflag = true;
	get_string_errorflag = true;
	test.load_loglevel2();
	get_int_errorflag = false;
	get_string_errorflag = false;
	// unit_test[179] load_loglevel カテゴリ名のマップのサイズと値にデフォルト値が設定されていることを確認
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)12 );

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

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_MEMORY );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_MEMORY );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_memory" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_ENDPOINT );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_endpoint" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_SIGNAL );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_signal" );

	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_SYSTEM_ENVIRONMENT );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_system_environment" );

	// unit_test[180] load_loglevel ログレベルのマップのサイズと値にデフォルト値が設定されていることを確認
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
	ts->add( BOOST_TEST_CASE( &handle_accept_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &handle_receive_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &handle_send_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_config_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_loglevel_snmpbridge_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
