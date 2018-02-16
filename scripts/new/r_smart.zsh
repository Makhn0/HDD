#!/bin/zsh
###########################
#Documentation conventions#
###########################

#Above every array declaration are some lines describing the variables and their various codes. 

#Above every major function may be a line "#Needs work(#)".
#The numbers in the parens express the priority / state the function is in.
#(1) Very minor bugs, or some funcionality expansion could be done cause doing moar things can be better.
#(2) Minor bug that doesn't appear to break things or optimization that can be done, but we shouldn't just leave them as is because that's lazy.
#(3) Major bug(s) or some functionality yet to be written. (probably cause the "programmer" isn't so great at this)  :P
#(4) Large portions of the function are not yet written.

#The ideas and/or action(s) needed are briefly described below the "#Needs work(#)" line.

#Each function conatins a brief description of the function.

####################################################
#Set up some global variables, locks, and what not.#
####################################################

#Lock codes are respectively binary (0 / 1) for (not locked / locked).
typeset -A Locks

#Support codes are respectively binary (0 / 1) for (not supported / supported).
typeset -A SmartSupport

#Presence codes are respectively binary (0 / 1) for (not present / present).
typeset -A Presence

#Logged codes are respectively binary (0 / 1) for (not logged / logged).
typeset -A DriveLoggedOut 

#Data collected codes are respectively binary (0 / 1) for (not collected / collected).
typeset -A DriveDataCollected

#SmartKill codes are (0 not killed) (1 killed) (2 some error).
typeset -A SmartKill 

#Holds the shred Pid # so we can check if its still running.
typeset -A ShredPid

#Holds the Smartctl Pid # so we can check if its still running.
typeset -A SmartctlScsiPid

#queue codes are:
#(0 not connected) (19 finished, passed) (20 finished, failed / error)
#(1 queued for smart test) (2 smart test in progress) (3 smart test complete) 
#(4 queued for dd write test) (5 dd write test in progress) (6 dd write test complete) 
#(7 queued for dd read test) (8 dd read test in progress) (9 dd read test complete) 
#(10 queued for hash compare) (11 hash compare in progress ) (12 hash compare complete)
#(13 queued for 0 out) (14 0 out running) (15 0 out complete)
#(16 queued for formatting) (17 formatting running) (18 formatting complete)
typeset -A Queue
typeset -A QueueStateString
QueueStateString=( 0 "Not Connected / Scheduled" 1 "Queued for Smart test" 2 "Smart test in progress" 3 "Smart test complete" 4 "Queued for dd write" 5 "dd write in progress" 6 "dd write complete" 7 "Queued for dd read" 8 "dd read in progress" 9 "dd read complete" 10 "Queued for Hash compare" 11 "Hash compare in progress" 12 "Hash compare complete" 13 "Queued for 0 out" 14 "0 out running" 15 "0 out complete" 16 "Queued for formatting" 17 "formatting running" 18 "formatting complete" 19 "Finished, passed" 20 "Finished, failed / error" )

#error codes are set: (0 no error) (1 smart testing error) (2 dd write error) (3 dd read error) (4 0 out error) (5 hash compare error) (6 formatting error) (7 logic broke and there are more than one errors)
typeset -A Error
typeset -A ErrorString
ErrorString=( 0 "No errors" 1 "Smart testing error" 2 "dd write error" 3 "dd read error" 4 "0 out error" 5 "Hash compare error" 6 "Formatting error" 7 "Logic broken, seek mediation" )

#Holds the serial, model, and manufacturer so we can make sure we don't test the same drive twice in a row, cause that'd just be silly.
typeset -A SerialNumber
typeset -A Model 
typeset -A ModelFamily
typeset -A UserCapacity

#Holds the percent completion of the current smartctl test.
typeset -A SmartPercentComplete
typeset -A SmartPercentLeft

#Array to hold the HDD names to test
typeset -a HDDs
typeset -A StartTime
typeset -A RunTime

#Gets the batch name and HDD type from args from the command line.
typeset Batch=$1
typeset HDDType=$2
typeset ShredPasses=$3

