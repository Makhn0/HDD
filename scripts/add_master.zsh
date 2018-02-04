#!/bin/bash
#./n_to_g.zsh ${1} 1

function cycle(){
	local name;
	if [[ ${1} == "sda" ]]; then 
		name=sdb
	elif [[ ${1} == "sdb" ]] ;then
		name=sdc
	elif [[ ${1} == "sdc" ]] ;then
		name=sdd
	elif [[ ${1} == "sdd" ]] ;then
		name=sde
	elif [[ ${1} == "sde" ]] ;then
		name=sdf
	elif [[ ${1} == "sdf" ]] ;then
		name=sdg
	elif [[ ${1} == "sdg" ]] ;then
		name=sdh
	elif [[ ${1} == "sdh" ]] ;then
		name=end
	fi	
	echo $name;
}
function number_of(){
	##takes the logfile and filename and finds the number of erasures begun with that filename
	#n=$(cat |
	#echo $n
	#echo 100
	>&2 echo inputs ${1} ${2}
	echo $(	grep ${1} -e "nwipe: info: Device /dev/${2} has serial number" | wc | grep -oP "^\s*\d*" | grep -oP "\d*")
	#	return;
}

function find_all(){
	###takes the logfile , finds all erasures from sdx=${2} ( needs to pass client name to ./find_n) and prints lines to stdout
	file=${1}
	dname=${2}
	client=${3}
	i=1
	N=$(number_of $file $dname)
	>&2 echo $dname": N="$N
	while (( $i <= $N )); do
		
		>&2 echo $dname":i="$i
		#out is csv line of Nth hd detection
		out=$(sudo ./find_n.zsh $file $dname $i $client)
		if [[ $i -gt $N ]]; then
			>&2 echo "timeout";
			break ;
		fi
		if [[ -z $out ]]; then
			>&2 echo "out empty"
			break;
		fi
		if [[ $out == noerase ]]; then
			>&2 echo noerase found;
		else
			printf "%s" "$out"
			printf "\n"
		fi
		i=$((i+1))
	done
	>&2 echo "end of find_all"
	return;
}
#echo start
function logfile_to_g(){
	#takes the nwipe logfile (${1}), and calls find_n for sd[a-h], N many times to extract all the erasures data into a readable csv file
	file=${1}
	client=$(echo $file |grep -oP "test\d{1,2}")
	dname=sda
	>&2 echo extracting data from $file
	while true; do
		#>&2 echo "finding all $name "
		out=$(find_all $file $dname $client  2>/dev/null )
		#>&2 echo out=$out
		if [[ $out ]] ; then
			#>&2 echo PRINTINGOUT
			printf "%s" "$out"
			printf "\n"
	
		fi
		name=$(cycle $dname);
		if [[ $dname == end ]]; then
			break;
		fi
	done
}
#find_all ${1} sda 2>/dev/null
logfile_to_g ${1} 

