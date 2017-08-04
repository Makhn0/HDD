#ifndef Exception_h
#define Exception_h
#include <exception>
#include "HDD.h"
class HDD_Exception : std::exception{
	protected:
		HDD * hdd;
		std::string error;
	public:
		std::string what(){ return hdd->path+error;} 
		HDD_Exception(HDD*a)
		{	
			hdd=a;
			std::cerr<<what()<<std::endl;
		}
		HDD_Exception(HDD*a,std::string b):HDD_Exception(a){
			error=b;
		}
};
class Smartctl_Exception : HDD_Exception{
	public:
		std::string what(){ return hdd->path+"Smartctl Error";}

};
class Pull_Out_Exception : HDD_Exception{
	public:
		Pull_Out_Exception(HDD* a) : HDD_Exception(a){
			error=" pulled out exception";
		}
		std::string what() {return hdd->path;}
};
class DD_Exception : HDD_Exception{
	public:
		DD_Exception(HDD* a) : HDD_Exception(a){
			std::cerr<<hdd->path
				<<"DD WRITE/READ ERROR"<<std::endl;
		}
		std::string what() {return hdd->path+" dd write/read exception";}
};
#endif