###########################################################################################
#Get some functions built so we can do things somewhat reliably and not duplicate efforts.#
###########################################################################################

#Initialize variables for drive $1.
function reinitialize_variables(){
	Locks+=(${1} 0);
	SmartSupport+=(${1} 0);
	Presence+=(${1} 0);
	DriveLoggedOut+=(${1} 0);
	DriveDataCollected+=(${1} 0);
	SmartKill+=(${1} 0); 
	ShredPid+=(${1} 0);
	SmartctlScsiPid+=(${1} 0);
	Queue+=(${1} 0);
	Error+=(${1} 0);
	SerialNumber+=(${1} "Unknown");
	Model+=(${1} "Unknown");
	ModelFamily+=(${1} "Unknown");
	UserCapacity+=(${1} 0);
	SmartPercentComplete+=(${1} 0);
	SmartPercentLeft+=(${1} 0);
	StartTime+=(${1} 0);
	RunTime+=(${1} 0);
}
#function list
#check_hdd_type
#fill_smartctl
#smartctl_watched
#check_smartctl_test_status
#find_hdd
#update_run_time
#test_report_log_out
#status_report_stdout
#partition disk   
#zero_out
#check_shred
#dd_write_file
#get_smartctl_test_percentage
#get_hdd_data
#queue_job_control
#main

#Needs Work(2)
#May need to update for ide drives in the future

#This kills the smartctl deer
function kill_smartctl(){
	SmartKill+=(${1} 0);
	sudo smartctl -X /dev/${1} 1>/dev/null;
	local SmartExitStatus=$?;
	if (( ${SmartExitStatus} == 0 )); then
		SmartKill+=(${1} 1);
	else
		SmartKill+=(${1} 0);
	fi
}

#Invoke smartctl on $1
function smartctl_watched(){
	
		sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long /dev/$1 1>/dev/null;
		SmartKill+=(${1} 0);
		Locks+=(${1} 1);
		Queue+=(${1} 2);
	
	
}

#Checks to see if smartctl is still running the scheduled test.
function check_smartctl_test_status(){
	local TestStatus="$(sudo smartctl -a /dev/${1} | awk '/Self-test execution status:/' | awk '{print substr($0,41,37)}')";
	local LogStatus="$(sudo smartctl -a /dev/${1} | awk '/No self-tests have been logged./ {print substr($0,0,32)}')";
	if [[ ${TestStatus} == *"elf-test routine in progress.."* ]]; then
		Queue+=(${1} 2);
	elif [[ ${TestStatus} == *"he previous self-test routine compl"* ]] && [[ ${LogStatus} != "No self-tests have been logged." ]]; then
		Queue+=(${1} 3);
	else
		Queue+=(${1} 20);
		if (( $Error[$1] == 0 )); then
			Error+=(${1} 1);
		else
			Error+=(${1} 7);
		fi
	fi
}

#Checks if the disk is plugged in, and updates Presence[$1] accordingly.
function find_hdd(){
	
		if [[ -e /dev/${1} ]] && (( $Queue[$1] < 2 )); then
				local SmartctlPresence="$(sudo smartctl -a /dev/${1} | awk '/Smartctl open device:/' | awk '{print substr($0,23,15)}')";	
				local SmartctlReadable="$(sudo smartctl -a /dev/${1} | awk '/Read SMART Data/' | awk '{print substr($0,17,6)}')";
				if [[ $SmartctlPresence != "failed" ]] && [[ $SmartctlReadable != "failed" ]]; then
					Presence+=(${1} 1);
				fi
		elif [[ -e /dev/${1} ]] && (( $Queue[$1] >= 2)); then
		else
		    reinitialize_variables $1;
		fi
	
}

#Updates the runtime clock of the disks
function update_run_time(){
	local DateTime="$(date +%s)";
	local RunTimeSeconds;
	for Disk in $HDDs; do
		if (( $Presence[$Disk] == 1 )); then
			(( RunTimeSeconds = $DateTime - $StartTime[$Disk] ));
			(( RunTimeMinutes = $RunTimeSeconds / 60 ));
			RunTime+=($Disk $RunTimeMinutes);
		else
		fi
	done;
}

