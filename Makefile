SRC= startDest.c
OBJ=$(SRC:.c=.o)
PRJ=startDest.exe

CXX=g++
CXXFLAGS= -g -m32 -Wall -std=c++0x
LDFLAGS= -mwindows

all: $(PRJ)

$(PRJ): $(OBJ)
	g++ -o $@ $< $(CXXFLAGS)

clean:
	del $(OBJ) $(PRJ)
