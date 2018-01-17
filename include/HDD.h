#ifndef HDD_h
#define HDD_h

#include <ostream>
#include <iostream>
#include <thread>
#include <time.h>
#include "HDD_Base.h"

enum Result_t{Unfinished,FinishedSuccess,FinishedFail,Incomplete};

class HDD: public HDD_Base {
	public:
		static int instances;

		//run properties
		Result_t Status=Unfinished;
		std::string SmartEta="";
		std::string PresentTask;
		std::string Exception="none";
		//printing commands
		std::string CmdString;
		std::string LastOutput;
		std::string TempLogFileName;
		int LastExitStatus;
		time_t StartTime=0;
		long EndTime=0;
		long RunTime;
	
		///* learn how to make constructor in src*/
		HDD(std::string path) : HDD_Base(path), StartTime(time(0)){
			
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
		std::string ResultTToString(Result_t a);
		
		void UpdateRunTime();
		std::string StdOut(std::string , bool);
		void Command(std::string ,std::string ,bool );
		void Command(std::string ,bool );

		/*why defaults no work?*/
		void reset();
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
		void resolve_size();

		std::ostream* task(std::string);
		void exception_catch(std::exception);
		void exception_catch(std::string);
		void exception_catch(const char *);
		void log(std::string*);
		void print(std::ostream *);
		void print_help(std::ostream *);
		void print_help2(std::ostream *);
		void print();
		void print_csv(std::fstream* );

		void erase (char);

};
#endif