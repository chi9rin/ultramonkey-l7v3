#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

#copy config file
\cp -f materials/B-48-54-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Start snmptrapd
${SNMPTRAPD} -c ./materials/B-48-54-snmptrapd.conf -Lf ${L7VS_LOG_DIR}/B-48-snmptrapd.log
if [ $? -ne 0 ]
then
        echo "Test failed: ${SNMPTRAPD} "
        exit 1
fi


#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 2000000

#Check Traplog
RET=`ps -e | grep l7vsd`
if [ -z "$RET" ]
then
        echo "Test failed: ps -e | grep l7vsd"
        exit 1
fi
RET=`cat  ${L7VS_LOG_DIR}/B-48-snmptrapd.log | grep -e  "SNMPv2-SMI::enterprises.32132.1.0.1.1 = STRING: \"[0-9]\{4\}/[0-9]\{2\}/[0-9]\{2\} [0-9]\{2\}:[0-9]\{2\}:[0-9]\{2\},TRAP00010001,l7vsd start."`

if [ -z "${RET}" ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/B-48-snmptrapd.log"
        exit 1
fi

exit 0
