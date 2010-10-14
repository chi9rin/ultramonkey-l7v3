#!/bin/bash
. ${SET_DEFAULT_CONF}

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

RealServer2=RealServer2
RealServer2_ADDR=127.0.0.1
RealServer2_PORT=50002
start_lighttpd -s $RealServer2 -a $RealServer2_ADDR -p $RealServer2_PORT

if [ $? -ne 0 ]
then
        echo "Test failed: start_lighttpd RealServer2"
        exit 1
fi



#Add Service
$L7VSD
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r 127.0.0.1:50001
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer1_ADDR}:${RealServer1_PORT}"
        exit 1
fi


$L7VSADM -A -t 127.0.0.1:40002 -m sessionless
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -A -t 127.0.0.1:40002 -m sessionless"
        exit 1
fi

$L7VSADM -a -t 127.0.0.1:40002 -m sessionless -r 127.0.0.1:50002
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -a -t 127.0.0.1:40001 -m sessionless -r ${RealServer2_ADDR}:${RealServer2_PORT}"
        exit 1
fi


#repeat 5
I=0;
for((I=0; I<50; I++)) { 
RET=`wget -t 1 -qO- http://127.0.0.1:40001/` 
echo "$RET"
if [ "${RET}" != "${RealServer1}" ]
then
	echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40001/"
	exit 1
fi 
}

for((I=0; I<50; I++)) { 
RET=`wget -t 1 -qO- http://127.0.0.1:40002/` 

if [ "$RET" != "$RealServer2" ]
then
	echo "Test failed: wget -t 1 -qO- http://127.0.0.1:40002/"
	exit 1
fi
}




#clear statistic
$L7VSADM -S -r 
if [ $? -ne 0 ]
then
	echo "Test failed: $L7VSADM -S -r -t 127.0.0.1:40001 -m sessionless"
        exit 1
fi

RET=`$L7VSADM -l -n 2>&1 | grep 127.0.0.1:50001 | awk '{print $5}'`
EXPECT="0"
echo "$RET"
if [ "${RET}" != "${EXPECT}" ]
then
    echo "Test failed: $L7VSADM -V -n 2>&1 | grep "127.0.0.1:50001" | awk '{print $5}'"
    exit 1
fi

RET=`$L7VSADM -l -n 2>&1 | grep 127.0.0.1:50002 | awk '{print $5}'`
EXPECT="0"
echo "$RET"
if [ "${RET}" != "${EXPECT}" ]
then
    echo "Test failed: $L7VSADM -V -n 2>&1 | grep "127.0.0.1:50001" | awk '{print $5}'"
    exit 1
fi




#Del IPaliasing
stop_lighttpd $RealServer1
stop_lighttpd $RealServer2

exit 0

