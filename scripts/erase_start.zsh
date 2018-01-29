#!/bin/bash
function testcond(){
	echo "/home/test1"
}
if [[ $(testcond) =~ ^/home/test[1-5]?[0-9]$ ]]; then #minor bug: test0 passes
	#sudo ./nwipe ${1} --autonuke --nogui -l$(pwd)/nwipe_2018
	opt=${1}
	i=1
	while [ $i -le ${#opt} ]
	do
		#sudo ./nwipe ${1} --autonuke --nogui -l$(pwd)/nwipe_2018
		char=$(echo ${opt} | cut -c ${i}-${i})
		printf "%s\n" "erasing /dev/sd${char}"
		##sudo ./nwipe /dev/sd${char} --autonuke --nogui -l$(pwd)/nwipe_2018
		(( i = i+1 ))
	done
	#printf "%s\n" "in "
else
	
	printf "%s\n" "you are in $(pwd)" "Please cd to home directory i.e. /home/test#  "

fi
