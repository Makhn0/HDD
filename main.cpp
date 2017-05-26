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
void print(HDD * HDDs[], int length)
{
	std::cout<<"Welcome to Eric's Wonderful Hard Drive Eraser !!! :D \n";
	std::cout<<"BatchName : "<<BatchName<<std::endl;
	std::cout<<"EraseCmd: "<<EraseCmd<<std::endl;
	std::cout<<"total HDDs  : "<<HDD::instances<<std::endl;
	std::cout<<"########################################################\n\n";
	for(int i=0;i<length;i++)	
	{
//		if(HDDs[i]->Present)
			HDDs[i]->print();
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
std::string month(int i){
	switch(i){
		case 1:return "Jan";
		case 2:return "Feb";
		case 3:return "Mar";
		case 4:return "Apr";
		case 5:return "May";
		case 6:return "June";
		case 7:return "July";
		case 8:return "Aug";
		case 9:return "Sep";
		case 0:return "Oct";
		case 10:return "Nov";
		case 11:return "Dec";
		default:return "Jan";
	}
}
int main(int argc, char * argv[]){
	//TODO test time is accurate on all clients
	time_t now=time(0);
	tm * date=localtime(&now);
	std::stringstream * temp=new std::stringstream();
		*temp<<(1900+ date->tm_year)
			<<"_"
			<<month(date->tm_mon)
			<<"_"
			<<(1+date->tm_mday);		
	BatchName=temp->str();
	if(argv[1])
	{
		BatchName=argv[1];
	}
	const int DriveNum=1;
	
	#ifdef _Debug
	std::cerr<<"cerr Debug"<<std::endl;
	std::cout<<"Debug Mode"<<std::endl;
	std::cout<< "making dev path prefix =";
	#endif
	std::string devPath="/dev/sd";
	#ifdef _Debug
	std::cout<<devPath<<std::endl;
	std::cout<<"BatchName :"<<BatchName<<std::endl;
	#endif

	HDD * HDDs[DriveNum];
	std::thread * runner[DriveNum];
	for(int i =0;i<DriveNum;i++)
	{
		HDDs[i]= new HDD(devPath+(char)('a'+i));
		runner[i]=new std::thread(&HDD::run, HDDs[i],&BatchName);
		#ifdef _Debug
		//sleep(1);
		#endif
	}
	#ifdef _Debug
	puts("ended instantiating HDD objects");
	//sleep(1);
	#endif

	#ifndef _Debug
	std::thread * printer= new std::thread(&contPrint,HDDs,DriveNum);
	printer->join();
	#endif
	for(int i =0;i<DriveNum;i++)
	{
		runner[i]->join();
	}
	return 0;
}
