#include "HDD.h"
#include <stdio.h>
#include <fstream>
#include <ostream>
#include <iostream>
#include <thread>
#include <exception>
#include <string>
#include <unistd.h>
#include <time.h>

int HDD::instances;
/* // might be more usefull than throwing strings
struct Exception : public exception{
	const char * what() const throw(){
		return "C++";
	}
}
*/
std::string month(int i){
	switch(i){
		case 0:return "Jan";
		case 1:return "Feb";
		case 2:return "Mar";
		case 3:return "Apr";
		case 4:return "May";
		case 5:return "June";
		case 6:return "July";
		case 7:return "Aug";
		case 8:return "Sep";
		case 9:return "Oct";
		case 10:return "Nov";
		case 11:return "Dec";
		default:return "Jan";
	}
}
#ifndef Result_To_String
#define Result_To_String
std::string ResultTToString(Result_t a)
{
	switch(a)
	{
		case Unfinished: return "Unfinished :|";
		case FinishedSuccess: return "Finished, Success :)";
		case FinishedFail: return "Finished, Failure :(";
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
	{// unsigned to avoid warning. i guess its bad.
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
	this->PresentTask = " Critical error, stopping. ";
	this->Status=FinishedFail;		
	*dstream<<this->path<<"#####################WARNING#####################"<<std::endl;
	*dstream<<this->path<<" : Exception thrown : "<<this->Exception<<std::endl;	
	*dstream<<this->path<<" : PresentTask : "<<this->PresentTask<<std::endl;
	*dstream<<this->path<<" : LastCommand : "<<this->CmdString<<std::endl;	
	*dstream<<this->path<<" : LastOutput : "<<this->LastOutput<<std::endl;
	*dstream<<this->path<<"#################################################"<<std::endl;
}
//void HDD::run_body(std::string* batch);
void HDD::run(std::string* batch){
	//std::thread a(&HDD::presence_checker,this,false);
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
		/*BEGIN TRY BLOCK*/
		try{	
			run_body(batch);
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
		/*END TRY BLOCK*/
		*dstream<<this->path<<" : end of run, no pull out test"<<std::endl;

		break;
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
	this->dd(batch);
	if(!this->presence()){return;}
	//*/
	//obviously needs more work for different methods
	if(*batch=="random"){
		this->erase(batch);
	}
	else{
		erase();
	}		
	if(!this->presence()){return;}
	#endif
	
	this->EndTime=time(0);
	if(!this->presence()){return;}
	*dstream<<this->path<<" : end of erase: writing to logs";

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
		throw (std::string) path+"hash rw failure...";
	}
	else{*dstream<<path<<" : Hashes are the Same"<<std::endl;}
	Command("sudo rm "+hashfile+" "+outputfile," Erasing Output and Input files...", true);
}
void HDD::erase(std::string * method)
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
	///*
	try{
		erase_c();
//		erase(new std::string("zero");
		//	this->erase_debrief();
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

	
	//*/
}

void HDD::Write_All(unsigned char pattern =0x00,long begin=0,long end=0){
	if(!end)end=size;
	std::ofstream drive(path.c_str(),std::ostream::out);
	if(!drive) throw "cannot open HDD";
	//watch out for this line
	else *dstream<<path<<" : opened drive and writing "
		<<(pattern!=0?(const char *)&pattern:"zero")<<"s from "<<begin<<" to "<<end<<std::endl;
	int bs=512;
	unsigned char block[bs];
	for(int i=0;i<bs;i++) block[i]=pattern;
	time_t begin_t=time(0);
	tm * date=localtime(&begin);
	*dstream<<path<<" :start erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
	time_t current_t=time(0);
	time_t Last_t=time(0);
	time_t delta;
	long check=50000000;
	for(currentLBA=begin//size//*3199/3200
		;
		currentLBA<end&&Present;
		currentLBA+=bs
	    )
	{
		drive.seekp(currentLBA);
		drive<<block;
		if(currentLBA%check==0)
		{	
			current_t=time(0);					
			delta=current_t-Last_t; 
			
			*dstream<<path<< " : erasing : "<<(currentLBA/1000000)<<"MB /"<<(end-begin)/1000000<<" MB : "<<((currentLBA-begin)*1.0/(end-begin))*100<<std::endl;
			*dstream<<path<< " :ave speed : "<<(currentLBA/current_t-begin_t)<<"lba/ms : inst. speed "<<(check/delta)<<"lba/ms  inst. based eta : "<<((size-currentLBA)*delta/check/60000)<<" mins"<<std::endl;
			
			*dstream<<std::endl;
			Last_t=current_t;
			
		}
	}
	/*get end LBA's just in case remainder modulo bs*/
	currentLBA-=bs;
	for(
		;
		currentLBA<end&&Present;
		currentLBA++
	    )
	{
		drive.seekp(currentLBA);
		drive<<pattern;
		if(currentLBA%check==0)
		{	
			current_t=time(0);					
			delta=current_t-Last_t; 
			
			*dstream<<path<< " : erasing : "<<(currentLBA/1000000)<<"MB /"<<(end-begin)/1000000<<" MB : "<<((currentLBA-begin)*1.0/(end-begin))*100<<std::endl;
			*dstream<<path<< " :ave speed : "<<(currentLBA/current_t-begin_t)<<"lba/ms : inst. speed "<<(check/delta)<<"lba/ms  inst. based eta : "<<((size-currentLBA)*delta/check/60000)<<" mins"<<std::endl;
			
			*dstream<<std::endl;
			Last_t=current_t;
			
		}
	}

	*dstream<<path<< " : erasing : "<<currentLBA/1000000<<" MB /32,000 MB : "<<((currentLBA-begin)*1.0/(end-begin))*100<<std::endl;
	*dstream<<"finished erasing... obstensibly... closing file"<<std::endl;
	drive.close();
	*dstream<<"closed"<<std::endl;
	time_t end_t=time(0);
	date=localtime(&begin);
	*dstream<<path<<" :start erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;

	time_t diff=end_t-begin_t;
	date=localtime(&diff);
	*dstream<<path<<" erased"<<(begin-end)<<" bytes in... :time elapsed:  "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
}
bool HDD::Long_Verify(unsigned char pattern =0x00,long begin=0, long end=0){
	if(!end)end=size;
	std::ifstream idrive(path.c_str(),std::istream::in);
	if(!idrive) throw "cannot open HDD to read";
	else *dstream<<path<<" : opened drive and verifying all  "<<pattern<<"from "<<begin <<" to "<<end<<std::endl;
	char buffer[1];
	bool fail;
	for(long i=begin//*3199/3200
		;i<end&&Present
		;i++)
	{
		idrive.seekg(i);
		idrive.read(buffer,1);

		if(i%50000000==0) {
	
*dstream<<path<< " : checking : "<<(i-begin)/1000000<<" MB "<<"/"<<(end-begin)/1000000<<" MB : "<<((i-begin+1)*1.0/(end+1))*100<<" char :"<<buffer<<":";}
		if(buffer[0]!=pattern) {
			fail= true;
			break;
		}
	}
	//if(fail) // throw " shit ain't all right oops D: ";
	idrive.close();
	return !fail;//TODO change to return in for loop;
}
void HDD::erase_c(){
	//Working
	PresentTask="Erasing Drive With ofstream";
	
	/* make sure that program can access whole drive*/
	std::ifstream in(path.c_str(),std::istream::in);
	in.seekg(0,std::ios_base::end);
	long size1 =in.tellg();
	in.close();

	if(size!=size1) throw "cannot resolve size";	

	*dstream<<path <<" : sizes detected: "<<size<< " : "<<size1<<std::endl;
	/* proceed with wiping*/
	/**/
	/* writes all single character*/
	//97=a currently b
	this->Write_All(98);

}
void HDD::erase_dd(){
	Command(
	"sudo dd if=/dev/zero of="
	+path
	,"erasing with dd write and /dev/zero"	,true);
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
	*textgohere<<":Erasing "<<(currentLBA/size)*100<<"% Complete"<<std::endl;
	*textgohere<<"Result: "
		<<ResultTToString(this->Status)
		<<std::endl;	*textgohere<<"______________________________________________________"<<std::endl;
}
///*
void HDD::print(){
	//TODO move base function so defaults work
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
