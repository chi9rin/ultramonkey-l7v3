
#include <iostream>
#include <boost/test/included/unit_test.hpp>
#include "snmpbridge.h"


using namespace boost::unit_test;
using namespace l7vs;

class snmpbridge_testclass : public snmpbridge{
public:
	l7ag_parameter&						get_snmp_param(){ return snmp_param; }
	char*								get_send_buffer(){ return send_buffer; }
	size_t&								get_send_buffer_size(){ return send_buffer_size; }
	boost::array<char, READBUF_SIZE>&	get_recv_buffer(){ return recv_buffer; }
	boost::asio::io_service&			get_snmp_io_service(){ return snmp_io_service; }
	bool&								get_connection_state(){ return connection_state; }
	snmp_logcategory_map_type&			get_snmp_loglevel_map(){ return snmp_loglevel_map; }
	string_loglevel_map_type&			get_levelstring_map(){ return levelstring_map; }
	l7vsd&								get_vsd(){ return vsd; }

	snmpbridge_testclass( l7vsd& l7vsd_in, boost::asio::io_service& io_service_in ) : snmpbridge(l7vsd_in, io_service_in){}
};

void create_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "create_snmpbridge_test" );
	// unit_test[1] snmpbridge $B%/%i%9$N@8@.%F%9%H(B
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[] $B@8@.;~$K@_Dj$5$l$k%Q%i%a!<%?$N3NG'(B
	BOOST_CHECK( test.get_send_buffer_size() == 0 );
	// unit_test[] $B@8@.;~$K@_Dj$5$l$k%Q%i%a!<%?$N3NG'(B
	BOOST_CHECK_EQUAL( test.get_connection_state(), false );
	// unit_test[] $B@8@.;~$K@_Dj$5$l$k%Q%i%a!<%?$N3NG'(B
	BOOST_CHECK_EQUAL( &(test.get_vsd()), &vsd );
	// unit_test[] $B@8@.;~$K@_Dj$5$l$k%Q%i%a!<%?$N3NG'(B
	BOOST_CHECK_EQUAL( &(test.get_snmp_io_service()), &io_service );
	// unit_test[] $B@8@.;~$K@_Dj$5$l$k%Q%i%a!<%?$N3NG'(B
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK( logcategory_map.size() == 0 );
	// unit_test[] $B@8@.;~$K@_Dj$5$l$k%Q%i%a!<%?$N3NG'(B
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK( levelstring_map.size() == 0 );

}

void initialize_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "initialize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// $B%^%C%W$N%5%$%:(B
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK( logcategory_map.size() == 0 );
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK( levelstring_map.size() == 0 );

	// unit_test[]
	// $B@5>o(B
	BOOST_CHECK_EQUAL( test.initialize(), 0 );

	// unit_test[] $B=i4|2=8e$N%Q%i%a!<%?$N3NG'(B
	// $B%^%C%W$N%5%$%:!"@_Dj$5$l$F$$$kCM(B
//	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK( logcategory_map.size() == 7 );

	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
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

	// unit_test[] $B=i4|2=8e$N%Q%i%a!<%?$N3NG'(B
	// $B%^%C%W$N%5%$%:!"@_Dj$5$l$F$$$kCM(B
//	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK( levelstring_map.size() == 5 );

	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
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

	// unit_test[] snmp_param$B$N3NG'(B
	l7ag_parameter& snmpparam = test.get_snmp_param();

	// unit_test[] $B%]!<%HHV9f$NCM$N%A%'%C%/(B
//	BOOST_CHECK_EQUAL( snmpparam.portno,  );

	// unit_test[] $B%$%s%?!<%P%k$NCM$N%A%'%C%/(B
//	BOOST_CHECK_EQUAL( snmpparam.interval,  );

	// unit_test[]
	// $B%"%/%;%W%?!"%(%s%I%]%$%s%H$N3NG'(B

	// unit_test[] $B%Q%i%a!<%?$,0[>o$J>l9g(B
	BOOST_CHECK_EQUAL( test.initialize(), -1 );
	// unit_test[] $B=i4|2=8e$N%Q%i%a!<%?$N3NG'(B
	// $B%^%C%W$N%5%$%:!"@_Dj$5$l$F$$$kCM(B
//	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK( logcategory_map.size() == 7 );

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

	// unit_test[] $B=i4|2=8e$N%Q%i%a!<%?$N3NG'(B
	// $B%^%C%W$N%5%$%:!"@_Dj$5$l$F$$$kCM(B
