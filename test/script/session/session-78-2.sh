#!/bin/bash
. ${SET_DEFAULT_CONF}

#Run http server
SorryServer=SorryServer
SorryServer_ADDR=[::1]
SorryServer_PORT=50001
start_lighttpd -s $SorryServer -a $SorryServer_ADDR -p $SorryServer_PORT -l ${L7VS_LOG_DIR}/session-78-2-sorry-access.log -i
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
$L7VSADM -A -t [::1]:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} --masq
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t [::1]:40001 -m sessionless -b ${SorryServer_ADDR}:${SorryServer_PORT} --masq
"
        exit 1
fi

#Connect
ifconfig lo add ::5
RET=`curl -g --retry 0 --interface ::5 http://[::1]:40001/`
if [ "${RET}" != "${SorryServer}" ]
then
        echo "curl --retry 0 --interface ::5 http://[::1]:40001/"
        exit 1
fi

#Check access logs
RET=`cat ${L7VS_LOG_DIR}/session-78-2-sorry-access.log | awk '{print $1}'`
EXPECT="::1"
if [ "$RET" != "$EXPECT" ]
then
        echo "Test failed: ${L7VS_LOG_DIR}/session-78-2-sorry-access.log"
        exit 1
fi

exit 0

