#!/bin/bash
master=/home/hdd-test-server/nwipe_2018/master.csv
if [[ ! -e $master ]]; then
	printf " file $master does not exist\n"

else
	#>&2 echo ${1}
	grep $master -e "${1}" \
		--color
fi
