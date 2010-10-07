#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-3-2-6-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# create dummy l7vsadm
dummy="/tmp/`uuidgen`.dummyadm"
dummylog="/tmp/`uuidgen`.dummylog"
cat > $dummy << END
#!/bin/sh
date=\`date\`
echo "[\$date] \$0 \$@" >> $dummylog
echo "Layer-7 Virtual Server version 3.0.0
Prot LocalAddress:Port ProtoMod Scheduler
     SSL_config_file
     Socket option
     Access_log_flag
     Access_log_file
     Access_log_rotate option
  -> RemoteAddress:Port           Forward Weight ActiveConn InactConn
TCP 127.0.0.1:10000 sessionless rr
    none
    none
    0
    none
    none"
END
chmod +x $dummy

# backup l7vsadm
bak="/tmp/`uuidgen`.l7vsadm"
mv $L7VSADM $bak

# install dummy l7vsadm
cp $dummy $L7VSADM

function cleanup {
	rm $dummy
	rm $dummylog
	mv $bak $L7VSADM
}

# Start l7directord
$INIT_L7DIRECTORD start 
# stop l7directord
$INIT_L7DIRECTORD stop 

# check /usr/sbin/l7vsadm -a -t
add_line=`grep " $L7VSADM -a -t" $dummylog | wc -l`
if [ $add_line -ne 2 ]
then
	cleanup
        echo "Test failed: Cannot find '$L7VSADM -a -t' in the log."
        exit 1
fi

cleanup
exit 0
