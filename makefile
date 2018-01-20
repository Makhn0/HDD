BinName:=bin/ewhde
CPFLAGS=--std=c++0x -pthread -Wall
DebugName:=$(BinName)_Debug
Rargs="-D_Erase"
src=src/methods.cpp src/HDD.cpp src/Erasure.cpp 

OBJ:= lib/Console.o lib/HDD_Base.o lib/HDD.o lib/Erasure.o 
ifeq ($(OS),Windows_NT)

Editor:=notepad++
del:=del /F /Q
Win:=-D_NT_
BinName:=$(BinName).exe
DebugName:=$(DebugName).exe

else

# needs to be after 
OS:=$(shell uname -s 2>/dev/null)

endif

ifeq ($(OS),Linux)

sudo:=sudo
Editor:=gedit
del:=sudo rm -f

endif

$(BinName) : $(OBJ) lib/main.o lib/main_help.o lib/methods.o
	$(CXX) $(CPFLAGS) $(args) $(Win) -Iinclude -o $(BinName) \
		lib/main.o lib/methods.o\
		  $(OBJ)
	$(CXX) $(CPFLAGS) $(args) $(Win) -Iinclude -D_Debug -o $(DebugName) \
		lib/main.o lib/methods.o\
		  $(OBJ)
lib/Console.o : src/Console.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/Console.o src/Console.cpp
lib/HDD_Base.o : src/HDD_Base.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/HDD_Base.o src/HDD_Base.cpp 
lib/HDD.o : src/HDD.cpp lib/HDD_Base.o
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/HDD.o src/HDD.cpp 
lib/Erasure.o : src/Erasure.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/Erasure.o src/Erasure.cpp 
lib/main.o: src/main.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/main.o src/main.cpp 
lib/main_help.o: src/main_help.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/main_help.o src/main_help.cpp 
lib/methods.o: src/methods.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/methods.o src/methods.cpp
lib/obs.a : $(OBJ)
	ar rvs lib/obs.a $(OBJ)
testing: lib/obs.a lib/main.o lib/main_help.o lib/methods.o
	$(CXX) $(CPFLAGS) $(args) $(Win) -Iinclude -o testing \
		 lib/methods.o  lib/main.o lib/obs.a
		  
	
run: $(BinName)
	$(sudo) $(BinName)
edit:
	$(Editor) include/* &
	$(Editor) src/* &
edit-scripts:
	$(Editor) scripts &
edit-all: edit edit-scripts
	$(Editor) makefile &
clean:
#without /* didn't work on linux
#didn't work with it on windows
	$(del) lib/* &
	$(del) bin/* &
update:
	$(sudo) git pull
	$(sudo) make all args=$(Rargs)
all: 
	$(sudo) make clean
	$(sudo) make args="$(args)"
	$(sudo) make load
install:
	$(sudo) cp ./$(BinName) /usr/bin/$(BinName)
load: $(BinName)
	#for use on server
	$(sudo) cp ./$(BinName) ~/$(BinName)
	$(sudo) ./homeupdate.zsh  1>/dev/null
push:
	$(sudo) make clean
	$(sudo) git commit -am "$(commit)"#commit
	$(sudo) git push
