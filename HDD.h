#ifndef HDD_h
#define HDD_h
#include <ostream>
#include <iostream>
#include <thread>
#include <time.h>

class HDD{
	public:
		const std::string path;
		static int instances;
		
		bool Present;
		bool Running;		
		
		bool SmartSupport=false;
		std::string Model;
		std::string ModelFamily;
		std::string SerialNumber;
		std::string UserCapacity="";
		
		long size=0;
		
		std::string PresentTask;
		std::string Exception="none";
		std::string CmdString;
		std::string LastOutput;
		std::string TempLogFileName;
		int LastExitStatus;


		
		long StartTime=0;
		long EndTime=0;
		long RunTime;
		
		///* learn how to make constructor in src*/
		HDD(std::string path) : path(path),StartTime(time(0)){
			instances++;
		}
		//*/
		~HDD(){
			instances--;
		}
		void UpdateRunTime(){
			RunTime=time(0)-StartTime;
		}
		void Command(std::string a,std::string b,bool err2out);
		void Command(std::string a);
		
		bool presence();
		void get_data();
		void smartctl_run();
		bool smartctl_running();
		void smartctl_kill();
		void dd_write(std::string*);
		void dd_read(std::string*);
		void hash_check(std::string*);
		void erase(std::string*);
		void erase();
		void erase_debrief();
		void partition();
		void verify();
		
		void log(std::string*);
		void print(std::ostream *);
		void print();
		void run_body(std::string*);
		void run(std::string*);
		void reset();
};
//enum SmartControl {Unavailable, Available_Disabled, Avialable_Enabled}
#endif
