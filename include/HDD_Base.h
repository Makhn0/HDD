#ifndef HDD_Base_H
#define HDD_Base_h
class HDD_Base{
	public:
		//filesystem
		std::ostream * dstream=&std::cerr;
		std::string HomePath;
		std::string path;
		//file descriptor (changes when switching harddrives out i think)
		int fd;
		bool Present;
		bool Running;
		//hdd properties
		bool SmartSupport=false;
		std::string Model;
		std::string ModelFamily;
		std::string SerialNumber;
		long size;//=0;
		//constructor
		HDD_Base(std::string path) :path(path){}
		//functions
		bool presence();
		bool presence(bool);
		void Presence_checker();
		void Presence_checker(bool);



		
};
#endif