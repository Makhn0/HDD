#!/bin/bash
function createMaster(){
	Count=1;
	server=/home/hdd-test-server
	outfile=$server/nwipe_2018/master.csv
	echo $outfile
	if [[ -e $outfile ]]; then
		echo outfile=$outfile
	else
		touch $outfile
	fi
	while (( $Count != 51 )); do
		>&2 echo $Count;
		client=/home/test$Count
		ndir=$client/nwipe_2018
		clientLog=$ndir/Log
		
		if [[ -e $clientLog ]]; then
			./add_master.zsh $clientLog #>> $outfile
			cat $clientLog >> $ndir/archive #move log contents to archive folder so successive calls to this function don't add the same data
			echo "" > $clientLog #erasesLog file
		else
			echo "nwipe_2018/Log file for $client"
		fi
		(( Count = $Count + 1 ));
	done;		
}

createMaster
