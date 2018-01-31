#!/bin/zsh
function updateClients(){
	Count=1;
	while (( $Count != 51 )); do
		echo $Count;
		#sudo cp /home/hdd-test-server/hdd_test.zsh /home/'test'$Count/;
		#why singlel quotes?
		sudo cp ~/HDD/scripts/hdd_test_smart.zsh /home/'test'$Count/;	#step 1
		sudo cp ~/HDD/scripts/hdd_test_smart0.zsh /home/'test'$Count/;	
		sudo cp ~/HDD/scripts/erase_start.zsh /home/'test'$Count/;	#step 2
		#sudo cp -r /usr/sbin/nwipe /home/'test'$Count/nwipe;
		sudo cp ~/HDD/bin/ewhde /home/'test'$Count/ewhde; #change to /usr/sbin?
		#sudo cp ~/HDD/DateTest /home/'test'$Count/DateTest;
		
		sudo chown 'test'$Count -R /home/'test'$Count;
		#sudo ls -a /home/'test'$Count/;
		(( Count = $Count + 1 ));
	done;
}
function createMaster(){
	Count=1;
	serverLog=/home/hdd-test-server/HDD_logs/
	while (( $Count != 51 )); do
		echo $Count;
		clientLog=/home/test$Count/logs/log.csv
		sudo cat $clientLog >> $serverLog
		sudo rm -f $clientLog
		(( Count = $Count + 1 ));
	done;		
}
updateClients
