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

using namespace std;
int main(int argc, char * argv[]){
	cout<<"main"<<endl;
	printToScreen_test();
	return 0;
	///*
	
	preamble();
	//debugstream=&std::cerr;
	//printstream= new std::stringstream("");
	#ifdef _Debug
		*printstream<<"Debug Mode"<<endl;
		*debugstream<< "making dev path prefix =";
	#endif
	string devPath="/dev/sd";
	#ifdef _Debug
		*debugstream<<devPath<<endl;
		cout<<"BatchName :"<<BatchName<<endl;
		const int DriveNum=1;
	#endif

	#ifndef _Debug
		const int DriveNum=4;
	#endif
	Erasure* HDDs[DriveNum];
	std::thread * Threads[DriveNum];
	
	cout<<"Start Instances : "<<HDD::instances<<endl;
	for(int i =0;i<DriveNum;i++)
	{
		HDDs[i]= new Erasure(devPath+(char)('a'+i));
		Threads[i]=new std::thread(&Erasure::run, HDDs[i],&BatchName,pattern);	
		sleep(1);
		cout<<i<<" in loop count "<<HDD::instances<<endl;
	}
	
	#ifdef _Debug
		*printstream<<"ended instantiating "<<HDD::instances<<" HDD objects"<<endl;
	#endif

	#ifndef _Debug
		//A/	if(argv[2][1]!='n'){
		cout<<"Begining Printer..."<<endl;
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