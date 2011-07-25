#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-1-4-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# logging uuid
uuid=`uuidgen`
logger -p mail.info $uuid

# log file
logfile="/var/log/maillog"

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 1

RET=`grep -A 100 $uuid $logfile | grep l7directord | grep -v ": \[" | wc -l`
if [ $RET -ne 0 ]
then
        echo "Test failed: $logfile has weird l7directord log."
        exit 1
fi

RET=`grep l7directord /var/log/messages | wc -l`
if [ $RET -eq 0 ]
then
        echo "Test failed: $logfile does not have l7directord log."
        exit 1
fi

exit 0
