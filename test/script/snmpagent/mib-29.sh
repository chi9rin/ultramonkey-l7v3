#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

\cp -f materials/mib-7-43-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer1"
        exit 1
fi

SorryServer=SorryServer
SorryServer_ADDR=127.0.0.1
SorryServer_PORT=50002
start_lighttpd -s $SorryServer -a $SorryServer_ADDR -p $SorryServer_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd SorryServer"
        exit 1
fi

#Start L7VSD Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

#Add Service
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless --sorry-uri /index.html -s rr -u 10000 -b ${SorryServer_ADDR}:${SorryServer_PORT} -f 0 -Q 100 -q 200 -z /etc/l7vs/sslproxy/sslproxy.target.cf 

if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless
                --sorry-uri /index.html -s rr -u 10000
                -b ${SorryServer_ADDR}:${SorryServer_PORT}
                -f 0 -Q 100 -q 200 -z /etc/l7vs/sslproxy/sslproxy.target.cf"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001 -w 100
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001
              -m sessionless -r 127.0.0.1:50001 -w 100"
        exit 1
fi


#Check Return Value
RET=`snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1`
echo "$RET"
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1 = INTEGER: 1"

if [ "${RET}" != "${EXPECT}" ]
then
        echo "Test failed: snmpget -v 2c -c public localhost ULTRAMONKEY-L7-MIB::l7vsVsSessionWaitCount.1"
        exit 1
fi

exit 0
