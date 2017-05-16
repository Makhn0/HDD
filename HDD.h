#ifndef HDD_h
#define HDD_h
#include <iostream>
#include <thread>
#include <time.h>
typedef void (*Proc)(std::string);
// type Proc to be used in run to make code pretier/better
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
		bool Present;
		bool Running;
		// bool getting data
			
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
		long StartTime;
		long RunTime;
		///* learn how to make constructor in src
		HDD(std::string path1) : path(path1),StartTime(time(0)){
			instances++;
		}
		//*/
		~HDD(){
			instances--;
		}
		void UpdateRunTime(){
			RunTime=time(0)-StartTime;
		}
		void count();
		std::thread * count_thread();
		
		bool presence();
		void get_data();
		void smartctl_run();
		void smartctl_kill();
		void dd_write(std::string*);
		void dd_read(std::string*);
		void hash_check(std::string*);
		void erase();
		void partition();
		void verify();
		void log(std::string*);
		void print();
		
		void run_body(std::string*);
		void run(std::string*);
		void reset();
		Proc * ProcQ;
		
};
//enum SmartControl {Unavailable, Available_Disabled, Avialable_Enabled}
#endif
