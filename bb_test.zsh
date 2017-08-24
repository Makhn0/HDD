#!/bin/zsh
function bb_test(){
	#local count=0; for more or less
	local bb_5="$(sudo smartctl -A /dev/${1} | awk '/5 Reallocated_Sector_Ct/' | awk '{print substr($0,85,6)}')";
	local bb_187="$(sudo smartctl -A /dev/${1} | awk '/187 Reported_Uncorrect/' | awk '{print substr($0,85,6)}')";
	local bb_188="$(sudo smartctl -A /dev/${1} | awk '/188 Command_Timeout/'| awk '{ print substr($0,85,6)}')";
	local bb_197="$(sudo smartctl -A /dev/${1} | awk '/197 Current_Pending_Sector/'| awk '{ print substr($0,85,6)}')";
	local bb_198="$(sudo smartctl -A /dev/${1} | awk '/198 Offline_Uncorrectable/' | awk '{ print substr($0,85,6)}')";
	local A="hidy ho";
	bb_187="$(echo '1  00' |awk '{ print substr($0,2,4)}')";
	if ((bb_5==0))&&((bb_187==0))&&((bb_188==0))&&((bb_197==0))&&((bb_198==0)) ;then
		#pass
		echo $A;
		echo lol;
		echo 0;
		echo $bb_5;
		echo $bb_187;
		echo $bb_188;
		echo $bb_197;
		echo $bb_198;
		echo pass;
	else
		#fail
		echo $bb_5;
		echo $bb_187;
		echo $bb_188;
		echo $bb_197;
		echo $bb_198;
		echo fail;
	fi
}
main()
{
	echo ${1};	
	bb_test ${1} ;
	echo here;
}
main $1;
