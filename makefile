CPFLAGS=--std=c++0x -pthread -Wall
os=linux

src=main.cpp HDD.cpp Exceptions.h
Rargs="-D_Erase"
ifeq ($(os),nt)
BinName:=ewhde.exe
TestName:=test.exe
Editor:=notepad++
del:=del
endif

ifeq ($(os),linux)
BinName:=ewhde
TestName:=test
Editor:=gedit
del:=sudo rm
endif

$(BinName): main.o HDD.o
	$(CXX) $(CPFLAGS) $(args) -o $(BinName) main.o HDD.o 
	$(CXX) $(CPFLAGS) $(args) -D_Test -o $(BinName)_test main.o HDD.o
HDD.o : HDD.cpp 
	$(CXX) -c $(CPFLAGS) $(args) HDD.cpp 
main.o : main.cpp
	$(CXX) -c $(CPFLAGS) $(args) main.cpp 
run : $(BinName)
	$(BinName)
edit :
	$(Editor) main.cpp &
	$(Editor) HDD.cpp &
	$(Editor) HDD.h &
	$(Editor) methods.cpp
edit-all:edit
	$(Editor) makefile &
	$(Editor) homeupdate.zsh &
clean-help:
	$(del) *.o
	$(del) $(BinName)
	$(del) *~
clean: 
	sudo make clean-help || echo 'already clean'

update:
	sudo git pull
	sudo make all args=$(Rargs)
all: 
	sudo make clean
	sudo make args="$(args)"
	sudo make load
install:
	sudo cp ./$(BinName) /usr/bin/$(BinName)
load: $(BinName)
	#for use on server
	sudo cp ./$(BinName) ~/$(BinName)
	sudo ./homeupdate.zsh  1>/dev/null
push:
	sudo make clean
	sudo git add -A .
	sudo git commit -m "$(commit)"
	sudo git push
test: test.cpp
	$(CXX) --std=c++0x $(args) -o $(TestName) test.cpp
open-test:
	$(Editor) test.cpp
