#!/bin/bash
function find_erase_between(){
	#inputs are line number of target drive(n), line number of next drive(m) and file name(dname) ex. sda
	#outputs passed if it can find lines verifying  erasure and reporting "blanked device" lines in between n, and m, otherwise outputs not passed

	dname=${1}
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
	#do variables have global scope?
	pass0=$(sed -n $sedcmd  $fname| grep "nwipe: notice: Verified that '$dname' is empty.")
	#>&2 echo pass0=$pass0
	pass1_t=$(sed -n $sedcmd $fname  | grep -n -e"nwipe: notice: Blanked device '$dname'."| sed -n 1,1p | grep -oP -e "\[.*\]"  ) #sends time of blanked device
	#>&2 echo pass1=$pass1
	#pass1_n=$(cat -n temp | sed -n ${M},${m}p|grep "nwipe: notice: Blanked device '$dname'."  | grep -oP "^\s*\d*" | grep -oP "\d*")
	if [[ -z $pass0 ]] || [[ -z $pass1_t ]]
	then 

		pass="not passed" #echo nothing
	else
		echo $pass1_t
	fi

	#echo $pass
}
function find_start_between(){
	dname=${1}
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
	E=$(sed -n $sedcmd $fname | grep "nwipe: notice: Invoking method '.*' on device '$dname'")
	echo $E
}
function find_size_between(){
	dname=${1}
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
	size=$( sed -n $sedcmd $fname | grep "nwipe: info: Device '$dname' is size" | sed -n 1,1p | grep -oP "\s\d*.$" | grep -oP "\d*")
	echo $size
}
function find_n(){
	########################parses nwipe's output log file and puts it into a more readable and searchable csv format
	#arguments are file namd ex. sda, N'th erasure and name of client logfile is from
	## intended to be piped into by cat logfile
	fname=${1}
	dname=/dev/${2} 
	N=${3}
	client=${4} 
	
	gstr="nwipe: info: Device $dname has serial number"
	starts=$(grep $fname -n -e "$gstr")
	startline=$(sed -n ${N},${N}p <<< "$starts")
	#printf "start=%s\n" "$starts" 
	#printf "\n"
	#printf "startline==%s\n" "$startline"
	A=$( grep -o "\S*\s*$" <<< "$startline" ) #some times logfile prints extra spaces at the end
	A_t=$( grep -oP -e "\[.*\]" <<< $startline) 
	A_n=$( grep -oP "^\d*" <<< "$startline")
	#gets line number of next drive in put into sda along with
	A_next_n=$(sed -n "$((N+1)),$((N+1))p" <<< "$starts" | grep -oP "^\d*" )
	E=$(find_start_between $dname $A_n $A_next_n)
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
	As=$(find_size_between $dname $A_n $A_next_n)
	#As_n=$(cat -n temp | grep "nwipe: info: Device '$dname' is size" |sed -n ${N},${N}p | grep -oP "^\s*\d*" | grep -oP "\d*" )

	#print "AS="$As
	pass_t=$(find_erase_between $dname $A_n $A_next_n)
	#echo pass_t=$pass_t/
	if [[ -z $pass_t ]]; then
		pass="not passed"
	else
		pass="passed"
	fi
	echo $client, $A_t, $pass_t, $A, $As, $pass


}

#todo change cat -n temp | grep to grep fname -n etc.
find_n ${1} ${2} ${3} ${4}
