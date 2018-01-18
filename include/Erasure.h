#ifndef Erasure_h
#define Erasure_h
#include "HDD.h"//include result_t data type?
#include <iostream>
#include <time.h>

class Erasure : public HDD{	
	private:	
		long currentLBA=0;

	public:
		long EraseStart=0;
		long EraseEnd=0;
		time_t eta;
		Result_t Status=Unfinished;
		Erasure(std::string pattern):HDD(pattern){}
		void run_body(std::string*,char);
		void run(std::string*,char);
		void PrintDate(std::string, tm * );
		void erase(std::string*);
		void erase(char);
		void erase_c(char);
		void erase_n(char);
			
		void Write_All( char pattern,long,long);
		void erase_dd();
		void erase_debrief();	
		bool Long_Verify(unsigned char pattern,long,long);
		void print(std::ostream * textgohere);

};


#endif