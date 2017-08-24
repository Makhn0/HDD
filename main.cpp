#include <ostream>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include "HDD.h"
#include <unistd.h>
#include <time.h>
#include <sstream>
#include <ctime>

std::string EraseCmd="nwipe";
std::string BatchName;
std::string argPath;
std::ostream * printstream;
std::ostream * debugstream;
void print(HDD * HDDs[], int length)
{
	std::cout<<"Welcome to Eric's Wonderful Hard Drive Eraser !!! :D \n";
	std::cout<<"BatchName : "<<BatchName<<std::endl;
	std::cout<<"EraseCmd: "<<EraseCmd<<std::endl;
	std::cout<<"total HDDs  : "<<HDD::instances<<std::endl;	std::cout<<"########################################################\n\n";
	for(int i=0;i<length;i++)	
	{
		if(HDDs[i]->Present)
			HDDs[i]->print(printstream);
	}
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
extern std::string month(int i);
int main(int argc, char * argv[]){
	//TODO test time is accurate on all clients
	std::cout<<"Start Instances :"<<HDD::instances<<std::endl;
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
	printstream=&std::cout;

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
	*printstream<<"ended instantiating "<<HDD::instances<<"HDD objects"<<std::endl;
	#endif

	#ifndef _Debug
//A/	if(argv[2][1]!='n'){
	std::cout<<"Beginng Printer..."<<std::endl;
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
	return 0;
}
