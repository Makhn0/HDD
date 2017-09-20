#ifndef HDD_h
#define HDD_h

#include <ostream>
#include <iostream>
#include <thread>
#include <time.h>

enum Result_t{Unfinished,FinishedSuccess,FinishedFail};

class HDD{
	public:
		//port properties
		const std::string HomePath;
		const std::string path;
		int fd;
		static int instances;
		bool Present;
		bool Running;		
		//hdd properties
		bool SmartSupport=false;
		std::string Model;
		std::string ModelFamily;
		std::string SerialNumber;
		long size;//=0;
	
		//run properties
		long currentLBA=0;
		Result_t Status=Unfinished;
		std::string PresentTask;
		std::string Exception="none";
		std::string CmdString;
		std::string LastOutput;
		std::string TempLogFileName;
		int LastExitStatus;
		std::ostream * dstream=&std::cerr;

		
		time_t StartTime=0;
		long EraseStart=0;
		long EraseEnd=0;
		long EndTime=0;
		time_t eta;
		long RunTime;
		
		///* learn how to make constructor in src*/
		HDD(std::string path) : path(path),StartTime(time(0)){
			//TODO define client
			/*
			Command("echo ~");
			HomePath=LastOutput;
			//*/
			instances++;
		}
		//*/
		~HDD(){
			instances--;
		}
		void UpdateRunTime(){
			RunTime=time(0)-StartTime;
		}

		std::string StdOut(std::string , bool);
		void Command(std::string ,std::string ,bool );
		void Command(std::string ,bool );

		/*why defaults no work?*/

		bool presence();
		bool presence(bool);
		void Presence_checker();
		void Presence_checker(bool);

		void get_data();
		void smartctl_run();
		bool smartctl_running();
		int smart_var(int&,std::string);
		bool bb_test();
		void smartctl_kill();
		void dd(std::string*);
		void dd_write(std::string*,std::string);
		void dd_read(std::string*,std::string);
		void hash_check(std::string*,std::string,std::string);
		void erase(std::string*);
		void erase(char);
		void erase_c(char);
		void erase_n(char);
		
		void Write_All( char pattern,long,long);
		void erase_dd();
		void erase_debrief();	
		bool Long_Verify(unsigned char pattern,long,long);

		void exception_catch(std::exception);
		void exception_catch(std::string);
		void exception_catch(const char *);
		void log(std::string*);
		void print(std::ostream *);
		void print();
		void print_csv(std::fstream* );
		void run_body(std::string*,char);
		void run(std::string*,char);
		void reset();
};

#endif