#Parse "sudo smartctl -l selftest /dev/$1" and "sudo smartctl -H /dev/$1" and return pass / fail, model number, serial number, etc to a log file.
function test_report_log_out(){
    local DateFinished="$(date +%Y/%m/%d)";
	#local SerialCheck="$(sudo cat ~/${Batch}_HDD.log | grep $SerialNumber[$1])";
	if (( $DriveLoggedOut[$1] == 0 )) && (( $Queue[$1] >= 19 )); then
			print "Testing Client:" $USER >> ~/${Batch}_HDD.log;
			print "Model Family:" $ModelFamily[$1] >> ~/${Batch}_HDD.log;
			print "Model:" $Model[$1] >> ~/${Batch}_HDD.log;
			print "Serial #:" $SerialNumber[$1] >> ~/${Batch}_HDD.log;
			print "User Capacity:" $UserCapacity[$1] "GB" >> ~/${Batch}_HDD.log;
			print "Test and format results:" $QueueStateString[$Queue[$1]] >> ~/${Batch}_HDD.log;
			print "Date testing completed:" $DateFinished >> ~/${Batch}_HDD.log;
			if (( $Error[$1] != 0 )); then
				print "Errors:" $ErrorString[$Error[$1]] >> ~/${Batch}_HDD.log;
				print "" >> ~/${Batch}_HDD.log
				print $USER,$ModelFamily[$1],$Model[$1],$SerialNumber[$1],$UserCapacity[$1],$QueueStateString[$Queue[$1]],$ErrorString[$Error[$1]],$DateFinished >> ~/${Batch}_HDD.csv;
			else
				print "Errors: N/A" >> ~/${Batch}_HDD.log
				print "" >> ~/${Batch}_HDD.log
				print $USER,$ModelFamily[$1],$Model[$1],$SerialNumber[$1],$UserCapacity[$1],$QueueStateString[$Queue[$1]] >> ~/${Batch}_HDD.csv;
			fi
			DriveLoggedOut+=(${1} 1);
	fi
}

function fail_log_out(){
	
	#consistent with other new log out
	#echo $client, $A_t, $pass_t, $A, $As, $pass
	if [[ $(pwd) =~ ^/home/test[1-5]?[0-9]$ ]]; then #minor bug: test0 passes
		client=$(pwd | grep -oP -e "[^/]*$")
		Dir=$(pwd)/smart_2018
		logfile=$Dir/Log
		if [[ -e $Dir ]]; then
			#>&2 printf "logfile exists continuing....\n"
		else 
			#>&2 printf "no Log Folder creating $Dir now...\n "
			sudo mkdir $Dir
		fi
		if [[ -e $logfile ]]; then
			#>&2 printf "logfile exists continuing... \n"
		else
			#>&2 printf "no logfile creating $logfile now...\n"
			sudo touch $logfile	
		fi	
smrtLog="~/smart_2018/Log"
		if (( $Error[$1] != 0 )); then
			echo "$client,, $SerialNumber[$1],$UserCapacity[$1], failed smartctl" >> $logfile
		fi
	fi
}
#Keeps the user informed of the states in which various tests are in regards to $1 by clearing the screen then printing to stdout.
function status_report_stdout(){
	#print "in here"
	for Disk in $HDDs; do
		#print "Disk=$Disk"
		if (( $Presence[$Disk] == 1 )); then
			print "Status of:" ${Disk};
			print "Model Family:" $ModelFamily[$Disk];
			print "Model:" $Model[$Disk];
			print "Serial #:" $SerialNumber[$Disk];
			print "User Capacity:" $UserCapacity[$Disk]"GB";
			print "Present Task:" $QueueStateString[$Queue[$Disk]];
			print "Run Time:" $RunTime[$Disk] "Minutes";
			print ""
		fi
	done;
	#print "going out"
}

