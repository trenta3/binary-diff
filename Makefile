# Makefile per compilare il programma e pulire le directories

all: compile

compile:
	mkdir -p bin
	gcc src/bindiff.c -o bin/bindiff -Wall -Wextra -Wsign-conversion

clean:
	rm -rf bin

