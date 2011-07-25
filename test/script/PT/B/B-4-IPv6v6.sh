#!/bin/bash

VS1="[::1]"
SS1="[::1]"

# set config file
. ${SET_DEFAULT_CONF}

\cp -f materials/B-4-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Virtual Service
$L7VSADM -A -t $VS1:8080 -m sessionless -s rr -u 100 -b $SS1:80 -f 0 -Q 200K -q 200K
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:8080 -m sessionless -s rr -u 100 -b 127.0.0.1:80 -f 0 -Q 200K -q 200K"
        exit 1
fi

#Check Virtual Service status
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
l7vsd_snmpagent                warn
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
TCP localhost:webcache sessionless rr --sorry-uri '/' --statistic 0
  Bypass Settings:
    Sorry Server                  localhost:http Masq
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
    TCP_QUICKACK                  auto
  Throughput:
    Current Upload / Limit        0.000000 Mbps / 0.200000 Mbps
    Current Download / Limit      0.000000 Mbps / 0.200000 Mbps
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
