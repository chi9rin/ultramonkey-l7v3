#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/snmpagent-1-19-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/snmpagent-1-19-snmptrapd.conf -Lf  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 500000

#Stop L7VSD Service
kill -SIGHUP `ps x | grep l7vsd | grep -v grep | awk '{print $1}'`

if [ $? -ne 0 ]
then
        echo "Test failed: kill -SIGHUP `ps x | grep l7vsd | grep -v grep | awk '{print $1}'`"
        exit 1
fi

usleep 2000000

#check L7VSD Stoped
RET=`ps x | grep l7vsd | grep -v grep | awk '{print $1}'`
if [ -n "${RET}" ]
then
        echo "Test failed:  ps x | grep l7vsd | grep -v grep | awk '{print \$1}'"
        exit 1
fi

#Check Traplog
RET=`cat  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.1.2 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00010002,l7vsd stop."`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log"
        exit 1
fi

exit 0
