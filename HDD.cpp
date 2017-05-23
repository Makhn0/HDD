#include "HDD.h"
#include <stdio.h>
#include <fstream>
#include <ostream>
#include <iostream>

#include <thread>
#include <exception>
#include <string>
#include <unistd.h>

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
	#ifdef _Debug
	std::cout<<this->path<<" : "<<this->PresentTask<<std::endl;
	#endif
	Command(a);
}
void HDD::Command(std::string a){
	this->CmdString=a;
	#ifdef _Debug
	std::cout<<"LastCommand:"<<this->CmdString<<std::endl;
	#endif
	this->LastOutput=StdOut(this->CmdString);
	#ifdef _Debug
	std::cout<<"LastOutput:"<<this->LastOutput<<std::endl;	
	#endif
}

std::string StdOut(std::string cmd) {
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(cmd.c_str(), "r");
    if(stream) {
        while(!feof(stream))
            if(fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
        pclose(stream);
    }
    return data;
}
long myStol(std::string a){
	long output=0;
	const char * data =a.c_str();
	for(unsigned int i=0;i<a.length();i++)
	{// unisigned to avoid warning. i guess its bad.
		if (data[i]>47 && data[i]<58){
			output*=10;
			output += (long)(data[i]-48);
		}
		if (data[i]=='[') break; //just in case we make awk substring in HDD::get_data() to big or small
	}
	return output;
}
void HDD::reset(){
	this->SmartSupport=false;
	this->Present=false;
	this->Exception="none";
	this->SerialNumber="";
	this->Model="";
	this->ModelFamily="";
	this->UserCapacity="";
	this->StartTime=time(0);
	this->RunTime=0;
	this->size=0;
}
void HDD::run_body(std::string* batch){
	#ifdef _Debug
	std::cout<<"running "<< this->path<<std::endl;
	sleep(5);
	#endif
	while(1)
	{
		PresentTask="waiting to detect...";
		while( !(presence()) ){
			sleep(2);
		}
		get_data();
		if(!presence()){continue;}
		smartctl_run();
		bool a=false;
		while(smartctl_running()){
			if(!presence()){
				a=true;
				break;
			}
			sleep(10);		
		}
		if(a) continue;
		#ifdef _Erase
		///*read to disk section do not uncomment unless you want to kill all
		//	disks on machine control-c won't save you.		
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
		//*/
		#endif
		
		this->EndTime=time(0);

		log(batch);
		PresentTask="done";		
		#ifdef _Debug
		std::cout<<this->path<<" : end of run, waiting"<<std::endl;
		#endif
		while(presence())
		{
			sleep(1000);
		}
		#ifdef _Debug
		std::cout<<this->path<<" : past waiting"<<std::endl<<std::endl;
		#endif
	}
}
void HDD::run(std::string* batch){
	try{
		run_body(batch);
	}
	catch(std::string e){
		this->Exception= e;
		this->PresentTask = "critical error, stopping. ";
		#ifdef _Debug
		std::cout<<e<<std::endl;	
		std::cout<<this->path<<" PresentTask : "<<this->PresentTask<<std::endl;
		#endif
	}
}
void HDD::print(std::ostream* textgohere){
	UpdateRunTime();
	/*
	printf("Status of: %-18s\n",this->path.c_str());
	printf("Presence : %20s\n",((this->Present)?"detected":"undetected"));
	printf("Smart Control Support: %s\n",(this->SmartSupport?"True":"False"));
	printf("Model Family: %10s\n",this->ModelFamily.c_str());
	printf("Model: %20s\n",this->Model.c_str());
	printf("Serial: %17s\n",this->SerialNumber.c_str());
	printf("User Capacity: %15s\n\n",this->UserCapacity.c_str());
	printf("Last Exception: %15s\n",this->Exception.c_str());
	printf("Present Tack: %20s\n",this->PresentTask.c_str());
	printf("Last/Current Command: %s\n",this->CmdString.c_str());
	printf("Last Output : %30s\n",this->LastOutput.c_str() );
	printf("RunTime: %15ld\n",this->RunTime);
	/*/
	*textgohere<<"Status of: "<<this->path<<std::endl;
	*textgohere<<"Presence :    "<<((this->Present)?"detected":"undetected")<<std::endl;
	*textgohere<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<std::endl;
	*textgohere<<"Model Family: "<<this->ModelFamily<<std::endl;
	*textgohere<<"Model  : "<<this->Model;//<<std::endl;
	*textgohere<<"Serial : "<<this->SerialNumber<<std::endl;
	*textgohere<<"User Capacity: "<<this->UserCapacity<<std::endl;
	*textgohere<<"Last Exception : "<<this->Exception<<std::endl;
	*textgohere<<"Present Task: "<<this->PresentTask<<std::endl;
	*textgohere<<"Last/Current Command :" << this->CmdString<<std::endl;
	*textgohere<<"Last Output "<<this->LastOutput<<std::endl;
	*textgohere<<"Start Time: "<<this->StartTime<<std::endl;
	*textgohere<<"End Time: "<<this->EndTime<<std::endl;
	*textgohere<<"Run Time: "<<this->RunTime<<std::endl;
	*textgohere<<"______________________________________________________"<<std::endl;
}	
void HDD::print(){
	print(&std::cout);
}
void HDD::log(std::string * batch){
	this->PresentTask ="Writing to the log file";	
	std::fstream LogFile(*batch+"_log.txt",std::ios::app);;
	#ifdef _Debug
	std::cout<<LogFile.is_open()<<std::endl;
	#endif
	print(&LogFile);
}
void HDD::log(){
	std::string * temp =new std::string("may1");
	log(temp);
	delete(temp);
}
bool HDD::presence(){
	#ifdef _Debug
	std::cout<<this->path<<" : checking presence... "<<std::endl;
	#endif
	this->Present=
		access( this->path.c_str(),0 )==0;
	#ifdef _Debug
		std::cout<<this->path<<" : presence "<<((this->Present)?"detected":"not detected")<<std::endl;
	#endif
	return this->Present;
}
void HDD::get_data(){
	this->PresentTask="getting data...";
	#ifdef _Debug
	std::cout<<this->path<<" :  "<<this->PresentTask<<std::endl;
	#endif
	std::string temp =StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/SMART support is:/' | sed -n '1,1p' | awk '{print substr($0,19,9)}'"
	);
	#ifdef _Debug
	std::cout<<this->path<<" : "<<temp<<std::endl; 
	#endif
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
	#ifdef _Debug
	print(&std::cout);
	#endif
}
void HDD::smartctl_run()
{
	this->smartctl_kill();
	Command(
		"sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "
		+ this->path
		//+" 1>/dev/null"
		,"Running Smart Control..."
	);
}
bool HDD::smartctl_running()
{
	Command("sudo smartctl -a "
		+this->path
		+" | awk '/Self-test execution status:/' "
		+"| awk '{print substr($0,35,4)}'"
		,"Checking Smart Control is still running..."
	);
	bool done=LastOutput=="   0\n";
	#ifdef _Debug
	std::cout<<this->path<<" smartctl: "<<((!done)?"is running":" has stopped ")<<std::endl;
	#endif
	if (done) return !done;
	if (LastOutput[0]==' '&&LastOutput[1]=='2'&& LastOutput[2]=='4'){
		return !done;
	}
		throw (std::string) "smartctl error";
}
void HDD::smartctl_kill()
{
	Command("sudo smartctl -X "
		+this->path
		,"Checking Smart Control..."
	);
}
void HDD::dd_write(std::string* batch)
{
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
void HDD::dd_read(std::string* batch)
{
	Command("sudo dd if="
		+this->path
		+" of=/tmp/"
		+*batch
		+"_FileRead.dd count=100KB "
		,"Reading With dd..."
	);
}
void HDD::hash_check(std::string* batch)
{
	Command("md5sum /tmp/"
		+*batch
		+"_File.dd | awk '{print substr($0,0,32)}'"
		,"Checking Hash..."
	);
	std::string MainHash=LastOutput;
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
void HDD::erase()
{
	Command(
	"sudo ./nwipe --autonuke --nogui --method=zero "
		+this->path
		//+ "1>/dev/null "
		,"Erasing With Nwipe..."
	);
}
void HDD::partition()
{
	Command(
		"sudo smar...  wait I'm skipping you anyway"
		,"Partitioning Drives..."
	);
}