#Makes a partition label, partition, and fat32 filesystem on the disk $1.
function partition_disk(){
	sudo parted -s -a optimal /dev/${1} mklabel msdos -- mkpart primary fat32 1 -1;
	local PartedExit=$?;
	sudo mkfs.msdos -F 32 /dev/${1}1;
	local MkfsExit=$?;
	if (( ${PartedExit} == 0 )) && (( ${MkfsExit} == 0 )); then
		Queue+=(${1} 18);
	else
		Queue+=(${1} 20);
		if (( $Error[$1] == 0 )); then
			Error+=(${1} 6);
		else
			Error+=(${1} 7);
		fi
	fi
}

#Does a shred write of zeros to the disk $1.
function zero_out(){
	
        if [[ $ShredPasses == '' ]]; then
            ShredPasses='1';
        fi
		sudo shred -n $ShredPasses /dev/${1} 1>/dev/null &;
        local ShredPidInt=0;
        local ShredPidSet=0;
        local ShredPsCheck=1;
        local ShredPidPs="sudo shred -n ${ShredPasses} /dev/${1}"
        local ShredPidCheck=""
        while [[ $ShredPidSet == 0 ]]; do
            if [[ $ShredPasses < 10 ]]; then 
                ShredPidCheck="$(ps aux | grep "sudo shred -n" | sed -n ${ShredPsCheck},${ShredPsCheck}p | awk '{print substr($0,66,24)}')";
            elif [[ $ShredPasses > 9 ]] && [[ $ShredPasses <100 ]]; then
                ShredPidCheck="$(ps aux | grep "sudo shred -n" | sed -n ${ShredPsCheck},${ShredPsCheck}p | awk '{print substr($0,66,25)}')";
            else
                ShredPidCheck="$(ps aux | grep "sudo shred -n" | sed -n ${ShredPsCheck},${ShredPsCheck}p | awk '{print substr($0,66,26)}')";
            fi
            if [[ $ShredPidCheck == $ShredPidPs ]]; then
                if (( $ShredPasses < 10 )); then 
                    ShredPidInt="$(ps aux | grep "sudo shred -n" | sed -n ${ShredPsCheck},${ShredPsCheck}p | awk '{print substr($0,10,6)}' | grep  -o '[0-9]\+')";
                elif (( $ShredPasses > 9 )) && (( $ShredPasses <100 )); then
                    ShredPidInt="$(ps aux | grep "sudo shred -n" | sed -n ${ShredPsCheck},${ShredPsCheck}p | awk '{print substr($0,10,7)}' | grep  -o '[0-9]\+')";
                else
                    ShredPidInt="$(ps aux | grep "sudo shred -n" | sed -n ${ShredPsCheck},${ShredPsCheck}p | awk '{print substr($0,10,8)}' | grep  -o '[0-9]\+')";
                fi
				ShredPid+=(${1} $ShredPidInt);
                ShredPidSet=1;
            else
                (( ShredPsCheck = $ShredPsCheck + 1 ));
            fi
        done;
 
}

#This checks whether the shred zero out is still running.
function check_shred(){
	
        	if (( $ShredPasses < 10 )); then
	      		local ShredPidPs="$(sudo ps $ShredPid[$1] | grep "sudo shred -n" | awk '{print substr($0,28,24)}')";
        	elif (( $ShredPasses > 9 )) && (( $ShredPasses < 100 )); then
	        	local ShredPidPs="$(sudo ps $ShredPid[$1] | grep "sudo shred -n" | awk '{print substr($0,28,25)}')";
	    	else 
            		local ShredPidPs="$(sudo ps $ShredPid[$1] | grep "sudo shred -n" | awk '{print substr($0,28,26)}')";
	    	fi

        if [[ ${ShredPidPs} != "sudo shred -n ${ShredPasses} /dev/${1}" ]] && [[ ${DDPidPs} != "dd if=/dev/zero of=/dev/" ]]; then 
		Queue+=(${1} 15);
        else
	fi
}

