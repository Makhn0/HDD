#!/bin/bash
function createMaster(){
	#retrieves all of the log data from all the clients' /nwipe_2018/log files, formats them with ./add_master redirects output to master.csv
	server=/home/hdd-test-server
	outfile=$server/nwipe_2018/master.csv
	>&2 echo outfile=$outfile
	if [[ -e $outfile ]]; then
		>&2 echo "outfile already exists"
	else
		>&2 echo "creating outputfile at $outfile"
		touch $outfile
	fi
	Count=1;
	while (( $Count != 51 )); do
		>&2 echo $Count;
		client=/home/test$Count
		ndir=$client/nwipe_2018
		clientLog=$ndir/Log
		
		if [[ -e $clientLog ]]; then
			sudo ./extract_log.zsh $clientLog >> $outfile
			#test above code before uncommenting below
			cat $clientLog >> $ndir/archive #move log contents to archive folder so successive calls to this function don't add the same data
			echo "" > $clientLog #erasesLog file
		else
			>&2 echo "no nwipe_2018/Log file for $client"
		fi
		(( Count = $Count + 1 ));
	done;	
	>&2 printf "%s\n" "finished creating $outfile"
}

createMaster 