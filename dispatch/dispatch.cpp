#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
	FILE *patch,*data, *result;
	patch= fopen(argv[1], "r");
	data = fopen(argv[2], "r");
	result=fopen(argv[3], "w");

	char * line = NULL;
    	size_t len = 0;
    	ssize_t read;
	while ( (read = getline(&line, &len, patch)) != -1) {
        	
    	}

	return 1;
}
