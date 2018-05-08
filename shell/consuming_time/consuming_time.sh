#!/bin/sh
#Author: patrick wuchunhuan@gmail.com

time_beg=`date +"%Y-%m-%d %H:%M:%S"`
sleep 2
time_end=`date +"%Y-%m-%d %H:%M:%S"`

unix_beg=`date -d  "$time_beg" +%s`
unix_end=`date -d  "$time_end" +%s`

interval=`expr $unix_end - $unix_beg`

echo "interval $interval seconds."