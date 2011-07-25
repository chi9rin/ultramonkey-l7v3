#!/bin/bash

. ${SET_DEFAULT_CONF}

# temp log
tmp_log="/tmp/`uuidgen`.log"

for sig in SIGHUP SIGINT SIGQUIT SIGUSR1 SIGUSR2 SIGPIPE SIGTERM; do
	flock -n -x $L7VSADM $L7VSADM >> $tmp_log 2>&1 &

	# sleep a little
	usleep 20000

	# send signal
	kill -$sig `pidof $L7VSADM`

	# wait for l7vsadm termination
	while true; do
		ps aux | grep $L7VSADM | grep -v grep > /dev/null 2>&1 || break
	done
done

diff_result=`diff $tmp_log ./materials/l7vsadm-12-1-1.log`
if [ $? -ne 0 ]; then
	rm $tmp_log
	echo "Test failed: l7vsadm signal log was not matched."
	echo "$diff_result"
	exit 1
fi

rm $tmp_log
exit 0

