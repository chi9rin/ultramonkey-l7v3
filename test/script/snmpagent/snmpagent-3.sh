#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/snmpagent-1-19-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/snmpagent-1-19-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

usleep 100000

#Check Traplog
cat ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log

RET=`cat  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.2.1 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00020001,A virtual service was added.vs:127.0.0.1:40001"`

if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log"
        exit 1
fi

exit 0
