#ifndef Console_h
#define Console_h
class Console{
	/* stores client and path data and has popen wrapper functions*/
	public:
		static int instances;
		std::string HomePath;
		std::string path;
		std::ostream * dstream=&std::cerr;
		std::string PresentTask;
		std::string CmdString;
		std::string LastOutput;
		Console(std::string);
		int LastExitStatus;
		/*do i need constructor?*/
		std::ostream* task(std::string);
		std::string StdOut(std::string , bool=true);	
		void Command(std::string ,std::string ,bool=true );
		void Command(std::string ,bool=true );
};
#endif
