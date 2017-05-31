#include "HDD.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <exception>
#include <string>
#include <unistd.h>// for access in presense
///*
int HDD::instances;

/* // might be more usefull than throwing strings
struct Exception : public exception{
	const char * what() const throw(){
		return "C++";
	}
	
}
*/
std::string StdOut(std::string cmd);
void HDD::Command(std::string a,std::string b  ){
	this->PresentTask=b;
	Command(a);
}
void HDD::Command(std::string a){
	this->CmdString=a;
	std::cout<<"LastCommand:"<<this->CmdString<<std::endl;
	this->LastOutput=StdOut(this->CmdString);
	std::cout<<"LastOutput:"<<this->LastOutput<<std::endl;	
}
std::string StdOut(std::string cmd) {
	// doesn't return exit status?
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
long myStol(std::string a){
	long output=0;
	const char * data =a.c_str();
	for(unsigned int i=0;i<a.length();i++)
	{// unisigned to avoid warning. i guess its bad.
		//std::cout<<data[i]<< (int) data[i]<<std::endl;
		if (data[i]>47 && data[i]<58){
			output*=10;
			output += (long)(data[i]-48);
		}
		if (data[i]=='[') break; //just in case we make awk substring in HDD::get_data() to big or small
	}
	return output;
}
//*/

void HDD::run_body(std::string* batch){
	std::cout<<"running "<< this->path<<std::endl;
	while(1)
	{
		PresentTask="waiting to detect...";
		while( !(presence()) ){sleep(5);} //waits for detection use interrupt?
		//*
		get_data();
		if(!presence()){continue;}
		/*
		smartctl_run();
		while(smartctl_running()){
		if(!presence()){continue;}
		sleep(10);		
		}
		//*/
		//this->smartctl_kill();//just testing exclude in final buildS
		if(!this->presence()){continue;}
		this->dd_write(batch);
		if(!this->presence()){continue;}
		this->dd_read(batch);
		if(!this->presence()){continue;}
		this->hash_check(batch);
		if(!this->presence()){continue;}
		this->erase();
		if(!this->presence()){continue;}
		PresentTask="done,";
		std::cout<<"end of run, waiting"<<std::endl;
		while(presence()){
			sleep(1000);
		}
	}
}
void HDD::run(std::string* batch){
	//this->ProcQ=(Proc*) malloc(1);
	//this->ProcQ[0]=(Proc) this->get_data;
	try{
		run_body(batch);
	}
	catch(std::string e){
		std::cout<<e<<std::endl;	
		this->Exception= e;
		throw e;
	}
	
}
void HDD::print(){
	UpdateRunTime();
	//*
	printf("Status of: %-18s\n",this->path.c_str());
	printf("Presence : %20s\n",((this->Present)?"detected":"undetected"));
	printf("Smart Control Support: %s\n",(this->SmartSupport?"True":"False"));
	printf("Model Family: %10s\n",this->ModelFamily.c_str());
	printf("Model: %20s\n",this->Model.c_str());
	printf("Serial: %17s\n",this->SerialNumber.c_str());
	printf("User Capacity: %15s\n",this->UserCapacity.c_str());
	printf("Last Exception: %15s\n",this->Exception.c_str());
	printf("Present Tack: %20s\n",this->PresentTask.c_str());
	printf("Last/Current Command: %s\n",this->CmdString.c_str());
	printf("RunTime: %15ld\n",this->RunTime);
	printf("Last Output : %30s\n",this->LastOutput.c_str() );
	/*/
	std::cout<<"Status of: "<<this->path<<std::endl;
	std::cout<<"Presence :    "<<((this->Present)?"detected":"undetected")<<std::endl;
	std::cout<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<std::endl;
	std::cout<<"Model Family: "<<this->ModelFamily<<std::endl;
	std::cout<<"Model  : "<<this->Model;//<<std::endl;
	std::cout<<"Serial : "<<this->SerialNumber<<std::endl;
	std::cout<<"User Capacity: "<<this->UserCapacity<<std::endl;
	std::cout<<"Last Exception : "<<this->Exception<<std::endl;
	std::cout<<"Present Task: "<<this->PresentTask<<std::endl;
	std::cout<<"Last/Current Command :" << this->CmdString<<std::endl;
	std::cout<<"Run Time: "<<this->RunTime<<std::endl;
	printf("Last Output : %30s\n",this->LastOutput.c_str() );
	//*/
}
bool HDD::presence(){
	this->Present=
		access( this->path.c_str(),0 )==0;
	return this->Present;
}
void HDD::get_data(){
	this->PresentTask="getting data...";
	std::string temp =StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/SMART support is:/' | sed -n '1,1p' | awk '{print substr($0,19,9)}'"
		);
	std::cout<<temp<<std::endl; 
	this->SmartSupport=(temp=="Available\n");		
	this->ModelFamily = StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Model Family:/' | awk '{print substr($0,19,35)}'"
	);
	this->Model = StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Device Model:/' | awk '{print substr($0,19,35)}'"
	);	
	this->SerialNumber = StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Serial Number:/' | awk '{print substr($0,19,35)}'"
	);	
	this->UserCapacity=StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/User Capacity:/' | awk '{print substr($0,19,20)}'"
	);
	this->size= myStol(this->UserCapacity);
	printf("%s Smart Support %s\n",this->path.c_str(),(this->SmartSupport)?"enabled":"disabled");
	printf("%s ModelFamily: %s\n",this->path.c_str(),this->ModelFamily.c_str());
	printf("%s Model %s\n",this->path.c_str(),this->Model.c_str());
	printf("%s Serial Number %s\n",this->path.c_str(),this->SerialNumber.c_str());
	printf("%s UserCapcity:  %s  %ld \n",this->path.c_str(),this->UserCapacity.c_str(),this->size);

}
void HDD::smartctl_run(){
	this->smartctl_kill();
	Command(
		"sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "
		+ this->path
		//+" 1>/dev/null"
		,"Running Smart Control..."
		);
}

