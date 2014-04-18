SRC= startDest.c
OBJ=$(SRC:.c=.o)
PRJ=startDest.exe

CC=gcc
CFLAGS= -Wall -std=c99 -m32
CXX=g++
CXXFLAGS= -Wall -m32

all: $(PRJ)

$(PRJ): $(OBJ)
	g++ $(CFLAGS) -o $@ $<

clean:
	del $(OBJ) $(PRJ)
