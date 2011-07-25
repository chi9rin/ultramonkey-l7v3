#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

\cp -f materials/B-47-sslproxy.target1.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target1.cf
\cp -f materials/B-47-sslproxy.target2.cf ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target2.cf

VS="[::1]"
RS="127.0.0.1"
SS="127.0.0.1"

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

#enable snmpagent
$L7VSADM -S -f 1 -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: L7VSADM -S -f 1 -i 0"
        exit 1
fi
usleep 100000
#Add Virtual Service
RET=`$L7VSADM -A -t $VS:40001 -m sessionless -s rr -b $SS:80 -f 0 -u 5 -Q 100K -q 200K -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target1.cf`
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS:40001 -m sessionless -s rr -b $SS:80 -f 0 -u 5 -Q 100K -q 200K -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target1.cf"
        exit 1
fi

$L7VSADM -A -t $VS:40002 -m ip -s wrr -b $SS:81 -f 0 -u 10 -Q 200K -q 200K -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target2.cf
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS:40002 -m ip -s wrr -b $SS:81 -f 0 -u 10 -Q 200K -q 200K -z ${L7VSD_CONF_DIR}/sslproxy/sslproxy.target2.cf"
        exit 1
fi


#Add Real Service
$L7VSADM -a -t $VS:40001 -m sessionless -r $RS:50001 -w 10
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS:40001 -m sessionless -r $RS:50001 -w 10"
        exit 1
fi
$L7VSADM -a -t $VS:40001 -m sessionless -r $RS:50002 -w 20
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS:40001 -m sessionless -r $RS:50002 -w 20"
        exit 1
fi

$L7VSADM -a -t $VS:40002 -m ip -r $RS:50003 -w 10
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS:40002 -m ip -r $RS:50003 -w 10"
        exit 1
fi

$L7VSADM -a -t $VS:40002 -m ip -r $RS:50004 -w 20
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS:40002 -m ip -r $RS:50004 -w 20"
        exit 1
fi


#Check Return Value
RET=`snmpwalk -v 2c -c public localhost ULTRAMONKEY-L7-MIB::ultramonkey-l7 2>&1`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsNumber.0 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsIndex.2 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsVsProtocol.1 = INTEGER: tcp(1)
ULTRAMONKEY-L7-MIB::l7vsVsProtocol.2 = INTEGER: tcp(1)
ULTRAMONKEY-L7-MIB::l7vsVsAddrType.1 = INTEGER: IPv6(2)
ULTRAMONKEY-L7-MIB::l7vsVsAddrType.2 = INTEGER: IPv6(2)
ULTRAMONKEY-L7-MIB::l7vsVsAddr.1 = STRING: \"::1\"
ULTRAMONKEY-L7-MIB::l7vsVsAddr.2 = STRING: \"::1\"
ULTRAMONKEY-L7-MIB::l7vsVsPort.1 = Gauge32: 40001
ULTRAMONKEY-L7-MIB::l7vsVsPort.2 = Gauge32: 40002
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleName.1 = STRING: sessionless
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleName.2 = STRING: ip
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleOptions.1 = STRING: 
ULTRAMONKEY-L7-MIB::l7vsVsProtocolModuleOptions.2 = STRING: 
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleName.1 = STRING: rr
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleName.2 = STRING: wrr
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleOptions.1 = STRING:  
ULTRAMONKEY-L7-MIB::l7vsVsScheduleModuleOptions.2 = STRING:  
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddrType.2 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsSorryAddr.2 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsVsSorryPort.1 = Gauge32: 80
ULTRAMONKEY-L7-MIB::l7vsVsSorryPort.2 = Gauge32: 81
ULTRAMONKEY-L7-MIB::l7vsVsSorryConnLimit.1 = Counter64: 5 Connections
ULTRAMONKEY-L7-MIB::l7vsVsSorryConnLimit.2 = Counter64: 10 Connections
ULTRAMONKEY-L7-MIB::l7vsVsSorryForceEnabled.1 = INTEGER: off(2)
ULTRAMONKEY-L7-MIB::l7vsVsSorryForceEnabled.2 = INTEGER: off(2)
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUp.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUp.2 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDown.1 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDown.2 = Counter64: 0 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUpQoS.1 = Counter64: 100000 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputUpQoS.2 = Counter64: 200000 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDownQos.1 = Counter64: 200000 B/s
ULTRAMONKEY-L7-MIB::l7vsVsThroughputDownQos.2 = Counter64: 200000 B/s
ULTRAMONKEY-L7-MIB::l7vsVsSslFlag.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsSslFlag.2 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsSessionPoolCount.1 = INTEGER: 31
ULTRAMONKEY-L7-MIB::l7vsVsSessionPoolCount.2 = INTEGER: 31
ULTRAMONKEY-L7-MIB::l7vsVsSessionActiveCount.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionActiveCount.2 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.2 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.2 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpGetCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpGetCount.2 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpPostCount.1 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsHttpPostCount.2 = Counter64: 0
ULTRAMONKEY-L7-MIB::l7vsVsRsNumber.1 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsVsRsNumber.2 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsRsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsIndex.2 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsRsIndex.3 = INTEGER: 3
ULTRAMONKEY-L7-MIB::l7vsRsIndex.4 = INTEGER: 4
ULTRAMONKEY-L7-MIB::l7vsRsVsIndex.1 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsVsIndex.2 = INTEGER: 1
ULTRAMONKEY-L7-MIB::l7vsRsVsIndex.3 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsRsVsIndex.4 = INTEGER: 2
ULTRAMONKEY-L7-MIB::l7vsRsAddrType.1 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsRsAddrType.2 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsRsAddrType.3 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsRsAddrType.4 = INTEGER: IPv4(1)
ULTRAMONKEY-L7-MIB::l7vsRsAddr.1 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsRsAddr.2 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsRsAddr.3 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsRsAddr.4 = STRING: \"127.0.0.1\"
ULTRAMONKEY-L7-MIB::l7vsRsPort.1 = Gauge32: 50001
ULTRAMONKEY-L7-MIB::l7vsRsPort.2 = Gauge32: 50002
ULTRAMONKEY-L7-MIB::l7vsRsPort.3 = Gauge32: 50003
ULTRAMONKEY-L7-MIB::l7vsRsPort.4 = Gauge32: 50004
ULTRAMONKEY-L7-MIB::l7vsRsForward.1 = INTEGER: masq(1)
ULTRAMONKEY-L7-MIB::l7vsRsForward.2 = INTEGER: masq(1)
ULTRAMONKEY-L7-MIB::l7vsRsForward.3 = INTEGER: masq(1)
ULTRAMONKEY-L7-MIB::l7vsRsForward.4 = INTEGER: masq(1)
ULTRAMONKEY-L7-MIB::l7vsRsWeight.1 = INTEGER: 10
ULTRAMONKEY-L7-MIB::l7vsRsWeight.2 = INTEGER: 20
ULTRAMONKEY-L7-MIB::l7vsRsWeight.3 = INTEGER: 10
ULTRAMONKEY-L7-MIB::l7vsRsWeight.4 = INTEGER: 20
ULTRAMONKEY-L7-MIB::l7vsRsActiveConn.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsActiveConn.2 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsActiveConn.3 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsActiveConn.4 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsInActiveConn.1 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsInActiveConn.2 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsInActiveConn.3 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsRsInActiveConn.4 = INTEGER: 0
ULTRAMONKEY-L7-MIB::l7vsReplicationMode.0 = INTEGER: single(1)"


if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpwalk -v 2c -c public localhost ULTRAMONKEY-L7-MIB::ultramonkey"
        exit 1
fi

exit 0

