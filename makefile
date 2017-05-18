CPFLAGS=--std=c++0x -pthread -Wall
os=NT

#ifeq($(os),"NT")
Editor:=notepad++
BinName:=ewhde.exe
del:=del
#endif

ifeq ($(os),linux)
Editor:=gedit
BinName:=ewhde
del:=sudo rm
endif
$(BinName): main.o HDD.o
	$(CXX) $(CPFLAGS) -o $(BinName)  main.o HDD.o
HDD.o : HDD.cpp
	$(CXX) -c $(CPFLAGS) HDD.cpp
main.o : main.cpp
	$(CXX) -c $(CPFLAGS) main.cpp	
run : $(BinName)
	$(BinName) ${args}
edit :
	$(Editor) main.cpp
	$(Editor) HDD.cpp
	$(Editor) HDD.h
all:edit
	$(Editor) makefile
clean:
	$(del) *.o
	$(del) $(BinName)

test:
	$(CXX) -o test.exe test.cpp
open-test:
	$(Editor) test.cpp
