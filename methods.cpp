#ifndef methods_cpp
#define methods_cpp

#include "HDD.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

long myStol(std::string a){
	long output=0;
	const char * data =a.c_str();
	for(unsigned int i=0;i<a.length();i++)
	{// unsigned to avoid warning. i guess its bad.
		if (47<data[i] && data[i]<58){
			output*=10;
			output += (long)(data[i]-48);
		}
		if (data[i]=='[') break; //just in case we make substring in HDD::get_data() too big or small
	}
	return output;
}
std::string StdOut0(std::string cmd, bool throwing=true) {
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(cmd.c_str(), "r");
    if(stream) {
        while(!feof(stream)/*&& Present*/)
            if(fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
	//LastExitStatus=pclose(stream);
      //  if ((LastExitStatus!=0)&&throwing)throw (std::string) "Critical error stopping";
    }
    return data;
}
std::string month(int i){
	switch(i){
		case 0:return "Jan";
		case 1:return "Feb";
		case 2:return "Mar";
		case 3:return "Apr";
		case 4:return "May";
		case 5:return "June";
		case 6:return "July";
		case 7:return "Aug";
		case 8:return "Sep";
		case 9:return "Oct";
		case 10:return "Nov";
		case 11:return "Dec";
		default:return "Jan";
	}
}
#ifndef Result_To_String
#define Result_To_String
std::string ResultTToString(Result_t a)
{
	switch(a)
	{
		case Unfinished: return "Unfinished :|";
		case FinishedSuccess: return "Finished, Success :)";
		case FinishedFail: return "Finished, Failure :(";
		default: return "Unfinished";
	}
	return "Unfinished";
}
#endif
void trim(std::string &a){
	if(a[a.length()-1]=='\n')
		a.erase(a.length()-1);
}


std::string SizeToString(long a){
	if(a<1000){
		return std::to_string(a)+" Bytes";
	}
	if(a<1000000){ return std::to_string(a/1000)+" KB";}
	if(a<1000000000){return std::to_string(a/1000000)+" MB";}
	if(a<1000000000000){return std::to_string(a/1000000000) + " GB";}
	if(a<1000000000000000){return std::to_string(a/1000000000000)+" TB";}
	if(a<1000000000000000000){return std::to_string(a/1000000000000000)+" PB";}
	return std::to_string(a)+" Bytes";
}
int other(){
	std::string url="dates.csv";
	std::fstream *a=new std::fstream(url,std::ios::out);
	std::string data=StdOut0("echo ~");
	trim(data);
	data+=" , "+StdOut0("date");
	trim(data);
	std::cout<<data<<std::endl;
	//a.open();
	std::cout<<"ok  a: "<<a->is_open()<<std::endl;
	*a<<data<<std::endl;
	a->close();
	return 0;
}
#endif
