#!/bin/bash

. ${SET_DEFAULT_CONF}

#Run http server
SorryServer1=SorryServer1
SorryServer1_ADDR=127.0.0.1
SorryServer1_PORT=50001
start_lighttpd -s $SorryServer1 -a $SorryServer1_ADDR -p $SorryServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer1"
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

$L7VSADM -A -t 127.0.0.1:40001 -m ip -b ${SorryServer1_ADDR}:${SorryServer1_PORT}
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -b ${SorryServer1_ADDR}:${SorryServer1_PORT}"
        exit 1
fi


RET=`$L7VSADM -V -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
L7vsd Log Level:
Category                       Level
l7vsd_network                  warn
l7vsd_network_qos              warn
l7vsd_network_bandwidth        warn
l7vsd_network_num_connection   warn
l7vsd_network_access           warn
l7vsd_mainthread               warn
l7vsd_virtualservice           warn
l7vsd_virtualservice_thread    warn
l7vsd_session                  warn
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
TCP 127.0.0.1:40001 ip rr --timeout 3600 --no-reschedule --sorry-uri '/'
    ${SorryServer1_ADDR}:${SorryServer1_PORT} 0 0
    0 0
    0 0
    none
    none
    0
    none
    --ac-rotate-type size --ac-rotate-max-backup-index 10 --ac-rotate-max-filesize 10M"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n"
        exit 1
fi

$L7VSADM -E -t 127.0.0.1:40001 -m ip -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t 127.0.0.1:40001 -m ip -f 1"
        exit 1
fi

RET=`$L7VSADM -V -n`
EXPECT="Layer-7 Virtual Server version 3.0.0-1
L7vsd Log Level:
Category                       Level
l7vsd_network                  warn
l7vsd_network_qos              warn
l7vsd_network_bandwidth        warn
l7vsd_network_num_connection   warn
l7vsd_network_access           warn
l7vsd_mainthread               warn
l7vsd_virtualservice           warn
l7vsd_virtualservice_thread    warn
l7vsd_session                  warn
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
TCP 127.0.0.1:40001 ip rr --timeout 3600 --no-reschedule --sorry-uri '/'
    ${SorryServer1_ADDR}:${SorryServer1_PORT} 0 1
    0 0
    0 0
    none
    none
    0
    none
    --ac-rotate-type size --ac-rotate-max-backup-index 10 --ac-rotate-max-filesize 10M"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V -n"
        exit 1
fi

exit 0

