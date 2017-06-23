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
#ifndef Result_To_String
#define Result_To_String
std::string ResultTToString(Result_t a)
{
	switch(a)
	{
		case Unfinished: return "Unfinished";
		case FinishedSuccess: return "Finished , Success";
		case FinishedFail: return "Finished , Failure";
		default: return "Unfinished";
	}
	return "Unfinished";
}
#endif

std::string HDD::StdOut(std::string cmd, bool throwing=true) {
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(cmd.c_str(), "r");
    if(stream) {
        while(!feof(stream)&& Present)
            if(fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
	LastExitStatus=pclose(stream);
        if ((LastExitStatus!=0)&&throwing)throw (std::string) "Critical error stopping";
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
void HDD::Command(std::string a,std::string task,bool throwing=true){
	PresentTask=task;
	*dstream<<path<<" : "<<PresentTask<<std::endl;
	Command(a,throwing);
	*dstream<<this->path<<" : "<<this->PresentTask<<" done "<<std::endl;
}
void HDD::Command(std::string a,bool throwing=true){
	a.append(" 2>&1");
	CmdString=a;
	*dstream<<path<<" : Last Command:"<<CmdString<<std::endl;
	this->LastOutput=StdOut(this->CmdString,throwing);
	
	*dstream<<this->path<<" : Last Output:"<<this->LastOutput
	<<"_::exit status :"<<LastExitStatus<<std::endl;	
}


//void HDD::run_body(std::string* batch);
void HDD::run(std::string* batch){
	while(1){
		*dstream<<this->path<<" : beginning running loop... "<< std::endl;
		//sleep(1);
		PresentTask="reseting";
		*dstream<<this->path<<" : resetting... "<<std::endl;
		reset();
		PresentTask="waiting to detect...";
		while(!presence()){
			sleep(5);
		}
		try{	
			run_body(batch);
		}
		catch(std::string e){
			this->Exception= e;
			this->PresentTask = " Critical error, stopping. ";
			this->Status=FinishedFail;
			
			*dstream<<this->path<<"#####################WARNING#####################"<<std::endl;
			*dstream<<this->path<<" : Exception thrown : "<<this->Exception<<std::endl;	
			*dstream<<this->path<<" : PresentTask : "<<this->PresentTask<<std::endl;
			*dstream<<this->path<<" : LastCommand : "<<this->CmdString<<std::endl;	
			*dstream<<this->path<<" : LastOutput : "<<this->LastOutput<<std::endl;
			*dstream<<this->path<<"#################################################"<<std::endl;
		}
		catch(std::exception e){
			this->Exception=e.what();
			this->PresentTask = "critical error, stopping. ";
			this->Status=FinishedFail;
			
			*dstream<<this->path<<"#####################WARNING#####################"<<std::endl;
			*dstream<<this->path<<" : Exception thrown : "<<this->Exception<<std::endl;	
			*dstream<<this->path<<" : PresentTask : "<<this->PresentTask<<std::endl;
			*dstream<<this->path<<" : LastCommand : "<<this->CmdString<<std::endl;	
			*dstream<<this->path<<" : LastOutput : "<<this->LastOutput<<std::endl;
			*dstream<<this->path<<"#################################################"<<std::endl;
		}
		*dstream<<this->path<<" : end of run, no pull out test"<<std::endl;
		
		while(presence())
		{
				sleep(10);
		}
		*dstream<<this->path<<" : pulled out starting over..."<<std::endl<<std::endl;
	}
	*dstream<<path<<" : we did it out of the loop"<<std::endl;;
}
void HDD::run_body(std::string* batch){
	this->StartTime=time(0);
	get_data();
	if(!presence()){return ;}
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
	if(a) return;
	#endif
	#ifdef _Erase
	///*
	if(!this->presence()){return;}
	this->dd_write(batch);
	if(!this->presence()){return;}
	this->dd_read(batch);
	if(!this->presence()){return;}
	this->hash_check(batch);
	if(!this->presence()){return;}
	//*/
	//obviously needs more work for different methods
	if(*batch=="random"){
		this->erase(batch);
	}
	else{
		try{
			this->erase();
			this->erase_debrief();
		}
		catch(std::string e){
			*dstream<<path<<" : string thrown"<<std::endl;
			this->erase_debrief();
			throw e;
		}
		catch(std::exception e){

			*dstream<<path<<" : exception thrown"<<std::endl;
			this->erase_debrief();
			throw e;
		}
	}		
	if(!this->presence()){return;}
	#endif
	
	this->EndTime=time(0);
	if(!this->presence()){return;}
	*dstream<<this->path<<" : end of erase: writing to logs";
	print(dstream);
	log(batch);	
}
void HDD::reset(){
	this->SmartSupport=false;
	this->Present=false;
	this->Exception="none";
	this->SerialNumber="";
	this->Model="";
	this->ModelFamily="";
	this->UserCapacity="";
	this->size=0;
	this->StartTime=-1;//time(0);
	this->EndTime=-1;
	this->RunTime=0;
	this->CmdString="";
	this->LastOutput="";
	this->LastExitStatus=0;
	this->Status=Unfinished;
}
bool HDD::presence(){
	*dstream<<this->path<<" : checking presence... "<<std::endl;
	
	this->Present=
		access( this->path.c_str(),0 )==0;
		
	*dstream<<this->path<<" : presence "<<((this->Present)?"detected":"not detected")<<std::endl;
	
	return this->Present;
}
void HDD::PresenceDetector(bool throwing=false){
	while(1){
		if (!presence()&&throwing )throw "Hard Drive Unplugged";
		sleep(10);
	}
}
void HDD::get_data(){
	//very concerned that this needs work
	Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/SMART support is:/' | sed -n '1,1p'"
		,"getting data...",true
	);
	std::string temp = LastOutput.substr(18,9);
	this->SmartSupport=(temp=="Available");	
	
	*dstream<<this->path<<" : SmartSupport : "<<temp<<" : "<<this->SmartSupport<<std::endl; 
	 Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Model Family:/'",true
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
		+" | awk '/Serial Number:/'",true
	);	
	this->SerialNumber =LastOutput.substr(18,35);

	Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/User Capacity:/'",true
	);
	if(LastOutput!=""){
		this->UserCapacity=LastOutput.substr(18,25);
		this->size= myStol(this->UserCapacity);
	}
	else
	{
		this->UserCapacity=" none detected";
		this->size =0;
		throw path+" no capacity detected";
	}

	*dstream<<"Data Extracted..."<<std::endl;
	print(dstream);
}
void HDD::smartctl_run()
{	
	this->smartctl_kill();
	Command(
		"sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "
		//+" -C"// no longer need smartctl_running
		+ this->path
		,"Running Smart Control...",true
	);
}
bool HDD::smartctl_running()
{
	Command("sudo smartctl -a "
		+this->path
		+" | awk '/Self-test execution status:/' "
		,"Checking Smart Control is still running...",true
	);
	//TODO add to class
	std::string code=LastOutput.substr(34,4);
	//only possible problem would be if " 240" is an error
	// it would return still running
	if (code[0]==' '&&code[1]=='2'&& code[2]=='4') return true;
	bool done=code=="   0";
	*dstream<<this->path<<" : smartctl: "<<((!done)?"is running":" has stopped ")<<" code :"<<code<<std::endl;
	if (done) return false;
	throw (std::string) path+" smartctl error test runtime  "+code;
}
void HDD::smartctl_kill()
{
	Command("sudo smartctl -X "
		+this->path
		,"Checking Smart Control...",true
	);
}
void HDD::dd_write(std::string* batch)
{
	std::string hashfile="/temp/"+*batch+"_File.dd";
	if(access( hashfile.c_str(),0 )==0){
		*dstream<<this->path<<" : old hash exists: "<<std::endl;
		Command("sudo rm "
			+hashfile
			,"Erasing Old Hash File"
			,true
		);
	}
	Command("sudo dd if=/dev/urandom of=/tmp/"
		+*batch
		+"_File.dd count=100KB "
		,"Making Input File..."
		,true
	);
	Command("sudo dd if=/tmp/"+*batch+"_File.dd of="
		+this->path
		+" count=100KB "
		,std::string("Copying Input File to Disk.."),true
	);
}
void HDD::dd_read(std::string* batch)
{
	Command("sudo dd if="
		+this->path
		+" of=/tmp/"
		+*batch
		+"_FileRead.dd count=100KB "
		,"Making Output File from Disk...",true
	);
}
void HDD::hash_check(std::string* batch)
{
	Command("md5sum /tmp/"
		+*batch
		+"_File.dd"
		,"Hashing Input File...",true
	);
	std::string MainHash=LastOutput.substr(0,32);
	Command(
	"md5sum /tmp/"
		+*batch 
		+"_FileRead.dd"
		,"Hashing Output File...",true
	);
	std::string ReadHash= LastOutput.substr(0,32);
	if(MainHash!=ReadHash){
		throw (std::string) path+"hash rw failure...";
	}
	else{*dstream<<path<<" : Hashes are the Same"<<std::endl;}
}
void HDD::erase(std::string * method=new std::string("zero"))
{  	
	std::string TempName("");
	TempName.append("Temp_");
	TempName.append( this->path.substr(this->path.size()-1,1) );
	TempName.append(".txt");
	this->TempLogFileName=TempName;
	*dstream<<"templogfilename : "<<TempLogFileName<<std::endl;
	Command("sudo rm "+TempLogFileName, "erasing old temporay log file, if it exists",false);
	Command("sudo touch "+TempLogFileName, "touching new temporary log file",true);
	Command("sudo ./nwipe --autonuke --nogui --method="
		+*method		
		+" -l"
		+TempLogFileName
		+ " "
		+this->path	
		,"Erasing With Nwipe...",true
	);
}
void HDD::erase()
{
	erase_c();

	//erase(new std::string("zero"));
}
void HDD::erase_c(){
	PresentTask="Erasing Drive With ofstream";
	std::ofstream drive(path.c_str(),std::ostream::out);
	unsigned char pattern=0x00;
	if(!drive) throw "cannot open HDD";
	else *dstream<<path<<" : opened drive and erasing"<<std::endl;
	for(int i=0;i<1000&&Present;i++)
	{
		drive<<pattern;
	}
	drive.close();
}
void HDD::erase_debrief(){
	Command("sudo cat "+TempLogFileName,"debriefing, retreiving log file contents",true);
	if(LastOutput.find("Failure")!=std::string::npos)
	{
		PresentTask= "Finished Erasing, Failed";
		Status=FinishedFail;
	}
	else if(
		(LastOutput.find("Success")!=std::string::npos)
		||(LastOutput.find("verified")!=std::string::npos)
		||(LastOutput.find("Blanked Device")!=std::string::npos)
	){
		PresentTask="Finished Erasing, Success";
		Status=FinishedSuccess;
	}
	Command("sudo rm "+TempLogFileName, "erasing temporay log file...",false);
}
//trouble with default pointer types; need to override function
void HDD::print(std::ostream* textgohere=&std::cout){
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
	*textgohere<<"Last Output : "<<this->LastOutput<<std::endl;
	*textgohere<<"Exit Status : "<<this->LastExitStatus<<std::endl;
	*textgohere<<"Start Time: "<<this->StartTime<<std::endl;
	*textgohere<<"End Time: "<<this->EndTime<<std::endl;
	*textgohere<<"Run Time: "<<this->RunTime<<std::endl;
	*textgohere<<"Result: "
		<<ResultTToString(this->Status)
		<<std::endl;	*textgohere<<"______________________________________________________"<<std::endl;
}
///*
void HDD::print(){
	print(&std::cout);
}
//*/
void HDD::log(std::string * batch){
	std::string filename="/home/hdd-test-server/HDD_logs/"
		+(*batch)+".log";
	this->PresentTask ="Writing to the log file:"+filename;	
	std::fstream* LogFile
		= new std::fstream(filename,std::ios::app);
	*dstream<<this->path<<" :  "<<this->PresentTask<<std::endl;
	*dstream<<this->path<<" : log file is open:"<<LogFile->is_open()<<std::endl;
	print(dstream);
	print(LogFile);
}
void HDD::partition()
{
	Command(
		"sudo smar...  wait I'm skipping you anyway"
		,"Partitioning Drives...",true
	);
}
