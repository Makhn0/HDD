CPFLAGS=--std=c++0x -pthread -Wall
os=linux

BinNameC:=$(BinName)_C

ifeq ($(os),nt)
BinName:=ewhde.exe
Editor:=notepad++
del:=del
endif

ifeq ($(os),linux)
BinName:=ewhde
Editor:=gedit
del:=sudo rm
endif

$(BinName):clean  main.o HDD.o
	$(CXX) $(CPFLAGS) $(args) -o $(BinName)  main.o HDD.o
$(BinNameC): main.o HDD_C.o
	$(CXX) $(CPFLAGS) $(args) -o $(BinNameC) main.o HDD_C.o
HDD_C.o:HDD_C.cpp
	$(CXX) -c $(CPFLAGS) $(args) HDD_C.cpp
HDD.o : HDD.cpp
	$(CXX) -c $(CPFLAGS) $(args) HDD.cpp
main.o : main.cpp
	$(CXX) -c $(CPFLAGS) $(args) main.cpp	
run : $(BinName)
	$(BinName)
runC : $(BinNameC) 
	$(BinNameC)
edit :
	$(Editor) main.cpp &
	$(Editor) HDD.cpp &
	$(Editor) HDD.h &
edit-all:edit
	$(Editor) makefile &
	$(Editor) HDD_C.cpp &
clean: 
	$(del) *.o
	$(del) $(BinName)
run-all: 
	make clean
	make edit-all
	make run
	echo "all running"
install:
	sudo cp ./$(BinName) /usr/bin/$(BinName)
load:
	#for use on server
	sudo cp ./$(BinName) ~/$(BinName)
	sudo ~/homeupdate.zsh
update:
	sudo git pull
test:
	$(CXX) $(args) -o test.exe test.cpp
open-test:
	$(Editor) test.cpp
