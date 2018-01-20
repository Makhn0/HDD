#include <iostream>
//#include <time.h>
#include <string>
#include <sstream>
#include <fstream>
#include <unistd.h>
//#include <sys/stat.h>
//#include <sys/types.h>//open();close()[ithink];
//#include <stdint.h>//u64 in nwipe_static_pass?
#include <fcntl.h>//O_RDWR
#include "Erasure.h"
#include "methods.h"
using namespace std;
class Timer{
	/*measures run time of erase functions*/
	protected:
	
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
		Timer(ostream * astream=&cerr,bool begin=false) : stream(astream){
			if(begin){
			begin_t=current_t=time(0);
			//current_t=begin_t;
			}
		}
		void begin(){
			begin_t=time(0);
		}
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
		tm * date(){
			return localtime(&current_t);
		}
		string time_str(tm * date){
			stringstream s;
			s<<date->tm_hour<<":"
				   <<date->tm_min<<":"
				   <<date->tm_sec<<endl;
			return s.str();
		}
		string date_str(tm * date){
			stringstream s;
			s<<(1900+ date->tm_year)
				<<"/"
				<<month(date->tm_mon)
				<<"/"
				<<(1+date->tm_mday)<<"  | "	;
			return s.str();
		}
		void print_out(string msg,tm * date){
			*stream<<msg//include path in message
				<<date_str(date)
				<<time_str(date);
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
				*stream<<time_str((localtime(&diff)));
			}
			else *stream<<"end_t not set"<<endl	;		
		}
};
class Tracker : public Timer{//could be template, but who has the time?
	public:
		time_t last_t;
		long current_x;
		long last_x;
		long total_x;
		Tracker(long x,ostream* stream=&std::cerr): Timer(stream),total_x(x){}
		string eta_str(){return "";}
		void update_time(){
			last_t=current_t;
			current_t=time(0);
		}
		void set_x(long x){
			last_x=current_x;
			current_x=x;
		}
		void update(long x){
			set_x(x);
			update_time();
		}
		long inst_v_l(long x=0){
			if(x)update(x);
			return (current_x-last_x)/(last_t-current_t);
		}
		long ave_v_l(long x=0){
			if(x) update(x);
			return (current_x)/(last_t-begin_t);
		}

