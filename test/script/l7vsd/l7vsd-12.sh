#!/bin/bash

# set config file
cp materials/l7vsd-12-l7vs.cf ${L7VSD_CONF_DIR}/l7vs.cf

# max open files will be 1024 * 1024
maxfileno=`expr 1024 \* 1024 + 1`

# get default open files
open_files=`ulimit -n`

# set maxfileno
sed -i -e "s/__MAXFILENO__/$maxfileno/" ${L7VSD_CONF_DIR}/l7vs.cf

# start l7vsd
start_l7vsd || exit 1

# check max open files
soft_limit=`grep "Max open files" /proc/\`pidof l7vsd\`/limits | awk '{print $4}'`
if [ $soft_limit -ne $open_files ]; then
        echo "Test failed: Max open files(Soft Limit) must be $open_files, but $soft_limit"
        exit 1
fi

hard_limit=`grep "Max open files" /proc/\`pidof l7vsd\`/limits | awk '{print $5}'`
if [ $hard_limit -ne $open_files ]; then
        echo "Test failed: Max open files(Hard Limit) must be $open_files, but $hard_limit"
        exit 1
fi

exit 0
