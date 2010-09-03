#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-70-2-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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

# Start l7vsd
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi
usleep 100000

# Start l7directord
$INIT_L7DIRECTORD start 
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD start"
        exit 1
fi
sleep 3

stop_lighttpd $RealServer1
sleep 60

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[WRN1102\]" | wc -l`
if [ $RET -ne 2 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

TIME1=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[WRN1102\]" | awk '{ print $4 }'`
do
	TIME1=`date -d "$TMP" +'%s'`
done

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0602\] Service down detected."`
if [ -z $RET ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

TIME2=$(date -d $(cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[ERR0602\]" | awk '{ print $4 }') +'%s')
if [ $TIME1 -lt $TIME2 ]
then
	echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
	exit 1
fi

exit 0

