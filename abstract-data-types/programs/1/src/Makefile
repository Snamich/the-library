CFLAGS=-Wall -g

all: gameoflife

gameoflife: clean
	cc main.c -o GameOfLife

clean:
	rm -rf GameOfLife

check-syntax:
	$(CC) -W -Wall $(SRC) $(CFLAGS)
