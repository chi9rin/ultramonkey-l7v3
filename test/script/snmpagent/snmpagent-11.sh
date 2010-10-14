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
TIME=`date`
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Virtual Service
$L7VSADM -A -t 127.0.0.1:40001 -m ip -q 4M
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

#Make 10M.dat in lighttpd's default folder
dd if=/dev/zero of="$LIGHTTPD_TMP_DIR"/"$RealServer1"/10M.dat bs=1M count=10
if [ $? -ne 0 ]
then
        echo "dd if=/dev/zero of="$LIGHTTPD_TMP_DIR"/"$RealServer1"/10M.dat bs=1M count=10"

        exit 1
fi

#Get The File
$WGET http://127.0.0.1:40001/10M.dat -t 1
if [ $? -ne 0 ]
then
        echo "Test failed: $WGET http://127.0.0.1:40001/10M.dat -t 1"
	
	#Delete temporary file
	\rm -f "$LIGHTTPD_TMP_DIR"/"$RealServer1"/10M.dat 
        exit 1
fi

usleep 1000000

#Check Traplog
RET=`cat  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.2.9 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00020009,Warning: The down-throughput has exceeded the threshold of down-QoS warning."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log"

	#Delete temporary file
	\rm -f "$LIGHTTPD_TMP_DIR"/"$RealServer1"/10M.dat 
        exit 1
fi

#Delete temporary file
\rm -f "$LIGHTTPD_TMP_DIR"/"$RealServer1"/10M.dat 

exit 0
