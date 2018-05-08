#!/bin/sh
#Author: Patrick wuchunhuan@gmail.com
#1. logs are rotated hourly as follows:
# demo.log.2018010101 demo.log.2018010102 demo.log.2018010103
#2. Only reserve ${reserve_days} days logs and remove other old logs
#3. Add this script to crontab task : 0 1 * * * /path/to/this/script/log_recycle.sh 7 >/dev/null 2>&1

log_path=.
log_file_prefix=demo
reserve_days=7
if [ $# -ge 1 ]
then
    reserve_days=$1
fi

cd ${log_path}
#1.move yesterday's logs to a folder named by date
yesterday=`date +"%Y%m%d" -d "-1 days"`
if [ ! -d ${yesterday} ]
then
	mkdir ${yesterday}
fi

logs_to_move=`ls | grep "${log_file_prefix}.${yesterday}"`
for i in ${logs_to_move}
do
	mv ${i} ${yesterday}
done

#2.recycle old folder and reserve $reserve_days days log folders
all_log_folder=`ls -F | grep '/$' | awk -F "/" '{print $1}' | grep "[[:digit:]]" | sort`
folder_num=`echo "$all_log_folder" | wc -l`
if [ ${folder_num} -gt ${reserve_days} ]
then
	rm_num=`expr ${folder_num} - ${reserve_days}`
	rm_folder=`echo "$all_log_folder" | sed -n 1,"${rm_num}"p`
	for i in ${rm_folder}
	do
		rm -rf ${i}
	done
fi
