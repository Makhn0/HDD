#include <iostream>
#include <string>
#include "Console.h"

using namespace std;
int Console::instances;
Console::Console(std::string path):path(path){	
	#ifndef _NT_
		Console::Command("echo ~","creating Console");
		HomePath=LastOutput;
	#endif
	instances++;
}
string Console::StdOut(string cmd, bool throwing) {
    string data;
    FILE * stream;
    const int max_buffer= 256;
    char buffer[max_buffer];
    stream= popen(cmd.c_str(), "r");
    if(stream) {
        while(!feof(stream))/* && Present was removed during refactoring */
            if(fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
	LastExitStatus=pclose(stream);
        if ((LastExitStatus!=0)&&throwing)throw  "Last System call returned not 0";
    }
    return data;
}
void Console::Command(string a,string task,bool throwing){
	this->task(task);
	Command(a,throwing);
	*this->task(task)<<" :done"<<endl;
}
void Console::Command(string a,bool throwing){
	a.append(" 2>&1");
	CmdString=a;
	*dstream<<path<<" : Last Command:"<<CmdString<<endl;
	LastOutput=StdOut(CmdString,throwing);
	
	*dstream<<path<<" : Last Output:"<<LastOutput
			<<"_::exit status :"<<LastExitStatus<<endl;	
}
std::ostream * Console::task(string task=""){
	PresentTask=task;
	return &(*dstream<<path<<" : "<<PresentTask<<endl);
}
