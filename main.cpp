#include <iostream>
#include <thread>
#include <stdlib.h>
//#include <system.h>
#include "HDD.h"

extern std::string GetStdoutFromCommand(std::string);
std::string BatchName="may1";
std::string EraseCmd="nwipe";

void print(HDD * HDDs[], int length)
{
	std::cout<<"Welcome to Eric's Wonderful Hard Drive Eraser !!! :D \n";
	std::cout<<"BatchName : "<<BatchName<<std::endl;
	std::cout<<"EraseCmd: "<<EraseCmd<<std::endl;
	std::cout<<"total HDDs  : "<<HDD::instances<<std::endl;
	std::cout<<"########################################################\n\n";
	for(int i=0;i<length;i++)	
	{
		if(HDDs[i]->Present) HDDs[i]->print();
	}
}
void contPrint(HDD * HDDs[], int length)
{
	while(true)
	{
		print(HDDs,length);
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		system("clear||cls");
	}
}
int main(int argc, char * argv[]){
	if(argv[1])
	{
		BatchName=argv[1];
	}
	const int DriveNum=4;
	std::cout<< "making dev path prefix =";
	std::string devPath="/dev/sd";
	HDD * HDDs[DriveNum];
	std::thread * runner[DriveNum];
	for(int i =0;i<DriveNum;i++)
	{
		HDDs[i]= new HDD(devPath+(char)('a'+i));
		runner[i]=new std::thread(&HDD::run, HDDs[i],&BatchName);
	}
	puts("here");
	std::thread * printer= new std::thread(&contPrint,HDDs,DriveNum);
	printer->join();
	for(int i =0;i<DriveNum;i++)
	{
		runner[i]->join();
	}
	return 0;
}