//	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK( levelstring_map.size() == 5 );

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

	// unit_test[] snmp_param$B$N3NG'(B
	snmpparam = test.get_snmp_param();

	// unit_test[] $B%]!<%HHV9f$,%G%U%)%k%H@_DjCM$K$J$C$F$$$k$+!)(B
	BOOST_CHECK_EQUAL( snmpparam.portno, PORT_DEFAULT );

	// unit_test[] $B%$%s%?!<%P%k$,%G%U%)%k%H@_DjCM$K$J$C$F$$$k$+!)(B
	BOOST_CHECK_EQUAL( snmpparam.interval, INTERVAL_DEFAULT );

	test.finalize();
}

void finalize_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "finalize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// $B@5>o(B
	test.finalize();
	// unit_test[]
	// $B%"%/%;%W%?$N3NG'(B
}

void send_trap_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "send_trap_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// $B@5>o(B
	int ret = 0;
//	ret = test.send_trap();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );
	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_traprequest_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// $B%a%b%j$,3NJ]$G$-$J$+$C$?>l9g(B
//	ret = test.send_trap();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// $BAw?.%5%$%:$,(B0$B$N>l9g(B
//	ret = test.send_trap();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
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
	// $B@5>o(B
//	ret = test.reload_config();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// $B%a%b%j$,3NJ]$G$-$J$+$C$?>l9g(B
//	ret = test.reload_config();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );


	// $BAw?.%5%$%:$,(B0$B$N>l9g(B
//	ret = test.reload_config();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
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
	// $B@5>o(B
	int ret = 0;
//	ret = test.change_loglevel();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
	size_t size = 0;
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) +
			sizeof( struct l7ag_settingcommand_message );
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// $B%a%b%j$,3NJ]$G$-$J$+$C$?>l9g(B
//	ret = test.change_loglevel();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// $BAw?.%5%$%:$,(B0$B$N>l9g(B
//	ret = test.change_loglevel();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
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
	// $B@5>o(B
	int ret = 0;
//	ret = test.change_loglevel_allcategory();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			sizeof( struct l7ag_payload_header ) * settingcmd_vec.size() +
			sizeof( struct l7ag_settingcommand_message ) * settingcmd_vec.size();
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// $B%a%b%j$,3NJ]$G$-$J$+$C$?>l9g(B
//	ret = test.change_loglevel_allcategory();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

//	ret = test.change_loglevel_allcategory();
	// $BAw?.%5%$%:$,(B0$B$N>l9g(B
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
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
	// $B@5>o(B
	int ret = 0;
//	ret = test.send_mibcollection();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );


	// $B%a%b%j$,3NJ]$G$-$J$+$C$?>l9g(B
//	ret = test.send_mibcollection();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	// $BAw?.%5%$%:$,(B0$B$N>l9g(B
//	ret = test.send_mibcollection();
	// unit_test[] $BLa$jCM$O(B-1$B$+!)(B
	BOOST_CHECK_EQUAL( ret, -1 );

	// unit_test[] $BAw?.%P%C%U%!$N%]%$%s%?$O(BNULL$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer(), (char *)NULL );

	// unit_test[] $BAw?.%5%$%:$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );

	test.finalize();
}

void get_connectionstate_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "get_connectionstate_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] snmpbridge get_connectionstate$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_connectionstate(), false );

//	handle_accept( );
	// unit_test[]
	// $B>uBV$,JQ$o$C$F$$$k$3$H$N3NG'(B
	BOOST_CHECK_EQUAL( test.get_connectionstate(), true );

	test.finalize();
}

void get_loglevel_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "get_loglevel_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_NONE );

	test.change_loglevel( LOG_CAT_SNMPAGENT_START_STOP, LOG_LV_DEBUG );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_DEBUG );

//	test.change_loglevel_allcategory( LOG_LV_INFO );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_INFO );
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_INFO );

	test.finalize();
}

void handle_accept_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "handle_accept_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// $B@5>o(B
//	test.handle_accept();

	test.finalize();
}

void handle_receive_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "handle_receive_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[]
	// $B@5>o(B
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
	// $B@5>o(B
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
	// $B@5>o(B
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
	// $B@5>o(B
//	test.load_loglevel();
	// $B%m!<%I$7$?@_DjCM$N3NG'(B

	// unit_test[]
	// $B%Q%i%a!<%?$,0[>o$J>l9g(B	
//	test.load_loglevel();
	// $B%G%U%)%k%HCM$,@_Dj$5$l$F$$$k$3$H$r3NG'(B

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
//	ts->add( BOOST_TEST_CASE( &get_connectionstate_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &get_loglevel_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_accept_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_receive_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &handle_send_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &load_config_snmpbridge_test ) );
//	ts->add( BOOST_TEST_CASE( &load_loglevel_snmpbridge_test ) );

	framework::master_test_suite().add( ts );

	return 0;
}
