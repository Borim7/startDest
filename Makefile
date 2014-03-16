SRC= startDest.c
OBJ=$(SRC:.c=.o)
PRJ=startDest.exe

CC=gcc
CFLAGS= -Wall -std=c99 -m32

all: $(PRJ)

$(PRJ): $(OBJ)
	gcc $(CFLAGS) -o $@ $<

clean:
	del $(OBJ) $(PRJ)
