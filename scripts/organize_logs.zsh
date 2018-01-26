#!/bin/bash
function createMaster(){
	Count=1;
	serverLog=/home/hdd-test-server/HDD_logs/
	while (( $Count != 51 )); do
		>&2 echo $Count;
		nwipeFolder=clientLog=/home/test$Count/nwipe_2018/
		clientLog=$nwipeFolder/Log
		./add_master.zsh $clientLog >> ~/nwipe_2018/master.csv
		sudo mv $clientLog $nwipeFolder/archive
		(( Count = $Count + 1 ));
	done;		
}
createMaster
