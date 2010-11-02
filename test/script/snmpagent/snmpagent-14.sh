#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/snmpagent-1-19-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/snmpagent-1-19-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log

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

#Start L7VAD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Virtual Service
$L7VSADM -A -t 127.0.0.1:40001 -m ip -Q 50M
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip"
        exit 1
fi

#Add Real Service
$L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m ip -r 127.0.0.1:50001"
        exit 1
fi

#Built link
#Make 28 active connection
for ((i=0;i<28;i++)){ nc 127.0.0.1 40001 -w 5& }

$TEST_CLIENT 127.0.0.1 40001 &
CONNECT1=$!
$TEST_CLIENT 127.0.0.1 40001 &
CONNECT2=$!
$TEST_CLIENT 127.0.0.1 40001 &
CONNECT3=$!
$TEST_CLIENT 127.0.0.1 40001 &
CONNECT4=$!
$TEST_CLIENT 127.0.0.1 40001 &
CONNECT5=$!
$TEST_CLIENT 127.0.0.1 40001 &

sleep 5

kill ${CONNECT1}
kill ${CONNECT2}
kill ${CONNECT3}
kill ${CONNECT4}
kill ${CONNECT5}

sleep 5

#Check Traplog
cat  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log

RET=`cat  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.2.12 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00020012,Warning release: The left-session has exceeded the release threshold of left-session warning."`

if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log"
        exit 1
fi

exit 0
