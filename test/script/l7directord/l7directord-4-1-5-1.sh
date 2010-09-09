#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-1-5-1-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
sleep 60

$INIT_L7DIRECTORD stop
if [ $? -ne 0 ]
then
        echo "Test failed: $INIT_L7DIRECTORD stop"
        exit 1
fi

RET=`cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[WRN3101\]" | wc -l`
if [ $RET -ne 10 ]
then
        echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
        exit 1
fi

TIME1=""
TIME2=""
for TMP in `cat ${L7VS_LOG_DIR}/l7directord.log | grep "\[WRN3101\]" | awk '{ print $4 }'`
do
	TIME2=`date -d "$TMP" +'%s'`
	if [ -n "$TIME1" ]
	then
		# retryinterval 2
		#               +
		# checktimeout  3
		#               ||
                #               5
		if [ $((TIME1+5)) -ne $((TIME2)) ]
		then
			echo "Test failed: cat ${L7VS_LOG_DIR}/l7directord.log"
			exit 1
		fi
	fi
	TIME1="$TIME2"
done

exit 0

