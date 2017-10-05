CC=g++
CFLAGS=-c -Wall

all: solver

solver: 
	$(CC) $(CFLAGS) solver.cpp -o solver

clean:
	rm -f solver
