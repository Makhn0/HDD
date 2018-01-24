
fname=/dev/${1}
#echo fname=$fname
sudo cat > temp

client= $(pwd |grep -oP "test\d{1,2}")
#client=$(pwd | grep -oP -e "scrip.{1,2}$")
A=$(cat temp | grep "nwipe: info: Device $fname has serial number" | grep -o "\S*$")
As=$(cat temp | grep "nwipe: info: Device '$fname' is size" | grep -oP "\s\d*.$" | grep -oP "\d*")
# | grep -o "\d")
pass0=$(cat temp | grep "nwipe: notice: Verified that '$fname' is empty.")
pass1=$(cat temp | grep "nwipe: notice: Blanked device '$fname'.")

echo $pass0
echo $pass1
if [[ -z $pass0 ]] || [[ -z $pass1 ]]
then 
#if pass0 or pass1 is empty set pass not passed
	pass="not passed"
else
	pass="passed"
fi
echo $client , $A, $As, $pass

echo '###########cat temp############'
cat temp
