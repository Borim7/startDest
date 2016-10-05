SRC= startDest.cc helper.cc
OBJ=$(SRC:.cc=.o)
PRJ=startDest.exe

CXX=g++
CXXFLAGS= -g -O2 -Wall -std=c++11 
LDFLAGS= -mwindows -static-libgcc -static-libstdc++ -static


all: $(PRJ)

$(PRJ): $(OBJ)
	g++ -o $@ $+ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(PRJ)
