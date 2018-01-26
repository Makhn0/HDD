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
function find_erase_between(){
	#inputs are line number of target drive(n), line number of next drive(m) and file name(fname) ex. sda
	#outputs passed if it can find lines verifying  erasure and reporting "blanked device" lines in between n, and m, otherwise outputs not passed

	fname=${1}
	n=${2}
	m=${3}
	if [[ -z $m ]]; then
		m='$'
	fi
	if [[ -z $n ]]; then
		n='$'
	fi
	sedcmd="${n},${m}p"
	#>&2 echo here
	#>&2 echo sedcmd=$sedcmd
	pass0=$(cat -n temp | sed -n $sedcmd | grep "nwipe: notice: Verified that '$fname' is empty.")
	#>&2 echo pass0=$pass0
	pass1=$(cat -n temp | sed -n $sedcmd | grep "nwipe: notice: Blanked device '$fname'."  )
	#>&2 echo pass1=$pass1
	#pass1_n=$(cat -n temp | sed -n ${M},${m}p|grep "nwipe: notice: Blanked device '$fname'."  | grep -oP "^\s*\d*" | grep -oP "\d*")
	if [[ -z $pass0 ]] || [[ -z $pass1 ]]
	then 

		pass="not passed"
	else
		pass="passed"
	fi

	echo $pass
}
function find_start_between(){
	fname=${1}
	n=${2}
	m=${3}
	if [[ -z $m ]]; then
		m='$'
	fi
	if [[ -z $n ]]; then
		n='$'
	fi
	sedcmd="${n},${m}p"
	#>&2 echo sedcmd=$sedcmd
	E=$(cat -n temp |sed -n $sedcmd | grep "nwipe: notice: Invoking method '.*' on device '$fname'")
	echo $E
}
function find_size_between(){
	fname=${1}
	n=${2}
	m=${3}
	if [[ -z $m ]]; then
		m='$'
	fi
	if [[ -z $n ]]; then
		n='$'
	fi
	sedcmd="${n},${m}p"
	#>&2 echo sedcmd=$sedcmd
	size=$(cat -n temp |sed -n $sedcmd | grep "nwipe: info: Device '$fname' is size" | sed -n 1,1p | grep -oP "\s\d*.$" | grep -oP "\d*")
	echo $size
}
function find_n(){
	########################parses nwipe's output log file and puts it into a more readable and searchable csv format
	#arguments are file namd ex. sda, N'th erasure
	## intended to be piped into by cat logfile
	N=${2}
	
	fname=/dev/${1} 
	#>&2 echo fname=$fname
	sudo cat > temp

	client= $(pwd |grep -oP "test\d{1,2}")
	#client=$(pwd | grep -oP -e "scrip.{1,2}$")
	#gets serial number from stdout
	A=$(cat -n temp | grep "nwipe: info: Device $fname has serial number" | sed -n ${N},${N}p |grep -o "\S*$" )
	
	
	A_n=$(cat -n temp | grep "nwipe: info: Device $fname has serial number"| sed -n ${N},${N}p | grep -oP "^\s*\d*\s" | grep -oP "\d*" )
	#gets line number of next drive in put into sda along with
	A_next_n=$(cat -n temp | grep "nwipe: info: Device $fname has serial number"| sed -n $((N+1)),$((N+1))p | grep -oP "^\s*\d*\s" | grep -oP "\d*" )
	E=$(find_start_between $fname $A_n $A_next_n)
	#>&2 echo E=$E
	#>&2 echo A=$A	
	#>&2 echo n=$A_n
	#>&2 echo m=$A_next_n

	if [[ -z $A ]] 
	then
		return; #returns meaning it outputs nill, which signals to calling function to terminate
	fi
	if [[ -z $E ]]
	then 
		echo noerase; #echo's no erase to tell calling function to move onto finding next harddrive
		return;
	fi
	#this is bad
	As=$(find_size_between $fname $A_n $A_next_n)
	#As_n=$(cat -n temp | grep "nwipe: info: Device '$fname' is size" |sed -n ${N},${N}p | grep -oP "^\s*\d*" | grep -oP "\d*" )

	#print "AS="$As
	pass=$(find_erase_between $fname $A_n $A_next_n)
	echo $client , $A, $As, $pass


}


find_n ${1} ${2}


