#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

$L7VSADM -A -t localhost:40001 -m ip -u 100
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t localhost:40001 -m ip -u 100"
        exit 1
fi

RET=`$L7VSADM -V`
EXPECT="Layer-7 Virtual Server version 3.0.0
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
l7vsd_snmpagent                info
l7vsd_protocol                 warn
l7vsd_schedule                 warn

Replication Mode:
SINGLE

SNMPAgent:
Agent Status                   inactive
log trap status                none
log trap level                 none
cache update interval          none
start date                     none
last request date              none
last trap date                 none
total GET requests             none
total SET requests             none
total trap counts              none

Prot LocalAddress:Port ProtoMod Scheduler Protomod_opt_string
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 ip rr --timeout 3600 --no-reschedule --sorry-uri '/' --statistic 0
  Bypass Settings:
    Sorry Server                  none
    Max Connection                100
    Sorry Flag                    off
  SSL Settings:
    SSL Config File               none
  Logging Settings:
    Access Log                    off
    Access Log File               none
    Access Log Rotate             --ac-rotate-type size --ac-rotate-max-backup-index 10 --ac-rotate-max-filesize 10M
  Socket Settings:
    TCP_DEFER_ACCEPT              disable
    TCP_NODELAY                   disable
    TCP_CORK                      disable
    TCP_QUICKACK                  disable
  Throughput:
    Current Upload / Limit        0.000000 Mbps / 0.000000 Mbps
    Current Download / Limit      0.000000 Mbps / 0.000000 Mbps
  Statistics:
    HTTP Total Requests           0
    HTTP GET Requests             0
    HTTP POST Requests            0"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi

$L7VSADM -E -t localhost:40001 -m ip -u 0
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -E -t localhost:40001 -m ip -u 0"
        exit 1
fi

RET=`$L7VSADM -V`
EXPECT="Layer-7 Virtual Server version 3.0.0
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
l7vsd_snmpagent                info
l7vsd_protocol                 warn
l7vsd_schedule                 warn

Replication Mode:
SINGLE

SNMPAgent:
Agent Status                   inactive
log trap status                none
log trap level                 none
cache update interval          none
start date                     none
last request date              none
last trap date                 none
total GET requests             none
total SET requests             none
total trap counts              none

Prot LocalAddress:Port ProtoMod Scheduler Protomod_opt_string
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP localhost:40001 ip rr --timeout 3600 --no-reschedule --sorry-uri '/' --statistic 0
  Bypass Settings:
    Sorry Server                  none
    Max Connection                0
    Sorry Flag                    off
  SSL Settings:
    SSL Config File               none
  Logging Settings:
    Access Log                    off
    Access Log File               none
    Access Log Rotate             --ac-rotate-type size --ac-rotate-max-backup-index 10 --ac-rotate-max-filesize 10M
  Socket Settings:
    TCP_DEFER_ACCEPT              disable
    TCP_NODELAY                   disable
    TCP_CORK                      disable
    TCP_QUICKACK                  disable
  Throughput:
    Current Upload / Limit        0.000000 Mbps / 0.000000 Mbps
    Current Download / Limit      0.000000 Mbps / 0.000000 Mbps
  Statistics:
    HTTP Total Requests           0
    HTTP GET Requests             0
    HTTP POST Requests            0"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: $L7VSADM -V"
        exit 1
fi
exit 0

