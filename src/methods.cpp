#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
using namespace std;
long myStol(string a){
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
string trim(string &a){
	if(a[a.length()-1]=='\n')
		a.erase(a.length()-1);
	return a;
}

string stdOut0(string cmd) {
    string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(cmd.c_str(), "r");
    if(stream) {
        while(!feof(stream)/*&& Present*/)
            if(fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
	//LastExitStatus=pclose(stream);
      //  if ((LastExitStatus!=0)&&throwing)throw (string) "Critical error stopping";
    }
    return trim(data);
}
string month(int i){
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

string sizeToString(long a){
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
	string url="dates.csv";
	std::fstream *a=new std::fstream(url,std::ios::out);
	string data=stdOut0("echo ~");
	trim(data);
	data+=" , "+stdOut0("date");
	trim(data);
	cout<<data<<endl;
	//a.open();
	cout<<"ok  a: "<<a->is_open()<<endl;
	*a<<data<<endl;
	a->close();
	return 0;
}