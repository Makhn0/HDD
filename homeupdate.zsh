#!/bin/zsh
Count=1;
while (( $Count != 51 )); do
	echo $Count;
	#sudo cp /home/hdd-test-server/hdd_test.zsh /home/'test'$Count/;


	sudo cp ~/HDD/hdd_test_smart.zsh /home/'test'$Count/;	
	sudo cp ~/HDD/hdd_test_smart0.zsh /home/'test'$Count/;	
	sudo cp -r /usr/sbin/nwipe /home/'test'$Count/nwipe;
	sudo cp ~/HDD/ewhde /home/'test'$Count/ewhde;
	sudo cp ~/HDD/DateTest /home/'test'$Count/DateTest;
	
	sudo chown 'test'$Count -R /home/'test'$Count;

	sudo ls -a /home/'test'$Count/;
	sudo cat /home/'test'$Count/dates.csv >> ~/dates.csv
	(( Count = $Count + 1 ));
done;
