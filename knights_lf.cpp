#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define MAXSIZE 25
#define MINSIZE 6

void initialize();
int findnext();
bool intravelled (int source, int target);
void visit (int loc);
void removepossible (int source, int target);
void backtrack();
void printResult();
void printAll();

int here;
int n;
int w;
int h;
int in;
int lasts[MAXSIZE*MAXSIZE];
int possibles[MAXSIZE*MAXSIZE][8];
int ipossibles[MAXSIZE*MAXSIZE];
int travelled[MAXSIZE*MAXSIZE][8];
int itravelled[MAXSIZE*MAXSIZE];
bool visited[MAXSIZE*MAXSIZE];

int main () {
	
	while ((w>MAXSIZE)||(w<MINSIZE)) {
		cout << "Enter chessboard width: ";
		cin >> w;
	}
	while ((h>MAXSIZE)||(h<MINSIZE)) {
		cout << "Enter chessboard height: ";
		cin >> h;
	}
	
	n = w*h;
	
	initialize();

	in = 0;
	here = -1;
	while ((here<0)||(here>=n)) {
		cout << "\nChoose starting point: ";
		cin >> here;
	}
	visit(here);
	lasts[here] = -1;
	
	while (in < n) {
		int next = findnext();
		if (next >= 0) {
			visit(next);
		} else {
			backtrack();
		};
	}
	
	printResult();
	
	cout << "\n Done.";	
	
}

int findnext() {
	int tempnext = -1;
	int tempi = 9;
	for (int i = 0; i < ipossibles[here]; i++) {
		cout << "\n\tTesting " << possibles[here][i];
		int testcase = possibles[here][i];
		if (ipossibles[testcase] < tempi) {
			if (!(intravelled(here,testcase))) {
				if (!(visited[testcase])) {
					tempnext = testcase;
					tempi = ipossibles[testcase];
				}
			}
		};
	};
	return tempnext;
}

bool intravelled(int source, int target) {
	for (int i = 0; i < itravelled[source]; i++) {
		if (travelled[source][i] == target) {
			return true;
		};
	};
	return false;
}

void visit(int loc) {
	in++;
	cout << "\nf " << in << " , " << loc;
	
	travelled[here][itravelled[here]] = loc;
	itravelled[here]++;
	visited[loc] = true;
	lasts[loc] = here;
	here = loc;
	int j = 0;
	for (int i = 0; i < ipossibles[here]; i++) {
		removepossible(possibles[here][i],here);
	}
}

void removepossible (int source, int target) {
	bool foundit = false;
	for (int i = 0; i < (ipossibles[source]); i++) {
		if (foundit) {
			possibles[source][i-1] = possibles[source][i];
		};
		if (possibles[source][i] == target) {
			foundit = true;
		};
	};
	if (foundit) {
		ipossibles[source]--;
		possibles[source][ipossibles[source]] = -1;
	};
}

void backtrack() {
	in--;
	cout << "\nb " << in << " , " << lasts[here];
	
	for (int i = 0; i < itravelled[here]; i++) {				// Resets list of locations travelled to from here.
		travelled[here][i] = -1;								// to -1.
	};
	itravelled[here] = 0;
	travelled[lasts[here]][itravelled[lasts[here]]] = here;		// Adds here to the end of the list of places travelled to from the previous place.
	itravelled[lasts[here]]++;									// Increments the endpoint of that list.

	for (int i = 0; i < ipossibles[here]; i++) {				// Adds this point back to the list of possibles of all adjacent points.
		int there = possibles[here][i];							// using a temp. integer called there to make syntax more readable.
		possibles[there][ipossibles[there]] = here;				// Adds here to the end of the possibles of there.
		ipossibles[there]++;									// increments possibles list endpoint.
	};
	
	visited[here] = false;
	
	int prev = lasts[here];
	lasts[here] = -1;											// Removes backwards link to previous point.
	here = prev;
}

void initialize () {
	for (int i = 0; i < n; i++) {
		
		lasts[i]=-1;
		for (int k = 0; k < 8; k++) {
			travelled[i][k] = -1;
		}
		itravelled[i] = 0;
		visited[i] = false;
		ipossibles[i] = 0;
		
		int * j = &ipossibles[i];
		int x = i % w;
		int y = i / w;
		for (int k = 0; k < 8; k++) {
			possibles[i][k] = -1;
		};
		if ((x<(w-1))&&(y>1)) {
			possibles[i][*j] = (i - (2*w) + 1);
			(*j)++;
		};
		if ((x<(w-2))&&(y>0)) {
			possibles[i][*j] = (i - w + 2);
			(*j)++;
		};
		if ((x<(w-2))&&(y<(h-1))) {
			possibles[i][*j] = (i + w + 2);
			(*j)++;
		};
		if ((x<(w-1))&&(y<(h-2))) {
			possibles[i][*j] = (i + (2*w) + 1);
			(*j)++;
		};
		if ((x>0)&&(y<(h-2))) {
			possibles[i][*j] = (i + (2*w) - 1);
			(*j)++;
		};
		if ((x>1)&&(y<(h-1))) {
			possibles[i][*j] = (i + w - 2);
			(*j)++;
		};
		if ((x>1)&&(y>0)) {
			possibles[i][*j] = (i - w - 2);
			(*j)++;
		};
		if ((x>0)&&(y>1)) {
			possibles[i][*j] = (i - (2*w) - 1);
			(*j)++;
		};
		cout << "\nThe possibles from " << i << " are: ";
		for (int k = 0; k < ipossibles[i]; k++) {
			cout << possibles[i][k] << " ";
		};
	}
}

void printResult() {
	while (lasts[here] >= 0) {
		cout << "\n" << here;
		here = lasts[here];
	};
	cout << "\nAnd finally, " << here;
}

void printAll() {
	for (int i = 0; i < n; i++) {
		cout << "\n The element behind " << i << " is " << lasts[i];
	}
}