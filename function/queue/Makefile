test: test.o queue.o
	gcc test.o queue.o -o test -lpthread

queue.o: queue.c queue.h
	gcc -Wall -g -c queue.c -o queue.o

test.o: test.c queue.h
	gcc -Wall -g -c test.c -o test.o

clean:
	rm -rf *.o
	
