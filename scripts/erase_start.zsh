#!/bin/bash
function testcond(){
	echo "/home/test1"
}
function callNwipe(){
	#erases /dev/sdx (given by opt=${1}) with args=${e} to logfile ${2}
	opt=${1}

	logfile=${2}
	args=${3}
	args="$args \
		--method=zero"
	i=1
	Narg=""
	while [ $i -le ${#opt} ]
	do
		char=$(echo ${opt} | cut -c ${i}-${i})
		Narg=$Narg" /dev/sd"$char
		(( i = i+1 ))
	done
	printf "erasing  %s\n" $Narg
	printf "logFile: %s\n" $logfile
	if [[ $args ]]; then
		printf "with args %s ...\n" $args
	fi
	printf "executing: sudo ./nwipe $args -l$logfile $Narg"
	sudo ./nwipe $args -l$logfile $Narg
	printf "finished\n"
}
function time(){
	date
	sudo ntpdate 192.168.1.1
	date
}
function erase_main(){
	time
	if [[ $(pwd) =~ ^/home/test[1-5]?[0-9]$ ]]; then #minor bug: test0 passes
		#sudo ./nwipe ${1} --autonuke --nogui -l$(pwd)/nwipe_2018./
		drives=${1}
		args=${2}
		>&2 echo drives=$drives
		>&2 echo args=$args
		Dir=$(pwd)/nwipe_2018
		logfile=$Dir/Log
		if [[ -e $Dir ]]; then
			printf "logfile exists continuing....\n"
		else 
			printf "no Log Folder creating $Dir now...\n "
			sudo mkdir $Dir
		fi
		if [[ -e $logfile ]]; then
			printf "logfile exists continuing... \n"
		else
			printf "no logfile creating nwipe_2018 now...\n"
			sudo touch $logfile	
		fi
		callNwipe $drives $logfile $args
	else
	
		printf "%s\n" "you are in $(pwd)" "Please cd to home directory i.e. /home/test#  "

	fi
}
if [[ ${1} == '-a' ]]; then
	printf "detects -a"
	erase_main ${2} "--autonuke"
else
	erase_main ${1} ${2}  #sdx, optional args
fi
