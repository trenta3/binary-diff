#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

int parse_space(char * buffer, int start, int* end) {
	int num=0,i; char temp;
	*end = start; num=0; temp=buffer[*end];
	while( temp != ' ' && temp != EOF && temp != '\n' )  {
		temp=buffer[*end];
		*end=(*end)+1;
	}
	for(i=start;i<*end-1;i++) {	
		num=10*num+buffer[i]-'0';
	}
	return num;
}

int main(int argc, char* argv[]) {
	FILE *patch, *result;
	ifstream data(argv[2]);
	patch= fopen(argv[1], "r");
	result=fopen(argv[3], "w");
	
	int i,num,LEN,ADDR; char temp;
	int * strend; strend = (int*)malloc(sizeof(int));

	// READING DATA
	vector< char> A; char c;
	while (data >> c) {
		A.push_back(c);
	}

	int counter=1; 
	char * line = NULL;
    	size_t len = 128;
	line = (char*)malloc(sizeof(int)*129);
    	ssize_t read;

	while ( (read = getline(&line, &len, patch)) != -1) {
		if( line[0] == '*') {
			if( line[1] == '0') {
				num=parse_space(line, 3, strend);
				for(i=0;i<num;i++) {
					fscanf(patch, "%c", &temp);
					fprintf(result, "%c", temp); 
				}
				

			} else if ( line[1] == '1') {
				//RECOVER line
				ADDR=parse_space(line, 3,strend);
				num=(*strend);
				LEN=parse_space(line,num,strend);
				for(i=ADDR;i<ADDR+LEN;i++) {
					if(i>=A.size() ) {
						fprintf(stderr, "Patch format is not valid. Please read dispatch-help.txt.\n");
						fprintf(stderr, "Problem reported at istruction line %d:\n", counter);
						fprintf(stderr, "Error in recover line: addressed data does not exist.\n");
						getchar();
					} else {
						fprintf(result, "%c", A[i]);
					}
				}	
			
			} else {
				fprintf(stderr, "Patch format is not valid. Please read dispatch-help.txt.\n");
				fprintf(stderr, "Problem reported at istruction line %d:\n", counter);
				fprintf(stderr, "Error: line is not starting with 1 nor 0.\n");
			}
			counter++;	
		}	
    	}
	fclose(patch);
	fclose(result);
	data.close();
	return 1;
}
