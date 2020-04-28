# choose your compiler
CC=gcc

mysh: sh.o get_path.o main.c 
	$(CC) -g main.c sh.o get_path.o -o mysh -g

sh.o: sh.c sh.h
	$(CC) -g -c sh.c -g

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

clean:
	rm -rf sh.o get_path.o mysh sleep