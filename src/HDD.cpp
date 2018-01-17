
#include "HDD.h"
#include "Exceptions.h"
#include "methods.h"
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

using namespace std;

int HDD::instances;
/* // might be more usefull than throwing strings
struct Exception : public exception{
	const char * what() const throw(){
		return "C++";
	}
}
*/

string ResultTToString(Result_t a)
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
string HDD::StdOut(string cmd, bool throwing=true) {
    string data;
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


void HDD::Command(string a,string task,bool throwing=true){
	PresentTask=task;
	*dstream<<path<<" : "<<PresentTask<<endl;
	Command(a,throwing);
	*dstream<<this->path<<" : "<<this->PresentTask<<" done "<<endl;
}
void HDD::Command(string a,bool throwing=true){
	a.append(" 2>&1");
	CmdString=a;
	*dstream<<path<<" : Last Command:"<<CmdString<<endl;
	this->LastOutput=StdOut(this->CmdString,throwing);
	
	*dstream<<this->path<<" : Last Output:"<<this->LastOutput
	<<"_::exit status :"<<LastExitStatus<<endl;	
}
void HDD::exception_catch(exception e){
	exception_catch(e.what());
}
void HDD::exception_catch(const char *e){
		//string * a= new string(*e);
		*dstream<<path<<"const char thrown :"<<e<<endl;
		exception_catch((string)e);
}
void HDD::exception_catch(string e){
	this->Exception= e;
	this->PresentTask= " Critical error, stopping. ";
	this->Status=FinishedFail;		
	*dstream<<this->path<<"#####################WARNING#####################"<<endl;
	*dstream<<this->path<<" : Exception thrown : "<<this->Exception<<endl;	
	*dstream<<this->path<<" : PresentTask : "<<this->PresentTask<<endl;
	*dstream<<this->path<<" : LastCommand : "<<this->CmdString<<endl;	
	*dstream<<this->path<<" : LastOutput : "<<this->LastOutput<<endl;
	*dstream<<this->path<<"#################################################"<<endl;
}
void HDD::run(string* batch,char pattern){
	//thread a(&HDD::presence_checker,this,false);
	while(1){
		*dstream<<this->path<<" : beginning running loop... "<< endl;
		//sleep(1);
		PresentTask="reseting";
		*dstream<<this->path<<" : resetting... "<<endl;
		reset();
		PresentTask="waiting to detect...";
		*dstream<<this->path<<" : "<<this->PresentTask<<endl;
		while(!presence()){
			sleep(5);
		}
		//PresentTask="reseting";
		//*dstream<<this->path<<" : resetting... "<<endl;
		//reset();
		/*BEGIN TRY BLOCK*/
		try{	
			run_body(batch,pattern);
			this->Status=FinishedSuccess;
			print(dstream);
			//log(batch);
		}
		catch(string e){
			exception_catch(e);
		}
		catch(const char * e){
			exception_catch(e);
		}
		catch(exception e){
			exception_catch(e);
		}
		*dstream<<this->path<<"closing fd"<<endl;
		close(fd);
		/*END TRY BLOCK*/
		*dstream<<this->path<<" : end of run_body"<<endl;

		while(presence())
		{
				sleep(10);
		}
		//break;
		*dstream<<this->path<<" : pulled out starting over..."<<endl<<endl;
	}
	*dstream<<path<<" : we did it out of the loop"<<endl;;
}
void HDD::run_body(string* batch,char pattern){
	*dstream<<"beginning run_body"<<endl;
	this->StartTime=time(0);
	get_data();
	if(!presence()){return ;}
	#ifndef _Skip_Smart
	//TODO stop already running smartctl?
	//TODO handle (41) self test interrupted
	smartctl_run();
	//bool a=false;
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
	*dstream<<this->path<<"closing fd.. "<<endl;
	close(fd);
	*dstream<<this->path<<" : end of erase: writing to logs"<<endl;
	log(batch);


}
void HDD::reset(){
	
	this->SmartSupport=false;
	this->Present=false;
	if(Present) this->fd=open(path.c_str(),O_RDWR);//open?
	this->Exception="none";
	this->SerialNumber="";
	this->Model="";
	this->ModelFamily="";
	this->size=0;
this->SmartEta="";
	
	this->StartTime=-1;//time(0);
	this->EndTime=-1;
	this->RunTime=0;
	this->CmdString="";
	this->LastOutput="";
	this->LastExitStatus=0;
	this->Status=Unfinished;
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
	string temp= LastOutput.substr(18,9);
	this->SmartSupport=(temp=="Available");	
	
	*dstream<<this->path<<" : SmartSupport : "<<temp<<" : "<<this->SmartSupport<<endl; 
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
		string a=LastOutput.substr(18,25);
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
	*dstream<<"Data Extracted..."<<endl;
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
	catch(exception e)
	{

		*dstream<<"oops bad substring"<<endl;
		*dstream<<LastOutput.size()<<" : "<<LastOutput.find("Please Wait")<<endl;
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
	string code=LastOutput.substr(34,4);
	//only possible problem would be if " 240" is an error
	// it would return still running
	if (code[0]==' '&&code[1]=='2'&& code[2]=='4') return true;
	bool done=code=="   0";
	*dstream<<this->path<<" : smartctl: "<<((!done)?"is running":" has stopped ")<<" code :"<<code<<endl;
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


int HDD::smart_var( int & var,string name){
	string output;
	Command(
		"sudo smartctl -A "+path+" | awk '/"+name+"/' ","checking smart variable "+name,true
	);
	if(LastOutput!=""){
		output=LastOutput.substr(85,6);	
		*(dstream)<<"output"<<output<<endl;
		try{
			var=stoi(output);
		}
		catch(exception e){ var=0;return 0;}
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
void HDD::dd(string * batch){
	string hashfile="/tmp/"+*batch+"_"+path.substr(5,3)+"_File.dd";
	string outputfile="/tmp/"+*batch+"_"+path.substr(5,3)+"_FileRead.dd";
	dd_write(batch,hashfile);
	dd_read(batch,outputfile);
	hash_check(batch,hashfile,outputfile);
}
void HDD::dd_write(string* batch,string hashfile)
{
	if(access( hashfile.c_str(),0 )==0){
		*dstream<<this->path<<" : old hash exists: "<<endl;
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
		,string("Copying Input File to Disk.."),true
	);
}
void HDD::dd_read(string* batch,string outputfile)
{
	Command("sudo dd if="
		+this->path
		+" of="
		+outputfile
		+" count=100KB "
		,"Making Output File from Disk...",true
	);
}
void HDD::hash_check(string* batch,string hashfile,string outputfile)
{
	Command("md5sum "
		+hashfile
		,"Hashing Input File...",true
	);
	string MainHash=LastOutput.substr(0,32);
	Command(
	"md5sum "+outputfile
		,"Hashing Output File...",true
	);
	string ReadHash= LastOutput.substr(0,32);
	if(MainHash!=ReadHash){
		throw  "hash rw failure...";
	}
	else{*dstream<<path<<" : Hashes are the Same"<<endl;}
	Command("sudo rm "+hashfile+" "+outputfile," Erasing Output and Input files...", true);
}
void HDD::resolve_size(){
	PresentTask="Resolving sizes...";
	
	/* make sure that program can access whole drive*/
	ifstream in(path.c_str(),istream::in);
	in.seekg(0,ios_base::end);
	long size1= in.tellg();
	in.close();

	if(size!=size1) throw "cannot resolve size";	

	*dstream<<path <<" : sizes detected: "<<size<< " : "<<size1<<endl;
	/* proceed with wiping*/
	/**/
	/* writes all single character*/
	//97=a currently d0
	PresentTask="Erasing ....";
}
//trouble with default pointer types; need to override function
void HDD::print(ostream* textgohere=&cout){
	UpdateRunTime();
	//TODO add info on which client is running
	*textgohere<<"Status of: "<<this->path<<endl;
	*textgohere<<"Presence :    "<<((this->Present)?"detected":"undetected")<<endl;
	*textgohere<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<endl;
	*textgohere<<"Model Family: "<<this->ModelFamily<<endl;
	*textgohere<<"Model  #: "<<this->Model<<endl;
	*textgohere<<"Serial #: "<<this->SerialNumber<<endl;
	*textgohere<<"User Capacity: "<<SizeToString(size)<<endl;
	*textgohere<<"Present Task: "<<this->PresentTask<<endl;

	if(PresentTask=="Running Smart Control..."||PresentTask=="Checking Smart Control is still running...")
	{
		*textgohere<<SmartEta<<endl;
		*textgohere<<"Last Output : "<<this->LastOutput<<endl;
	}
	else if(this->Exception!="none"){
		*textgohere<<"Last Exception : "<<this->Exception<<endl;
		*textgohere<<"Last/Current Command :" << this->CmdString<<endl;
		*textgohere<<"Last Output : "<<this->LastOutput<<endl;
		*textgohere<<"Exit Status : "<<this->LastExitStatus<<endl;	
	}
	*textgohere<<"Result: "
		<<ResultTToString(this->Status)
		<<endl;			*textgohere<<"______________________________________"<<endl;
	*textgohere<<"##end##"<<endl;

}
///*
void HDD::print(){
	print(&cout);
}
void HDD::print_csv(std::fstream * textgohere){

	/* appends csv file of batch file*/

	/* format is client HomePath,Model fam,model,serial,capacity,client,start time,percent complete,runtime,errors/n*/


	//*textgohere<<"Status of: "<<this->path<<endl;
	//*textgohere<<"Presence :    "<<((this->Present)?"detected":"undetected")<<endl;
	//*textgohere<<"Smart Support: "<<(this->SmartSupport?"available":"unavailable")<<endl;
	*textgohere<<this->HomePath<<",";	
	*textgohere<<this->ModelFamily<<",";
	*textgohere<<this->Model<<",";
	*textgohere<<this->SerialNumber<<",";
	*textgohere<<this->size<<",";
//	*textgohere<<this->PresentTask<<",";

	*textgohere<<(ctime(&StartTime))<<",";//<<endl;
	//if(EndTime>0)*textgohere<<"End Time: "<<this->EndTime<<endl;
	*textgohere<<this->RunTime<<",";//endl;
	//*textgohere<<"Erasing "<<(currentLBA*1.0/size)*100<<"% Complete"<<endl;
	//*textgohere<<"ETA: "<<(eta/3600)<<"hours "<<((eta%3600)/60)<<" min(s) "<<(eta%60)<<"second(s)"<<endl;
	//if(this->Exception!="none"){
	*textgohere<<this->Exception<<",";
		//*textgohere<<"Last/Current Command :" << this->CmdString<<endl;
		//*textgohere<<"Last Output : "<<this->LastOutput<<endl;
		//*textgohere<<"Exit Status : "<<this->LastExitStatus<<endl;	
	*textgohere
		<<ResultTToString(this->Status)
		<<","
		<<endl;
}
//*/
void HDD::log(string * batch){
	string filename="/home/hdd-test-server/HDD_logs/"
		+(*batch)+".csv";
	this->PresentTask="Writing to the log file:"+filename;	
	std::fstream* LogFile= new std::fstream(filename,std::ios::app);
	print_csv(LogFile);
	*dstream<<this->path<<" :  "<<this->PresentTask<<endl;
	*dstream<<this->path<<" : log file is open:"<<LogFile->is_open()<<endl;
	//print(dstream);
	//print(LogFile);
}