		long ieta_l(long x=0){
			if(x) update(x);
			return (total_x-current_x)/(inst_v_l());// type problems?
		}
		long aeta_l(long x=0){
			if(x) update(x);
			return (total_x-current_x)/(ave_v_l());
		}
		double inst_v_d(long x=0)
		{	
			if(x) update(x);
			return (current_x-last_x)*1.0/(last_t-current_t);
		}
		double ave_v_d(long x=0)
		{	
			if(x) update(x);
			return (current_x)*1.0/(last_t-begin_t);
		}
		double ieta_d(long x=0){
			if(x) update(x);
			return (total_x-current_x)*1.0/(inst_v_l());// type problems?
		}
		double aeta_d(long x=0){
			if(x) update(x);
			return (total_x-current_x)*1.0/(ave_v_l());
		}
		string report(long x = 0){
			if(x) update(x);
			stringstream s;
			s<< " : erasing : "
				<<(current_x/1000000)
				<<"MB /"<<(total_x)/1000000
				<<" MB : "
				<<((current_x)*1.0/(total_x))*100
				<<" percent done"<<endl
				<< " : Ave speed : "
				<<ave_v_l()
				<<"LBA/sec : inst. speed "
				<<inst_v_l()
				<<"LBA/sec  inst. based eta : "	;
				
			s<<report_precise();
			return s.str();
			
		}
		string report_precise(){
			stringstream s;
			
			if(int a=(ieta_l()/3600))  s<<(a)<<" hours ";
			s<<(ieta_l()/60)
				<<" min(s)"	
				<<(ieta_l()%60)<<" sec ";
			if(int b=aeta_l()/36000)  s<<(b)<<" hours ";
			s<<" avbased eta: "
				<<(aeta_l()/60)
				<<" min(s)"<<(aeta_l()%60)<<" sec "<<endl;
				return s.str();
		}
};
void Erasure::print(std::ostream* textgohere=&std::cout){
	HDD::print(textgohere);
	if(PresentTask=="Erasing..."){
		*textgohere<<"Start Time: "<<(ctime(&StartTime));//<<endl;
		if(EndTime>0)*textgohere<<"End Time: "<<EndTime<<endl;
		*textgohere<<"Run Time: "<<(RunTime/3600)<<"hours "<<((RunTime%3600)/60)<<" min(s) "<<(RunTime%60)<<"second(s)"<<endl;
		//*textgohere<<"Run Time: "<<RunTime<<endl;
		*textgohere<<"Erasing "<<(currentLBA*1.0/size)*100<<"% Complete"<<endl;
		*textgohere<<"ETA: "<<(eta/3600)<<"hours "<<((eta%3600)/60)<<" min(s) "<<(eta%60)<<"second(s)"<<endl;
	}else print_help(textgohere);
	print_help2(textgohere);

}
void Erasure::erase(string * method)
{  	
	string TempName("");
	TempName.append("Temp_");
	TempName.append( path.substr(path.size()-1,1) );
	TempName.append(".txt");
	TempLogFileName=TempName;
	*dstream<<"templogfilename : "<<TempLogFileName<<endl;
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
	catch(string e){
		puts(" : string thrown");
		this->erase_debrief();
		throw e;
	}
	catch(std::exception e){
		puts(" : exception thrown");
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
	else *p()<<" : opened drive and writing "
		<<(pattern!=0?(const char *)&pattern:"zero")<<"s from "<<begin<<" to "<<end<<endl;
	const long bs=512;
	char block[bs];
	for(int i=0;i<bs;i++) block[i]=pattern;
	*dstream<<"block declared:"<<endl;
	(*dstream).write(block,bs);
	*dstream<<":end"<<endl;
	Tracker * timer_i=new Tracker(size,dstream);
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
		//*dstream<<"beginL"<<endl;
		drive.seekp(currentLBA);
		//drive<<block;
		drive.write(block,bs);
		
		if(currentLBA%check==0||currentLBA/bs<9)
		{	
			if(!presence_check())throw "pulled out while erasing";
			if(currentLBA/bs<9)	
				*dstream<<"beggining blocks :"<<currentLBA<<" tellp "<<drive.tellp()<<endl;
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
			*p()<< " : erasing : "
				<<(currentLBA/1000000)
				<<"MB /"<<(end-begin)/1000000
				<<" MB : "
				<<((currentLBA-begin)*1.0/(end-begin))*100
				<<" percent done"<<endl
				<< " : Ave speed : "
				<<aV
				<<"LBA/sec : inst. speed "
				<<v
				<<"LBA/sec  inst. based eta : "	;
			if(int a=(ieta/3600))
				*dstream<<(a)
					<<" hours ";
			*dstream<<(ieta/60)
				<<" min(s)"<<(ieta%60)<<" sec ";
			*dstream<<" avbased eta: "
				<<(aeta/60)
				<<" min(s)"<<(aeta%60)<<" sec "<<endl;
			eta=aeta;
			Last_t=current_t;
			lastLBA=currentLBA;
			//*dstream<<"end of if"<<endl;
		}
	
		//*dstream<<"endL :"<<currentLBA<<endl;
	}
	*p()<<"out"<<endl;
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
			cout<<"tellg = "<<drive.tellp()<<endl;
			current_t=time(0);	
			delta_t=current_t-Last_t; 
			elapsed_t=current_t-begin_t;
			if(!elapsed_t)elapsed_t++;
			if(delta_t>0)v=(currentLBA-lastLBA)/delta_t;
			if(!v)v++;
			
			//dividing by: elapsed_t,v and constants
			*p()<< " : erasing : "
				<<(currentLBA/1000000)
				<<"MB /"<<(end-begin)/1000000
				<<" MB : "
				<<((currentLBA-begin)*1.0/(end-begin))*100
				<<" percent done"<<endl;
			*p()<<" delta_t: "<<delta_t<< "s : Ave speed : "
				<<((currentLBA-begin)/elapsed_t)
				<<"LBA/sec : inst. speed "
				<<v
				<<"LBA/sec  inst. based eta : "	
				<<(eta/60)<<" min"
				<<(eta%60)<<" sec"<<endl;
			
			Last_t=current_t;
			lastLBA=currentLBA;
			
		}
		
	}
	*p()<<" out coda"<<endl;

	*p()<< " : erasing : "
		<<currentLBA/1000000<<" MB /32,000 MB : "
		<<((currentLBA-begin)*1.0/(end-begin))*100<<endl;
	*dstream<<"finished erasing... obstensibly... closing file"
		<<endl;
	drive.close();
	*dstream<<"closed"<<endl;
	
	timer_i->end();
	//time_t end_t=time(0);
