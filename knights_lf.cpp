#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

/*

This program's purpose is to find Knight's Tours by employing a "least freedom" algorithm. It works by emulating a knight as it attempts this tour, prompting the knight to move to spaces from which it will have the least freedom first. This method causes the Knight to preferentially seek out the "difficult to reach" sections, which improves the speed at which a knight's tour can be found. In fact, knight's tours can frequently be found on the first try using this algorithm.

The general form of this program is to keep track of the spaces and moves in a series of arrays. The current location of the piece is kept in an integer variable called "here", which represents the location in the array of that space. The currently traced path is held in the "lasts" array, in which each space's entry is the integer location of the previous space visited.

The possible moves out of a space are stored in a 2-D aray called "possibles", in which there is an array of maximum size 8 for each space on the board. A separate array called ipossibles records the number of remaining moves out of a space, to facilitate the least freedom algorithm. The routes already taken are stored in the travelled and itravelled arrays in a similar fashion. The array "visisted" keeps track of whether or not a space has been visited so far.

Finally, a counter is maintained to keep track of progress. When the counter reaches the number of spaces on the board, a knight's tour has been found.

*/



#define MAXSIZE 25
#define MINSIZE 6


//////
//	Function declarations.
//////
void initialize();									// Creates the board and moves.
int findnext();										// Executes the least freedom algorithm by determining which space to move to next.
bool intravelled (int source, int target);			// Checks to see if the target has been travelled to from the source.
void visit (int loc);								// Visits a space, changing all the values required.
void removepossible (int source, int target);		// Removes source from target's list of possible moves.
void backtrack();									// Moves backward in the knight's tour, called when the algorithm hits a dead end.
void printResult();									// Prints the knight's tour in order of visitation.
void printAll();									// Prints the knight's tour by space number.

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

int findnext() { 					// Executes the least freedom algorithm by determining which space to move to next.
	int tempnext = -1;				// Tempnext/tempi represent the possible place to move to next.
	int tempi = 9;					// Initially, it points to a non existent place with an absurd degree of freedom. (Max on a chess board is 8)
	for (int i = 0; i < ipossibles[here]; i++) {	// then start scanning through the possibles of here
		cout << "\n\tTesting " << possibles[here][i];	// print out for kicks
		int testcase = possibles[here][i];			// And use the word testcase to simplify the code's appearance
		if (ipossibles[testcase] < tempi) {		// if the testcase has less freedom, then that looks good
			if (!(intravelled(here,testcase))) {	//
				if (!(visited[testcase])) {
					tempnext = testcase;
					tempi = ipossibles[testcase];
				}
			}
		};
	};
	return tempnext;
}

bool intravelled(int source, int target) {		// Checks to see if the target has been travelled to from the source.
	for (int i = 0; i < itravelled[source]; i++) {
		if (travelled[source][i] == target) {
			return true;
		};
	};
	return false;
}

void visit(int loc) {				// Visits a space, changing all the values required.
	in++;		// Increment the number of spaces visited counter.
	cout << "\nf " << in << " , " << loc; // Print out where you are for kicks.
	
	travelled[here][itravelled[here]] = loc;	// Add here to the last place's travelled to list.
	itravelled[here]++;
	visited[loc] = true;		// set the visited for the new place to true
	lasts[loc] = here;			// set the last place to this place's "lasts"
	here = loc;					// then move forward
	int j = 0;					// and remove the new place from all adjacent space's possibles lists.
	for (int i = 0; i < ipossibles[here]; i++) {
		removepossible(possibles[here][i],here);
	}
}

void removepossible (int source, int target) {			// Removes target from source's list of possible moves.
	bool foundit = false;
	for (int i = 0; i < (ipossibles[source]); i++) {	// scan through the list looking for target,
		if (foundit) {
			possibles[source][i-1] = possibles[source][i];	// once you've found it, start sliding values over.
		} else if (possibles[source][i] == target) {		// and keep looking for it if you haven't.
			foundit = true;
		};
	};
	if (foundit) {		// And then handle the last element in the list.
		ipossibles[source]--;
		possibles[source][ipossibles[source]] = -1;
	};
}

void backtrack() {									// Moves backward in the knight's tour, called when the algorithm hits a dead end.
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

void initialize () {											// Creates the board and moves.
	for (int i = 0; i < n; i++) {	// For each space on the board...
		
		lasts[i]=-1;				// Initialize its previous space.
		for (int k = 0; k < 8; k++) {
			travelled[i][k] = -1;	// Initialize its travelled to list.
		}
		itravelled[i] = 0;
		visited[i] = false;			// Make it not yet visited.
		ipossibles[i] = 0;			// Start with 0 possible moves then...
		
		int * j = &ipossibles[i];	// Using a pointer to the ipossibles counter,
		int x = i % w;				// Find the x and y coordinates of the space,
		int y = i / w;
		for (int k = 0; k < 8; k++) {
			possibles[i][k] = -1;	// Initialize the set of possibles, then...
		};
		if ((x<(w-1))&&(y>1)) {		// Start adding all the possible moves by evaluating where on the board this space is.
			possibles[i][*j] = (i - (2*w) + 1); // Check if a given space should have that move,
			(*j)++;								// And add it, incrementing the ipossibles counter, if it does.
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
		cout << "\nThe possibles from " << i << " are: "; // For kicks, print out the list of possibles for that space.
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