main.exe : main.o HDD.o
	g++ -o main.exe --std=c++0x -Wall main.o HDD.o
HDD.o : HDD.cpp
	g++ -c --std=c++0x -Wall HDD.cpp
main.o : main.cpp
	g++ -c --std=c++0x -Wall main.cpp
run : main.exe
	main
open :
	notepad++ main.cpp
	notepad++ HDD.cpp
	notepad++ HDD.h
all:open
	notepad++ makefile
clean:
	del main.o
	del HDD.o
test:
	g++ -o test.exe test.cpp
open-test:
	notepad++ test.cpp
