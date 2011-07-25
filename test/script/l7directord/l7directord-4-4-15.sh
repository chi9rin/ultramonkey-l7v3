#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-15-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Run server (sleep 0 sec. 200 OK)
PATH=$PATH:$COMMON_SCRIPT_DIR
test_server -p 20000 ./materials/l7directord-4-4-15-response.txt &

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
sleep 2

# [WRN0203] Service check OK. HTTP response is valid. HTTP response status line is `200 OK'
grep "WRN0203.*200 OK" ${L7VS_LOG_DIR}/l7directord.log > /dev/null 2>&1
if [ $? -ne 0 ]; then
	pkill test_server
        exit 1
fi

# Run server (sleep 5 sec. 200 OK)
test_server -p 20000 -s 5 ./materials/l7directord-4-4-15-response.txt &

sleep 5

# [WRN1102] Service check NG. HTTP response is not ok. Response status line is ...
grep "WRN1102" ${L7VS_LOG_DIR}/l7directord.log > /dev/null 2>&1
if [ $? -ne 0 ]; then
	pkill test_server
        exit 1
fi

# Stop l7directord
stop_l7directord || exit 1

exit 0