//	*dstream<<"time(0);"<<endl;
	//date=localtime(&begin_t);
	//*dstream<<"date=local..;"<<endl;
	//PrintDate(" :started eraseing:  ",date);
	timer_i->print_begin(" :started erasing : ");
	timer_i->print_full();
	date=localtime(&end_t);
	*dstream<<" :ended erasing:";
	//PrintDate(" :ended erasing:  ",date);
	//time_t diff_t=end_t-begin_t;
	//date=localtime(&diff_t);
	timer_i->print_full();/*
	*p()<<" erased"<<(begin-end)<<" bytes in... :time elapsed:  "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<endl; */
}
bool Erasure::Long_Verify(unsigned char pattern =0x00,long begin=0, long end=0){
	if(!end)end=size;
	std::ifstream idrive(path.c_str(),std::istream::in);
	if(!idrive) throw "cannot open HDD to read";
	else *p()<<" : opened drive and verifying all  "<<pattern<<"from "<<begin <<" to "<<end<<endl;
	char buffer[1];
	bool fail;
	for(long i=begin//*3199/3200
		;i<end&&Present
		;i++)
	{
		idrive.seekg(i);
		idrive.read(buffer,1);

		if(i%50000000==0) {
	
			*p()<< " : checking : "<<(i-begin)/1000000<<" MB "<<"/"<<(end-begin)/1000000<<" MB : "<<((i-begin+1)*1.0/(end+1))*100<<" char :"<<buffer<<":";}
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
	*p()<<" :started erasing:  "
		<<(1900+ date->tm_year)
		<<"/"
		<<month(date->tm_mon)
		<<"/"
		<<(1+date->tm_mday)<<"  | "	
		<<date->tm_hour<<":"
		<<date->tm_min<<":"
		<<date->tm_sec<<endl;
	date=localtime(&end_t);
	*p()<<" :ended erasing:  "
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
	*p()<<" erased"<<(size)<<" bytes w/erase_n in... :time elapsed:  "	
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
	//thread a(&HDD::presence_check_checker,this,false);
	while(1){
		task(" : beginning running loop... ");
		//sleep(1);
		task("resetting...");
		reset();
		task("waiting to detect...");
		while(!presence_check()){
			sleep(5);
		}

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
		task("closing fd");
		
		close(fd);
		/*END TRY BLOCK*/
		task(" : end of run_body");

		while(presence_check())
		{
				sleep(10);
		}
		//break;
		*(task(" : pulled out starting over..."))<<endl;
	}
	
	task(" : we did it out of the loop");
}
void Erasure::run_body(string* batch,char pattern){
	
	task("beginning run_body");
	StartTime=time(0);
	get_data();
	if(!presence_check()){return ;}
	#ifndef _Skip_Smart
	//TODO handle (41) self test interrupted
	smartctl_start();
	//bool a=false;
	/* smart ctl*/
	/*
	while(smartctl_running()){
		if(!presence_check()){
			a=true;
			smartctl_kill();
			break;
		}
		sleep(10);		
	}
	*/
	while(presence_check())
	{
		sleep(5);
		if(!smartctl_check()) {
			break;
		}
		sleep(5);
	}
	#endif
	/* back blaze test*/
	if(bb_test()) throw " >0 of smart 5;187;188;197;198 is >0  : drive likely to fail soon";
	///* confirm read write
	if(!presence_check()){return;}
		dd(batch); //TODO add back in
	if(!presence_check()){return;}
	//*/
	resolve_size();
	if(!presence_check()){return;}
	//erase
	#ifdef _Erase
		erase(pattern);		//compile w/ -d_Erase
		if(!presence_check()){return;}
	#endif

	EndTime=time(0);
	if(!presence_check()){return;}
	task("closing fd.. ");
	close(fd);
	task(" : end of erase: writing to logs");
	log(batch);
}
