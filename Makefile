SRC= startDest.c
OBJ=$(SRC:.c=.o)
PRJ=startDest.exe

CXX=g++
CXXFLAGS= -g -Wall -std=c++11
LDFLAGS= -mwindows

all: $(PRJ)

$(PRJ): $(OBJ)
	g++ -o $@ $< $(CXXFLAGS)

clean:
	rm -f $(OBJ) $(PRJ)
