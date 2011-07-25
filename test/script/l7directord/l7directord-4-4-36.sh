#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-36-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Run server (wait 1 sec. SMTP)
PATH=$PATH:$COMMON_SCRIPT_DIR
test_server -p 20000 -w 1 ./materials/l7directord-4-4-36-response1.txt ./materials/l7directord-4-4-36-response2.txt ./materials/l7directord-4-4-36-response3.txt &

# Start l7vsd
start_l7vsd
if [ $? -ne 0 ]; then
	pkill test_server
        exit 1
fi

# Start l7directord
start_l7directord
if [ $? -ne 0 ]; then
	pkill test_server
        exit 1
fi

# wait health check
sleep 5

# [WRN0204] Service check OK. Successfully connect SMTP server.
grep "WRN0204" ${L7VS_LOG_DIR}/l7directord.log > /dev/null 2>&1
if [ $? -ne 0 ]; then
	pkill test_server
        exit 1
fi

# Stop l7directord
stop_l7directord || exit 1

exit 0
