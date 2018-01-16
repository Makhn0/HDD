#include <unistd.h> //write() access()

#include <iostream>
#include "HDD_Base.h"
bool HDD_Base::presence(){
	return presence(false);
}
//*/
bool HDD_Base::presence(bool print ){
	//print for explicit
	if(print) *dstream<<this->path<<" : checking presence... "<<std::endl;
	
	this->Present=
		access( this->path.c_str(),0 )==0;
		
	if(print) *dstream<<this->path<<" : presence "<<((this->Present)?"detected":"not detected")<<std::endl;
	if(!Present) close(this->fd);//do i need this?
	return this->Present;
}
void HDD_Base::Presence_checker(){
	Presence_checker(false);
}
void HDD_Base::Presence_checker(bool throwing){
	while(1){
		if (!presence()&&throwing )throw "Hard Drive Unplugged";
		sleep(10);
	}
}