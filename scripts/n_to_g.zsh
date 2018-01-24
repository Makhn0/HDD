#!/bin/bash
function find_dev(){
	

	sudo cat > temp
	client= $(pwd |grep -oP "test\d{1,2}")
	N=0

	A=/dev/sda
	B=/dev/sda
	C=/dev/sda
	D=/dev/sda
	A_sn=
	A_s=
	A_pass0=
	A_pass1=
	
}
function find_nm(){
	########################parses nwipe's output log file and puts it into a more readable and searchable csv format
	N=${2}
	M=${3}
	fname=/dev/${1} 
	#echo fname=$fname
	sudo cat > temp

	client= $(pwd |grep -oP "test\d{1,2}")
	#client=$(pwd | grep -oP -e "scrip.{1,2}$")
	A=$(cat -n temp | grep "nwipe: info: Device $fname has serial number" | sed -n ${N},${N}p |grep -o "\S*$" )
	A_n=$(cat -n temp | grep "nwipe: info: Device $fname has serial number"| sed -n ${N},${N}p | grep -oP "^\s*\d*\s" | grep -oP "\d*" )
	A_next_n=$(cat -n temp | grep "nwipe: info: Device $fname has serial number"| sed -n $((N+1)),$((N+1))p | grep -oP "^\s*\d*\s" | grep -oP "\d*" )

	#pass1_n=2

	if [[ -z $A ]]
	then
		return;
	fi
	As=$(cat -n temp | grep "nwipe: info: Device '$fname' is size" | sed -n ${N},${N}p | grep -oP "\s\d*.$" | grep -oP "\d*")
	As_n=$(cat -n temp | grep "nwipe: info: Device '$fname' is size" |sed -n ${N},${N}p | grep -oP "^\s*\d*" | grep -oP "\d*" )

	pass0=$(cat temp | grep "nwipe: notice: Verified that '$fname' is empty."|sed -n ${M},${M}p)
	pass1=$(cat -n temp | grep "nwipe: notice: Blanked device '$fname'." | sed -n ${M},${M}p)
	pass1_n=$(cat -n temp | grep "nwipe: notice: Blanked device '$fname'." | sed -n ${M},${M}p | grep -oP "^\s*\d*" | grep -oP "\d*")
	#echo A_next_n=$A_next_n
	#echo pass1_n=$pass1_n
	if [[ $A_next_n ]] ;
	then 
		if (( $A_next_n < $pass1_n )) ; then
			return;
		fi
	fi
	if [[ -z $pass0 ]] || [[ -z $pass1 ]]
	then 

		pass="not passed"
	else
		pass="passed"
	fi
	echo $client , $A, $As, $pass


}

function find_n(){
	N=${2}
	M=${3}
	line=$(find_nm ${1} $N $M)
	if [[ $line ]]; then
	echo $line
	fi



}

find_n ${1} ${2} ${3}


