#include <unistd.h> //write() access()

#include <iostream>
#include <sstream>
#include "HDD_Base.h"
#include "methods.h"
using namespace std;

///*//for some reason defalut value doesn't work
bool HDD_Base::presence(){
	return presence(false);
	}
//*/
bool HDD_Base::presence(bool print=false ){
	//print for explicit
	if(print) *dstream<<path<<" : checking presence... "<<std::endl;
	Present= access( path.c_str(),0 )==0;	
	if(print) *dstream<<path<<" : presence "<<((Present)?"detected":"not detected")<<std::endl;
	if(!Present) close(this->fd);//do i need this?
	return Present;
}
void HDD_Base::Presence_checker(bool throwing=false){
	while(1){
		if (!presence(false)&&throwing )throw "Hard Drive Unplugged";
		//else
		sleep(10);
	}
}
std::string HDD_Base::print(){
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