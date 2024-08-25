CC=gcc
CFLAGS=-Wall -Wextra -pedantic

all: database

database: ./main.c utils.c database.c dblang.c
	$(CC) $(CFLAGS) -I./ -o database main.c utils.c database.c dblang.c -lm