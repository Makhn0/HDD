#include "HDD.h"
#include "HDD.h"
#include <iostream>
#include <thread>
#include <exception>
#include <string>
#include <unistd.h>
#include <unistd.h>// for access in presense
///*
std::string StdOut(std::string cmd) {
//what what
//thats whats up
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    //cmd.append(" 2>&1"); // Do we want STDERR?

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}
//*/
int HDD::instances;
/* // might be more usefull than throwing strings
struct Exception : public exception{
	const char * what() const throw(){
		return "C++";
	}
	
}
*/
void HDD::run_body(std::string batch){
while(1)
	{
		std::cout<<"running "<< this->path<<std::endl;
		//std::thread* cnt=this->count_thread();
		while( !(this->presence()) ){break;} //waits for detection use interrupt?
		this->get_data();
		if(!this->presence()){continue;}
		/*this->smartctl_run();
		if(!this->presence()){continue;}
		this->dd_write(batch);
		if(!this->presence()){continue;}
		this->dd_read(batch);
		if(!this->presence()){continue;}
		this->hash_check(batch);
		if(!this->presence()){continue;}
		//this->erase();
		//this->partition();
		this->PresentTask="done, passed";
		//this->Idle();
		//delete cnt;//->terminate();
		//cnt->join();
		//*/
		//this->run_body(batch);
		
	}
}
void HDD::run(std::string batch){
	
	try{
		this->run_body(batch);
	}
	catch(std::string e){
		std::cout<<e<<std::endl;	
		this->Exception= e;
		throw e;
	}
	
}
std::thread * HDD::count_thread(){
	std::thread* cnt=new std::thread(&HDD::count,this);
	return cnt;
}
void HDD::count(){
	while(true){
		this->RunTime+=2;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
void HDD::print(){
	this->Model= StdOut("echo solid as a rock");
	std::cout<<" smrtCtrl  :"<<this-SmartSupport<<std::endl;
	std::cout<<"Status of: "<<this->path<<std::endl;
	std::cout<<"Model Family: "<<this->ModelFamily<<std::endl;
	std::cout<<"Model : "<<this->Model;//<<std::endl;
	std::cout<<"Serial : "<<this->SerialNumber<<std::endl;
	std::cout<<"User Capacity: "<<this->UserCapacity<<std::endl;
	std::cout<<"Last Exception : "<<this->Exception<<std::endl;
	std::cout<<"Present Task: "<<this->PresentTask<<std::endl;
	std::cout<<"Last/Current Command :" << this->CmdString<<std::endl;
	std::cout<<"Run Time: "<<this->RunTime<<std::endl;
}
bool HDD::presence(){
	/*works
	int temp=access(this->path.c_str(),0);
	std::cout<<this->path<<" access :"<<temp<<std::endl;
	return temp==0;
	*/	
	this->present=access( this->path.c_str(),0 )==0;//got off internet never used "access" before
	return this->present;
}
void HDD::get_data(){
///*
//	local SmartSupportRaw="$(sudo smartctl -i /dev/${1} | awk '/SMART support is:/' | awk '{print substr($0,19,9)}; NR == 1 {exit}')";
		//std::string output =StdOut("echo Available");
		//std::cout<<"echo Available :"<<output<<"#"<<std::endl;
		bool temp="Available\n"==StdOut(//"echo Available");
			///*
			"sudo smartctl -i /dev/"
			+this->path
			+" | awk '/Model Family:/' | awk '{print substr($0,19,35)}'"
			);// returns non boolean ?? why?
			//*/
		std::cout<<temp<<std::endl; 
		this->SmartSupport=temp;
	/*		
		this->ModelFamily = StdOut(
			"sudo smartctl -i /dev/"
			+this->path+
			" | awk '/Model Family:/' | awk '{print substr($0,19,35)}"
			);
		//local ModelFamilyRaw=sudo smartctl -i /dev/"+this->path+" | awk '/Model Family:/' | awk '{print substr($0,19,35)};
		this->Model = StdOut(
			"sudo smartctl -i /dev/"
			+this->path
			+" | awk '/Device Model:/' | awk '{print substr($0,19,35)}'"
			);	
		//local ModelRaw="$(sudo smartctl -i /dev/"+this->path+" | awk '/Device Model:/' | awk '{print substr($0,19,35)}')";
		this->SerialNumber = StdOut(
			"sudo smartctl -i /dev/"
			+this->path
			+" | awk '/Serial Number:/' | awk '{print substr($0,19,35)}'"
			);	
		//local SerialNumberRaw="$(sudo smartctl -i /dev/"+this->path+" | awk '/Serial Number:/' | awk '{print substr($0,19,35)}')";
		this->UserCapacity=StdOut(
			"sudo smartctl -i /dev/"
			+this->path
			+" | awk '/User Capacity:/' | awk -F\"[\" '{print $2}' | grep -o '[0-9]\\+' | head -n 1"
			);
		this->size= std::stol(this->UserCapacity);
		//local UserCapacityGB="$(sudo smartctl -i /dev/"+this->path+" | awk '/User Capacity:/' | awk -F"[" '{print $2}' | grep -o '[0-9]\\+' | head -n 1)";
	//		*/
}
void HDD::smartctl_run(){
	this->PresentTask="Checking Smart Control...";
	//_linux_
	this->CmdString="sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "+ this->path+" 1>/dev/null";
	if( system( this->CmdString.c_str()  )){
		throw (std::string) "smartctl error";
	}
}
void HDD::smartctl_kill(){
	this->PresentTask="Checking Smart Control...";
	//_linux_
	this->CmdString="sudo -X "+this->path+" 1>/dev/null";
	if(system(this->CmdString.c_str() )){throw (std::string) "smartct kill error";}
}
void HDD::dd_write(std::string batch){
	this->PresentTask="Writing With dd...";
	//_linux_
	this->CmdString="sudo dd if=/dev/urandom of=/tmp/"+batch+"_File.dd count=100KB 1>/dev/null";
	if(system(this->CmdString.c_str())){throw (std::string) "smartctl error";}
	this->CmdString="sudo dd if=/tmp/"+batch+"_File.dd of=/dev/"+this->path+" count=100KB 1>/dev/null";
	if(system(this->CmdString.c_str())){throw (std::string) "sdd write error";}
}
void HDD::dd_read(std::string batch){
	this->PresentTask="Reading With dd...";
	//_linux_
	this->CmdString="sudo dd if=/dev/"+this->path+" of=/tmp/"+batch+"_FileRead.dd count=100KB 1>/dev/null";
	if(system(this->CmdString.c_str())){throw (std::string) "dd read error";}
}
void HDD::hash_check(std::string batch){
	this->PresentTask="Checking Hash...";
	//_linux_
	this->CmdString="md5sum /tmp/"+batch+"_File.dd |awk '{print substr($0,0,32)}')";
	std::string MainHash= StdOut(this->CmdString);//may need .c_str();
	this->CmdString="md5sum /tmp/"+batch +"_FileRead.dd |awk '{print substr($0,0,32)}')";
	std::string ReadHash= StdOut(this->CmdString);
	if(MainHash!=ReadHash){
		throw (std::string) "hash rw failure";
	}
	else{
		//move along nothing to see here;
	}
}
void HDD::erase(){
	this->PresentTask="Erasing With Nwipe..." ;
	this->CmdString="sudo ./nwipe --autonuke --nogui --method=zero  "+this->path+ "1>/dev/null &";
	//_linux_
	if(system(this->CmdString.c_str())){throw (std::string) "nwipe error";}
}
void HDD::partition(){
	this->PresentTask="Partitioning Drives...";
	//_linux_
	this->CmdString="sudo smar...  wait I'm skipping you anyway";
	if(system(this->CmdString.c_str())){throw (std::string) "partitioning error";}
}
void HDD::reset(){
	this->Lock=false;
	this->SmartSupport=false;
	this->present=false;
	this->SmartKill=false;
	this->SmartctlScsiPid=0;
	this->Exception="none";
	this->SerialNumber="";
	this->Model="";
	this->ModelFamily="";
	this->UserCapacity="";
	this->RunTime=0;
	this->size=0;
}
