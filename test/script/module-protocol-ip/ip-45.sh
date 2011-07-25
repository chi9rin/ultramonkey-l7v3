#!/bin/bash

. ${SET_DEFAULT_CONF}

#Add Service
$L7VSD
if [ $? -ne 0 ]
then
        echo "Test failed: $L7VSD"
        exit 1
fi

usleep 100000
RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip --sorry-uri 2>&1 | grep "PARSE ERROR"`
if [ "$RET" != "PARSE ERROR: protocol module argument error(--proto-module): You have to set option value '-S/--sorry-uri'." ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip --sorry-uri"
        exit 1
fi

RET=`$L7VSADM -A -t 127.0.0.1:40001 -m ip -S 2>&1 | grep "PARSE ERROR"`
if [ "$RET" != "PARSE ERROR: protocol module argument error(--proto-module): You have to set option value '-S/--sorry-uri'." ]
then
        echo "Test failed: $L7VSADM -A -t 127.0.0.1:40001 -m ip -S"
        exit 1
fi

exit 0

