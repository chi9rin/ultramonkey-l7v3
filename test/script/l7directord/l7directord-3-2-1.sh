#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-3-2-1-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# create dummy l7vsadm
dummy="/tmp/`uuidgen`.dummyadm"
dummylog="/tmp/`uuidgen`.dummylog"
cat > $dummy << END
#!/bin/sh
date=\`date\`
echo "[\$date] \$0 \$@" >> $dummylog
END
chmod +x $dummy

# backup l7vsadm
bak="/tmp/`uuidgen`.l7vsadm"
mv /usr/sbin/l7vsadm $bak

# install dummy l7vsadm
cp $dummy /usr/sbin/l7vsadm
cp $dummy /sbin/l7vsadm

function cleanup {
	rm $dummy
	rm $dummylog
	mv $bak /usr/sbin/l7vsadm
}

# Start l7directord
$INIT_L7DIRECTORD start 
# stop l7directord
$INIT_L7DIRECTORD stop 

# /usr/sbin/l7vsadm should be used
grep " /usr/sbin/l7vsadm -K -n" $dummylog > /dev/null 2>&1
if [ $? -ne 0 ]
then
	cleanup
        echo "Test failed: Cannot find '/usr/sbin/l7vsadm -K -n' in the log."
        exit 1
fi

# remove dummy l7vsadm
rm /usr/sbin/l7vsadm

# Start l7directord
$INIT_L7DIRECTORD start 
# stop l7directord
$INIT_L7DIRECTORD stop 

# /sbin/l7vsadm should be used
grep " /sbin/l7vsadm -K -n" $dummylog > /dev/null 2>&1
if [ $? -ne 0 ]
then
	cleanup
        echo "Test failed: Cannot find '/sbin/l7vsadm -K -n' in the log."
        exit 1
fi

cleanup
exit 0
