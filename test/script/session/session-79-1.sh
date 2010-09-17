#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
SorryServer=SorryServer
SorryServer_ADDR=127.0.0.1
SorryServer_PORT=50001
start_lighttpd -s $SorryServer -a $SorryServer_ADDR -p $SorryServer_PORT -l ${L7VS_LOG_DIR}/session-79-sorry-access.log
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer"
        exit 1
fi

# Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Add service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} -T
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} -T
"
        exit 1
fi

#Connect
RET=`curl --retry 0 --interface 127.0.0.5 http://127.0.0.1:40001/`
if [ "${RET}" != "${SorryServer}" ]
then
        echo "curl --retry 0 --interface 127.0.0.5 http://127.0.0.1:40001/"
        exit 1
fi

#Check access logs
RET=`cat ${L7VS_LOG_DIR}/session-79-sorry-access.log | awk '{print $1}'`
EXPECT="127.0.0.5"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/session-79-sorry-access.log"
        exit 1
fi

exit 0

