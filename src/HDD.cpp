
#include "HDD.h"
//#include "Exceptions.h"
//#include "methods.h"
//#include <stdio.h>
#include <fstream>
//#include <ostream>
//#include <iostream>
//#include <thread>
//#include <exception>
//#include <string>
#include <unistd.h> //access()/// sleep()


#include <fcntl.h> //O_RDWR? // yes
//#include <stdint.h>//u64 in nwipe_static_pass?

//#include <time.h>

using namespace std;

HDD::HDD(std::string path) : HDD_Base(path), StartTime(time(0)){}
/* // might be more usefull than throwing strings
struct Exception : public exception{
	const char * what() const throw(){
		return "C++";
	}
}
*/

string HDD::ResultTToString(Result_t a)
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
void HDD::UpdateRunTime(){
	RunTime=time(0);
}


/* untested take out if bad */



void HDD::exception_catch(exception e){
	exception_catch(e.what());
}
void HDD::exception_catch(const char *e){
		//string * a= new string(*e);
		*p()<<"const char thrown :"<<e<<endl;
		exception_catch((string)e);
}
void HDD::exception_catch(string e){
	Exception= e;
	PresentTask= " Critical error, stopping. ";
	Status=FinishedFail;		
	*p()<<"#####################WARNING#####################"<<endl
		<<path<<" : Exception thrown : "<<Exception<<endl	
		<<path<<" : PresentTask : "<<PresentTask<<endl
		<<path<<" : LastCommand : "<<CmdString<<endl
		<<path<<" : LastOutput : "<<LastOutput<<endl
		<<path<<"#################################################"<<endl;
}

void HDD::reset(){
	
	SmartSupport=false;
	Present=false;
	if(Present) fd=open(path.c_str(),O_RDWR);//open?
	Exception="none";
	SerialNumber="";
	Model="";
	ModelFamily="";
	size=0;
	SmartEta="";
	
	StartTime=-1;//time(0);
	EndTime=-1;
	RunTime=0;
	CmdString="";
	LastOutput="";
	LastExitStatus=0;
	Status=Unfinished;
}

void HDD::smartctl_run()
{	
	smartctl_kill();
	Command(
		"sudo smartctl --device=auto --smart=on --saveauto=on --tolerance=normal --test=long "
		//+" -C"// no longer need smartctl_running
		+ path
		,"Running Smart Control...",true
	);
	try{	
		SmartEta=LastOutput.substr(LastOutput.find("Please wait"),90);
	}
	catch(exception e)
	{
		*dstream<<"oops bad substring"<<endl
				<<LastOutput.size()<<" : "<<LastOutput.find("Please Wait")<<endl;
	}
	sleep(2);
}

bool HDD::smartctl_running()
{
	Command("sudo smartctl -a "
		+path
		+" | awk '/Self-test execution status:/' "
		,"Checking Smart Control is still running...",true
	);
	string code=LastOutput.substr(34,4);
	//only possible problem would be if " 240" is an error
	// it would return still running
	if (code[0]==' '&&code[1]=='2'&& code[2]=='4') return true;
	bool done=code=="   0";
	*p()<<" : smartctl: "<<((!done)?"is running":" has stopped ")<<" code :"<<code<<endl;
	if (done) return false;
	throw " smartctl error test runtime  "+code;
}
void HDD::smartctl_kill()
{
	Command("sudo smartctl -X "
		+path
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
		puts(" : old hash exists: ");
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
		+path
		+" count=100KB "
		,string("Copying Input File to Disk.."),true
	);
}
void HDD::dd_read(string* batch,string outputfile)
{
	Command("sudo dd if="
		+path
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
	else{*p()<<" : Hashes are the Same"<<endl;}
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

	*p() <<" : sizes detected: "<<size<< " : "<<size1<<endl;
	/* proceed with wiping*/
	/**/
	/* writes all single character*/
	//97=a currently d0
	PresentTask="Erasing ....";
}
//trouble with default pointer types; need to override function
void HDD::print_help2(ostream* textgohere){
	*textgohere<<"Result: "
		<<ResultTToString(this->Status)<<endl
		<<"______________________________________"<<endl
		<<"##end##"<<endl;
}
void HDD::print_help(ostream* textgohere){
	if(PresentTask=="Running Smart Control..."||PresentTask=="Checking Smart Control is still running...")
	{
		*textgohere<<SmartEta<<endl
			<<"Last Output : "<<LastOutput<<endl;
	}
	else if(this->Exception!="none"){
		*textgohere<<"Last Exception : "<<this->Exception<<endl
			<<"Last/Current Command :" << CmdString<<endl
			<<"Last Output : "<<LastOutput<<endl
			<<"Exit Status : "<<LastExitStatus<<endl;	
	}
}
void HDD::print(ostream* textgohere=&cout){	
	UpdateRunTime();
	//TODO add info on which client is running
	*textgohere<<HDD_Base::str()
		<<"Present Task: "<<PresentTask<<endl;
	print_help(textgohere);
	print_help2(textgohere);
}
void HDD::print_csv(std::fstream * textgohere){
	/* appends csv file of batch file*/
	/* format is client HomePath,fam,model,serial,capacity,client,start time,percent complete,runtime,errors/n*/

	*textgohere<<HomePath<<","
		<<ModelFamily<<","
		<<Model<<","
		<<SerialNumber<<","
		<<size<<",";
	*textgohere<<(ctime(&StartTime))<<","
		<<RunTime<<","
		<<this->Exception<<","	
		<<ResultTToString(this->Status)
		<<","
		<<endl;
}
//*/
void HDD::log(string * batch){
	//don't believe that the program will be able to access this
	string filename="/home/hdd-test-server/HDD_logs/"
		+(*batch)+".csv";
	this->task("Writing to the log file:"+filename);	
	std::fstream* LogFile= new std::fstream(filename,std::ios::app);
	print_csv(LogFile);
	*p()<<" :  "<<PresentTask<<endl
			<<path<<" : log file is open:"<<LogFile->is_open()<<endl;
	//print(dstream);
	//print(LogFile);
}
