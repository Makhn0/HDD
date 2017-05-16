CPFLAGS = --std=c++0x -Wall
CC= g++
editor=notepad++
bwipe.exe: main.o HDD.o
	$(CC) -o bwipe.exe $(CPFlAGS) main.o HDD.o
HDD.o : HDD.cpp
	$(CC) -c $(CPFLAGS) HDD.cpp
main.o : main.cpp
	$(CC) -c $(CPFLAGS) main.cpp	
run : bwipe.exe
	bwipe.exe
edit :
	notepad++ main.cpp
	notepad++ HDD.cpp
	notepad++ HDD.h
all:edit
	notepad++ makefile
clean:
	del *.o
	del bwipe.exe
	del bwipe.out
	
test:
	$(CC) -o test.exe test.cpp
open-test:
	notepad++ test.cpp
linux:
#	$2 I'm only gussing this will work
	make -f makefile-linux ${arg}
