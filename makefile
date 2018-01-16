CPFLAGS=--std=c++0x -pthread -Wall

src=src/methods.cpp src/HDD.cpp src/Erasure.cpp 

Rargs="-D_Erase"
BinName:=bin/ewhde.exe
DebugName:=$(BinName)_Debug

ifeq ($(OS),Windows_NT)
Editor:=notepad++
del:=del
Win:=-D_NT_
BinName:=$(BinName).exe
DebugName:=$(BinName)_Debug.exe
else

OS:=$(shell uname -s 2>/dev/null)
args:=-D_test_
endif

ifeq ($(OS),Linux)

sudo:=sudo
Editor:=gedit
del:=sudo rm -rf
endif


<<<<<<< HEAD
=======

>>>>>>> master
#all: $(binName) $(src)
$(BinName): lib/main.o lib/HDD.o lib/main_help.o lib/methods.o
	$(CXX) $(CPFLAGS) $(args) $(Win) -o $(BinName) -Iinclude  lib/main.o lib/HDD.o lib/methods.o 
	$(CXX) $(CPFLAGS) $(args) $(Win) -D_Debug -o $(DebugName) -Iinclude lib/main.o lib/HDD.o lib/methods.o 
lib/HDD.o : src/HDD.cpp 
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/HDD.o  src/HDD.cpp 
lib/main.o : src/main.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/main.o src/main.cpp 
lib/main_help.o : src/main_help.cpp
<<<<<<< HEAD
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -o lib/main_help.o src/main_help.cpp -Iinclude

=======
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/main_help.o src/main_help.cpp 
lib/methods.o : src/methods.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) -Iinclude -o lib/methods.o src/methods.cpp
>>>>>>> master
run : $(BinName)
	$(sudo) $(BinName)
edit :
	$(Editor) lib
	$(Editor) src
edit-all: edit
	$(Editor) makefile &
clean-help:
	$(del) lib
	$(del) bin
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
	$(sudo) git commit -am "$(commit)"
	$(sudo) git push #only for master
test: test.cpp
	$(CXX) --std=c++0x $(args) -o $(TestName) test.cpp
open-test:
	$(Editor) test.cpp
