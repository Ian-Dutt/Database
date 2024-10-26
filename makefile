CC=gcc
CFLAGS=-Wall -Wextra -pedantic -fPIC

all: database

database: ./main.c database.c dblang.c allocator.c
	$(CC) $(CFLAGS) -I./ -shared -o libdatabase.so database.c dblang.c allocator.c -lm

clean:
	rm *.o
