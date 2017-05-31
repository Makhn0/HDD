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
void HDD::Command(std::string a,std::string b,bool err2out=true  ){
	PresentTask=b;
	#ifdef _Debug
	std::cout<<path<<" : "<<PresentTask<<std::endl;
	#endif
	if(err2out)a.append(" 2>&1");
	Command(a);
}
void HDD::Command(std::string a){
	CmdString=a;
	
	#ifdef _Debug
	std::cout<<path<<" Last Command:"<<CmdString<<std::endl;
	#endif
	
	this->LastOutput=StdOut(this->CmdString);
	
	#ifdef _Debug
	std::cout<<this->path<<" Last Output:"<<this->LastOutput
	<<"_::exit status :"<<LastExitStatus<<std::endl;	
	std::cout<<this->path<<" "<<this->PresentTask<<" done "<<std::endl;
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
	int LastExitStatus=pclose(stream);
        if (LastExitStatus==-1)throw (std::string) "critical error stopping";
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
		if (data[i]=='[') break; //just in case we make substring in HDD::get_data() to big or small
	}
	return output;
}
//void HDD::run_body(std::string* batch);
void HDD::run(std::string* batch){
	while(1){
		#ifdef _Debug
		std::cout<<this->path<<" : beginning running loop... "<< std::endl;
		//sleep(1);
		#endif
		try{
			run_body(batch);
		}
		catch(std::string e){
			this->Exception= e;
			this->PresentTask = " Critical error, stopping. ";
			
			#ifdef _Debug
			std::cout<<this->path<<" Exception thrown : "<<this->Exception<<std::endl;		
			std::cout<<this->path<<" PresentTask : "<<this->PresentTask<<std::endl;
			#endif
		}
		catch(std::exception e){
			this->Exception=e.what();
			this->PresentTask = "critical error, stopping. ";
			
			#ifdef _Debug
			std::cout<<this->path<<" Exception thrown : "<<this->Exception<<std::endl;	
			std::cout<<this->path<<" PresentTask : "<<this->PresentTask<<std::endl;
			#endif
		}
		/* just stop when finished for now uncomment when  making release
		while(presence())
		{
			sleep(30);
		}
		/*/
		break;
		//*/
	}
}
void HDD::run_body(std::string* batch){
	while(1)
	{
		PresentTask="reseting";
		#ifdef _debug
		std::cout<<this->path<<" : resetting... "<<std::endl;
		#endif
		reset();
		PresentTask="waiting to detect...";
		while( !(presence()) ){
			sleep(2);
		}
		get_data();
		if(!presence()){continue;}
		#ifndef _Skip_Smart
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
		#endif
		#ifdef _Erase
		/*if(!this->presence()){continue;}
		this->dd_write(batch);
		if(!this->presence()){continue;}
		this->dd_read(batch);
		if(!this->presence()){continue;}
		this->hash_check(batch);
		if(!this->presence()){continue;}
		//*/
		this->erase();
		this->erase_debrief();
		break;
		if(!this->presence()){continue;}

		#endif
		
		this->EndTime=time(0);
	
		#ifdef _Debug	
		std::cout<<this->path<<" end of erase: writing to logs";
		print();
		#endif
		log(batch);
		PresentTask="done";		
		while(presence())
		{
		#ifdef _Debug
		std::cout<<this->path<<" : end of run, no pull out test"<<std::endl;
		break;
		#endif
			sleep(100);
		}
		#ifdef _Debug
		std::cout<<this->path<<" : past waiting"<<std::endl<<std::endl;
		#endif
	}
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
	//very concerned that this needs work
	Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/SMART support is:/' | sed -n '1,1p'"
		,"getting data..."
	);
	std::string temp = LastOutput.substr(18,9);
	this->SmartSupport=(temp=="Available");	
	
	#ifdef _Debug
	std::cout<<this->path<<" SmartSupport : "<<temp<<" : "<<this->SmartSupport<<std::endl; 
	#endif	
	 Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Model Family:/'"
	);
	//TODO unconfirmed substr parameters
	if(LastOutput!=""){
		this->ModelFamily=LastOutput.substr(18,35);	
	}
	else
	{
		this->ModelFamily="none detected";
	}
	this->Model = StdOut(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Device Model:/'"
	);
	if(LastOutput!=""){	
		this->Model= LastOutput.substr(18,35);	
	}
	else
	{
		this->Model=" none detected";
	}
	Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Serial Number:/'"
	);	
	this->SerialNumber =LastOutput.substr(18,35);

	Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/User Capacity:/'"
	);
	if(LastOutput!=""){
		this->UserCapacity=LastOutput.substr(18,20);
		this->size= myStol(this->UserCapacity);
	}
	else
	{
		this->UserCapacity=" none detected";
		this->size =0;
		throw path+" no capacity detected";
	}

	#ifdef _Debug
	puts("Data Extracted...");
	print(&std::cout);
	#endif
}
void HDD::smartctl_run()
{	
	this->smartctl_kill();
	Command(
		"sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "
		//+" -C"// no longer need smartctl_running
		+ this->path
		,"Running Smart Control..."
	);
}
bool HDD::smartctl_running()
{
	Command("sudo smartctl -a "
		+this->path
		+" | awk '/Self-test execution status:/' "
		,"Checking Smart Control is still running..."
	);
	//TODO add to class
	std::string code=LastOutput.substr(34,4);
	//only possible problem would be if " 240" is an error
	// it would return still running
	if (code[0]==' '&&code[1]=='2'&& code[2]=='4') return true;
	bool done=code=="   0";
	#ifdef _Debug
	std::cout<<this->path<<" smartctl: "<<((!done)?"is running":" has stopped ")<<" code :"<<code<<std::endl;
	#endif
	if (done) return false;
	throw (std::string) path+" smartctl error test runtime  "+code;
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
	std::string hashfile="/temp/"+*batch+"_File.dd";
	if(access( hashfile.c_str(),0 )==0){
		#ifdef _Debug
		std::cout<<this->path<<" old hash exists: "<<std::endl;
		#endif
		Command("sudo rm "
			+hashfile
			,"erasing old hash file"
		);
	}
	Command("sudo dd if=/dev/urandom of=/tmp/"
		+*batch
		+"_File.dd count=100KB "
		,"Writing With dd..."
	);
	Command("sudo dd if=/tmp/"+*batch+"_File.dd of="
		+this->path
		+" count=100KB "
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
		+"_File.dd"
		,"Checking Hash..."
	);
	std::string MainHash=LastOutput.substr(0,32);
	Command(
	"md5sum /tmp/"
		+*batch 
		+"_FileRead.dd"
	);
	std::string ReadHash= LastOutput.substr(0,32);
	if(MainHash!=ReadHash){
		throw (std::string) path+"hash rw failure...";
	}
}
void HDD::erase()
{  
	//* make this throw when fails if it doesn't already
	PresentTask="Erasing With Nwipe...";
	CmdString=std::string()
		//"sudo xterm -hold -title "+path+" -e"
		+" sudo ./nwipe --autonuke --nogui --method=zero " 
		+this->path 
		+" 2>&1 "
		;
	LastOutput="";
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	#ifdef _Debug
	std::cout<< path <<" Present Task : "
		<<PresentTask<<std::endl;
	std::cout<<path<<" Last Command : "<<CmdString<<std::endl;
	#endif
	stream = popen(CmdString.c_str(), "r");
	int i=0;
	std::cout <<"stream= "<<stream<<std::endl;
	if(stream) {
		std::cout<<"in outer if"<<std::endl;
		while(!feof(stream)){
			
			if (i%50==0)std::cout<<"in loop"<<std::endl;
			i++;
			if(fgets(buffer, max_buffer, stream) != NULL){
				std::cout<<"in loop and if 1"<<std::endl;
				#ifdef _Debug
				std::cout<<"in loop and if 2 "<<std::endl;
				
				std::cout<<buffer;
				for(int j=0;j<256;j++){	
					std::cout<<(int) buffer[j]<<" , ";
				}
				std::cout<<std::endl;
				#endif
				LastOutput.append(buffer);
			}
		}
	        if (pclose(stream)==-1)throw (std::string) "critical error stopping";
	}
	#ifdef _Debug
	std::cout<<path<<" "<<PresentTask<<" done"<<std::endl;
	std::cout<<path<<" final nwipe output : "
		<<LastOutput<<std::endl;
	#endif
	/*/
	Command(
	"sudo ./nwipe --autonuke --nogui --method=zero "
		+this->path
		,"Erasing With Nwipe..."
	);
	//*/
}
void HDD::erase_debrief(){}
void HDD::print(std::ostream* textgohere){
	UpdateRunTime();
	//TODO add info on which client is running
//extra \ns?
	*textgohere<<"Status of: "<<this->path<<std::endl;
	*textgohere<<"Presence :    "<<((this->Present)?"detected":"undetected")<<std::endl;
	*textgohere<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<std::endl;
	*textgohere<<"Model Family: "<<this->ModelFamily<<std::endl;
	*textgohere<<"Model  : "<<this->Model<<std::endl;
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
	std::string filename="/home/hdd-test-server/HDD_logs"
		+(*batch)+".log";
	this->PresentTask ="Writing to the log file:"+filename;	
	std::fstream* LogFile
		= new std::fstream(filename,std::ios::app);
	#ifdef _Debug
	std::cout<<this->path<<" :  "<<this->PresentTask<<std::endl;
	std::cout<<this->path<<"log file is open:"<<LogFile->is_open()<<std::endl;
	print(&std::cout);
	#endif
	print(LogFile);
}
void HDD::partition()
{
	Command(
		"sudo smar...  wait I'm skipping you anyway"
		,"Partitioning Drives..."
	);
}