bool HDD::smartctl_running(){
	Command("sudo smartctl -a "
		+this->path
		+" | awk '/Self-test execution status:/' "
		+"| awk '{print substr($0,35,4)}'"
		,"Checking Smart Control is still running..."
		);
	bool done=LastOutput=="   0\n";
	std::cout<<!done<<std::endl;
	if (done) return !done;
	if (LastOutput[0]==' '&&LastOutput[1]=='2'&& LastOutput[2]=='4'){
		return !done;
	}
		throw (std::string) "smartctl error";
}
void HDD::smartctl_kill(){
	Command("sudo smartctl -X "
		+this->path
		,"Checking Smart Control...");
}
void HDD::dd_write(std::string* batch){
	try{
		Command("sudo rm /temp/"+*batch+"_File.dd","erasing old hash file");
	}
	catch(std::exception e)
	{

	}
	Command("sudo dd if=/dev/urandom of=/tmp/"
		+*batch
		+"_File.dd count=100KB "
		//+"1>/dev/null"
		,"Writing With dd..."
		);
	Command("sudo dd if=/tmp/"+*batch+"_File.dd of="
		+this->path
		+" count=100KB "
		//+"1>/dev/null"
		);
}
void HDD::dd_read(std::string* batch){
	Command("sudo dd if="
		+this->path
		+" of=/tmp/"
		+*batch
		+"_FileRead.dd count=100KB "
		,"Reading With dd..."
	);
}
void HDD::hash_check(std::string* batch){
	Command("md5sum /tmp/"
		+*batch
		+"_File.dd | awk '{print substr($0,0,32)}'"
		,"Checking Hash...");
	std::string MainHash= LastOutput;
	Command(
	"md5sum /tmp/"
		+*batch 
		+"_FileRead.dd |awk '{print substr($0,0,32)}'"
	);
	std::string ReadHash= LastOutput;
	if(MainHash!=ReadHash){
		throw (std::string) "hash rw failure";
	}
	else{
		//move along nothing to see here;
	}
}
void HDD::erase(){
	std::ofstream in;
	in.open(path.c_str(),std::ostream::out);
	if(!in) throw "cannot open HDD";
	for(int i=0;i<1000;i++)
	{
		in<<\0;
	}
}
void HDD::partition(){
	Command(
		"sudo smar...  wait I'm skipping you anyway"
		,"Partitioning Drives..."
	);
}
void HDD::reset(){
	this->SmartSupport=false;
	this->Present=false;
	this->Exception="none";
	this->SerialNumber="";
	this->Model="";
	this->ModelFamily="";
	this->UserCapacity="";
	this->RunTime=0;
	this->size=0;
}
