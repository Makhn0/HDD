#!/bin/bash
function testcond(){
	echo "/home/test1"
}
if [[ $(pwd) =~ ^/home/test[1-5]?[0-9]$ ]]; then #minor bug: test0 passes
	#sudo ./nwipe ${1} --autonuke --nogui -l$(pwd)/nwipe_2018./
	opt=${1}
	i=1
	Narg=""
	logfile=$(pwd)/nwipe_2018/Log
	while [ $i -le ${#opt} ]
	do
		
		char=$(echo ${opt} | cut -c ${i}-${i})
		Narg=$Narg" /dev/sd"$char
		(( i = i+1 ))
	done
	printf "erasing  %s\n" $Narg
	printf "logFile: %s\n" $logfile

	#sudo ./nwipe -l$logfile $Narg
	printf "finished\n"
	#printf "%s\n" "in "
else
	
	printf "%s\n" "you are in $(pwd)" "Please cd to home directory i.e. /home/test#  "

fi
