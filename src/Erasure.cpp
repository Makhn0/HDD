#include <iostream>
#include <time.h>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>//O_RDWR?
#include <sys/types.h>//open();close()[ithink];
#include <stdint.h>//u64 in nwipe_static_pass?
#include <fcntl.h>
#include "Erasure.h"
#include "methods.h"
using namespace std;
class Timer{
	private:
/*
		time_t begin_t=;
		tm * date=localtime(&begin_t);
		tm * date=localtime(&begin_t)
*/
		time_t begin_t;
		time_t current_t;
		time_t end_t;
		ostream * stream;
		
	public:
		Timer(ostream * astream=&cout) : begin_t(time(0)),current_t(begin_t),stream(astream){}
		void set(){
			current_t=time(0);	
		}
		void reset(){
			begin_t=time(0);
			current_t=begin_t;
			end_t=-1;
		}
		void end(){
			end_t=time(0);
 		}
		void print_time(tm * date){
			*stream<<date->tm_hour<<":"
				   <<date->tm_min<<":"
				   <<date->tm_sec<<endl;
		}
		tm * date(){
			return localtime(&current_t);
		}
		void print_out(string msg,tm * date){

			*stream<<msg//include path in message
				<<(1900+ date->tm_year)
				<<"/"
				<<month(date->tm_mon)
				<<"/"
				<<(1+date->tm_mday)<<"  | "	;
			print_time(date);
			
		}
		void print_begin(string msg){
			print_out(msg,localtime(&begin_t));
		}
		void print_end(string msg){
			print_out(msg,localtime(&end_t));
		}
		void print_current(string msg){
			print_out(msg,localtime(&current_t));
		}
		void print_elapsed(string msg){
			time_t time_elapsed=current_t-begin_t;
			print_out( msg,localtime(&time_elapsed));
		}
		void print_full(){
			if(end_t){
				time_t diff= end_t-begin_t;
				print_time(localtime(&diff));
			}
			else *stream<<"end_t not set"<<endl	;		
		}
};
void Erasure::print(std::ostream* textgohere=&std::cout){
	HDD::print(textgohere);
	if(PresentTask=="Erasing..."){
		*textgohere<<"Start Time: "<<(ctime(&StartTime));//<<std::endl;
		if(EndTime>0)*textgohere<<"End Time: "<<EndTime<<std::endl;
		*textgohere<<"Run Time: "<<(RunTime/3600)<<"hours "<<((RunTime%3600)/60)<<" min(s) "<<(RunTime%60)<<"second(s)"<<std::endl;
		//*textgohere<<"Run Time: "<<RunTime<<std::endl;
		*textgohere<<"Erasing "<<(currentLBA*1.0/size)*100<<"% Complete"<<std::endl;
		*textgohere<<"ETA: "<<(eta/3600)<<"hours "<<((eta%3600)/60)<<" min(s) "<<(eta%60)<<"second(s)"<<std::endl;
	}else print_help(textgohere);
	print_help2(textgohere);

}
void Erasure::erase(std::string * method)
{  	
	std::string TempName("");
	TempName.append("Temp_");
	TempName.append( path.substr(path.size()-1,1) );
	TempName.append(".txt");
	TempLogFileName=TempName;
	*dstream<<"templogfilename : "<<TempLogFileName<<std::endl;
	Command("sudo rm "+TempLogFileName, "erasing old temporay log file, if it exists",false);
	Command("sudo touch "+TempLogFileName, "touching new temporary log file",true);
	Command("sudo ./nwipe --autonuke --nogui --method="
		+*method		
		+" -l"
		+TempLogFileName
		+ " "
		+path	
		,"Erasing With Nwipe...",true
	);
}
void Erasure::erase(char pattern)
{
	///*
	try{
		erase_n(0x00);
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
void Erasure::Write_All( char pattern =0x00,long begin=0,long end=0){
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
	(*dstream).write(block,bs);
	*dstream<<":end"
		<<std::endl;
	Timer * timer_i=new Timer(dstream);
	timer_i->print_begin("started erasing");
	
	time_t begin_t=time(0);
	tm * date=localtime(&begin_t);
	time_t end_t=-1;
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
	
	timer_i->end();
	//time_t end_t=time(0);
//	*dstream<<"time(0);"<<std::endl;
	//date=localtime(&begin_t);
	//*dstream<<"date=local..;"<<std::endl;
	//PrintDate(" :started eraseing:  ",date);
	timer_i->print_begin(" :started erasing : ");
	timer_i->print_full();
	date=localtime(&end_t);
	*dstream<<" :ended erasing:";
	//PrintDate(" :ended erasing:  ",date);
	//time_t diff_t=end_t-begin_t;
	//date=localtime(&diff_t);
	timer_i->print_full();/*
	*dstream<<path<<" erased"<<(begin-end)<<" bytes in... :time elapsed:  "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<std::endl; */
}
bool Erasure::Long_Verify(unsigned char pattern =0x00,long begin=0, long end=0){
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


void Erasure::erase_n(char pattern){
	PresentTask="Erasing...";
	Timer * timer_i = new Timer(dstream); //timer_instance
	timer_i->print_begin(path+" :start erasing w/erase_n:");
	time_t begin_t=time(0);
	tm * date;

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
		*dstream<<"null pattern pointer"<<endl;
		return -1;
	}
	*///will never happen pattern not pointer in this code
	/*if( pattern->length <= 0 )
	{
		// Caught insanity. 
		*dstream<<"__FUNCTION__: The pattern length member is "<<, pattern->length<<endl ;
		return -1;
	}*///will never happen

	// Create the output buffer. 
	b= (char *) malloc( blocksize  +2 );

	// Check the memory allocation. 
	if( ! b )
	{
		*dstream<<"unable to create buffer"<<endl;
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
	*dstream<<"filedescriptor int fd = "<<fd<<endl;
	offset= lseek( fd, 0, SEEK_SET );

	// Reset the pass byte counter. 
	//c->pass_done = 0; don't need for copy

	if( offset== (off64_t)-1 )
	{
		//nwipe_perror( errno, __FUNCTION__, "lseek" );
		//nwipe_log( NWIPE_LOG_FATAL, "Unable to reset the '%s' file offset.", c->device_name );
		*dstream<<"unable to reset offset"<<endl;
		throw " unable to reset ofset";//return ;//return -1;
	}

	if( offset!= 0 )
	{
		//* This is system insanity. 
		//nwipe_log( NWIPE_LOG_SANITY, "__FUNCTION__: lseek() returned a bogus offset on '%s'.", c->device_name );
		*dstream<<"lseek returned bad offset"<<endl;
		throw "lseek returned bad offset";//return ;//return -1;
	}

	*dstream<<"actually erasing part"<<endl;
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
			*dstream<<"the size of "<< path<< " is not a multiple of block size "<<blocksize<<endl;
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
			*dstream<<" unable to write fully to"<<path<<endl;
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

			*dstream <<"partial write errors = "<<errors<<endl;

			//* Bump the file pointer to the next block. 
			offset= lseek( fd, s, SEEK_CUR );

			if( offset== (off64_t)-1 )
			{
				//nwipe_perror( errno, __FUNCTION__, "lseek" );
				//nwipe_log( NWIPE_LOG_ERROR, "Unable to bump the '%s' file offset after a partial write.", c->device_name );
				*dstream<<"unable to bump the file offset after a partial write"<<endl;
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
	//timer_i.set();
	timer_i->end();
	timer_i->print_begin(" :started erasing: ");
	timer_i->print_end(" :ended erasing:   ");
	*dstream<<" : erased"<<size<<" bytes w/erase_n in ... :time elapsed:  ";
	timer_i->print_full();
	/*
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
		<<date->tm_sec<<endl;
	date=localtime(&end_t);
	*dstream<<path<<" :ended erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<endl;
	time_t diff_t=end_t-begin_t;
	date=localtime(&diff_t);
	*dstream<<path<<" erased"<<(size)<<" bytes w/erase_n in... :time elapsed:  "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<endl;
    //
	*/
}
void Erasure::erase_dd(){
	Command(
	"sudo dd if=/dev/zero of="
	+path
	,"erasing with dd write and /dev/zero"	,true);
}
void Erasure::erase_debrief(){
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
void Erasure::run(string* batch,char pattern){
	//thread a(&HDD::presence_checker,this,false);
	while(1){
		*dstream<<path<<" : beginning running loop... "<< endl;
		//sleep(1);
		PresentTask="reseting";
		*dstream<<path<<" : resetting... "<<endl;
		reset();
		PresentTask="waiting to detect...";
		*dstream<<path<<" : "<<PresentTask<<endl;
		while(!presence()){
			sleep(5);
		}
		//PresentTask="reseting";
		//*dstream<<path<<" : resetting... "<<endl;
		//reset();
		/*BEGIN TRY BLOCK*/
		try{	
			run_body(batch,pattern);
			Status=FinishedSuccess;
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
		*dstream<<path<<"closing fd"<<endl;
		close(fd);
		/*END TRY BLOCK*/
		*dstream<<path<<" : end of run_body"<<endl;

		while(presence())
		{
				sleep(10);
		}
		//break;
		*dstream<<path<<" : pulled out starting over..."<<endl<<endl;
	}
	*dstream<<path<<" : we did it out of the loop"<<endl;;
}
void Erasure::run_body(string* batch,char pattern){
	*dstream<<"beginning run_body"<<endl;
	StartTime=time(0);
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
	if(!presence()){return;}
		dd(batch); //TODO add back in
	if(!presence()){return;}
	//*/
	resolve_size();
	if(!presence()){return;}
	//erase
	#ifdef _Erase
	erase(pattern);		//compile w/ -d_Erase
	if(!presence()){return;}
	#endif

	EndTime=time(0);
	if(!presence()){return;}
	*dstream<<path<<"closing fd.. "<<endl;
	close(fd);
	*dstream<<path<<" : end of erase: writing to logs"<<endl;
	log(batch);
}