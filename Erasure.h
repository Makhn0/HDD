#ifndef Erasure_h
#define Erasure_h
#include "HDD.h"//include result_t data type?
#include <iostream>

class Erasure : public HDD{

	

	long currentLBA=0;
	Result_t Status=Unfinished;

	time_t StartTime=0;
	long EraseStart=0;
	long EraseEnd=0;
	long EndTime=0;
	time_t eta;
	long RunTime;
	public:	
	Erasure(std::string pattern):HDD(pattern){
		
	}
	void erase(std::string*);
	void erase(char);
	void erase_c(char);
	void erase_n(char);
		
	void Write_All( char pattern,long,long);
	void erase_dd();
	void erase_debrief();	
	bool Long_Verify(unsigned char pattern,long,long);



};


#endif
