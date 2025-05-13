main: vec.o server.o
	gcc -g server.o vec.o -o main -lpthread


vec.o: libraries/vec.c
	gcc -c libraries/vec.c -o vec.o

server.o: server.c
	gcc -c server.c -o server.o