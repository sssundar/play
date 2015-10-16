all: testOperations

testOperations: testOperations.o
	gcc -o testOperations testOperations.o

testOperations.o: testOperations.c
	gcc -c testOperations.c

clean:
	rm -f testOperations *.o *~

.PHONY: all clean
