#include <ostream>
#include <fstream>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include "HDD.h"
#include <unistd.h>
#include <time.h>
#include <sstream>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


//#include "methods.cpp"
std::string BatchName;
std::string argPath;
std::stringstream * printstream;
std::ostream * debugstream;
extern std::string month(int i);
extern std::string StdOut0(std::string i);//needs methods link?
extern std::string trim(std::string&);

std::string reorganize(std::string a[],int length,int ScreenHeight){
	std::string output="";
	for(int i=0;i<length;i++)
	{
		output+=a[i]+" s"+a[ScreenHeight+i];
	}
	return output;
}
char * reorganize(char**a)
{
	//?
	while(a){
	//	std::cout<<a[i]
		break;
	}
	return a[0];	
}
void PrintToScreen(std::stringstream * a,int n=50){
			
	//std::cout<<a->str()<<std::endl;
	/*
	std::string buffer;
		getline(*a,buffer);
	std::cout<<buffer<<std::endl;
	/*/
	int NextCollumnAt=80;
	std::string buffer[n];
	std::string buffer1[n];
	int outsize;
	std::cout<<" printing to screen"<<std::endl;
	for(int i=0;i<n;i++)
	{
		getline(*a,buffer[i]);
		//std::cout<<buffer[i]<<std::endl;
	}
	for(int i=0;i<n;i++)
	{
		
		trim(buffer[i]);
		getline(*a,buffer1[i]);
		trim(buffer[i]);
		//std::cout<<"buffer going out"<<std::endl;
		//formatted
		
		outsize= (int)buffer[i].size();
		if(outsize>NextCollumnAt)
		{
			for(int j=0;j<outsize;j++)
			{
				if(j%(NextCollumnAt-5)==0)
					std::cout<<'\n';
				std::cout
				//<<"first ish"
				<<buffer[i][j];
			}
			std::cout<<std::endl;
		}
		else
		{
			std::cout<<buffer[i];
		}
		for(int j=0;j<(int)(NextCollumnAt-outsize);j++){
			std::cout<<" ";
		}
		std::cout<<buffer1[i]<<std::endl;
	}
	while(!a->eof()){
		getline(*a,buffer[0]);
		std::cout
//		<<"last bit"
		<<buffer[0]<<std::endl;
	}
	if((a->rdstate()&(std::ifstream::badbit|std::ifstream::failbit))!=0)a->clear();

	//*/
}
//void PrintToScreen(std::ostream * a,int n){} //overload for when argument is pointer to cout
void print(HDD * HDDs[], int length)
{
	printstream->str("");
	printstream->flush();
	printstream->str("hello again world");
	std::cout<<printstream->str()<<std::endl;;
	std::cout<<"Welcome to Eric's Wonderful Hard Drive Eraser !!! :D \n";
	std::cout<<"BatchName : "<<BatchName<<std::endl;
	//std::cout<<"EraseCmd: "<<EraseCmd<<std::endl;
	std::cout<<"total HDDs  : "<<HDD::instances<<std::endl;
	std::cout<<"########################################################\n\n";
	static int count;
	count++;
	std::cout<<count<<std::endl;
	for(int i=0;i<length;i++)	
	{
		
		if(HDDs[i]->Present)
			HDDs[i]->print(
	//			(std::ostream*) 
			printstream);
	}
	//std::cout<<printstream->str();
	PrintToScreen(printstream,30);
	
	std::cout<<"end"<<std::endl;
}
void contPrint(HDD * HDDs[], int length)
{
	while(true)
	{
		system("clear||cls");
		print(HDDs,length);
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	}
}

