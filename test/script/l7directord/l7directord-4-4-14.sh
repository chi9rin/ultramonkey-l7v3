#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-14-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Run server (200 OK)
PATH=$PATH:$COMMON_SCRIPT_DIR
test_server -p 20000 ./materials/l7directord-4-4-14-response1.txt &

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
        echo "Test failed: Cannot find '200 OK' health check log"
        exit 1
fi

# Start server (404 Not Found)
test_server -p 20000 ./materials/l7directord-4-4-14-response2.txt &

# wait health check
sleep 3

# [WRN1102] Service check NG. HTTP response is not ok. Response status line is `404 Not Found'
grep "WRN1102.*404 Not Found" ${L7VS_LOG_DIR}/l7directord.log > /dev/null 2>&1
if [ $? -ne 0 ]; then
	pkill test_server
        echo "Test failed: Cannot find '404 Not Found' health check log"
        exit 1
fi

# Start server (500 Internal Server Error)
test_server -p 20000 ./materials/l7directord-4-4-14-response3.txt &

# wait health check
sleep 3

# [WRN1102] Service check NG. HTTP response is not ok. Response status line is `500 Internal Server Error'
grep "WRN1102.*500 Internal Server Error" ${L7VS_LOG_DIR}/l7directord.log > /dev/null 2>&1
if [ $? -ne 0 ]; then
	pkill test_server
        echo "Test failed: Cannot find '500 Internal Server Error' health check log"
        exit 1
fi

# Stop l7directord
stop_l7directord || exit 1

exit 0
