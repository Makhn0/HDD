#ifndef main_help_h
#define main_help_h

std::string BatchName;
std::string argPath;
std::stringstream * printstream;
std::ostream * debugstream;
char pattern = 0x00;

extern std::string month(int i);
extern std::string StdOut0(std::string i);//needs methods link?
extern std::string trim(std::string&);

void printhelp(std::string a[], std::string b[],int n,int col);
void BuffClear(std::string a[],int size);

//reads from a up to H lines until special string or end of file is found, writes to buffer in strings of length col
//returns number of lines read from stringstream i.e. the "height" of the text
int getlines(std::stringstream *a,std::string buffer[],unsigned int H,int col);
//prints the contents of a in two collumns of height H, and width 90
void PrintToScreen(std::stringstream * a,int H,int);

void print(HDD * HDDs[], int length);

void contPrint(HDD * HDDs[], int length);

void printhelptest();
void preamble();
void PrintToScreen_test();
#endif