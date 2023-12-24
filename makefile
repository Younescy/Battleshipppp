CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99

all: battleship

battleship: main.o game.o
	$(CC) $(CFLAGS) -o battleship main.o game.o

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

game.o: game.c game.h
	$(CC) $(CFLAGS) -c game.c

clean:
	rm -f battleship *.o
