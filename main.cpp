#include <iostream>
#include <thread>
#include <stdlib.h>
//#include <system.h>
#include "HDD.h"

extern std::string GetStdoutFromCommand(std::string);
std::string BatchName="may1";
std::string EraseCmd="nwipe";

void print(HDD * HDDs[], int length){
	std::cout<<"HDD's are erasing \n";
	std::cout<<"BatchName : "<<BatchName<<std::endl;
	std::cout<<"EraseCmd: "<<EraseCmd<<std::endl<<std::endl;
	std::cout<<"#################################################\n\n";
	for(int i=0;i<length;i++){
	//		if(HDDs[i]->Present){	
			HDDs[i]->print();
			std::cout<<"___________________________"<<std::endl;
	//		}	
	}
	std::cout<<"total HDDs  : "<<HDD::instances<<std::endl;
}
void contPrint(HDD * HDDs[], int length)
{
	while(true){
		print(HDDs,length);
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		system("clear||cls");
	}
}
int main(int argc, char * argv[]){
	if(argv[1]){
		//BatchName=*argv[1];
	}
	int DriveNum=1;//max number of drives
	std::cout<< "making dev path prefix =";
	std::string devPath="/dev/sd";
	puts(devPath.c_str());

	HDD * HDDs[1];
	HDDs[0]= new HDD(devPath+'a');
	/*
	//HDDs[1]= new HDD(devPath+'b');
	//HDDs[2]= new HDD("/dev/sdc");
	//HDDs[3]= new HDD("/dev/sdd");
	//*/
	///*	
	std::thread * runner[DriveNum];
	for(int i =0;i<DriveNum;i++){
		runner[i]=new std::thread(HDD::run, HDDs[i],&BatchName);
		//runner[i]->join();
	}
	//*/
	/*
	try{
		throw ((std::string) "name");
	}
	catch(std::string e){
		std::cout<<e<<std::endl;
		HDDs[0]->Exception=e;
	}
	//*/
	std::cout<<"here"<<std::endl;
	std::thread * printer= new std::thread(&contPrint,HDDs,1);
	printer->join();
	
//just in case threads don't exit when main does
	for(int i =0;i<DriveNum;i++){
		//runner[i]=new std::thread(&HDD::run,HDDs[i],&BatchName);
		runner[i]->join();
	}
	
	return 0;
}
