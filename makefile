CPFLAGS=--std=c++0x -pthread -Wall
src=main.cpp HDD.cpp Exceptions.h
classes=Erasure.o HDD.o HDD_Base.o
lib=methods.o
Rargs="-D_Erase"

BinName:=ewhde
DebugName:=$(BinName)_Debug

ifeq ($(OS),Windows_NT)
Editor:=notepad++
del:=del
Win:=-D_NT_

else
OS:=$(shell uname -s 2>/dev/null)
args:=-D_test_
endif

ifeq ($(OS),Linux)
sudo:=sudo
Editor:=gedit
del:=sudo rm
endif

$(BinName): main.o HDD.o Erasure.o HDD_Base.o methods.o
	$(CXX) $(CPFLAGS) $(args) $(Win) -o $(BinName) main.o $(classes) $(lib)
	$(CXX) $(CPFLAGS) $(args) $(Win) -D_Debug -o $(DebugName) main.o $(classes) $(lib)
main.o : main.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) main.cpp 
Erasure.o : Erasure.cpp 
	$(CXX) -c $(CPFLAGS) $(args) $(Win) Erasure.cpp 
HDD.o : HDD.cpp 
	$(CXX) -c $(CPFLAGS) $(args) $(Win) HDD.cpp 
HDD_Base.o : HDD_Base.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) HDD_Base.cpp
methods.o: methods.cpp
	$(CXX) -c $(CPFLAGS) $(args) $(Win) methods.cpp
run : $(BinName)
	$(sudo) $(BinName)
edit :
	$(Editor) main.cpp &
	$(Editor) HDD.cpp &
	$(Editor) HDD.h &
	$(Editor) methods.cpp
edit-all: edit
	$(Editor) makefile &
	$(Editor) homeupdate.zsh &
clean-help:
	$(del) *.o
	$(del) $(BinName)
	$(del) *~
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
