SRC= startDest.cc helper.cc
OBJ=$(SRC:.cc=.o)
PRJ=startDest.exe

CXX=g++
CXXFLAGS= -g -O2 -Wall -std=c++11 
# for debugging mwindows need to be removed, option is needed to start cmd windows
LDFLAGS= -mwindows -static-libgcc -static-libstdc++ -static


all: $(PRJ)

$(PRJ): $(OBJ)
	g++ -o $@ $+ $(CXXFLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ) $(PRJ)
