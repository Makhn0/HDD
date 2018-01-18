#include <ostream>
#include <fstream>
#include <iostream>
#include <thread>
#include <stdlib.h>

#include <unistd.h>
#include <time.h>
#include <sstream>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
//#include "HDD.h"
#include "Erasure.h"
#include "main_help.cpp"

//#include "methods.cpp"


int main(int argc, char * argv[]){

	printToScreen_test();
	return 0;
	///*
	
	preamble();
	//debugstream=&std::cerr;
	//printstream= new std::stringstream("");
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
	Erasure* HDDs[DriveNum];
	std::thread * Threads[DriveNum];
	
	std::cout<<"Start Instances : "<<HDD::instances<<std::endl;
	for(int i =0;i<DriveNum;i++)
	{
		HDDs[i]= new Erasure(devPath+(char)('a'+i));
		Threads[i]=new std::thread(&Erasure::run, HDDs[i],&BatchName,pattern);	
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
			(&contPrint,(HDD**)HDDs,DriveNum
		//		-dumbvariable
				);
		//seems as though this requires final argument to be variable of type const int or an expression that begins with one
		printer->join();
	//A/ }	
	#endif
	for(int i =0;i<DriveNum;i++)
	{
		Threads[i]->join();
	}
	delete(printstream);
	delete(debugstream);
	return 0;
	//*/
}