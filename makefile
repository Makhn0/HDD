#ECHO on
BinName:=bin/ewhde
CPFLAGS=--std=c++0x -pthread -Wall
DebugName:=$(BinName)_Debug
OS:=$(shell uname -s 2>/dev/null)
Rargs="-D_Erase"
src=src/methods.cpp src/HDD.cpp src/Erasure.cpp 

ifeq ($(OS),Windows_NT)
Editor:=notepad++
del:=del
Win:=-D_NT_
BinName:=$(BinName).exe
DebugName:=$(DebugName).exe
else
endif

ifeq ($(OS),Linux)
sudo:=sudo
Editor:=gedit
del:=sudo rm -rf

endif

$(BinName) : lib/main.o lib/HDD.o lib/main_help.o lib/methods.o lib/Erasure.o
	$(CXX) $(CPFLAGS) $(args) $(Win) -Iinclude -o $(BinName) \
		lib/main.o lib/methods.o lib/HDD_Base.o lib/HDD.o lib/Erasure.o 
	$(CXX) $(CPFLAGS) $(args) $(Win) -Iinclude -D_Debug -o $(DebugName) \
		lib/main.o lib/methods.o lib/HDD_Base.o lib/HDD.o lib/Erasure.o 
lib/HDD_Base.o : src/HDD_Base.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/HDD_Base.o src/HDD_Base.cpp 
lib/HDD.o : src/HDD.cpp lib/HDD_Base.o
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/HDD.o src/HDD.cpp 
lib/Erasure.o : src/Erasure.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/Erasure.o src/Erasure.cpp 
lib/main.o : src/main.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/main.o src/main.cpp 
lib/main_help.o : src/main_help.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/main_help.o src/main_help.cpp 
lib/methods.o : src/methods.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/methods.o src/methods.cpp
run : $(BinName)
	$(sudo) $(BinName)
edit :
	$(Editor) include
	$(Editor) src
edit-scripts :
	$(Editor) scripts &
edit-all : edit edit-scripts
	$(Editor) makefile &
clean-help :
	@echo omg $(OS)
	$(del) lib &
	$(del) bin &
clean: 
	$(sudo) make clean-help || echo 'already clean'
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