#Does a dd write of a file to $1 with a known hash that we can go back to try to read and check the hash against.
function dd_write_file(){
	if ls ~/testtmp/${1}_File.dd 1>/dev/null 2>&1; then
		exec"$(sudo rm /tmp/${1}_File.dd 1>/dev/null))";
	fi
	sudo dd if=/dev/urandom of=/tmp/${1}_File.dd count=100KB 1>/dev/null;
	sudo dd if=/tmp/${1}_File.dd of=/dev/${1} count=100KB 1>/dev/null;
	local DDExitStatus=$?;
	if (( ${DDExitStatus} == 0 )); then
		Queue+=(${1} 6);
	else
		Queue+=(${1} 20);
		if (( $Error[$1] == 0 )); then
			Error+=(${1} 2);
		else
			Error+=(${1} 7);
		fi
	fi
}

#Does a dd read from disk $1 of a file with a known hash so that we can compare.
function dd_read_file(){
	sudo dd if=/dev/${1} of=/tmp/${1}_FileRead.dd count=100KB 1>/dev/null;
	local DDExitStatus=$?;
	if (( ${DDExitStatus} == 0 )); then
		Queue+=(${1} 9);
	else
		Queue+=(${1} 20);
		if (( $Error[$1] == 0 )); then
			Error+=(${1} 3);
		else
			Error+=(${1} 7);
		fi
	fi
}

#Does a hash comparison between the dd_write and dd_read files.
function hash_compare_file(){
	local MainFileHash="$(md5sum /tmp/${1}_File.dd | awk '{print substr($0,0,32)}')"
	local ReadFileHash="$(md5sum /tmp/${1}_FileRead.dd | awk '{print substr($0,0,32)}')"
	sudo rm /tmp/${1}_FileRead.dd
	sudo rm /tmp/${1}_File.dd
	if [[ ${MainFileHash} == ${ReadFileHash} ]]; then
		Queue+=($1 12);
	else
		Queue+=(${1} 20);
		if (( $Error[$1] == 0 )); then
			Error+=(${1} 5);
		else
			Error+=(${1} 7);
		fi
	fi
}

