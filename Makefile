

all: test

test: mirr
	./mirr

mirr: mirr.cpp
	g++ -Wall -pedantic -lrt mirr.cpp -o mirr

