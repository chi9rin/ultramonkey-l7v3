#!/bin/bash
. ${SET_DEFAULT_CONF}
\cp ./materials/l7directord-1-2-47-l7directord.cf ${L7DIRECTORD_CONF_DIR}/l7directord.cf

# this config file has 89 errors
error_count=89

for i in `seq 1 $error_count`
do
	# l7directord config file syntax check
	RET1=`$L7DIRECTORD configtest 2>&1`
	if [ $? -eq 0 ]
	then
	        echo "Test failed: $L7DIRECTORD configtest - no more error($i)"
	        exit 1
	fi
	RET2=`$L7DIRECTORD -t 2>&1`
	if [ $? -eq 0 ]
	then
	        echo "Test failed: $L7DIRECTORD -t - no more error($i)"
	        exit 1
	fi
	RET3=`$INIT_L7DIRECTORD configtest 2>&1`
	
	# all result should be same.
	if [ "$RET1" != "$RET2" -o "$RET1" != "$RET3" ]
	then
		echo "Test failed: configtest result was not matched."
		exit 1
	fi
	
	# check the error line number
	error_line=`echo $RET1 | sed -e "s/.*at line \([0-9]*\):.*/\1/"`
	if [ "$error_line" = "$RET1" ]
	then
		echo "Test failed: Unknown error occurred - $RET1"
		exit 1
	fi

	# delete syntax error line
	sed -i -e ${error_line}d ${L7DIRECTORD_CONF_DIR}/l7directord.cf
done

# l7directord config file syntax check
RET1=`$L7DIRECTORD configtest 2>&1`
if [ $? -eq 1 ]
then
        echo "Test failed: $L7DIRECTORD configtest should be no error."
        exit 1
fi
RET2=`$L7DIRECTORD -t 2>&1`
if [ $? -eq 1 ]
then
        echo "Test failed: $L7DIRECTORD -t should be no error."
        exit 1
fi
RET3=`$INIT_L7DIRECTORD configtest 2>&1`
if [ "$RET1" != "$RET2" -o "$RET1" != "$RET3" ]
then
	echo "Test failed: configtest result was not matched."
	exit 1
fi
	
EXPECT="Syntax OK"
if [ "$RET1" != "$EXPECT" ]
then
        echo "Test failed: $L7DIRECTORD configtest should be Syntax OK."
        exit 1
fi

exit 0
