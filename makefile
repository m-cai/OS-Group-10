all: rr

rr: main.o rr.o
	gcc -std=c99 -g -o rr main.o rr.o

rr.o: rr.c rr.h
	gcc -c -std=c99 -g rr.c

main.o: main.c rr.h
	gcc -c -std=c99 -g main.c

clean:
	rm *.o rr
