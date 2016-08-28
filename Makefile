SHELL:=/bin/bash
# Makefile per compilare il programma e pulire le directories

.PHONY: all compile test clean

all: compile test

# Da aggiungere -std=c99 -pedantic -Werror

compile:
	mkdir -p bin
	gcc src/bindiff.c -O3 -o bin/bindiff -Wall -Wextra -Wsign-conversion
	gcc src/binpatch.c -O3 -o bin/binpatch -Wall -Wextra -Wsign-conversion

test:
	./test.sh

clean:
	rm -rf bin tmp

