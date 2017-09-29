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
void HDD::erase(char pattern)
{
	///*
	try{
		//erase_c(pattern);
		erase_n(0x00);
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

void HDD::Write_All( char pattern =0x00,long begin =0,long end =0){
	if(!end)end=size;
	std::ofstream drive(path.c_str(),std::ostream::out);
	if(!drive) throw "cannot open HDD";
	//watch out for this line
	else *dstream<<path<<" : opened drive and writing "
		<<(pattern!=0?(const char *)&pattern:"zero")<<"s from "<<begin<<" to "<<end<<std::endl;
	const long bs=512;
	char block[bs];
	for(int i=0;i<bs;i++) block[i]=pattern;
	*dstream<<"block declared:"<<std::endl	;
	dstream->write(block,bs);
	*dstream<<":end"
		<<std::endl;
	time_t begin_t=time(0);
	tm * date=localtime(&begin_t);
	*dstream<<path<<" :start erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
	long lastLBA=-1;
	time_t Last_t=time(0)-1;
	time_t current_t=time(0);

	time_t delta_t=current_t-Last_t;
	time_t elapsed_t=current_t-begin_t;
	
	if(!elapsed_t)elapsed_t++;
	long v=1;       //1234567890 //10^10=10 mb
	long aV=1;       //1234567890 //10^10=10 mb
	eta=-1;//(end-currentLBA)/aV;
	long ieta=-1;
	long aeta=-1;
	const long check=10000000;//10mb
	for(currentLBA=begin//size//*3199/3200
		;
		currentLBA<end&&Present;
		currentLBA+=bs
	    )
	{
		//ERASE HERE TODO uncomment to erase
		//*dstream<<"beginL"<<std::endl;
		drive.seekp(currentLBA);
		//drive<<block;
		drive.write(block,bs);
		
		if(currentLBA%check==0||currentLBA/bs<9)
		{	
			if(!presence())throw "pulled out while erasing";
			if(currentLBA/bs<9)	
				*dstream<<"beggining blocks :"<<currentLBA<<" tellp "<<drive.tellp()<<std::endl;
			current_t=time(0);	
			delta_t=current_t-Last_t; 
			elapsed_t=current_t-begin_t;
			if(!elapsed_t)elapsed_t++;
			if(delta_t>0)v=(currentLBA-lastLBA)/delta_t;
			if(!v)v++;
			aV=(currentLBA-begin)/elapsed_t;
			if(!aV)aV++;
			ieta=(end-currentLBA)/v;
			aeta=(end-currentLBA)/aV;
			//dividing by: elapsed_t,v and constants
			*dstream<<path;
			*dstream<< " : erasing : ";
			*dstream<<(currentLBA/1000000)
				<<"MB /"<<(end-begin)/1000000
				<<" MB : "
				<<((currentLBA-begin)*1.0/(end-begin))*100
				<<" percent done"<<std::endl;
			*dstream<<path<< " : Ave speed : ";
			*dstream<<aV
				<<"LBA/sec : inst. speed "
				<<v;
			*dstream<<"LBA/sec  inst. based eta : "	;
			if(int a=(ieta/3600))
				*dstream<<(a)
					<<" hours ";
			*dstream<<(ieta/60)
				<<" min(s)"<<(ieta%60)<<" sec ";
			*dstream<<" avbased eta: "
				<<(aeta/60)
				<<" min(s)"<<(aeta%60)<<" sec "<<std::endl;
			eta=aeta;
			Last_t=current_t;
			lastLBA=currentLBA;
			//*dstream<<"end of if"<<std::endl;
		}
	
		//*dstream<<"endL :"<<currentLBA<<std::endl;
	}
	*dstream<<"out"<<std::endl;
	/*get end LBA's just in case remainder modulo bs!=0*/
	currentLBA-=bs;
	for(
		;
		currentLBA<end&&Present;
		currentLBA++
	    )
	{
		//drive.seekp(currentLBA);
		//drive.write(block,1);
		
		if(currentLBA%check==0)
		{	
			std::cout<<"tellg = "<<drive.tellp()<<std::endl;
			current_t=time(0);	
			delta_t=current_t-Last_t; 
			elapsed_t=current_t-begin_t;
			if(!elapsed_t)elapsed_t++;
			if(delta_t>0)v=(currentLBA-lastLBA)/delta_t;
			if(!v)v++;
			
			//dividing by: elapsed_t,v and constants
			*dstream<<path;
			*dstream<< " : erasing : ";
			*dstream<<(currentLBA/1000000)
				<<"MB /"<<(end-begin)/1000000
				<<" MB : "
				<<((currentLBA-begin)*1.0/(end-begin))*100
				<<" percent done"<<std::endl;
			*dstream<<path<<" delta_t: "<<delta_t<< "s : Ave speed : ";
			*dstream<<((currentLBA-begin)/elapsed_t)
				<<"LBA/sec : inst. speed "
				<<v;
			*dstream<<"LBA/sec  inst. based eta : "	
				<<(eta/60)<<" min"
				<<(eta%60)<<" sec"<<std::endl;
			
			Last_t=current_t;
			lastLBA=currentLBA;
			
		}
		
	}
	*dstream<<path<<" out coda"<<std::endl;

	*dstream<<path<< " : erasing : "
		<<currentLBA/1000000<<" MB /32,000 MB : "
		<<((currentLBA-begin)*1.0/(end-begin))*100<<std::endl;
	*dstream<<"finished erasing... obstensibly... closing file"
		<<std::endl;
	drive.close();
	*dstream<<"closed"<<std::endl;
	time_t end_t=time(0);
	*dstream<<"time(0);"<<std::endl;
	date=localtime(&begin_t);
	*dstream<<"date=local..;"<<std::endl;
	*dstream<<path<<" :started erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
	date=localtime(&end_t);
	*dstream<<path<<" :ended erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
	time_t diff_t=end_t-begin_t;
	date=localtime(&diff_t);
	*dstream<<path<<" erased"<<(begin-end)<<" bytes in... :time elapsed:  "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
}
bool HDD::Long_Verify(unsigned char pattern =0x00,long begin =0, long end =0){
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
void HDD::erase_c(char pattern){
	//Working
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
	this->Write_All(pattern,0,size);
	
}
void HDD::erase_n(char pattern){
	PresentTask="Erasing...";
	time_t begin_t=time(0);
	tm * date=localtime(&begin_t);
	*dstream<<path<<" :start erasing w/erase_n:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
/* The result holder. */
	int r;

	/* The IO size. */
	size_t blocksize=512;

	/* The result buffer for calls to lseek. */
	off64_t offset;

	/* The output buffer. */
	char* b;

	/* A pointer into the output buffer. */
	char* p;

	/* The output buffer window offset. */
	int w= 0;

	/* The number of bytes remaining in the pass. */
	unsigned long z=size; //originally unsigned long long in nwipe
	int errors=0; 		//should use long long for size everywhere?
	this->fd=open(path.c_str(),O_RDWR);//just incase reset didn't get it because of no presence or somethign
//* tottaly copy pasted from nwipe's github
	
	/*if( pattern == NULL )
	{
		// Caught insanity. 
		*dstream<<"null pattern pointer"<<std::endl;
		return -1;
	}
	*///will never happen pattern not pointer in this code
	/*if( pattern->length <= 0 )
	{
		// Caught insanity. 
		*dstream<<"__FUNCTION__: The pattern length member is "<<, pattern->length<<std::endl ;
		return -1;
	}*///will never happen

	// Create the output buffer. 
	b= (char *) malloc( blocksize  +2 );

	// Check the memory allocation. 
	if( ! b )
	{
		*dstream<<"unable to create buffer"<<std::endl;
		throw "unable to create buffer" ;//return;//return -1;
	}

	for( p= b ; p < b + 512  + 1 ; p += 1 )
	{
		// Fill the output buffer with the pattern.
		//memcpy( p, pattern, 1 ); 
		*p=pattern;
	}
///
	// Reset the file pointer. 
	*dstream<<"filedescriptor int fd = "<<fd<<std::endl;
	offset= lseek( fd, 0, SEEK_SET );

	// Reset the pass byte counter. 
	//c->pass_done = 0; don't need for copy

	if( offset== (off64_t)-1 )
	{
		//nwipe_perror( errno, __FUNCTION__, "lseek" );
		//nwipe_log( NWIPE_LOG_FATAL, "Unable to reset the '%s' file offset.", c->device_name );
		*dstream<<"unable to reset offset"<<std::endl;
		throw " unable to reset ofset";//return ;//return -1;
	}

	if( offset!= 0 )
	{
		//* This is system insanity. 
		//nwipe_log( NWIPE_LOG_SANITY, "__FUNCTION__: lseek() returned a bogus offset on '%s'.", c->device_name );
		*dstream<<"lseek returned bad offset"<<std::endl;
		throw "lseek returned bad offset";//return ;//return -1;
	}

	*dstream<<"actually erasing part"<<std::endl;
	while( z > 0 )
	{
		if( 512 <= z )
		{
			blocksize= 512 ;
		}
		else
		{
			//* This is a seatbelt for buggy drivers and programming errors because 
			//* the device size should always be an even multiple of its blocksize. 
			blocksize= z;
			*dstream<<"the size of "<< path<< " is not a multiple of block size "<<blocksize<<std::endl;
			//nwipe_log( NWIPE_LOG_WARNING,
			//  "%s: The size of '%s' is not a multiple of its block size %i.",
		//	  __FUNCTION__, c->device_name, c->device_stat.st_blksize );
		}

		//* Fill the output buffer with the random pattern. 
		//* Write the next block out to the device. 
		//
		r= write( fd, &b[w], blocksize );

		//* Check the result for a fatal error. 
		if( r < 0 )
		{
			//nwipe_perror( errno, __FUNCTION__, "write" );
			//nwipe_log( NWIPE_LOG_FATAL, "Unable to write to '%s'.", c->device_name );
			*dstream<<" unable to write fully to"<<path<<std::endl;
			throw "unable to write fully";//return ; //return -1;
		}

		//* Check for a partial write. 
		// cast as unsigned to avoid warning. is it better though?
		if( (unsigned int )r != blocksize )
		{
			//* TODO: Handle a partial write. 

			//* The number of bytes that were not written. 
			int s= blocksize - r;
			
			//* Increment the error count. 
			errors+= s;

			*dstream <<"partial write errors = "<<errors<<std::endl;

			//* Bump the file pointer to the next block. 
			offset= lseek( fd, s, SEEK_CUR );

			if( offset== (off64_t)-1 )
			{
				//nwipe_perror( errno, __FUNCTION__, "lseek" );
				//nwipe_log( NWIPE_LOG_ERROR, "Unable to bump the '%s' file offset after a partial write.", c->device_name );
				*dstream<<"unable to bump the file offset after a partial write"<<std::endl;
				throw "unable to bump the file offset after a partial write";//return ;//return -1;
			}

		} //* partial write 


		//* Adjust the window. 
		//w = ( c->device_stat.st_blksize  + w ) % pattern->length;

		/* Intuition check: 
		 *
		 *   If the pattern length evenly divides the block size
		 *   then ( w == 0 ) always.
		 */
		///*

		//* Decrement the bytes remaining in this pass. 
		z -= r;

		//* Increment the total progress counterr. 
		//c->pass_done += r;
		//c->round_done += r;

		//pthread_testcancel();

	} //* remaining bytes 

	//* Tell our parent that we are syncing the device. 
	//c->sync_status = 1;

	//* Sync the device. 
	//r = fdatasync( c->device_fd );

	//* Tell our parent that we have finished syncing the device. 
	//c->sync_status = 0;

	//if( r != 0 )
	//{
		//* FIXME: Is there a better way to handle this? 
	//	nwipe_perror( errno, __FUNCTION__, "fdatasync" );
	//	nwipe_log( NWIPE_LOG_WARNING, "Buffer flush failure on '%s'.", c->device_name );
	//}

	//* Release the output buffer. 
	free( b );
	
	//* We're done. 
 //return 0; 

//*/
	time_t end_t=time(0);
	date=localtime(&begin_t);
	*dstream<<path<<" :started erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
	date=localtime(&end_t);
	*dstream<<path<<" :ended erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;
	time_t diff_t=end_t-begin_t;
	date=localtime(&diff_t);
	*dstream<<path<<" erased"<<(size)<<" bytes w/erase_n in... :time elapsed:  "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl;

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
		<<std::endl;	*textgohere<<"______________________________________________________"<<std::endl;
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

