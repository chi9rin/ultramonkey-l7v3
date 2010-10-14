#!/bin/bash

. ${SET_DEFAULT_CONF}
\cp -f materials/virtualservice-59-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

# Start l7vsd
start_l7vsd || exit 1

$L7VSADM -A -t 127.0.0.1:40001 -m sessionless
if [ $? -ne 0 ]; then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m sessionless -u 1 -b 127.0.0.1:30000"
        exit 1
fi

sleep 3

# Stop l7vsd
stop_l7vsd || exit 1

timing=`grep handle_throughput_update /var/log/l7vs/l7vsd.log | grep in_function | awk '{print $2}'`
if [ `echo "$timing" | wc -l` -ne 3 ]; then
	echo "Test failed: throughput calc interval may be not 1 sec."
	echo "$timing"
	exit 1
fi

exit 0
