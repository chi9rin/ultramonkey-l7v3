#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-5-2-25-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# backup l7directord
bak="/tmp/`uuidgen`.l7directord"
cp $L7DIRECTORD $bak

# temp log
tmp_log="/tmp/`uuidgen`.log"

# insert Dumper code
sed -i -e 's/read_config();/read_config();ld_log(Dumper \\%CONFIG); sleep 1; exit;/' $L7DIRECTORD

cleanup() {
	mv $bak $L7DIRECTORD
}

# Start l7vsd
start_l7vsd
if [ $? -ne 0 ]; then
	cleanup
	exit 1
fi

# Start l7directord
$L7DIRECTORD start > $tmp_log &

# sleep until l7directord stop
while :
do
	ps aux | grep $L7DIRECTORD | grep -v grep > /dev/null 2>&1
	if [ $? -ne 0 ]
	then
		break
	fi
	usleep 10000
done

# analyze result
analyze="/tmp/`uuidgen`.log"
grep -A 600 VAR $tmp_log | sed -e "s/^\[[^\]*\] //" > $analyze
diff_result=`diff $analyze ./materials/l7directord-5-2-25.log`
if [ $? -ne 0 ]
then
	cleanup
        echo "Test failed: Dumper result was not matched."
	echo "$diff_result"
	exit 1
fi

cleanup
exit 0
