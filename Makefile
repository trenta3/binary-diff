# Makefile per compilare il programma e pulire le directories

all: compile

compile:
	mkdir -p bin
	gcc src/bindiff.c -o bin/bindiff

clean:
	rm -rf bin

