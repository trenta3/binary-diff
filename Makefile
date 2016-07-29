SHELL:=/bin/bash
# Makefile per compilare il programma e pulire le directories

.PHONY: all compile test clean

all: compile test

compile:
	mkdir -p bin
	gcc src/bindiff.c -o bin/bindiff -Wall -Wextra -Wsign-conversion
	gcc src/binpatch.c -o bin/binpatch -Wall -Wextra -Wsign-conversion

test:
	./test.sh

clean:
	rm -rf bin tmp

