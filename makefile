CC=gcc
CFLAGS=-Wall -Wextra -pedantic

all: database

database: ./main.c utils.c database.c
	$(CC) $(CFLAGS) -I./ -o database main.c utils.c database.c 