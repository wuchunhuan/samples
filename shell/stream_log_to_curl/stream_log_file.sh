#!/bin/sh
#Author: Patrick wuchunhuan@gmail.com

tail -f /path/to/log/demo.log | grep "key words" | awk '{print $6}' | awk -F ".com" '{print $2}' | awk -F "," '{print $1}' | awk '{print "www.demo.com:1234"$0}' | while read LINE; do
        curl -s ${LINE} > /dev/null
done