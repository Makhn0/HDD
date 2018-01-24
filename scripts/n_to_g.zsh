

sudo cat > temp

echo "hello"
A=$(cat temp | grep "nwipe: info: Device /dev/sda has serial number" | grep -o "\S*$")
echo $A

As=$(cat temp | grep "nwipe: info: Device '/dev/sda' is size" | grep -o "(\S*).$")
# | grep -o "\d")
echo $As
echo 
echo
cat temp
