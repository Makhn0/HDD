#include "HDD.h"
#include <iostream>
#include <thread>
///*
std::string GetStdoutFromCommand(std::string cmd) {

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
void HDD::run(std::string batch){
	//std::thread* cnt=this->count_thread();
	this->presence();
	this->smartctl_run();
	this->dd_write(batch);
	this->dd_read(batch);
	this->hash_check(batch);
	this->erase();
	//this->partition();
	this->PresentTask="done, passed";
	//delete cnt;//->terminate();
	//cnt->join();
}
std::thread * HDD::count_thread(){
	std::thread* cnt=new std::thread(HDD::count,this);
	return cnt;
}
void HDD::count(){
	while(true){
		this->runTime+=2;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}
void HDD::print(){
	this->Model=GetStdoutFromCommand("echo solid as a rock");
	std::cout<<"Status of: "<<this->path<<std::endl;
	std::cout<<"Model Family: "<<this->ModelFamily<<std::endl;
	std::cout<<"Model : "<<this->Model;//<<std::endl;
	std::cout<<"Serial : "<<this->SerialNumber<<std::endl;
	std::cout<<"User Capacity: "<<this->UserCapacity<<std::endl;
	//std::cout<<"Present Task: "<<this->PresentTask<<std::endl;
	std::cout<<"Run Time: "<<this->runTime<<std::endl;
}
void HDD::presence(){
	this->present=1;
}
void HDD::smartctl_run(){
	this->PresentTask="Checking Smart Control...";
	//_linux_
	//this->CmdString="sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "+this->path" 1>/dev/null";
	//system(CmdString);
}
void HDD::smartctl_kill(){
	this->PresentTask="Checking Smart Control...";
	//_linux_
	//this->CmdString="sudo -X "+this->path+" 1>/dev/null";
	//system(this->CmdString);
}
void HDD::dd_write(std::string batch){
	this->PresentTask="Writing With dd...";
	//_linux_
	//this->CmdString="sudo dd if=/dev/urandom of=/tmp/"+batch+"_File.dd count=100KB 1>/dev/null";
	//system(this->CmdString);
	//this->CmdString="sudo dd if=/tmp/"+batch+"_File.dd of=/dev/"+this->path+" count=100KB 1>/dev/null";
	//system(this->CmdString);
}
void HDD::dd_read(std::string batch){
	this->PresentTask="Reading With dd...";
	//_linux_
	//this->cmdString="sudo dd if=/dev/"+this->path+" of=/tmp/"+batch+"_FileRead.dd count=100KB 1>/dev/null";
	//if(system(this->CmdString)){throw error;};
}
void HDD::hash_check(std::string batch){
	this->PresentTask="Checking Hash...";
	//_linux_
	//this->CmdString="md5sum /tmp/"+batch+"_File.dd |awk '{print substr($0,0,32)}')";_
	//std::string MainHash=GetStdoutFromCmd(this->CmdString);
	//this->CmdString="md5sum /tmp/"+batch +"_FileRead.dd |awk '{print substr($0,0,32)}')";_
	//std::string ReadHash=GetStdoutFromCmd(this->CmdString);
	//if(MainHash==ReadHash){
		//throw error;
	//	}
//	else{
	//move along nothing to see here;
	//}
}
void HDD::erase(){
	this->PresentTask="Erasing With Nwipe..." ;
	//this->CmdString="sudo ./nwipe --autonuke --nogui --method=zero  "+this->path+ "1>/dev/null &";
	//_linux_
	//system(this->CmdString);
}
void HDD::partition(){
	this->PresentTask="Partitioning Drives...";
	//_linux_
	//this->CmdString="sudo smar...  wait I'm skipping you anyway
	//system(CmdString);
}