int main(int argc, char * argv[]){
/*	
	std::string stuff="first line\n ooo\n222222\n3333333\n what??\n132456789012345678901234567980123456790123456790123456790123456790123456790123456790\nwhat in tarnation\nbut what theremare \n ohaas\n even more\thats righ\n o yea \nothing to see here \n one more\n end";
	HDD * a=new HDD("/dev/sdf");
	HDD*b=new HDD("/dev/sdg");
	a->reset();
	b->reset();
	//a->print();
	std::stringstream* s=new std::stringstream;
	std::cout<<"s.good="<<s->good()<<std::endl;
	a->print(s);
	b->print(s);
        *s<<"some other stuff: \n asdf asdf\n"<<stuff<<std::endl;
	PrintToScreen(s,20);
	a->print(s);
	b->print(s);
	
	std::cout<<"second PrintToScreen call"<<std::endl;
	PrintToScreen(s,20);
	std::cout<<"end"<<std::endl;

	return 0;
	//*/
	printstream=new std::stringstream;
	std::string csvpath=StdOut0("echo ~");
	std::cout<<"client home =" <<csvpath<<std::endl; 
	csvpath.append("/batch_csv");
	std::cout<<"checking for directory ~/batch_csv viz. "<< csvpath<<std::endl;
	DIR * Diropen = opendir(csvpath.c_str());
	if(!Diropen){	
		std::cout<<"creating directory ~/batch_csv"<<std::endl;
		int dirmake=mkdir(csvpath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(dirmake==-1) std::cout<<" couldn't create directory try sudo"<<std::endl;
		else std::cout<<" created !!!"<<std::endl;
	}
	else{	
		std::cout<< csvpath<< " already exists"<<std::endl;
	}
	std::cout<<" syncing clocks "<<std::endl;
	//TODO uncomment time sync
	//std::cout<<StdOut0(" sudo ntpdate 192.168.1.1 ; date ")<<std::endl;
	std::cout<<"Start Instances : "<<HDD::instances<<std::endl;

	/*
	argv 1 is batchname
	argv 2 is whether to run printer
	argv 3 is pattern
	*/
	/*
	std::cout<<"argv[1] :"<<argv[1]<<std::endl;
	std::cout<<"argv[2] :"<<argv[2]<<std::endl;
	std::cout<<"argv[2][0] :"<<argv[2][0]<<std::endl;
	std::cout<<"argv[2]!=anp :"<<(argv[2]!="anp")<<std::endl;
	std::cout<<"argv[2][0]=='a'?0:3 "<<(argv[2][0]=='a'?3:0)<<std::endl;
	std::cout<<"argv[3] "<<argv[3]<<std::endl;
	char pattern=(char)std::stoi(std::string(argv[3]));
	std::cout<<"pattern "<<pattern<<std::endl;
	/*/
	char pattern = 0x00;
	//*/
	sleep(1);	
	debugstream=&std::cerr;
	printstream= new std::stringstream("");
		//&std::cout;

	if(
		1//argv[1]=="date"
	)
	{
		//if time needed for other things take out of else
		time_t now=time(0);
		tm * date=localtime(&now);
		std::stringstream * temp=new std::stringstream();
			*temp<<(1900+ date->tm_year)
				<<"_"
				<<month(date->tm_mon)
				<<"_"
				<<(1+date->tm_mday);	
		BatchName=temp->str();
	}
	else{
		BatchName="default";
	}
		


	#ifdef _Debug
	*printstream<<"Debug Mode"<<std::endl;
	*debugstream<< "making dev path prefix =";
	#endif
	std::string devPath="/dev/sd";
	#ifdef _Debug
	*debugstream<<devPath<<std::endl;
	std::cout<<"BatchName :"<<BatchName<<std::endl;
	const int DriveNum=1;
	#endif

	#ifndef _Debug
	const int DriveNum=4;
	#endif
	HDD * HDDs[DriveNum];
	std::thread * runner[DriveNum];
	for(int i =0;i<DriveNum;i++)
	{
		HDDs[i]= new HDD(devPath+(char)('a'+i));
		runner[i]=new std::thread(&HDD::run, HDDs[i],&BatchName,pattern);	
		sleep(1);
		std::cout<<i<<" in loop count "<<HDD::instances<<std::endl;
	}
	
	#ifdef _Debug
	*printstream<<"ended instantiating "<<HDD::instances<<" HDD objects"<<std::endl;
	#endif

	#ifndef _Debug
//A/	if(argv[2][1]!='n'){
	std::cout<<"Begining Printer..."<<std::endl;
	std::thread * printer;
	printer= new std::thread			
		(&contPrint,HDDs,DriveNum
	//		-dumbvariable
			);
//seems as though this requires final argument to be variable of type const int or an expression that begins with one
	printer->join();
	//A/ }	
	#endif
	for(int i =0;i<DriveNum;i++)
	{
		runner[i]->join();
	}
	delete(printstream);
	delete(debugstream);
	return 0;
}
