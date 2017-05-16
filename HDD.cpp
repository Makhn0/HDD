#include "HDD.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <exception>//haven't used yet
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
std::string StdOut(std::string cmd) {
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
		
		/* follwing code will produce warnings and idgaf*/
		PresentTask="waiting to detect...";
		while( !(presence()) ){} //waits for detection use interrupt?
		//*
		get_data();
		if(!presence()){continue;}
		/*
		smartctl_run();
		while(smartctl_running()){
		if(!presence()){continue;}
		sleep(100);		
		}
		/*
		this-smartctl_kill();//just testing exclude in final buildS
		if(!this->presence()){continue;}
		//*
		this->dd_write(batch);
		if(!this->presence()){continue;}
		//*
		this->dd_read(batch);
		if(!this->presence()){continue;}
		/*
		this->hash_check(batch);
		if(!this->presence()){continue;}
		/*
		//this->erase();
		if(!this->presence()){continue;}
		/*
		//this->partition();
		//this->verify(); //undeclared
		
		*/
		PresentTask="done,";
		while(presence()){}//waiting for it to be unplugged
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
	this->Model= StdOut("echo solid as a rock");
	/* //printf not as cool as I thought :(
	printf("Smart Control Support: %B",this->SmartSupport);
	printf("Status of: %S",this->path.c_str());
	printf("Model Family: %s",this->ModelFamily.c_str());
	printf("Model: %s",this->Model.c_str());
	printf("Serial: %s",this->SerialNumber.c_str());
	printf("User Capacity: %s",this->UserCapacity.c_str());
	printf("Last Exception: %s",this->Exception.c_str());
	printf("Present Tack: %s",this->PresentTask.c_str());
	printf("Last/Current Command: %s",this->CmdString.c_str());
	printf("RunTime: %d",this->RunTime);
	/*/
	std::cout<<"Status of: "<<this->path<<std::endl;
	std::cout<<"Presence :    "<<((this->Present)?"detected":"undetected")/*<<this->Present*/<<std::endl;
	std::cout<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<std::endl;
	std::cout<<"Model Family: "<<this->ModelFamily<<std::endl;
	std::cout<<"Model  : "<<this->Model;//<<std::endl;
	std::cout<<"Serial : "<<this->SerialNumber<<std::endl;
	std::cout<<"User Capacity: "<<this->UserCapacity<<std::endl;
	std::cout<<"Last Exception : "<<this->Exception<<std::endl;
	std::cout<<"Present Task: "<<this->PresentTask<<std::endl;
	std::cout<<"Last/Current Command :" << this->CmdString<<std::endl;
	std::cout<<"Run Time: "<<this->RunTime<<std::endl;
	//*/
}
bool HDD::presence(){
	/*//works windows && linux
	int temp=access(this->path.c_str(),0);
	std::cout<<this->path<<" access :"<<temp<<std::endl;
	return temp==0;
	/*/
	//std::ifstream infile(this->path.c_str());
	this->Present=
		//infile.good();
		access( this->path.c_str(),0 )==0;//got off internet never used "access" before
	return this->Present;
	//*/

}
void HDD::get_data(){
	this->PresentTask="getting data...";
	std::string temp =StdOut(//"echo Available");
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
	printf("%s smart support %d\n",this->path.c_str(),this->SmartSupport);
	printf("%s ModelFamily: %s\n",this->path.c_str(),this->ModelFamily.c_str());
	printf("%s Model %s\n",this->path.c_str(),this->Model.c_str());
	printf("%s Serial Number %s\n",this->path.c_str(),this->SerialNumber.c_str());
	printf("%s UserCapcity:  %s  %ld \n",this->path.c_str(),this->UserCapacity.c_str(),this->size);

}


void HDD::smartctl_run(){
	this->smartctl_kill();
	this->PresentTask="Running Smart Control...";
	this->CmdString="sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "
		+ this->path
		+" 1>/dev/null"// last argument redundant with system()?
	;
	//*
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString.c_str());
	/*	/
	if( system( this->CmdString.c_str()  )){
		throw (std::string) "smartctl error";
	}
	*/
}
bool HDD::smartctl_running(){
this->PresentTask="Checking Smart Control is still running...";
	this->CmdString="sudo smartctl --a"
		+this->path
		+" | awk '/Self-test execution status:/' | awk '{pring substr($0,41,37)}'";
		/*local TestStatus="$(sudo smartctl -a /dev/${1} | awk '/Self-test execution status:/' | awk '{print substr($0,41,37)}')";
*/
		
	std::cout<<this->CmdString<<std::endl;
	std::string output=StdOut(this->CmdString.c_str());
	bool foutput=output=="running";
	std::cout<<output<<foutput<<std::endl;
	return foutput;
}
void HDD::smartctl_kill(){
	this->PresentTask="Checking Smart Control...";
	this->CmdString="sudo smartctl -X "+this->path
		//+" 1>/dev/null"
	;
	//*
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString);
	/*/
	if(system(this->CmdString.c_str() )){
		throw (std::string) "smartct kill error";
	}
	//*/
}
void HDD::dd_write(std::string* batch){
	this->PresentTask="Writing With dd...";
	this->CmdString="sudo dd if=/dev/urandom of=/tmp/"
		+*batch
		+"_File.dd count=100KB 1>/dev/null";
	//*
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString);
	/*/
	if(system(this->CmdString.c_str())){
		throw (std::string) "smartctl error";
	}
	/*/
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString);
	/*/
	this->CmdString="sudo dd if=/tmp/"+*batch+"_File.dd of=/dev/"
		+this->path
		+" count=100KB 1>/dev/null";
	if(system(this->CmdString.c_str())){
		throw (std::string) "sdd write error";
	}
	//*/
}
void HDD::dd_read(std::string* batch){
	this->PresentTask="Reading With dd...";
	this->CmdString="sudo dd if=/dev/"
		+this->path
		+" of=/tmp/"
		+*batch
		+"_FileRead.dd count=100KB "
		//+1>/dev/null"
		;
	//*
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString);
	/*/
	if(system(this->CmdString.c_str())){	
		throw (std::string) "dd read error";
	}
	//*/
}
void HDD::hash_check(std::string* batch){
	this->PresentTask="Checking Hash...";
	this->CmdString="md5sum /tmp/"
		+*batch
		+"_File.dd |awk '{print substr($0,0,32)}')";
	std::string MainHash= StdOut(this->CmdString);
	this->CmdString="md5sum /tmp/"
		+*batch 
		+"_FileRead.dd |awk '{print substr($0,0,32)}')";
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
	this->CmdString="sudo ./nwipe --autonuke --nogui --method=zero  "
		+this->path
		+ "1>/dev/null &";
	//*
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString);
	/*/
	if(system(this->CmdString.c_str())){
		throw (std::string) "nwipe error";
	}
	//*/
}
void HDD::partition(){
	this->PresentTask="Partitioning Drives...";
	this->CmdString="sudo smar...  wait I'm skipping you anyway";
	//*
	std::cout<<this->CmdString<<std::endl;
	std::cout<<StdOut(this->CmdString);
	/*/
	if(system(this->CmdString.c_str())){
		throw (std::string) "partitioning error";
	}
	//*/
}
void HDD::reset(){
	this->Lock=false;
	this->SmartSupport=false;
	this->Present=false;
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
