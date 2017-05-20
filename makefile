CPFLAGS=--std=c++0x -pthread -Wall
#os=linux
BinName:=ewhde
BinNameC:=$(BinName)_C
ifeq ($(os),nt)
Editor:=notepad++
del:=del
endif

ifeq ($(os),linux)
Editor:=gedit
del:=sudo rm
endif

$(BinName): main.o HDD.o
	$(CXX) $(CPFLAGS) -o $(BinName) main.o HDD.o
$(BinNameC): main.o HDD_C.o
	$(CXX) $(CPFLAGS) -o $(BinNameC) main.o HDD_C.o
HDD_C.o:HDD_C.cpp
	$(CXX) -c $(CPFLAGS) HDD_C.cpp
	
HDD.o : HDD.cpp
	$(CXX) -c $(CPFLAGS) HDD.cpp
main.o : main.cpp
	$(CXX) -c $(CPFLAGS) main.cpp	
run : $(BinName)
	$(BinName) ${args}
runC : $(BinNameC) 
	$(BinNameC) ${args}
edit :
	$(Editor) main.cpp
	$(Editor) HDD.cpp
	$(Editor) HDD.h
edit-all:edit
	$(Editor) makefile
	$(Editor) HDD_C.cpp
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
	$(CXX) -o test.exe test.cpp
open-test:
	$(Editor) test.cpp