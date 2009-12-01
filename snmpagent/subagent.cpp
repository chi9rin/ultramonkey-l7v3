#include <sstream>
#include <sys/time.h>

#include "error_code.h"
#include "parameter.h"
#include "subagent.h"
#include "netsnmp_bridge.h"

#define NIC_DEFAULT         "eth0"
#define ADDR_DEFAULT        "127.0.0.1"
#define PORT_DEFAULT        (60162)
#define INTERVAL_DEFAULT    (1000)
#define SLEEPNANOTIME       (1000000)


/*!
 * l7ag_subagent class constructor
 */
l7ag_subagent::l7ag_subagent()
{
    stop_flag   = false;
    stat        = STOP;

    dispatcher  = new l7ag_dispatcher( &msg_queue, &trap_queue );
    messenger   = new MessengerClient( &msg_queue );

    loglevelmap_init();
}

/*!
 * l7ag_subagent class destructor
 */
l7ag_subagent::~l7ag_subagent()
{
    delete  messenger;
    delete  dispatcher;
}

/*!
 * main loop
 */
bool
l7ag_subagent::start()
{
    struct timespec req;
    req.tv_sec = 0;
    req.tv_nsec = SLEEPNANOTIME;

    // initialize
    reload_config();

    // convert int to string
    std::stringstream   ss;
    std::string         port;
    ss << snmpparam.portno;
    ss >> port;

    messenger->setEndpoint( snmpparam.address.c_str(), port.c_str() );

    // start IPC thread
    messenger->start();

    // start dispatcher thread
    dispatcher->start();

    netsnmp_ds_set_boolean(NETSNMP_DS_APPLICATION_ID, NETSNMP_DS_AGENT_ROLE, 1);
    init_agent("l7snmpagent");
    init_netsnmp_bridge(messenger);
    init_snmp("l7snmpagent");

    //loop
    while(1){
        agent_check_and_process(0); // non-blocking
        if( stop_flag ){
            //他スレッドの終了指示
            messenger->stop();
            dispatcher->stop();
            break;
        }
        //Trapキューチェック
        //Trapキューにデータがある場合
        if( !dispatcher->get_TrapQueue()->empty() ){
            //Trap値設定
            struct l7_status* stat = l7ag_store_mibdata::getInstance().getStatmibdata();
            strncpy(stat->message, dispatcher->get_TrapQueue()->front().message.c_str(), DISPLAYSTRING_LEN);
            l7ag_store_mibdata::getInstance().setStatmibdata(*stat);
            //Trap送信
            send_um7MessageAlert_trap();
            dispatcher->get_TrapQueue()->pop();
        }

        int ret = nanosleep( &req, NULL );
        if ( ret == -1 ) {
            break; // nanosleep failed
        }
    }
    //finalize

    //completed
    return true;
}

/*!
 * stop main loop
 */
void
l7ag_subagent::stop()
{
    stop_flag = true;
}

void
l7ag_subagent::reload_config()
{
	l7vs::Parameter		param;
	l7vs::error_code	err;
    //パラメータの読み込み
    //NIC
	snmpparam.nic = param.get_string( l7vs::PARAM_COMP_SNMPAGENT, "nic", err );
	if( err ){
		snmpparam.nic = NIC_DEFAULT;
	}
    //EndPoint
	snmpparam.address = param.get_string( l7vs::PARAM_COMP_SNMPAGENT, "ip_addr", err );
	if( err ){
        snmpparam.address = ADDR_DEFAULT;
    }
	snmpparam.portno = param.get_int( l7vs::PARAM_COMP_SNMPAGENT, "port", err );
	if( err ){
        snmpparam.portno = PORT_DEFAULT;
    }
    //RequestInterval
	snmpparam.interval = param.get_int( l7vs::PARAM_COMP_SNMPAGENT, "interval", err );
	if( err ){
        snmpparam.interval = INTERVAL_DEFAULT;
    }
    //status
	snmpparam.status = param.get_int( l7vs::PARAM_COMP_SNMPAGENT, "status", err );
	if( err ){
        snmpparam.status = 0;
    }

    //loglevel設定
    load_loglevel();
}

void
l7ag_subagent::load_loglevel()
{
	l7vs::Parameter		param;
	l7vs::error_code	err;
	std::string			tmpstr;
    //log level 取得
    snmpparam.loglevel.clear();

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_start_stop", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_START_STOP,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_START_STOP,
			l7vs::LOG_LV_INFO) );
	}

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_manager_receive", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_MANAGER_RECEIVE,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_MANAGER_RECEIVE,
			l7vs::LOG_LV_INFO ) );
	}

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_manager_send", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_MANAGER_SEND,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_MANAGER_SEND,
			l7vs::LOG_LV_INFO ) );
	}

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_l7vsd_receive", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_L7VSD_RECEIVE,
			l7vs::LOG_LV_INFO ) );
	}

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_l7vsd_send", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_L7VSD_SEND,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_L7VSD_SEND,
			l7vs::LOG_LV_INFO ) );
    }

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_logger", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_LOGGER,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_LOGGER,
			l7vs::LOG_LV_INFO ) );
    }

	tmpstr = param.get_string( l7vs::PARAM_COMP_LOGGER, "snmpagent_parameter", err );
	if( !err ){
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_PARAMETER,
			levelstring_map[tmpstr] ) );
	}else{
		snmpparam.loglevel.insert(
			std::pair<l7vs::LOG_CATEGORY_TAG,l7vs::LOG_LEVEL_TAG>( l7vs::LOG_CAT_SNMPAGENT_PARAMETER,
			l7vs::LOG_LV_INFO ) );
    }
}

void
l7ag_subagent::loglevelmap_init(){
    //loglevelのテーブル作成
    levelstring_map.clear();
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("debug",l7vs::LOG_LV_DEBUG) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("info",l7vs::LOG_LV_INFO) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("warn",l7vs::LOG_LV_WARN) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("error",l7vs::LOG_LV_ERROR) );
    levelstring_map.insert( std::pair<std::string,l7vs::LOG_LEVEL_TAG>("fatal",l7vs::LOG_LV_FATAL) );
}