#Grabs the percentage left then turns it into percentage complete.
function get_smartctl_test_percentage(){
	local PercentLeft="$(sudo smartctl -a /dev/${1} | awk '/% of test remaining./' | awk '{print substr($0,5,3)}' | awk -F\	 '{ print $NF}')";
	local PercentComplete=$(( 100 - ${PercentLeft} ));
	SmartPercentComplete+=(${1} ${PercentComplete});
	SmartPercentLeft="$(sudo smartctl -a /dev/${1} | awk '/% of test remaining./' | awk '{print substr($0,3,6)}')";
}
#checks that the relevant SMART variables are all zero
function bb_test(){
	#local count=0; for more or less
	local   bb_5="$(sudo smartctl -A /dev/${1} | awk '/5 Reallocated_Sector_Ct/' | awk '{print substr($0,85,6)}')";
	local bb_187="$(sudo smartctl -A /dev/${1} | awk '/187 Reported_Uncorrect/' | awk '{print substr($0,85,6)}')";
	local bb_188="$(sudo smartctl -A /dev/${1} | awk '/188 Command_Timeout/'| awk '{ print substr($0,85,6)}')";
	local bb_197="$(sudo smartctl -A /dev/${1} | awk '/197 Current_Pending_Sector/'| awk '{ print substr($0,85,6)}')";
	local bb_198="$(sudo smartctl -A /dev/${1} | awk '/198 Offline_Uncorrectable/' | awk '{ print substr($0,85,6)}')";
	#bb_5="500";
	if ((bb_5==0))&&((bb_187==0))&&((bb_188==0))&&((bb_197==0))&&((bb_198==0)) ;then
		#pass
		#echo $bb_5;
		#echo $bb_187;
		#echo $bb_188;
		#echo $bb_197;
		#echo $bb_198;
		#echo pass;
		Queue+=(${1} 5);
		dd_write_file $1;
	else
		#fail
		Queue+=(${1} 20);
		#echo fail;
	fi
}
#This gets the data for $SmartSupport, $ModelFamily, $Model, $SerialNumber.
function get_hdd_data(){
	if (( $DriveDataCollected[$1] == 0 )) && (( $Presence[$1] == 1 )) && (( $Queue[$1] != 14 )) ; then
		
			local smart_i="$(sudo smartctl -i /dev/${1})"
			local SmartSupportRaw="$( awk '/SMART support is:/' <<< $smart_i | awk '{print substr($0,19,9)}; NR == 1 {exit}')";
			local ModelFamilyRaw="$( awk '/Model Family:/' <<< $smart_i | awk '{print substr($0,19,35)}')";
			local ModelRaw="$(awk '/Device Model:/' <<< $smart_i | awk '{print substr($0,19,35)}')";
			local SerialNumberRaw="$( awk '/Serial Number:/' <<< $smart_i | awk '{print substr($0,19,35)}')";
			local UC="$( awk '/User Capacity:/' <<< $smart_i | awk -F"[" '{print $2}' ) "
			local TB="$( grep -o TB <<< $UC)"
			local UserCapacityGB="$( grep -o '[0-9]\+' <<< $UC | head -n 1)";
			if [[ $TB ]]; then
				UserCapacityGB+="000"
			fi
			local DateTime="$(date +%s)";
			StartTime+=(${1} $DateTime);
		
		if [[ $ModelFamilyRaw != '' ]]; then
			ModelFamily+=(${1} ${ModelFamilyRaw});
		fi
		if [[ $ModelRaw != '' ]]; then
			Model+=(${1} ${ModelRaw});
		fi
		if [[ $SerialNumberRaw != '' ]]; then
			SerialNumber+=(${1} ${SerialNumberRaw});
		fi
		if [[ $UserCapacityGB != '' ]]; then
			UserCapacity+=(${1} ${UserCapacityGB});
		fi
		if [[ ${SmartSupportRaw} == "Available" ]]; then
			SmartSupport+=(${1} 1);
		fi
		DriveDataCollected+=(${1} 1);
	fi
}

#Tells the machine what to do base on where the drive is in the Queue scheduling.
function queue_job_control(){
	if (( $Queue[$1] == 0 )); then
		if (( $Presence[$1] == 1 )); then
			Queue+=(${1} 1);
		fi
	elif (( $Queue[$1] == 1 )); then
		#1 Queued for smart testing
		smartctl_watched $1;
	elif (( $Queue[$1] == 2 )); then
		#2 Smart test in progress
		#We don't want to perform any action except to update the percentage completion which smartctl is at.
		check_smartctl_test_status $1;	
	elif (( $Queue[$1] == 3 )); then
		#3 Smart testing complete
		#check_smartctl_status has updated the queue to here or the error queue code, so we must pass it along the line.
	
		Queue+=(${1} 4);
	elif (( $Queue[$1] == 4 )); then
		#4 Queued for dd write
		#dd_write will update the queue when it's complete, so until then we will just set the queue to 5.
		bb_test $1;
		#E bb_test sets queue to 5 and calls dd_write_file, if it passes, sets queue to 21 otherwise
		
	elif (( $Queue[$1] == 5 )); then
		#5 dd write in progress
		#This is just a placeholder so that we don't do anything at this time.
	elif (( $Queue[$1] == 6 )); then
		#6 dd write complete
		#dd_write has updated the queue to here or the error queue code, so we must pass it along the line.
		Queue+=(${1} 7);
	elif (( $Queue[$1] == 7 )); then
		#7 Queued for dd read
		#dd_read will update the queue when it's complete, so until then we will just set the queue to 8.
		Queue+=(${1} 8);
		dd_read_file $1;
	elif (( $Queue[$1] == 8 )); then
		#8 dd read in progress
		#This is just a placeholder so that we don't do anything at this time.
	elif (( $Queue[$1] == 9 )); then
		#9 dd read complete
		#dd_read has updated to queue to here or the error queue code, so we must pass it along the line.
		Queue+=(${1} 10);
	elif (( $Queue[$1] == 10 )); then
		#10 Queued for Hash compare
		#hash_compare will update the queue when it's complete, so until then we will just set the queue to 11.
		Queue+=(${1} 11);
		hash_compare_file $1;
	elif (( $Queue[$1] == 11 )); then
		#11 Hash compare in progress
		#This is just a placeholder so that we don't do anything at this time.
	elif (( $Queue[$1] == 12 )); then
		#12 Hash compare complete
		#hash_compare has updated the queue to here or the error queue code, so we must pass it along the line.
		#E
		Queue+=(${1} 19);
	elif (( $Queue[$1] == 13 )); then
		#13 Queued for 0 out
		#zero_out will update the queue when it's complete, so until then we will just set the queue to 14.
		Queue+=(${1} 14);
		zero_out $1;
	elif (( $Queue[$1] == 14 )); then
		#14 0 out running
		check_shred $1;
	elif (( $Queue[$1] == 15 )); then
		#15 0 out complete
		#zero_out has updated to queue to here or the error queue code, so we must pass it along the line.
		Queue+=(${1} 16);
	elif (( $Queue[$1] == 16 )); then
		#16 Queued for formatting
		#partition_disk will update the queue when it's complete, so until then we will just set the queue to 17.
		Queue+=(${1} 17);
		partition_disk $1;
	elif (( $Queue[$1] == 17 )); then
		#17 formatting running
		#This is just a placeholder so that we don't do anything at this time.
	elif (( $Queue[$1] == 18 )); then
		#18 formatting complete
		#partition_disk has updated to queue to here or the error queue code, so we must pass it along the line.
		Queue+=(${1} 19);
	elif (( $Queue[$1] == 19 )); then
		#19 Finished, passed
		#find_hdd with reinitialize_variables will reset things if the disk is unplugged.
		test_report_log_out $1;
	elif (( $Queue[$1] == 20 )); then
		#20 Finished, failed / error
		#find_hdd with reinitialize_variables will reset things if the disk is unplugged.
		test_report_log_out $1;
		fail_log_out $1;
	else
		print "Error with Queue code for:" $Disk;
		print "How did we even get here?";
	fi
}

#This is the main algorithm that keeps everything running as it should.
main(){
	clear;
	HDDs=( "sda" "sdb" "sdc" "sdd" "sde" "sdf" "sdg" "sdh" "sdi" "sdj" "sdk" "sdl" "sdm" "sdn" "sdo" "sdp" )
	if [[ ${Batch} != '' ]]; then
		touch ~/${Batch}_HDD.log;
		touch ~/${Batch}_HDD.csv;
	fi
	for Disk in $HDDs; do
		reinitialize_variables $Disk;
	done;
	while true; do
		for Disk in $HDDs; do
			find_hdd $Disk;
			#print " Disk=$Disk";
                	if (( $Presence[$Disk] == 1 )) && (( $DriveLoggedOut[$Disk] == 0 )) && (( $DriveDataCollected[$Disk] == 1 )); then
				queue_job_control $Disk;
			elif (( $Presence[$Disk] == 1 )) && (( $DriveLoggedOut[$Disk] == 0 )) && (( $DriveDataCollected[$Disk] == 0 )); then
				get_hdd_data $Disk;
				queue_job_control $Disk;
			elif (( $Presence[$Disk] == 1 )) && (( $DriveLoggedOut[$Disk] == 1 )); then
			else
				reinitialize_variables $Disk;
			fi
		done;
		clear;
		update_run_time;
		print "Batch Name: " $Batch;
		#print "this is printing"
		#print ""
		status_report_stdout;
		#print " this is printing"
		sleep 1;		
	done;
}
function test_fail(){
reinitialize_variables $1;
find_hdd $1;
get_hdd_data $1;
#queue_job_control $Disk;
status_report_stdout;
Error[${1}]=1;
#echo Error=$Error[$1];
fail_log_out $1;
}
main $1 ;

