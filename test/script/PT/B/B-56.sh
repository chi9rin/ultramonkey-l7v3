#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/B-56-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/B-56-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/B-56-snmptrapd.log

#check service snmpd is running
RET=`pgrep snmpd`
if [ -z "${RET}" ]
then
    echo "Test failed: snmp is not running"
    exit 1
fi


#Start L7VAD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000

#Set Trap Log Level
$L7VSADM -S -t 1 -l warn
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -t 1 -l warn"
        exit 1
fi

#Add virtual Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi
#Add virtual Service Again
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless

#need time to write log file
usleep 1000000


#Check Trap Log
RET=`cat  ${L7VS_LOG_DIR}/B-56-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.3.3 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},VSD3.*"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/B-56-snmptrapd.log"
        exit 1
fi

#Check MIB
RET=`snmpwalk -v 2c -c public localhost  ULTRAMONKEY-L7-MIB::ultramonkey-l7`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsProtocol.1 = INTEGER: tcp(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsPort.1 = Gauge32: 40001
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleName.1 = STRING: sessionless
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleOptions.1 = STRING: 
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleName.1 = STRING: rr
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleOptions.1 = STRING:  
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddr.1 = STRING: \"0.0.0.0\"
ULTRAMONKEY-L7-MIB::l7vsVsSorryPort.1 = Gauge32: 0
ULTRAMONKEY-L7-MIB::l7vsVsSorryConnLimit.1 = Counter64: 0 Connections
ULTRAMONKEY-L7-MIB::l7vsVsSorryForceEnabled.1 = INTEGER: off(2)
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUp.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDown.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUpQoS.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDownQos.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsSslFlag.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionPoolCount.1 = INTEGER: 31
ULTRAMONKEY-L7-MIB::l7vsVsSessionActiveCount.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpGetCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpPostCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsRsNumber.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"

if [ "${RET}" != "${EXPECT}" ]
then
	echo "Test failed: snmpwalk -v 2c -c public localhost  ULTRAMONKEY-L7-MIB::ultramonkey-l7"
	exit 1
fi

#restart snmpd
${INIT_SNMPD} stop
sleep 2
${INIT_SNMPD} start
sleep 2

#Add virtual Service Again
#warnning log trap 
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
sleep 1

RET=`cat ${L7VS_LOG_DIR}/B-56-snmptrapd.log | grep -e "SNMPv2-SMI::enterprises.32132.1.0.3.3 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},VSD3.*" | wc -l`
echo "TEST:: $RET"
if [ "${RET}" != "2" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/B-56-snmptrapd.log"
        exit 1
fi

sleep 20

#Check MIB
RET=`snmpwalk -v 2c -c public localhost  ULTRAMONKEY-L7-MIB::ultramonkey-l7`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsProtocol.1 = INTEGER: tcp(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsPort.1 = Gauge32: 40001
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleName.1 = STRING: sessionless
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleOptions.1 = STRING: 
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleName.1 = STRING: rr
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleOptions.1 = STRING:  
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddr.1 = STRING: \"0.0.0.0\"
ULTRAMONKEY-L7-MIB::l7vsVsSorryPort.1 = Gauge32: 0
ULTRAMONKEY-L7-MIB::l7vsVsSorryConnLimit.1 = Counter64: 0 Connections
ULTRAMONKEY-L7-MIB::l7vsVsSorryForceEnabled.1 = INTEGER: off(2)
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUp.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDown.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUpQoS.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDownQos.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsSslFlag.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionPoolCount.1 = INTEGER: 31
ULTRAMONKEY-L7-MIB::l7vsVsSessionActiveCount.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpGetCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpPostCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsRsNumber.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"

echo "$EXPECT" > /tmp/1.txt
echo "$RET" > /tmp/2.txt
diff /tmp/1.txt /tmp/2.txt

if [ "${RET}" != "${EXPECT}" ]
then
	echo "Test failed: snmpwalk -v 2c -c public localhost  ULTRAMONKEY-L7-MIB::ultramonkey-l7"
	exit 1
fi



exit 0
