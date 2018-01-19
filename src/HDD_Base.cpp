#include <unistd.h> //write() access()

#include <iostream>
#include <sstream>
#include <fcntl.h> //O_RDWR? // yes
#include <sys/stat.h>//O_RDWR?// apparently not
#include <sys/types.h>//open();close()(ithink);
#include "HDD_Base.h"
#include "methods.h"
using namespace std;

///*//for some reason defalut value doesn't work
bool HDD_Base::presence_check (){
	/* stores and returns presence of harddrive */
	//print for explicit
	//if(print) *p()<<" : checking presence... "<<std::endl;
	Present= access( path.c_str(),0 )==0;	
	//if(print) *p()<<" : presence "<<((Present)?"detected":"not detected")<<std::endl;
	//if(!Present) close(this->fd);//do i need this?
	return Present;
}
void HDD_Base::Presence_checker(bool throwing=false){
	/* periodically checks for harddrive*/
	while(1){
		if (!presence_check()&&throwing )throw "Hard Drive Unplugged";
		//else
		sleep(10);
	}
}
void HDD_Base::get_data(){
	/*fetches fd, smart controlsupport, Model Family, Model, Serial Number and Size*/
	//fd is in reset() as well, this is just in case it changes;
	if(Present) this->fd=open(path.c_str(),O_RDWR);//std::open?
	Command(
		"sudo smartctl -i "
		+path
		+" | awk '/SMART support is:/' | sed -n '1,1p'"
		,"getting data...",true
	);
	string temp= LastOutput.substr(18,9);
	SmartSupport=(temp=="Available");	
	
	*p()<<" : SmartSupport : "<<temp<<" : "<<SmartSupport<<endl; 
	 Command(
		"sudo smartctl -i "
		+path
		+" | awk '/Model Family:/'",true
	);
	if(LastOutput!=""){
		ModelFamily=LastOutput.substr(18,35);
			
	}
	else
	{
		ModelFamily="none detected";
	}

	Model= StdOut(
		"sudo smartctl -i "
		+path
		+" | awk '/Device Model:/'"
	);
	if(LastOutput!=""){	
		Model= LastOutput.substr(18,35);	
	}
	else
	{
		Model=" none detected";
	}
	Command(
		"sudo smartctl -i "
		+path
		+" | awk '/Serial Number:/'",true
	);	
	SerialNumber =LastOutput.substr(18,35);

	Command(
		"sudo smartctl -i "
		+path
		+" | awk '/User Capacity:/'",true
	);
	if(LastOutput!=""){
		string a=LastOutput.substr(18,25);
		size= myStol(a);
	}
	else
	{
		size=-1;
		throw " no capacity detected";
	}
	trim(ModelFamily);
	trim(SerialNumber);
	trim(Model);
	*dstream<<"Data Extracted..."<<endl;
	print(dstream);
}
std::string HDD_Base::str(){
	/* returns string with basic harddrive info*/
	stringstream s;
	s<<"Status of: "<<path<<endl
	<<"Presence :    "<<((Present)?"detected":"undetected")<<endl
	<<"Smart Support: "<<(SmartSupport?"available":"unavailable")<<endl
	<<"Model Family: "<<ModelFamily<<endl
	<<"Model  #: "<<Model<<endl
	<<"Serial #: "<<SerialNumber<<endl
	<<"User Capacity: "<<sizeToString(size)<<endl;
	return s.str();
}
std::ostream* HDD_Base::print(std::ostream * stream=&std::cout){
	return	&(*stream<<str());
}
