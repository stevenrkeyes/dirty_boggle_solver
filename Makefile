CC=g++
CFLAGS=-Wall -std=c++11

all: solver

solver: solver.cpp
	$(CC) $(CFLAGS) solver.cpp -o solver

clean:
	rm -f solver
