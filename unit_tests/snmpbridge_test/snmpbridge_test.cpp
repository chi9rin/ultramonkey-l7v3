
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
	// unit_test[1] snmpbridge $B%/%i%9$r@8@.$9$k(B
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[2] snmpbridge $B@8@.;~$K@_Dj$5$l$k(Bsend_buffer_size$B$NCM$r%A%'%C%/(B
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), (size_t)0 );
	// unit_test[3] snmpbridge $B@8@.;~$K@_Dj$5$l$k(Bconnection_state$B$NCM$r%A%'%C%/(B
	BOOST_CHECK_EQUAL( test.get_connection_state(), false );
	// unit_test[4] snmpbridge $B@8@.;~$K@_Dj$5$l$k(Bvsd$B$NCM$r%A%'%C%/(B
	BOOST_CHECK_EQUAL( &(test.get_vsd()), &vsd );
	// unit_test[5] snmpbridge $B@8@.;~$K@_Dj$5$l$k(Bsnmp_io_service$B$NCM$r%A%'%C%/(B
	BOOST_CHECK_EQUAL( &(test.get_snmp_io_service()), &io_service );
	// unit_test[6] snmpbridge $B@8@.;~$K@_Dj$5$l$k(Bsnmp_loglevel_map$B$N%^%C%W$N%5%$%:$r%A%'%C%/(B
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[7] snmpbridge $B@8@.;~$K@_Dj$5$l$k(Blevelstring_map$B$N%^%C%W$N%5%$%:$r%A%'%C%/(B
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

}

void initialize_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "initialize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	// unit_test[8] initialize $B=i4|2=A0$N%+%F%4%jL>$N%^%C%W$N%5%$%:$N%A%'%C%/(B
	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)0 );
	// unit_test[9] initialize $B=i4|2=A0$N%m%0%l%Y%k$N%^%C%W$N%5%$%:$N%A%'%C%/(B
	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)0 );

	// unit_test[10] initialize $BLa$jCM$N%A%'%C%/!J@5>o7O!K(B
	int ret = 0;
	ret = test.initialize();
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%5%$%:$N%A%'%C%/(B
//	std::map<LOG_CATEGORY_TAG, std::string> logcategory_map = test.get_snmp_loglevel_map();
	logcategory_map = test.get_snmp_loglevel_map();
	BOOST_CHECK_EQUAL( logcategory_map.size(), (size_t)7 );

	std::map<LOG_CATEGORY_TAG, std::string>::iterator logcategory_map_it = logcategory_map.begin();
	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_START_STOP$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_START_STOP );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_start_stop" );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_MANAGER_RECEIVE$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_receive" );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_MANAGER_SEND$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_MANAGER_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_manager_send" );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_L7VSD_RECEIVE$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_RECEIVE );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_receive" );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_L7VSD_SEND$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_L7VSD_SEND );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_l7vsd_send" );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_LOGGER$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_LOGGER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_logger" );

	// unit_test[] initialize $B=i4|2=8e$N%+%F%4%jL>$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(BLOG_CAT_SNMPAGENT_PARAMETER$B!K(B
	logcategory_map_it = logcategory_map.find( LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->first, LOG_CAT_SNMPAGENT_PARAMETER );
	BOOST_CHECK_EQUAL( logcategory_map_it->second, "snmpagent_parameter" );

	// unit_test[] initialize $B=i4|2=8e$N%m%0%l%Y%k$N%^%C%W$N%5%$%:$r%A%'%C%/(B
