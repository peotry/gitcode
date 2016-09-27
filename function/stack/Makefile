test: test.o stack.o
	gcc test.o stack.o -o test -lpthread

stack.o: stack.c stack.h
	gcc -Wall -g -c stack.c -o stack.o

test.o: test.c stack.h
	gcc -Wall -g -c test.c -o test.o

clean:
	rm -rf *.o
	
