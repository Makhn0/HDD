#ifndef HDD_h
#define HDD_h

#include <ostream>
#include <iostream>
#include <thread>
#include <time.h>
#include "HDD_Base.h"

enum Result_t{Unfinished,FinishedSuccess,FinishedFail,Incomplete};
/*begins the testing procedure before erasing */
class HDD: public HDD_Base {
	public:
		

		//run properties
		bool Running;
		Result_t Status=Unfinished;
		std::string SmartEta="";
		std::string Exception="none";
		time_t StartTime=0;
		long EndTime=0;
		long RunTime;
	
		HDD(std::string path);
		
		std::string ResultTToString(Result_t a);
		
		void UpdateRunTime();
		


		/*why defaults no work?*/
		void reset();
		
		
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


		void exception_catch(std::exception);
		void exception_catch(std::string);
		void exception_catch(const char *);
		void log(std::string*);
		void print(std::ostream *);
		void print_help(std::ostream *);
		void print_help2(std::ostream *);
		void print_csv(std::fstream* );

		void erase (char);

};
#endif
