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
		name=end
	fi	
	echo $name;
}
#echo start
function master_stdout(){
	a=begin
	N=1
	name=sda
	#echo name=$name
	while true; do
		#echo "finding $name "
		while true; do
			a=$(cat ${1} | ./n_to_g.zsh $name $N);
			
			N=$((N+1))
			#echo N=$Ncat
			
			if [[ $N == 10 ]]; then
				#echo "timeout";
				break ;
			fi
			if [[ -z $a ]]; then
				#echo "a empty"
				break;
			fi
			echo $a
			#echo "end"
		done

		name=$(cycle $name);
		N=1
		#echo name=$name
		if [[ $name == end ]]; then
			break;
		fi
	done
}
master_stdout ${1} >> master.csv

