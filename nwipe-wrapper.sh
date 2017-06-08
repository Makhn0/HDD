# !/bin/bash
echo " erasing ${1} with nwipe!!1~!:D"
echo "test $?" $?
sudo ./nwipe --autonuke --nogui --method=zero ${1} 2>&1
echo $?
	
