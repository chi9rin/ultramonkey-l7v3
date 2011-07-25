#!/bin/bash

# set config file
. ${SET_DEFAULT_CONF}

VS1="[::1]"

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

#Start L7VSD Service
$INIT_L7VSD start
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7VSD start"
        exit 1
fi
usleep 100000

#enable snmpagent
$L7VSADM -S -f 1 -i 0
if [ $? -ne 0 ]
then
        echo "Test failed: L7VSADM -S -f 1 -i 0"
        exit 1
fi
usleep 100000
#Add Virtual Service
$L7VSADM -A -t $VS1:40001 -m sessionless -F -c 1 
if [ $? -ne 0 ]
then
        echo "Test failed: L7VSADM -A -t $VS1:40001 -m sessionless -F -c 1"
        exit 1
fi

#Add Real Service
$L7VSADM -a -t $VS1:40001 -m sessionless -r ${RealServer1_ADDR}:$RealServer1_PORT
if [ $? -ne 0 ]
then
        echo "Test failed: L7VSADM -a -t $VS1:40001 -m sessionless -r ${RealServer1_ADDR}:$RealServer1_PORT"
        exit 1
fi

RET=`ifconfig lo | grep ::2`
if [ -z "${RET}" ]
then
	ifconfig lo add ::2
fi

GET_REQ="GET / HTTP/1.0\r\n\r\n"
for((I=0;I<100;I++))
{
    echo -e -n "${GET_REQ}" | nc ::1 40001 -s ::2
}

RET=`snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.1`
EXPECT="ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.1 = Counter64: 100"
echo "$RET"
if [ "${RET}" != "${EXPECT}" ]
then
        echo "snmpget -v 2c -c public 127.0.0.1 ULTRAMONKEY-L7-MIB::l7vsVsHttpTotalCount.1"
        exit 1
fi

exit 0

