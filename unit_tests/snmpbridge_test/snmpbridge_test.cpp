
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
											snmp_param = snmp_param_in; }
	void								set_connection_state(bool in){ connection_state = in; }

};

void create_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "create_snmpbridge_test" );
	// unit_test[1] snmpbridge �N���X�𐶐�����
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[2] snmpbridge �������ɐݒ肳���send_buffer_size�̒l���`�F�b�N
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
	// unit_test[3] snmpbridge �������ɐݒ肳���connection_state�̒l���`�F�b�N
	BOOST_CHECK_EQUAL( test.get_connection_state(), false );
	// unit_test[4] snmpbridge �������ɐݒ肳���vsd�̒l���`�F�b�N
	BOOST_CHECK_EQUAL( &(test.get_vsd()), &vsd );
	// unit_test[5] snmpbridge �������ɐݒ肳���snmp_io_service�̒l���`�F�b�N
	BOOST_CHECK_EQUAL( &(test.get_snmp_io_service()), &io_service );
	// unit_test[6] snmpbridge �������ɐݒ肳���snmp_loglevel_map�̃}�b�v�̃T�C�Y���`�F�b�N
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[7] snmpbridge �������ɐݒ肳���levelstring_map�̃}�b�v�̃T�C�Y���`�F�b�N
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

}

void initialize_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "initialize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[8] initialize �������O�̃J�e�S�����̃}�b�v�̃T�C�Y�̃`�F�b�N
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[9] initialize �������O�̃��O���x���̃}�b�v�̃T�C�Y�̃`�F�b�N
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

	// unit_test[10] initialize �߂�l��0�ł��邱�Ƃ��m�F�i����n�j
	int ret = 0;
	int set_portno_val = 60162;
	int set_interval_val = 1000;
	set_portno(set_portno_val);
	set_interval(set_interval_val);

	ret = test.initialize();
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̃T�C�Y�̃`�F�b�N
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)7 );

	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_START_STOP�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_start_stop" );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_MANAGER_RECEIVE�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_receive" );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_MANAGER_SEND�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_send" );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_L7VSD_RECEIVE�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_receive" );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_L7VSD_SEND�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_send" );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_LOGGER�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_logger" );

	// unit_test[] initialize ��������̃J�e�S�����̃}�b�v�̒l�̃`�F�b�N�iLOG_CAT_SNMPAGENT_PARAMETER�j
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_parameter" );

	// unit_test[] initialize ��������̃��O���x���̃}�b�v�̃T�C�Y���`�F�b�N
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

	// unit_test[] initialize ��������̃��O���x���̃}�b�v�̒l�̃`�F�b�N�idebug�j
	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
	levelstring_map_it = levelstring_map.find( "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_DEBUG );

	// unit_test[] initialize ��������̃��O���x���̃}�b�v�̒l�̃`�F�b�N�iinfo�j
	levelstring_map_it = levelstring_map.find( "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_INFO );

	// unit_test[] initialize ��������̃��O���x���̃}�b�v�̒l�̃`�F�b�N�iwarn�j
	levelstring_map_it = levelstring_map.find( "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_WARN );

	// unit_test[] initialize ��������̃��O���x���̃}�b�v�̒l�̃`�F�b�N�ierror�j
	levelstring_map_it = levelstring_map.find( "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_ERROR );

	// unit_test[] initialize ��������̃��O���x���̃}�b�v�̒l�̃`�F�b�N�ifatal�j
	levelstring_map_it = levelstring_map.find( "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_FATAL );

	// initialize snmp_param�̊m�F
	l7ag_parameter& snmpparam = test.get_snmp_param();
	// unit_test[] initialize �|�[�g�ԍ��̒l�̃`�F�b�N
	BOOST_CHECK_EQUAL( snmpparam.portno, set_portno_val );
	// unit_test[] initialize �C���^�[�o���̒l�̃`�F�b�N
	BOOST_CHECK_EQUAL( snmpparam.interval, set_interval_val );

	// unit_test[] initialize snmp_acceptor��open����Ă��邱�Ƃ̃`�F�b�N
	boost::asio::ip::tcp::acceptor& acc = test.get_snmp_acceptor();
	BOOST_CHECK_EQUAL( acc.is_open(), true );
	// unit_test[] initialize snmp_acceptor��reuse_address�̐ݒ�̃`�F�b�N
	boost::asio::socket_base::reuse_address option(true);
	boost::asio::socket_base::reuse_address option1;
	acc.get_option(option1);
	BOOST_CHECK_EQUAL( option1, option );

	// �p�����[�^���ُ�ȏꍇ�̊m�F
	// set_parameter
	// ret = test.initialize();
#if 0
	// unit_test[] initialize �߂�l���|1�ł��邱�Ƃ��m�F�i�ُ�n�j
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif
	// unit_test[] initialize �J�e�S�����̃}�b�v�̃T�C�Y�ƒl�̃`�F�b�N
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

	// unit_test[] initialize ���O���x���̃}�b�v�̃T�C�Y�ƒl�̃`�F�b�N
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

	// initialize snmp_param�̊m�F
	snmpparam = test.get_snmp_param();
	// unit_test[] initialize �|�[�g�ԍ��̃f�t�H���g�ݒ�l�̃`�F�b�N
	BOOST_CHECK_EQUAL( snmpparam.portno, PORT_DEFAULT );

	// unit_test[] initialize �C���^�[�o���̃f�t�H���g�ݒ�l�̃`�F�b�N
	BOOST_CHECK_EQUAL( snmpparam.interval, INTERVAL_DEFAULT );

	test.finalize();
}

void finalize_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "finalize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] finalize�̌Ăяo���̃`�F�b�N
	test.finalize();
	// unit_test[] finalize connection_state�̒l���`�F�b�N
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );
	// unit_test[] finalize snmp_acceptor��close����Ă��邱�Ƃ̃`�F�b�N
	BOOST_CHECK_EQUAL( test.get_snmp_acceptor().is_open(), false );
	// unit_test[] finalize snmp_socket��close����Ă��邱�Ƃ̃`�F�b�N
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
	// unit_test[] send_trap �߂�l��0�ł��邱�Ƃ��m�F�i����n�j
	BOOST_CHECK_EQUAL( ret, 0 );
	// send_trap ���M�f�[�^�̐ݒ�͐��������H
	char * buffer;
	buffer = test.get_send_buffer();
	struct l7ag_message_header* header        = (struct l7ag_message_header*) buffer;
	struct l7ag_payload_header* payloadheader = (struct l7ag_payload_header*) (header + 1);
	struct l7ag_traprequest_message* trapmes  = (struct l7ag_traprequest_message*) (payloadheader + 1);

	// unit_test[] send_trap header->magic[0]�̃`�F�b�N
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[] send_trap header->magic[1]�̃`�F�b�N
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
	// unit_test[] send_trap ���M�T�C�Y�̃`�F�b�N
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_traprequest_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// ���������m�ۂł��Ȃ������ꍇ
//	ret = test.send_trap();
	// unit_test[] send_trap �߂�l���|1�ł��邱�Ƃ��m�F�i�ُ�n�j
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] send_trap ���M�o�b�t�@��NULL�`�F�b�N
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] send_trap ���M�T�C�Y�̃`�F�b�N
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// ���M�T�C�Y��0�̏ꍇ
//	ret = test.send_trap();
	// unit_test[] send_trap �߂�l���|1�ł��邱�Ƃ��m�F�i�ُ�n�j
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] send_trap ���M�o�b�t�@��NULL�`�F�b�N
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] send_trap ���M�T�C�Y�̃`�F�b�N
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

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
	// unit_test[] reload_config �߂�l��0�ł��邱�Ƃ��m�F�i����n�j
//	BOOST_CHECK_EQUAL( ret, 0 );

	// reload_config ���M�f�[�^�̐ݒ�͐��������H
	char * buffer;
	buffer = test.get_send_buffer();
    struct l7ag_message_header* header             = (struct l7ag_message_header*) buffer;
    struct l7ag_payload_header* payloadheader      = (struct l7ag_payload_header*) (header + 1);
    struct l7ag_settingcommand_message* settingcmd = (struct l7ag_settingcommand_message*) (payloadheader + 1);

	// unit_test[] reload_config header->magic[0]�̃`�F�b�N
	BOOST_CHECK_EQUAL( header->magic[0], 0x4d );
	// unit_test[] reload_config header->magic[1]�̃`�F�b�N
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

	// unit_test[] reload_config ���M�T�C�Y�̃`�F�b�N
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// ���������m�ۂł��Ȃ������ꍇ
//	ret = test.reload_config();
	// unit_test[] reload_config �߂�l���|1�ł��邱�Ƃ��m�F�i�ُ�n�j
//	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] reload_config ���M�o�b�t�@��NULL�`�F�b�N
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] reload_config ���M�T�C�Y�̃`�F�b�N
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );


	// ���M�T�C�Y��0�̏ꍇ
//	ret = test.reload_config();
	// unit_test[] reload_config �߂�l���|1�ł��邱�Ƃ��m�F�i�ُ�n�j
//	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] reload_config ���M�o�b�t�@��NULL�`�F�b�N
	buffer = test.get_send_buffer();
	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] reload_config ���M�T�C�Y�̃`�F�b�N
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

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
//	ret = test.change_loglevel();
	// unit_test[]  change_loglevel �߂�l��0�ł��邱�Ƃ��m�F�i����n�j
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] change_loglevel ���M�f�[�^�̐ݒ�͐��������H

	// unit_test[] change_loglevel ���M�T�C�Y�͐��������H
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// ���������m�ۂł��Ȃ������ꍇ
//	ret = test.change_loglevel();
	// unit_test[] change_loglevel �߂�l���|1�ł��邱�Ƃ��m�F
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] change_loglevel ���M�o�b�t�@�̃|�C���^��NULL���H
//	buffer = test.get_send_buffer();
//	BOOST_CHECK( buffer == (char *)NULL );

	// unit_test[] change_loglevel ���M�T�C�Y��0���H
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// ���M�T�C�Y��0�̏ꍇ
//	ret = test.change_loglevel();
	// unit_test[] change_loglevel �߂�l��-1���H
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] change_loglevel ���M�o�b�t�@�̃|�C���^��NULL���H
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] change_loglevel ���M�T�C�Y��0���H
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void change_loglevel_allcategory_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "change_loglevel_allcategory_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
	int ret = 0;
//	ret = test.change_loglevel_allcategory();
	// unit_test[] �߂�l��0���H
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] ���M�f�[�^�̐ݒ�͐��������H

	// unit_test[] ���M�T�C�Y�͐��������H
	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) * settingcmd_vec.size() +
			sizeof( struct l7ag_settingcommand_message ) * settingcmd_vec.size();
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// ���������m�ۂł��Ȃ������ꍇ
//	ret = test.change_loglevel_allcategory();
	// unit_test[] �߂�l��-1���H
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] ���M�o�b�t�@�̃|�C���^��NULL���H
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] ���M�T�C�Y��0���H
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

//	ret = test.change_loglevel_allcategory();
	// ���M�T�C�Y��0�̏ꍇ
	// unit_test[] �߂�l��-1���H
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] ���M�o�b�t�@�̃|�C���^��NULL���H
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] ���M�T�C�Y��0���H
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void send_mibcollection_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "send_mibcollection_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
	int ret = 0;
	// virtualservice�P�����̏ꍇ
	struct l7ag_mibrequest_message payload;
	ret = test.send_mibcollection(&payload);
	// unit_test[] �߂�l��0���H
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] ���M�f�[�^�̐ݒ�͐��������H

	// unit_test[] ���M�T�C�Y�͐��������H
	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 0;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice�P�Arealserver�P�̏ꍇ
//	ret = test.send_mibcollection();
	// unit_test[] �߂�l��0���H
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] ���M�f�[�^�̐ݒ�͐��������H

	// unit_test[] ���M�T�C�Y�͐��������H
//	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 1 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 1;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice�Q�̏ꍇ
//	ret = test.send_mibcollection();
	// unit_test[] �߂�l��0���H
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] ���M�f�[�^�̐ݒ�͐��������H

	// unit_test[] ���M�T�C�Y�͐��������H
//	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 2 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 0;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice�Q�Arealserver�Q�̏ꍇ
//	ret = test.send_mibcollection();
	// unit_test[] �߂�l��0���H
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] ���M�f�[�^�̐ݒ�͐��������H

	// unit_test[] ���M�T�C�Y�͐��������H
//	size_t size = 0;
#if 1
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * 2 +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * 2;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// ���������m�ۂł��Ȃ������ꍇ
//	ret = test.send_mibcollection();
	// unit_test[] �߂�l��-1���H
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] ���M�o�b�t�@�̃|�C���^��NULL���H
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] ���M�T�C�Y��0���H
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// ���M�T�C�Y��0�̏ꍇ
//	ret = test.send_mibcollection();
	// unit_test[] �߂�l��-1���H
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] ���M�o�b�t�@�̃|�C���^��NULL���H
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] ���M�T�C�Y��0���H
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void get_connectionstate_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_connectionstate_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] snmpbridge get_connectionstate�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );

	// unit_test[] snmpbridge get_connectionstate�̃e�X�g
	test.set_connection_state(true);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	// unit_test[] snmpbridge get_connectionstate�̃e�X�g
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
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel�̃e�X�g
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), LOG_LV_NONE );


	// unit_test[] snmpbridge loglevel�̃e�X�g
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

void get_loglevel_allcategory_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "get_loglevel_allcategory_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	std::map<LOG_CATEGORY_TAG, LOG_LEVEL_TAG>	loglevelmap;
	test.get_loglevel_allcategory( loglevelmap );
	BOOST_CHECK_EQUAL( loglevelmap.size(), (size_t)2 );//debug
	std::map<LOG_CATEGORY_TAG, LOG_LEVEL_TAG>::iterator it = loglevelmap.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( it->second, LOG_LV_INFO );
	it = loglevelmap.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( it->second, LOG_LV_INFO );

	test.finalize();
}

void handle_accept_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_accept_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
//	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );
//	test.handle_accept();

	// unit_test[] handle_accept���Ă΂ꂽ�Ƃ��ɏ�Ԃ��ς���Ă��邱�Ƃ̊m�F
//	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	test.finalize();
}

void handle_receive_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_receive_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
//	test.handle_receive();

	test.finalize();
}

void handle_send_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "handle_send_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
//	test.handle_send();

	test.finalize();
}

void load_config_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "load_config_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
//	test.load_config();

	test.finalize();
}

void load_loglevel_snmpbridge_test(){
//	BOOST_TEST_MESSAGE( "load_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// ����
//	test.load_loglevel();
	// ���[�h�����ݒ�l�̊m�F

	// unit_test[]
	// �p�����[�^���ُ�ȏꍇ	
//	test.load_loglevel();
	// �f�t�H���g�l���ݒ肳��Ă��邱�Ƃ��m�F

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
//	ts->add( BOOST_TEST_CASE( &change_loglevel_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &change_loglevel_allcategory_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &send_mibcollection_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_connectionstate_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &get_loglevel_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &get_loglevel_allcategory_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_accept_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_receive_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_send_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_config_snmpbridge_test ) );
	ts->add( BOOST_TEST_CASE( &load_loglevel_snmpbridge_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