//	std::map<std::string, LOG_LEVEL_TAG> levelstring_map = test.get_levelstring_map();
	levelstring_map = test.get_levelstring_map();
	BOOST_CHECK_EQUAL( levelstring_map.size(), (size_t)5 );

	// unit_test[] initialize $B=i4|2=8e$N%m%0%l%Y%k$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(Bdebug$B!K(B
	std::map<std::string, LOG_LEVEL_TAG>::iterator levelstring_map_it = levelstring_map.begin();
	levelstring_map_it = levelstring_map.find( "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "debug" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_DEBUG );

	// unit_test[] initialize $B=i4|2=8e$N%m%0%l%Y%k$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(Binfo$B!K(B
	levelstring_map_it = levelstring_map.find( "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "info" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_INFO );

	// unit_test[] initialize $B=i4|2=8e$N%m%0%l%Y%k$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(Bwarn$B!K(B
	levelstring_map_it = levelstring_map.find( "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "warn" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_WARN );

	// unit_test[] initialize $B=i4|2=8e$N%m%0%l%Y%k$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(Berror$B!K(B
	levelstring_map_it = levelstring_map.find( "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "error" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_ERROR );

	// unit_test[] initialize $B=i4|2=8e$N%m%0%l%Y%k$N%^%C%W$N%-!<$HCM$N%A%'%C%/!J(Bfatal$B!K(B
	levelstring_map_it = levelstring_map.find( "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->first, "fatal" );
	BOOST_CHECK_EQUAL( levelstring_map_it->second, LOG_LV_FATAL );

	// initialize snmp_param$B$N3NG'(B
	l7ag_parameter& snmpparam = test.get_snmp_param();
	// unit_test[] initialize $B%]!<%HHV9f$NCM$N%A%'%C%/(B
//	BOOST_CHECK_EQUAL( snmpparam.portno,  );
	// unit_test[] initialize $B%$%s%?!<%P%k$NCM$N%A%'%C%/(B
//	BOOST_CHECK_EQUAL( snmpparam.interval,  );

	// unit_test[] initialize $B%"%/%;%W%?!"%(%s%I%]%$%s%H$N%A%'%C%/(B

	// $B%Q%i%a!<%?$,0[>o$J>l9g$N3NG'(B
	// set_parameter
	// ret = test.initialize();
#if 0
	// unit_test[] initialize $BLa$jCM$N%A%'%C%/!J0[>o7O!K(B
//	BOOST_CHECK_EQUAL( ret, -1 );
#endif
	// unit_test[] initialize $B=i4|2=8e$N%Q%i%a!<%?$N3NG'(B
	// $B%^%C%W$N%5%$%:!"@_Dj$5$l$F$$$kCM(B
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

	// unit_test[] initialize $B=i4|2=8e$N%Q%i%a!<%?$N3NG'(B
	// $B%^%C%W$N%5%$%:!"@_Dj$5$l$F$$$kCM(B
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

	// unit_test[] initialize snmp_param$B$N3NG'(B
	snmpparam = test.get_snmp_param();

	// unit_test[] initialize $B%]!<%HHV9f$,%G%U%)%k%H@_DjCM$K$J$C$F$$$k$+!)(B
	BOOST_CHECK_EQUAL( snmpparam.portno, PORT_DEFAULT );

	// unit_test[] initialize $B%$%s%?!<%P%k$,%G%U%)%k%H@_DjCM$K$J$C$F$$$k$+!)(B
	BOOST_CHECK_EQUAL( snmpparam.interval, INTERVAL_DEFAULT );

	test.finalize();
}

void finalize_snmpbridge_test(){
	BOOST_TEST_MESSAGE( "finalize_snmpbridge_test" );
	boost::asio::io_service io_service;
	l7vsd vsd;
	snmpbridge_testclass test(vsd, io_service);

	test.initialize();

	// unit_test[] finalize$B$N8F$S=P$7$N%A%'%C%/(B
	test.finalize();
	// unit_test[] finalize connection_state$B$NCM$r%A%'%C%/(B
	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );
	// unit_test[] finalize snmp_acceptor$B$,(Bclose$B$5$l$F$$$k$3$H$N%A%'%C%/(B
	BOOST_CHECK_EQUAL( test.get_snmp_acceptor().is_open(), false );
	// unit_test[] finalize snmp_socket$B$,(Bclose$B$5$l$F$$$k$3$H$N%A%'%C%/(B
	BOOST_CHECK_EQUAL( test.get_snmp_socket().is_open(), false );
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
	// virtualservice$B#1$D$@$1$N>l9g(B
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

	// virtualservice$B#1$D!"(Brealserver$B#1$D$N>l9g(B
//	ret = test.send_mibcollection();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
//	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice$B#2$D$N>l9g(B
//	ret = test.send_mibcollection();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
//	size_t size = 0;
#if 0
	size = sizeof( struct l7ag_message_header ) +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_vs ) ) * payload_vs_count +
			( sizeof( struct l7ag_payload_header ) + sizeof( struct l7ag_mibdata_payload_rs ) ) * payload_rs_count;
#endif
	BOOST_CHECK_EQUAL( test.get_send_buffer_size(), size );

	// virtualservice$B#2$D!"(Brealserver$B#2$D$N>l9g(B
//	ret = test.send_mibcollection();
	// unit_test[] $BLa$jCM$O(B0$B$+!)(B
	BOOST_CHECK_EQUAL( ret, 0 );

	// unit_test[] $BAw?.%G!<%?$N@_Dj$O@5$7$$$+!)(B

	// unit_test[] $BAw?.%5%$%:$O@5$7$$$+!)(B
//	size_t size = 0;
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
	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );

	// unit_test[] snmpbridge get_connectionstate$B$N%F%9%H(B
	test.set_connection_state(true);
	BOOST_CHECK_EQUAL( test.get_connectionstate(), 0 );

	// unit_test[] snmpbridge get_connectionstate$B$N%F%9%H(B
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
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_START_STOP ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_RECEIVE ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_MANAGER_SEND ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_RECEIVE ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_L7VSD_SEND ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_LOGGER ), LOG_LV_NONE );
	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
	BOOST_CHECK_EQUAL( test.get_loglevel( LOG_CAT_SNMPAGENT_PARAMETER ), LOG_LV_NONE );


	// unit_test[] snmpbridge loglevel$B$N%F%9%H(B
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
	// $B@5>o(B
//	BOOST_CHECK_EQUAL( test.get_connectionstate(), -1 );
//	test.handle_accept();

	// unit_test[] handle_accept$B$,8F$P$l$?$H$-$K>uBV$,JQ$o$C$F$$$k$3$H$N3NG'(B
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
