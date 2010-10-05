#!/bin/bash
. ${SET_DEFAULT_CONF}
cat /dev/null > ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# backup l7directord
bak="/tmp/`uuidgen`.l7directord"
cp /usr/sbin/l7directord $bak

# insert Dumper code
sed -i -e "s/my \$current_service = ld_read_l7vsadm();/my \$current_service = ld_read_l7vsadm(); use Data::Dumper; ld_log(Dumper \$current_service); exit;/" $L7DIRECTORD

function cleanup {
	mv $bak /usr/sbin/l7directord
}

# Start l7vsd
$L7VSD

# sleep until l7vsd start
while :
do
	if [ -e "/var/run/l7vs/l7vs" ]
	then
		break
	fi
	usleep 10000
done

# Add various service
$L7VSADM -A -t 127.0.0.1:10000 -m sessionless -s wrr -q 100M -Q 200M -b 127.0.0.1:12345 -f 1
$L7VSADM -a -t 127.0.0.1:10000 -m sessionless -r 127.0.0.2:20000 -w 10
$L7VSADM -a -t 127.0.0.1:10000 -m sessionless -r 127.0.0.3:30000 -w 20
$L7VSADM -A -t 127.0.0.1:10001 -m sslid --maxlist 100  -s lc -q 100G -Q 200G -b 127.0.0.1:11111
$L7VSADM -A -t 127.0.0.1:20000 -m ip --forwarded-for --no-reschedule
$L7VSADM -a -t 127.0.0.1:20000 -m ip -r 127.0.0.2:20000
$L7VSADM -a -t 127.0.0.1:20000 -m ip -r 127.0.0.3:30000 -w 0
$L7VSADM -A -t [::1]:10060 -m sessionless -s wrr -q 100M -Q 200M -b 127.0.0.1:12345 -f 1
$L7VSADM -a -t [::1]:10060 -m sessionless -r [::2]:20000 -w 10
$L7VSADM -a -t [::1]:10060 -m sessionless -r [::3]:30000 -w 20
$L7VSADM -A -t [::1]:10061 -m sslid --maxlist 100  -s lc -q 100G -Q 200G -b 127.0.0.1:11111
$L7VSADM -A -t [::1]:20060 -m ip --forwarded-for --no-reschedule
$L7VSADM -a -t [::1]:20060 -m ip -r 127.0.0.1:20000
$L7VSADM -a -t [::1]:20060 -m ip -r 127.0.0.1:30000 -w 0

# Start l7directord
$INIT_L7DIRECTORD start 

# sleep until l7directord stop
while :
do
	ps aux | grep /usr/sbin/l7directord | grep -v grep
	if [ $? -ne 0 ]
	then
		break
	fi
	usleep 10000
done

# analyze result
analyze="/tmp/`uuidgen`.log"
grep -A 300 VAR /var/log/l7vs/l7directord.log | sed -e "s/^\[[^\]*\] //" > $analyze
diff $analyze ./materials/l7directord-3-2-9.log
if [ $? -ne 0 ]
then
	cleanup
        echo "Test failed: Dumper result was not matched."
	exit 1
fi

cleanup
exit 0
