CFLAGS=-Wall -g -ll

clean:
	rm -rf nas
	rm -rf *.o
	rm -rf lex.yy.c
	rm -rf *.dSYM

all: nas.o lex.yy.o
	gcc -o nas nas.o lex.yy.o $(CFLAGS)

nas.o:
	gcc -c nas.c

lex.yy.o:
	flex lex.c
	gcc -c lex.yy.c
