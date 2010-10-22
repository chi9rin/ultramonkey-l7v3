#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/B-48-54-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

VS1="127.0.0.1"
RS1="127.0.0.1"

#Start snmptrapd
${SNMPTRAPD} -c ./materials/B-48-54-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/B-53-IPv4v4-snmptrapd.log


#nc server
#Run nc server
nc -i 1 -l 50001 > ${TMP_DIR}/nc_tmp &
NC_PID=$!

#Start L7VAD Service
TIME=`date`
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add virtual service
$L7VSADM -A -t $VS1:40001 -m ip
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:40001 -m ip"
        exit 1
fi

#Add real service
$L7VSADM -a -t $VS1:40001 -m ip -r $RS1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:40001 -m ip -r $RS1:50001"
        exit 1
fi

#Make 28 active connection
for ((i=0;i<28;i++)){ nc 127.0.0.1 40001 -w 5& }
wait $NC_PID

usleep 5000000
#Check traplog
RET=`cat  ${L7VS_LOG_DIR}/B-53-IPv4v4-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.2.11 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00020011,Warning: The left-session has fell below the threshold of left-session warning."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/B-53-IPv4v4-snmptrapd.log"
        exit 1
fi

exit 0
