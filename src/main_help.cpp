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

#include "HDD.h"
#include "main_help.h"
#include "methods.h"

using namespace std;
void preamble(){	printstream=new std::stringstream;
	string csvpath=stdOut0("echo ~");
	cout<<"client home =" <<csvpath<<endl; 
	csvpath.append("/batch_csv");
	cout<<"checking for directory ~/batch_csv viz. "<< csvpath<<endl;
	//windows no likey
	#ifndef _NT_
	DIR * Diropen = opendir(csvpath.c_str());
	if(!Diropen){	
		cout<<"creating directory ~/batch_csv"<<endl;
		int dirmake=mkdir(csvpath.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(dirmake==-1) cout<<" couldn't create directory try sudo"<<endl;
		else cout<<" created !!!"<<endl;
	}
	else{	
		cout<< csvpath<< " already exists"<<endl;
	}
	cout<<" syncing clocks "<<endl;
	//TODO uncomment time sync
	cout<<stdOut0(" sudo ntpdate 192.168.1.1 ; date ")<<endl;
		
	//*/
	sleep(1);	

	debugstream=&std::cerr;
	printstream= new std::stringstream("");
	#endif
		//&cout;

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
}

void buffClear(string a[],int size)
{
	for(int i=0;i<size;i++)
	{
		a[i].clear();
	}
}
//reads from a up to H lines until special string or end of file is found, writes to buffer in strings of length col
//returns number of lines read from stringstream i.e. the "height" of the text
int getLines(std::stringstream *a,string buffer[],unsigned int H,int col){
	string buf;
	unsigned int j=0;
	for(unsigned int i=0;i<H;i+=j)
	{
		getline(*a,buf);
		
		//cout<<"vuf"<<buf<<endl;
		//if end of file or special line is reached,stops writing to buffer array 
		if(buf=="##end##"||a->eof()){
			return i;//passed to printHelp, so that extra blank lines aren't printed
			break;
		}
		//cout<<buffer[i]<<endl;
		//cout<<"0:"<<i<<endl;
		//this while loop allows in-collumn word wrapping
		j=0;
		while((j*col)<buf.size()){
		//	cout<<j<<"j*col"<<(j*col)<<endl;
			if((i+j)<H)buffer[i+j]=buf.substr(j*col,col);
			else break;
			j+=1;
		}
		
		buf="";
	}
	return -1;
}
//prints the contents of a in two collumns of height H, and width 90
void printToScreen(std::stringstream * a,int H=15,int col=90)
{
	//cout<<"printToScree"<<endl;
	if(col==-1)
	{
		cout<<"-1"<<endl;
		cout<<a->str();
		return;
	}
	string buffer[H];
	string buffer1[H];
	int h=H;
	int h1=H;
	//cout<<" printing to screen"<<endl;
	while(!(a->eof())){
		h=H;
		h1=H;
		h=getLines(a,buffer,H,col);
		//cout<<"got first buffer"<<endl;
		if(a->eof()) break;
		h1=getLines(a,buffer1,H,col);	
		printHelp(buffer,buffer1,((h<h1)?h1:h),col);
		buffClear(buffer,H);
		buffClear(buffer1,H);
	}
	if((a->rdstate()&(std::ifstream::badbit|std::ifstream::failbit))!=0)a->clear();

	//*/
}
void print_head(){
	//cout<<printstream->str()<<endl;;
	cout<<"Welcome to Eric's Wonderful Hard Drive Eraser !!! :D \n";
	cout<<"running on:"<<stdOut0("pwd");
	cout<<"BatchName : "<<BatchName<<endl;
	//cout<<"total HDDs  : "<<HDD::instances<<endl;
	cout<<"########################################################\n\n";
}
void print(HDD * HDDs[], int length)
{
	print_head();
	static int count;//don't remember why that is static
	count++;
	cout<<count<<endl;
	for(int i=0;i<length;i++)	
	{	
		if(HDDs[i]->Present)
			HDDs[i]->print(
			printstream
			//&cout
			);
	}
	//cout<<printstream->str();
	printToScreen(printstream);
	
	//cout<<"end"<<endl;
}

void printHelp(string a[], string b[],int n,int col)
{
	//prints two collumns of output side by side, a in first collumn
	//b in second, n is length of arrays, col is width of the collums
	bool eos=false;
	
	//cout<<"printHelp start"<<endl;
	for(int i =0; i<n;i++){
		//cout<<i<<":";
		for(int j=0;j<col;j++){
			if(!eos){
				if(a[i][j])cout<<(a[i][j]);
				else eos=true;			
			}//pad till end of collumn
			else cout<<" ";
		}
		eos=false;
		//separate collumns
		cout<<"  ";
		for(int j=0;j<col;j++){
			if(!eos){
				if(b[i][j])cout<<(b[i][j]);
				else eos=true;			
			}
			else{break;} //cout<<" "<<endl;
		}
		cout<<endl;
		eos=false;		
	}

}
void printHelpTest(){
	string a[3];
	a[0]="look ma";
	a[2]="no hands";
	a[1]="no more";
	string b[3];
	b[0]="look m5a";
	b[2]="no hands";
	b[1]="no more";
	printHelp(a,b,4,10);
}

void printToScreen_test(){
	cout<<"printToScreen_test():"<<endl;\
	print_head();
	string stuff="first line\n second line\n33333333333\n4444444444444\n wh5at??\n66613245678901234567890123456798012345679012\n3456790123456790123456790123456790123456790\nwhat in tarnation\nbut what theremare \n ohaas\n even more\thats righ\n o yea \nothing to see here \n one more\n end";
	HDD * a=new HDD("/dev/sdf");
	HDD*b=new HDD("/dev/sdg");
	a->reset();
	b->reset();
	//a->print();
	std::stringstream* s=new std::stringstream;
	a->print(s);
	b->print(s);
 	*s<<stuff<<endl;
	printToScreen(s);
///*	
	s->str("");s->flush();s->sync();
	a->print(s);
	b->print(s);
	cout<<"second printToScreen call"<<endl;
	printToScreen(s);
	cout<<"end test"<<endl;
	//*/
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
