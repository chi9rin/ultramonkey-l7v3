#!/bin/bashgg

# set config file
. ${SET_DEFAULT_CONF}
\cp -f materials/session-52-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf
\cp -f materials/session-52-sslproxy.target.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target.cf"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi


RET=`l7vsadm -V -n`
EXPECT="Layer-7 Virtual Server version 3.0.0
L7vsd Log Level:
Category                       Level
l7vsd_network                  warn
l7vsd_network_qos              warn
l7vsd_network_bandwidth        warn
l7vsd_network_num_connection   warn
l7vsd_network_access           warn
l7vsd_mainthread               warn
l7vsd_virtualservice           debug
l7vsd_virtualservice_thread    warn
l7vsd_session                  debug
l7vsd_session_thread           warn
l7vsd_realserver               warn
l7vsd_sorryserver              warn
l7vsd_module                   warn
l7vsd_replication              warn
l7vsd_replication_sendthread   warn
l7vsd_parameter                warn
l7vsd_logger                   warn
l7vsd_command                  warn
l7vsd_start_stop               warn
l7vsd_system                   warn
l7vsd_system_memory            warn
l7vsd_system_endpoint          warn
l7vsd_system_signal            warn
l7vsd_system_environment       warn
l7vsd_snmpbridge               warn
l7vsd_protocol                 warn
l7vsd_schedule                 warn

Replication Mode:
SINGLE

SNMPAgent Connection Status:
non-connecting

SNMPAgent Log Level:
Category                       Level
snmpagent_start_stop           warn
snmpagent_manager_receive      warn
snmpagent_manager_send         warn
snmpagent_l7vsd_receive        warn
snmpagent_l7vsd_send           warn
snmpagent_logger               warn
snmpagent_parameter            warn
snmpagent_system               warn
snmpagent_system_memory        warn
snmpagent_system_endpoint      warn
snmpagent_system_signal        warn
snmpagent_system_environment   warn

Prot LocalAddress:Port ProtoMod Scheduler Protomod_opt_string
     SorryAddress:Port Sorry_cc Sorry_flag
     QoS-up   Throughput-up
     QoS-down Throughput-down
     SSL_config_file
     Socket option
     Access_log_flag
     Access_log_file
     Access_log_rotate option
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:40001 sessionless rr --sorry-uri '/'
    none 0 0
    0 0
    0 0
    /etc/l7vs/sslproxy/sslproxy.target.cf
    none
    0
    none
    --ac-rotate-type size --ac-rotate-max-backup-index 10 --ac-rotate-max-filesize 10M
  -> 127.0.0.1:50001              Masq    1      0          0         "
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: l7vsadm -V -n"
        exit 1
fi

RET=`wget -qO- https://127.0.0.1:40001/ --no-check-certificate`
EXPECT="${RealServer1}"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: wget -qO- https://127.0.0.1:40001/ --no-check-certificate"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | grep "\[DEBUG\] l7vsd_virtualservice VSD10700105 out_function: bool virtualservice_tcp::get_ssl_parameter():" | grep "private_key_dir = /etc/l7vs/sslproxy/, private_key_file = server.pem, private_key_filetype = 1"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7vsd.log | egrep "\[DEBUG\] l7vsd_session VSD10900070 Thread ID\[.*\] ssl session handshaking start: set handshake timer 30sec"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7vsd.log"
        exit 1
fi

exit 0

