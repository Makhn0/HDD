#ifndef HDD_Base_H
#define HDD_Base_h
#include "Console.h"
/* checks the sata ports to see if a hard drive is present and reads basic hard drive data*/
class HDD_Base: public Console{
	public:
		//file descriptor (changes when switching harddrives out i think)
		int fd;
		bool Present; //whether there is a harddrive in the port
		//hdd properties
		bool SmartSupport=false;
		std::string Model;
		std::string ModelFamily;
		std::string SerialNumber;
		long size;//=0;

		HDD_Base(std::string path) :Console(path){}
		bool presence_check();
		void Presence_checker(bool);// continuosly checks to see if hard drive is plugged in
		void get_data();
		std::string str();
		std::ostream * print(std::ostream *);		
};
#endif
