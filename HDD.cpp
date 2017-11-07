
#include "HDD.h"
#include "Exceptions.h"
#include "methods.cpp"
#include <stdio.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <thread>
#include <exception>
#include <string>
#include <unistd.h> //write()
#include <sys/stat.h>//O_RDWR?
#include <sys/types.h>//open();close()[ithink];

#include <stdint.h>//u64 in nwipe_static_pass?
#include <fcntl.h>
#include <time.h>

int HDD::instances;
/* // might be more usefull than throwing strings
struct Exception : public exception{
	const char * what() const throw(){
		return "C++";
	}
}
*/

std::string ResultTToString(Result_t a)
{
	switch(a)
	{
		case Unfinished: return "Unfinished :|";
		case FinishedSuccess: return "Finished, Success :)";
		case FinishedFail: return "Finished, Failure :(";
		case Incomplete : return " Program ended, Incomplete ";
		default: return "Unfinished";
	}
	return "Unfinished";
}
std::string HDD::StdOut(std::string cmd, bool throwing=true) {
    std::string data;
    FILE * stream;
    const int max_buffer= 256;
    char buffer[max_buffer];
    stream= popen(cmd.c_str(), "r");
    if(stream) {
        while(!feof(stream)&& Present)
            if(fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
	LastExitStatus=pclose(stream);
        if ((LastExitStatus!=0)&&throwing)throw  "Last System call returned not 0";
    }
    return data;
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
void HDD::exception_catch(std::exception e){
	exception_catch(e.what());
}
void HDD::exception_catch(const char *e){
		//std::string * a= new std::string(*e);
		*dstream<<path<<"const char thrown :"<<e<<std::endl;
		exception_catch((std::string)e);
}
void HDD::exception_catch(std::string e){
	this->Exception= e;
	this->PresentTask= " Critical error, stopping. ";
	this->Status=FinishedFail;		
	*dstream<<this->path<<"#####################WARNING#####################"<<std::endl;
	*dstream<<this->path<<" : Exception thrown : "<<this->Exception<<std::endl;	
	*dstream<<this->path<<" : PresentTask : "<<this->PresentTask<<std::endl;
	*dstream<<this->path<<" : LastCommand : "<<this->CmdString<<std::endl;	
	*dstream<<this->path<<" : LastOutput : "<<this->LastOutput<<std::endl;
	*dstream<<this->path<<"#################################################"<<std::endl;
}
void HDD::run(std::string* batch,char pattern){
	//std::thread a(&HDD::presence_checker,this,false);
	while(1){
		*dstream<<this->path<<" : beginning running loop... "<< std::endl;
		//sleep(1);
		PresentTask="reseting";
		*dstream<<this->path<<" : resetting... "<<std::endl;
		reset();
		PresentTask="waiting to detect...";
		*dstream<<this->path<<" : "<<this->PresentTask<<std::endl;
		while(!presence()){
			sleep(5);
		}
		//PresentTask="reseting";
		//*dstream<<this->path<<" : resetting... "<<std::endl;
		//reset();
		/*BEGIN TRY BLOCK*/
		try{	
			run_body(batch,pattern);
			this->Status=FinishedSuccess;
			print(dstream);
			//log(batch);
		}
		catch(std::string e){
			exception_catch(e);
		}
		catch(const char * e){
			exception_catch(e);
		}
		catch(std::exception e){
			exception_catch(e);
		}
		*dstream<<this->path<<"closing fd"<<std::endl;
		close(fd);
		/*END TRY BLOCK*/
		*dstream<<this->path<<" : end of run_body"<<std::endl;

		while(presence())
		{
				sleep(10);
		}
		//break;
		*dstream<<this->path<<" : pulled out starting over..."<<std::endl<<std::endl;
	}
	*dstream<<path<<" : we did it out of the loop"<<std::endl;;
}
void HDD::run_body(std::string* batch,char pattern){
	*dstream<<"beginning run_body"<<std::endl;
	this->StartTime=time(0);
	get_data();
	if(!presence()){return ;}
	#ifndef _Skip_Smart
	//TODO stop already running smartctl?
	//TODO handle (41) self test interrupted
	smartctl_run();
	bool a=false;
	/* smart ctl*/
	/*
	while(smartctl_running()){
		if(!presence()){
			a=true;
			smartctl_kill();
			break;
		}
		sleep(10);		
	}
	*/
	while(presence(true))
	{
		sleep(5);
		if(!smartctl_running()) {
			break;
		}
		sleep(5);
	}
	

	#endif
	/* back blaze test*/
	if(bb_test()) throw " >0 of smart 5;187;188;197;198 is >0  : drive likely to fail soon";

	///* confirm read write
	if(!this->presence()){return;}
		this->dd(batch); //TODO add back in
	if(!this->presence()){return;}
	//*/
	resolve_size();
	if(!this->presence()){return;}
	//erase
	#ifdef _Erase
	erase(pattern);		
	if(!this->presence()){return;}
	#endif

	this->EndTime=time(0);
	if(!this->presence()){return;}
	*dstream<<this->path<<"closing fd.. "<<std::endl;
	close(fd);
	*dstream<<this->path<<" : end of erase: writing to logs"<<std::endl;
	log(batch);


}
void HDD::reset(){
	
	this->SmartSupport=false;
	this->Present=false;
	if(Present) this->fd=open(path.c_str(),O_RDWR);//std::open?
	this->Exception="none";
	this->SerialNumber="";
	this->Model="";
	this->ModelFamily="";
	this->size=0;
this->SmartEta="";
	this->currentLBA=0;
	this->StartTime=-1;//time(0);
	this->EndTime=-1;
	this->RunTime=0;
	this->CmdString="";
	this->LastOutput="";
	this->LastExitStatus=0;
	this->Status=Unfinished;
}
///*
bool HDD::presence(){
	return presence(false);
}
//*/
bool HDD::presence(bool print ){
	//print for explicit
	if(print) *dstream<<this->path<<" : checking presence... "<<std::endl;
	
	this->Present=
		access( this->path.c_str(),0 )==0;
		
	if(print) *dstream<<this->path<<" : presence "<<((this->Present)?"detected":"not detected")<<std::endl;
	if(!Present) close(this->fd);
	return this->Present;
}
void HDD::Presence_checker(){
	Presence_checker(false);
}
void HDD::Presence_checker(bool throwing){
	while(1){
		if (!presence()&&throwing )throw "Hard Drive Unplugged";
		sleep(10);
	}
}
void HDD::get_data(){
	/*  gets
	SmartSupport
	ModelFamily
	Model
	Serial #
	Capacity
		*/
	//fd is in reset() as well, this is just in case it changes;
	if(Present) this->fd=open(path.c_str(),O_RDWR);//std::open?
	Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/SMART support is:/' | sed -n '1,1p'"
		,"getting data...",true
	);
	std::string temp= LastOutput.substr(18,9);
	this->SmartSupport=(temp=="Available");	
	
	*dstream<<this->path<<" : SmartSupport : "<<temp<<" : "<<this->SmartSupport<<std::endl; 
	 Command(
		"sudo smartctl -i "
		+this->path
		+" | awk '/Model Family:/'",true
	);
	if(LastOutput!=""){
		this->ModelFamily=LastOutput.substr(18,35);
			
	}
	else
	{
		this->ModelFamily="none detected";
	}

	this->Model= StdOut(
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
		std::string a=LastOutput.substr(18,25);
		this->size= myStol(a);
	}
	else
	{
		this->size=-1;
		throw " no capacity detected";
	}
	
	trim(this->ModelFamily);
	trim(SerialNumber);
	trim(Model);
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
	try{	

		SmartEta=this->LastOutput.substr(LastOutput.find("Please wait"),90);
	}
	catch(std::exception e)
	{

		*dstream<<"oops bad substring"<<std::endl;
		*dstream<<LastOutput.size()<<" : "<<LastOutput.find("Please Wait")<<std::endl;
	}
	sleep(2);
}

bool HDD::smartctl_running()
{
	Command("sudo smartctl -a "
		+this->path
		+" | awk '/Self-test execution status:/' "
		,"Checking Smart Control is still running...",true
	);
	std::string code=LastOutput.substr(34,4);
	//only possible problem would be if " 240" is an error
	// it would return still running
	if (code[0]==' '&&code[1]=='2'&& code[2]=='4') return true;
	bool done=code=="   0";
	*dstream<<this->path<<" : smartctl: "<<((!done)?"is running":" has stopped ")<<" code :"<<code<<std::endl;
	if (done) return false;
	throw " smartctl error test runtime  "+code;
}
void HDD::smartctl_kill()
{
	Command("sudo smartctl -X "
		+this->path
		,"Checking Smart Control...",true
	);
}


int HDD::smart_var( int & var,std::string name){
	std::string output;
	Command(
		"sudo smartctl -A "+path+" | awk '/"+name+"/' ","checking smart variable "+name,true
	);
	if(LastOutput!=""){
		output=LastOutput.substr(85,6);	
		*(dstream)<<"output"<<output<<std::endl;
		try{
			var=stoi(output);
		}
		catch(std::exception e){ var=0;return 0;}
		return 1;
	}
	var=0;return 0;
}
bool HDD::bb_test(){
	/*back blaze test*/
	int smart_5;
	int smart_187;
	int smart_188;
	int smart_197;
	int smart_198;
	if(!smart_var(smart_5,"5 Reallocated_Sector_Ct"))
		smart_var(smart_5,"5 Retired_Block_Count");
	smart_var(smart_187,"187 Reported_Uncorrect");	
	smart_var(smart_188,"188 Command_Timeout");
	smart_var(smart_197,"197 Current_Pending_Sector");
	smart_var(smart_198,"198 Offline_Uncorrectable");
	/*jsut for testing*///return 1;
	return smart_5 || smart_187 || smart_188 || smart_197 || smart_198;
	//returns 0 if passed 1 if failed
/*
	local bb_5="$(sudo smartctl -A /dev/${1} | awk '/5 Reallocated_Sector_Ct/' | awk '{print substr($0,85,6)}')";
	local bb_187="$(sudo smartctl -A /dev/${1} | awk '/187 Reported_Uncorrect/' | awk '{print substr($0,85,6)}')";
	local bb_188="$(sudo smartctl -A /dev/${1} | awk '/188 Command_Timeout/'| awk '{ print substr($0,85,6)}')";
	local bb_197="$(sudo smartctl -A /dev/${1} | awk '/197 Current_Pending_Sector/'| awk '{ print substr($0,85,6)}')";
	local bb_198="$(sudo smartctl -A /dev/${1} | awk '/198 Offline_Uncorrectable/' | awk '{ print substr($0,85,6)}')";
*/
}
void HDD::dd(std::string * batch){
	std::string hashfile="/tmp/"+*batch+"_"+path.substr(5,3)+"_File.dd";
	std::string outputfile="/tmp/"+*batch+"_"+path.substr(5,3)+"_FileRead.dd";
	dd_write(batch,hashfile);
	dd_read(batch,outputfile);
	hash_check(batch,hashfile,outputfile);
}
void HDD::dd_write(std::string* batch,std::string hashfile)
{
	if(access( hashfile.c_str(),0 )==0){
		*dstream<<this->path<<" : old hash exists: "<<std::endl;
		Command("sudo rm "
			+hashfile
			,"Erasing Old Hash File"
			,true
		);
	}
	Command("sudo dd if=/dev/urandom of="
		+hashfile
		+" count=100KB "
		,"Making Input File..."
		,true
	);
	Command("sudo dd if="
		+hashfile
		+" of="
		+this->path
		+" count=100KB "
		,std::string("Copying Input File to Disk.."),true
	);
}
void HDD::dd_read(std::string* batch,std::string outputfile)
{
	Command("sudo dd if="
		+this->path
		+" of="
		+outputfile
		+" count=100KB "
		,"Making Output File from Disk...",true
	);
}
void HDD::hash_check(std::string* batch,std::string hashfile,std::string outputfile)
{
	Command("md5sum "
		+hashfile
		,"Hashing Input File...",true
	);
	std::string MainHash=LastOutput.substr(0,32);
	Command(
	"md5sum "+outputfile
		,"Hashing Output File...",true
	);
	std::string ReadHash= LastOutput.substr(0,32);
	if(MainHash!=ReadHash){
		throw  "hash rw failure...";
	}
	else{*dstream<<path<<" : Hashes are the Same"<<std::endl;}
	Command("sudo rm "+hashfile+" "+outputfile," Erasing Output and Input files...", true);
}
void HDD::resolve_size(){
	PresentTask="Resolving sizes...";
	
	/* make sure that program can access whole drive*/
	std::ifstream in(path.c_str(),std::istream::in);
	in.seekg(0,std::ios_base::end);
	long size1= in.tellg();
	in.close();

	if(size!=size1) throw "cannot resolve size";	

	*dstream<<path <<" : sizes detected: "<<size<< " : "<<size1<<std::endl;
	/* proceed with wiping*/
	/**/
	/* writes all single character*/
	//97=a currently d0
	PresentTask="Erasing ....";
}

void print_time();

//trouble with default pointer types; need to override function
void HDD::print(std::ostream* textgohere=&std::cout){
	UpdateRunTime();
	//TODO add info on which client is running
	*textgohere<<"Status of: "<<this->path<<std::endl;
	*textgohere<<"Presence :    "<<((this->Present)?"detected":"undetected")<<std::endl;
	*textgohere<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<std::endl;
	*textgohere<<"Model Family: "<<this->ModelFamily<<std::endl;
	*textgohere<<"Model  #: "<<this->Model<<std::endl;
	*textgohere<<"Serial #: "<<this->SerialNumber<<std::endl;
	*textgohere<<"User Capacity: "<<SizeToString(size)<<std::endl;
	*textgohere<<"Present Task: "<<this->PresentTask<<std::endl;

	if(PresentTask=="Erasing..."){
		*textgohere<<"Start Time: "<<(ctime(&StartTime));//<<std::endl;
		if(EndTime>0)*textgohere<<"End Time: "<<this->EndTime<<std::endl;
		*textgohere<<"Run Time: "<<(this->RunTime/3600)<<"hours "<<((this->RunTime%3600)/60)<<" min(s) "<<(this->RunTime%60)<<"second(s)"<<std::endl;
		//*textgohere<<"Run Time: "<<this->RunTime<<std::endl;
		*textgohere<<"Erasing "<<(currentLBA*1.0/size)*100<<"% Complete"<<std::endl;
		*textgohere<<"ETA: "<<(eta/3600)<<"hours "<<((eta%3600)/60)<<" min(s) "<<(eta%60)<<"second(s)"<<std::endl;
	}

	else if(PresentTask=="Running Smart Control..."||PresentTask=="Checking Smart Control is still running...")
	{
		*textgohere<<SmartEta<<std::endl;
		*textgohere<<"Last Output : "<<this->LastOutput<<std::endl;
	}

	else if(this->Exception!="none"){
		*textgohere<<"Last Exception : "<<this->Exception<<std::endl;
		*textgohere<<"Last/Current Command :" << this->CmdString<<std::endl;
		*textgohere<<"Last Output : "<<this->LastOutput<<std::endl;
		*textgohere<<"Exit Status : "<<this->LastExitStatus<<std::endl;	
	}
	*textgohere<<"Result: "
		<<ResultTToString(this->Status)
		<<std::endl;			*textgohere<<"______________________________________"<<std::endl;
*textgohere<<"##end##"<<std::endl;

}
///*
void HDD::print(){
	print(&std::cout);
}
void HDD::print_csv(std::fstream * textgohere){

	/* appends csv file of batch file*/

	/* format is client HomePath,Model fam,model,serial,capacity,client,start time,percent complete,runtime,errors/n*/


	//*textgohere<<"Status of: "<<this->path<<std::endl;
	//*textgohere<<"Presence :    "<<((this->Present)?"detected":"undetected")<<std::endl;
	//*textgohere<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<std::endl;
	*textgohere<<this->HomePath<<",";	
	*textgohere<<this->ModelFamily<<",";
	*textgohere<<this->Model<<",";
	*textgohere<<this->SerialNumber<<",";
	*textgohere<<this->size<<",";
//	*textgohere<<this->PresentTask<<",";

	*textgohere<<(ctime(&StartTime))<<",";//<<std::endl;
	//if(EndTime>0)*textgohere<<"End Time: "<<this->EndTime<<std::endl;
	*textgohere<<this->RunTime<<",";//std::endl;
	//*textgohere<<"Erasing "<<(currentLBA*1.0/size)*100<<"% Complete"<<std::endl;
	//*textgohere<<"ETA: "<<(eta/3600)<<"hours "<<((eta%3600)/60)<<" min(s) "<<(eta%60)<<"second(s)"<<std::endl;
	//if(this->Exception!="none"){
	*textgohere<<this->Exception<<",";
		//*textgohere<<"Last/Current Command :" << this->CmdString<<std::endl;
		//*textgohere<<"Last Output : "<<this->LastOutput<<std::endl;
		//*textgohere<<"Exit Status : "<<this->LastExitStatus<<std::endl;	
	*textgohere
		<<ResultTToString(this->Status)
		<<","
		<<std::endl;
}
//*/
void HDD::log(std::string * batch){
	std::string filename="/home/hdd-test-server/HDD_logs/"
		+(*batch)+".csv";
	this->PresentTask="Writing to the log file:"+filename;	
	std::fstream* LogFile= new std::fstream(filename,std::ios::app);
	print_csv(LogFile);
	*dstream<<this->path<<" :  "<<this->PresentTask<<std::endl;
	*dstream<<this->path<<" : log file is open:"<<LogFile->is_open()<<std::endl;
	//print(dstream);
	//print(LogFile);
}

