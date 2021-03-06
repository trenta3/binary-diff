#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#define Q 10

using namespace std;

typedef struct my_pair {
	int first;
	int second;	
	} my_pair;

int min(int a, int b) {
	return (a>b)?b:a;
	}

my_pair bounded_next(my_pair P, my_pair L) {
		my_pair R;
		if(P.second >0) {
			R.first =P.first+1;
			R.second=P.second-1;
		} else {
			R.first=0;
			R.second=P.first+1;
		}
		R.first=min(P.first,L.first);
		R.second=min(P.second, L.second);
		return R;
	}

int quality_condition(vector<char> A, vector<char> B, my_pair start, int quality) {
	int r=0; int flag=1;
	while(flag == 1 && start.first+r < A.size() && start.second+r < B.size() && r < quality) {
		if(A[start.first+r] == B[start.second+r]) {
			r++;
		} else {
			flag=0;
		}
	}
	if (r >= quality) {
		return 1; //coinciding
	} else if (start.first+r >= A.size() && start.second+r >= B.size() ) {
		return 2;
	} else {
		return 0; //not coinciding
	}
}

int common_len(vector<char > A, vector<char> B, my_pair start) {
	int r=0; int flag=1;
	while(flag == 1 && start.first+r < A.size() && start.second+r < B.size()) {
		if(A[start.first+r] == B[start.second+r]) {
			r++;
		} else {
			flag=0;
		}
	}
	return r;
}
int main(int argc, char* argv[]) {
	ifstream before(argv[1]);
	ifstream after(argv[2]);
	FILE *patch;
	patch=fopen(argv[3], "w");
	//READING FILES
	int i,counter,a,len;
	my_pair start; my_pair iterator;
	vector< char> A; vector<char > B; char c;
	while(before >> c) {
		A.push_back(c);
	};
	while(after >> c) {
		B.push_back(c);
	}
	my_pair limit; limit.first=A.size(); limit.second=B.size();
	counter =0; start.first=0; start.second=0;
	int check;
	while(start.second < B.size() ) {
		printf("Entering while: start.second=%d\n", start.second);
		iterator.first=start.first;
		iterator.second=start.second;
		check=quality_condition(A,B,iterator,Q);
		printf("Quality condition: checked. Result: %d\n", check);
		while(check==0) { // c'è un problema alla fine!!
			printf("while. iterator=(%d,%d), check=%d\n, limit=(%d,%d)", iterator.first, iterator.second, check, limit.first, limit.second); getchar();
			iterator=bounded_next(iterator, limit);
			check=quality_condition(A,B,iterator,Q);
		}
		if(check==1) {
			 //NEWDATA line
			if(iterator.second > start.second) {
				fprintf(patch, "*0 %d\n", iterator.second-start.second);
				for(i=start.second;i<iterator.second;i++) {
					fprintf(patch, "%c", B[i]);
				}
			}
			
			//RECOVER line
			len=common_len(A,B,iterator);
			fprintf(patch, "*1 %d %d\n", iterator.first, len);
			start.first=iterator.first+len+1; // mmm con il +1? Non lo so.
			start.second=iterator.second+len+1;
			
		} else  if(check==2) {
			
			fprintf(patch, "*0 %d\n", (int)(B.size())-start.second);
			for(i=start.second;i<B.size();i++) {
				fprintf(patch, "%c", B[i]);
			}
			start.second=B.size();
		} else {
			fprintf(stderr, "Something went wrong. Expected check=0,1, while check=%d\n.", check);
		}
		
	}
	

	return 1;
}
