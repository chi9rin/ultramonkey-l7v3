#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

VS1="[::1]"
RS1="127.0.0.1"

#copy config file
\cp -f materials/B-48-54-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/B-48-54-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/B-50-IPv6v4-snmptrapd.log

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 500000

#Add Virtual Service
$L7VSADM -A -t $VS1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t $VS1:40001 -m sessionless"
        exit 1
fi

#Add Real Service
$L7VSADM -a -t $VS1:40001 -m sessionless -r $RS1:50001
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t $VS1:40001 -m sessionless -r $RS1:50001"
        exit 1
fi
usleep 100000

#Check Traplog
cat ${L7VS_LOG_DIR}/B-50-IPv6v4-snmptrapd.log
RET=`cat  ${L7VS_LOG_DIR}/B-50-IPv6v4-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.2.4 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00020004,A real server was added.vs:\[::1\]:40001,rs:127.0.0.1:50001"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/B-50-IPv6v4-snmptrapd.log"
        exit 1
fi

exit 0
