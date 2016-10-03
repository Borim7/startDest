SRC= startDest.cc helper.cc
OBJ=$(SRC:.cc=.o)
PRJ=startDest.exe

CXX=g++
CXXFLAGS= -g -Wall -std=c++11
LDFLAGS= -mwindows

all: $(PRJ)

$(PRJ): $(OBJ)
	g++ -o $@ $+ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(PRJ)
