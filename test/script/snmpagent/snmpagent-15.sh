#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/snmpagent-1-19-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/snmpagent-1-19-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log

#Start L7VAD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Set log level debug
$L7VSADM -L -c all -l debug
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -L -c all -l debug"
        exit 1
fi

#Set Trap Log Level debug
$L7VSADM -S -t 1 -l debug
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -S -t 1 -l debug"
        exit 1
fi

#Put debug log out
$L7VSADM -V

usleep 1000000

#Check debug log trap
RET=`cat  ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.3.5 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},VSD1.*"`
if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/snmpagent-1-19-snmptrapd.log"
        exit 1
fi

exit 0
