CC=gcc
CFLAGS=-std=c11 -Wall -Wextra -O2

all: SnakeGame.exe

SnakeGame.exe: main.c
	$(CC) $(CFLAGS) main.c -o SnakeGame.exe

clean:
	del /Q SnakeGame.exe 2>NUL
