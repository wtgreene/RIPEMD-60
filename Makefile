CC = gcc
CFLAGS = -Wall -std=c99 -g

hash: hash.o ripeMD.o byteBuffer.o

hash.o: hash.c ripeMD.o byteBuffer.o
ripeMD.o: ripeMD.c ripeMD.h byteBuffer.o
byteBuffer.o: byteBuffer.c byteBuffer.h

#testdriver
testdriver: ripeMD.c ripeMD.h byteBuffer.c byteBuffer.h testdriver.c
	gcc -Wall -std=c99 -g -DTESTABLE testdriver.c ripeMD.c byteBuffer.c -o testdriver

clean:
	rm -f *.o
	rm -f hash
	rm -f testdriver
	rm -f output*.txt
	rm -f stderr.txt
	rm -f stdout.txt
