#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-1-3-2-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

#Run http server
RealServer1=RealServer1
RealServer1_ADDR=127.0.0.1
RealServer1_PORT=50001
start_lighttpd -s $RealServer1 -a $RealServer1_ADDR -p $RealServer1_PORT -l ${L7VS_LOG_DIR}/l7directord-4-1-3-2-access.log

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
sleep 5
 
del_lighttpd_content $RealServer1 index.html
sleep 30

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord-4-1-3-2-access.log`
if [ -z "$RET" ]
then        echo "Test failed: ${L7VS_LOG_DIR}/l7directord-4-1-3-2-access.log"
        exit 1
fi

TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord-4-1-3-2-access.log | grep " 404 " | awk '{ print $4 }' | sed 's/:/ /' | awk '{ print $2}'`
do
        TIME2=`date -d "$TMP" +'%s'`
        if [ -n "$TIME1" ]
        then
                if [ $((TIME1+3)) -gt $((TIME2)) ]
                then                        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord-4-1-3-2-access.log"
                        exit 1
                fi
        fi
        TIME1="$TIME2"
done

exit 0

