#ifndef HDD_h
#define HDD_h
#include <iostream>
#include <thread>

class HDD{
	public:

		// 0 not killed, 1 killed, 2 some errors//probably not used though
		int SmartKill;
		//probably not going to be used
		int ShredPid;
		int SmartctlScsiPid;
		//old error codes
		
		std::string path;
		
		bool Lock;//not used yet
		bool present;
		bool running;
		
			
		bool SmartSupport=false;
		std::string Model;
		std::string ModelFamily;
		std::string SerialNumber;
		std::string UserCapacity="";//probaby change to int
		long size=0;
		
		std::string PresentTask;
		std::string Exception="none";
		std::string CmdString;
		static int instances;
		int RunTime;
		///* learn how to make constructor in src
		HDD(std::string path1){
			
			this->path=path1;
			this->RunTime=0;
			instances++;
		}
		//*/
		~HDD(){
			instances--;
		}
		void count();
		std::thread * count_thread();
		
		bool presence();
		void get_data();
		void smartctl_run();
		void smartctl_kill();
		void dd_write(std::string);
		void dd_read(std::string);
		void hash_check(std::string);
		void erase();
		void partition();
		
		void print();
		
		void run_body(std::string);
		void run(std::string);
		void reset();
		
};
#endif