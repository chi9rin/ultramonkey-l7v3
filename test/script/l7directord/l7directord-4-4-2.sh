#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-4-4-2-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# Start l7vsd
start_l7vsd || exit 1

# Start l7directord
start_l7directord || exit 1

# sleep for health check...
sleep 5

# Stop l7directord
stop_l7directord || exit 1

#[Wed Oct  6 14:38:41 2010|28124] [INF0402] Create health check process with pid: 30859.
start_line=`grep "INF0402" ${L7VS_LOG_DIR}/l7directord.log`
if [ -n $start_line ]; then
        echo "Test failed: Cannot find process creation message in the log."
        exit 1
fi

#[Wed Oct  6 14:38:44 2010|28197] [WRN3201] Service check NG. Cannot connect socket to server.
timeout_line=`grep "WRN3201" ${L7VS_LOG_DIR}/l7directord.log`
if [ -n $timeout_line ]; then
        echo "Test failed: Cannot find health check timeout message in the log."
        exit 1
fi

start_time=`echo $start_line | sed -e "s/^.* ..:..:\(..\).*/\1/" | xargs expr 1 \* `
timeout_time=`echo $timeout_line | sed -e "s/^.* ..:..:\(..\).*/\1/" | xargs expr 1 \* `

[ $start_time -gt $timeout_time ] && timeout_time=`expr $timeout_time + 60`

# this diff time will be same as checktimeout value.
diff_time=`expr $timeout_time - $start_time`
if [ $diff_time -ne 3 -a $diff_time -ne 4 ]; then
        echo "Test failed: Timeout must occurred in 3 (or 4) sec, but it passed $diff_time sec this time."
        exit 1
fi

exit 0
