#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/B-48-54-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

VS1="[::1]"


#Start snmptrapd
${SNMPTRAPD} -c ./materials/B-48-54-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/B-52-IPv6v6-snmptrapd.log

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=[::1]
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -i
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
$L7VSADM -A -t $VS1:40001 -m ip -q 4M
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:40001 -m ip"
        exit 1
fi

#Add Real Service
$L7VSADM -a -t $VS1:40001 -m ip -r $RealServer1_ADDR:$RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:40001 -m ip -r $RealServer1_ADDR:$RealServer1_PORT"
        exit 1
fi

#Make 10M.dat in lighttpd's default folder
dd if=/dev/zero of="$LIGHTTPD_TMP_DIR"/"$RealServer1"/10M.dat bs=1M count=10
if [ $? -ne 0 ]
then
        echo "dd if=/dev/zero of=$LIGHTTPD_TMP_DIR/$RealServer1/10M.dat bs=1M count=10"
        exit 1
fi

#Get The File
$WGET -q -O /dev/null http://$VS1:40001/10M.dat -t 1
if [ $? -ne 0 ]
then
        echo "Test failed: $WGET http://$VS1:40001/10M.dat -t 1"
        exit 1
fi

usleep 5000000

#Check Traplog
RET=`cat  ${L7VS_LOG_DIR}/B-52-IPv6v6-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.2.9 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00020009,Warning: The down-throughput has exceeded the threshold of down-QoS warning."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/B-52-IPv6v6-snmptrapd.log"
        exit 1
fi

exit 0
