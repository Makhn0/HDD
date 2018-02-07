#!/bin/bash
server=/home/hdd-test-server
ndir_s=$server/nwipe_2018
outfile=$ndir_s/master.csv
function print_noargs(){
	printf "tell me where to pull from \n"	
	}
function extract_from(){
	#takes client number and extracts /home/test#/nwipe_2018/Log to master
	Count=${1}
	>&2 echo $Count;
		client=/home/test$Count
		ndir_c=$client/nwipe_2018
		clientLog=$ndir_c/Log
		
		if [[ -e $clientLog ]]; then
			sudo ./extract_log.zsh $clientLog >> $outfile
			#test above code before uncommenting below
			cat $clientLog >> $ndir_c/archive #move log contents to archive folder so successive calls to this function don't add the same data
			echo "" > $clientLog #erasesLog file
		else
			>&2 echo "no nwipe_2018/Log file for $client"
		fi
	
}
function createMaster_all(){
	#retrieves all of the log data from all the clients' /nwipe_2018/log files, formats them with ./add_master redirects output to master.csv
	Count=1;
	while (( $Count != 51 )); do
		extract_from $Count
		Count=$(($Count+1))
	done;	
	
}
function createMaster(){
	>&2 echo outfile=$outfile
	if [[ -e $ndir_s ]]; then
		>&2 echo "~/nwipe_2018 exists"
	else
		>&2 echo "creating ~/nwipe_2018 folder"
		sudo mkdir $ndir_s
	fi
	if [[ -e $outfile ]]; then
		>&2 echo "outfile already exists"
	else
		>&2 echo "creating outputfile at $outfile"
		sudo touch $outfile
	fi
	if [[ ${1} == '--all' ]]; then
		#todo add "are u sure" method
		sudo createMaster_all
	
	elif [[ ${1} == '' ]]; then
		print_noargs
	else
		extract_from ${1}
	fi
	>&2 printf "%s\n" "finished updating $outfile"
	
}
createMaster ${1}

##add to .bashrc .zshrc/alias.zsh later find way to load on clients
#the idea is to have standard commands for testing erasing etc. 
##whose implementation we can change w/o changing
#name of command
#hdir=/home/hdd-test-server/HDD/scripts
#alias starttest=$hdir/oldscripts/hdd_test_smart0.zsh
#alias erase=$hdir/erase_start.zsh
#alias pullLogs=$hdir/pull_logs.zsh
#alias update=$hdir/oldscripts/homeupdate.zsh
