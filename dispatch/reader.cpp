#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

int main(int argc, char* argv[]) {
	vector< char> A;
	ifstream data(argv[1]); char c;
	while( data >> c) {
		A.push_back(c);
	}
	int i;
	for(i=0;i<A.size();i++) {
		printf("A[%d]: %c\n", i,A[i]);
	}
	return 1;
}
