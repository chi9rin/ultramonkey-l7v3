#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#stop snmpd
${INIT_SNMPD} stop >/dev/null
if [ $? -ne 0 ]
then
	echo "Test failed: ${INIT_SNMPD} stop"
	exit 1
fi

#SNMPv3  configuration
#createUser MD5User MD5 mysecretpass DES mysecretpass
#rwuser  MD5User auth
\cp -f /etc/snmp/snmpd.conf ${TMP_DIR}/snmpd.conf.org
\cp -f ./materials/mib-58-snmpd.conf /etc/snmp/snmpd.conf

${INIT_SNMPD} start >/dev/null
if [ $? -ne 0 ]
then
	echo "Test failed: ${INIT_SNMPD} start"
        \cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
        ${INIT_SNMPD} restart >/dev/null
	exit 1
fi
usleep 1000000

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        \cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
        ${INIT_SNMPD} restart >/dev/null
        exit 1
fi
usleep 100000

RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
SorryServer_ADDR=127.0.0.1
SorryServer_PORT=50002

#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT}"
        \cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
        ${INIT_SNMPD} restart >/dev/null
        exit 1
fi



$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001 
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001
              -m sessionless -r 127.0.0.1:50001 -w 100"
        \cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
        ${INIT_SNMPD} restart >/dev/null
        exit 1
fi

$L7VSADM -S -f 1
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        \cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
        ${INIT_SNMPD} restart >/dev/null
        exit 1
fi

usleep 10000000
RET=`snmpwalk -v 3 -u MD5User -l auth -a MD5 -A mysecretpass 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7 2>&1`
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
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsSorryPort.1 = Gauge32: 50002
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
ULTRAMONKEY-L7-MIB::l7vsVsRsNumber.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsRsAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsRsPort.1 = Gauge32: 50001
ULTRAMONKEY-L7-MIB::l7vsRsForward.1 = INTEGER: masq(1)
ULTRAMONKEY-L7-MIB::l7vsRsWeight.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsActiveConn.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsInActiveConn.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpwalk -v 3 -u MD5User -l auth -a MD5 -A mysecretpass 127.0.0.1 ULTRAMONKEY-L7-MIB::ultramonkey-l7"
        \cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
        ${INIT_SNMPD} restart >/dev/null
        exit 1
fi

\cp -f ${TMP_DIR}/snmpd.conf.org /etc/snmp/snmpd.conf
${INIT_SNMPD} restart >/dev/null
exit 0
