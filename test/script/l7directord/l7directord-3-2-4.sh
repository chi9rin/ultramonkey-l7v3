#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-3-2-4-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

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
    none
TCP 127.0.0.1:10001 sessionless rr
    none
    none
    0
    none
    none"
END
chmod +x $dummy

# backup l7vsadm
bak="/tmp/`uuidgen`.l7vsadm"
mv /usr/sbin/l7vsadm $bak

# install dummy l7vsadm
cp $dummy /usr/sbin/l7vsadm

function cleanup {
	rm $dummy
	rm $dummylog
	mv $bak /usr/sbin/l7vsadm
}

# Start l7directord
$INIT_L7DIRECTORD start 
# stop l7directord
$INIT_L7DIRECTORD stop 

# check /usr/sbin/l7vsadm -E -t
edit_line=`grep " /usr/sbin/l7vsadm -E -t" $dummylog | wc -l`
if [ $edit_line -ne 2 ]
then
	cleanup
        echo "Test failed: Cannot find '/usr/sbin/l7vsadm -E -t' in the log."
        exit 1
fi

cleanup
exit 0
