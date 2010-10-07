#!/bin/bash
. ${SET_DEFAULT_CONF}
echo "supervised" > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# temp log
tmp_log="/tmp/`uuidgen`.log"

# Start l7vsd
start_l7vsd || exit 1

# Start l7directord
$L7DIRECTORD start > $tmp_log 2>&1 &

sleep 2

# Stop l7directord
stop_l7directord || exit 1

# [WRN0001] l7directord `/etc/ha.d/conf/l7directord.cf' received signal: TERM. Terminate process.
grep "WRN0001" $tmp_log >/dev/null 2>&1
if [ $? -ne 0 ]; then
	echo "Test failed: l7directord log should be written to stdout by supervised mode."
	exit 1
fi
exit 0
