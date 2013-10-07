#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
using namespace std;

#define MAXSIZE 25
#define MINSIZE 6

/*
This program's purpose is to implement an object oriented neural network based on Takefuji and Lee's paper in Neurocomputing. It solves the nxn Knight's Tour problem, representing each space on the board as a "node." The nodes are connected by "neurons" that represent each legal knight's move on the board. The neurons themselves have two imporant attributes: output and state. The output is either 0 or 1, and signifies wether or not the neuron is part of the final solution, meaning that move is part of the knight's tour. The state is a number which varies each time the network updates unless the neuron only has two active neighbors.

The code works as follows:
Initialization:
	Each node in the grid is created.
	Neurons are created connecting each node, being careful not to create duplicate neurons.
	Neurons' lists of neighboring neurons is populated.
	
Updates:
	Each neuron in the network is updated sequentially, in row-major order.
	If any neuron's state changes, it runs the loop again.
	
Termination:
	If the number of network epochs (an epoch represents each network updating once) exceeds 1000, the loop terminates early, assuming it will not converge further.
	The system then checks if the solution obtained is an actual solution. If not, the network is reset, and the code run again.
	
Results:
	Once a solution is obtained, the results are printed by node. Each node entry lists the adjacent nodes that share and active neuron with it.
*/



class Node;
class Neuron;
static int n;
static bool isdone;
static Node* nodes[MAXSIZE][MAXSIZE]; // This is the list of nodes. It is stored as a 2D array, with a maximum size of 25.
void initialize();
void printSolution();
bool checkSolution(Node* node);
void restartNodes();


// Class Declarations
/*
Node Class:
	The node class represents a space on the chess board. It knows what neurons it is attached to, and contains methods for handling those neurons. It also contains methods for printing a variety of its attributes.
	Note that the list of adjacent neurons (adj) is a list of pointers to those neurons, not of the neurons themselves. The newneuron pointer is used when adding a new neuron to its adjacency list.
*/
class Node {
	Neuron* newneuron;
	int x;
	int y;
	
	public:
		bool visited;
		Neuron* adj[8];
		int iadj;
		Node(int inx,int iny);
		bool isconnected(Node* target);
		void addadj(Neuron* neur);
		void makeneuron(Node* target);
		void createneurons();
		void resetneurons();
		void updateneurons();
		int getPosition();
		void printAdjacent();
		void printActive();
};

/*
Neuron Class:
	The neuron class represents legal knights moves on the chess board. It knows where its two ends are, and tracks its output and state from turn to turn. It also keeps a list of its neighbor neurons.
	It has methods for initializing and updating itself, along with keeping track of wether or not it has been updated.
	Note that the list of neighbors(nabrs), and the information about the two endpoints of the neuron are stored as pointers.
*/
class Neuron {
	int state;
	int prevstate;
	int output;
	int prevout;
	bool updated;
	bool populated;
	Neuron* nabrs[14];
	int inabr;
	
	public:
		Neuron (Node* one, Node* two);
		void update ();
		void populatenabrs ();
		void addnbr (Neuron* neur);
		void reset();
		void restart();
		int getprev ();
		Node* start;
		Node* end;
		void printAdjacent();
		bool isActive();
};


///////////////////////
//
//		Function definitions for Node Class
//
//////////////////////
// Initializes the node with its position, as given when the node is created. iadj represents the number of adjacent neurons, and is used to navigate the node's adjacency list.
Node::Node (int inx,int iny) {
	x = inx;
	y = iny;
	iadj = 0;
	visited = false;
};
// Checks to see if this node is connected to target node by looking at the adjacent neurons. Used only during neuron creation, to make sure no duplicates are made.
bool Node::isconnected (Node* target) {
	for (int i = 0; i < iadj; i++) {
		if (((adj[i])->start == target)||(adj[i]->end == target)) {
			return true;
		};
	};
	return false;
};
// Takes a pointer to a Neuron and adds it to the list of adjacent neurons for the node. iadj keeps track of the current number of adjacent nuerons. Code is called by the neuron, when it is made.
void Node::addadj (Neuron* neur) {
	adj[iadj] = neur;
	iadj++;
};
// Creates a new neuron connecting self to target. Note that it must take a pointer to the target, as that is how Neurons store that information. Uses isconnected to prevent the creation of duplicates.
void Node::makeneuron(Node* target) {
	if (!(isconnected(target))) {
		newneuron = new Neuron(this,target);
	};
};
// While makeneuron makes one neuron from here to there, this method is what generates all of the neurons leading out of a node. It is called once for each node during initialization.
// It runs by checking the location of the node against the edges of the board to see where legal moves are, and therefore where to create new neurons.
void Node::createneurons () {
	if ((x < n-1)&&(y > 1)) {
		makeneuron(nodes[x+1][y-2]);
	};
	if ((x < n-2)&&(y > 0)) {
		makeneuron(nodes[x+2][y-1]);
	};
	if ((x < n-2)&&(y < n-1)) {
		makeneuron(nodes[x+2][y+1]);
	};
	if ((x < n-1)&&(y < n-2)) {
		makeneuron(nodes[x+1][y+2]);
	};
	if ((x > 0)&&(y < n-2)) {
		makeneuron(nodes[x-1][y+2]);
	};
	if ((x > 1)&&(y < n-1)) {
		makeneuron(nodes[x-2][y+1]);
	};
	if ((x > 1)&&(y > 0)) {
		makeneuron(nodes[x-2][y-1]);
	};
	if ((x > 0)&&(y > 1)) {
		makeneuron(nodes[x-1][y-2]);
	};
};
// Used during the update cycle, called once per node. It stores the old information regarding the state/output of the neurons, but that is handled by the neuron method reset().
void Node::resetneurons() {
	for (int i = 0; i < iadj; i++) {
		adj[i]->reset();
	};
};
// Used during the update cycle, called once per node. It runs the updating algorithm on the neurons, which is handled by the neuron method update().
void Node::updateneurons() {
	for (int i = 0; i < iadj; i++) {
		adj[i]->update();
	};
};
// Gets the current position of the node and returns it as a string. Used for printing results and bug tracking.
int Node::getPosition() {
	return (10*x+y);
}
// Prints out the list of adjacent neurons to this node. States which node it is being called for (by position), and then lists the neurons, by their address, each on its own line. Used in bugtracking.
void Node::printAdjacent() {
	cout << "\nThis node at x: " << x << " y: " << y;
	for (int i = 0; i < iadj; i++) {
		cout << "\n" << adj[i];
	}
};
// Method primarily responsible for printing results. It prints the position of the node, followed by the list of nodes that share an active neuron with it. All moves out of the neuron are listed on one line to save space.
// It works by looking at all the adjacent neurons, and checking if they are active. If they are, it returns the endpoint on that neuron that is NOT the node that is calling this method.
void Node::printActive() {
	cout << "\n" << getPosition() << ": ";
	for (int i = 0; i < iadj; i++) {
		if (adj[i]->isActive()) {
			if (adj[i]->start == this) {
				cout << " " << adj[i]->end->getPosition();
			}
			else {
				cout << " " << adj[i]->start->getPosition();
			}
		}
	}
}


///////////////////////
//
//		Function definitions for Neuron Class
//
/////////////////////
// Initializes the neuron using the two endpoints. It makes sure each node has it in their list of adjacent neurons, and then sets the initial state and output of the neuron.
// inabr will keep track of the number of neighbors of the neuron, and nabrs, defined in the class declaration, is the list of these nabrs.
Neuron::Neuron (Node* one, Node* two) {
	one->addadj(this);
	start = one;
	two->addadj(this);
	end = two;
	output = rand()%2;
	prevout = output;
	state = 0;
	prevstate = state;
	populated = false;
	updated = false;
	inabr = 0;
};
// This handles the updating algorithm for this neural network. Information on the algorithm can be found in the header.
// During the update cycle, this is called twice for each neuron, because the updates run by node via updateneurons(). The updated bool value tracks if it needs to be updated this time.
// If the state has changed, the global isdone variable is changed to false, indicating the code should run the update loop again.
// For bugtracking purposes, it currently outputs text each time its output (not its state) changes.
void Neuron::update () {
	if (updated == false) {
		state += 2;
		for (int i = 0; i < inabr; i++) {
			state -= nabrs[i]->output; // subtract the ouput of each neighboring neuron.
		};
		if (state > 3) {
			output = 1;
		}
		else if (state < 0) {
			output = 0;
		};
		updated = true;
		if (state != prevstate) {
			isdone = false;
		};
		if (output != prevout) {
			cout << "\nOutput of neuron " << this << " changed.";
		}
	};
};
// This is called twice per neuron during initialization, because it is called for each node. It populates the list of neighbors to this neuron by looking at the neruons adjacent to the nodes at its endpoints.
// Note that it checks to make sure it does not add itself to its list of neighbors. It relies on the addnbr method to actually add them, though.
void Neuron::populatenabrs () {
	if (!populated) {
		for (int i = 0; i < start->iadj; i++) {
			if (start->adj[i] != this) {
				addnbr(start->adj[i]);
			}
		}
		for (int i = 0; i < end->iadj; i++) {
			if (end->adj[i] != this) {
				addnbr(end->adj[i]);
			}
		}
	populated = true;
	}
}
// This is called by the populatenabrs() method, and actually populates nabrs. It needs to be passed a Neuron*, as it saves the list of neighbors as a list of pointers.
void Neuron::addnbr (Neuron* neur) {
	nabrs[inabr] = neur;
	inabr++;
}
// This is called during the update cycle to prepare all of the neurons for updating. It is called twice per neuron, because the resets are called from the nodes via resetneurons()
// It doesn't matter that it gets called twice, though, since all it is doing is saving the previous epoch's information.
void Neuron::reset () {
	updated = false;
	prevout = output;
	prevstate = state;
};
// Method that returns the previous output of the neuron, and was at one point used by the updating algorithm. Right now, not in use.
int Neuron::getprev () {
	return prevout;
};
// Method that checks to see if the Neuron is active. Is used by the Node method printActive() in order to print the results of the network.
bool Neuron::isActive() {
	return (output == 1 ? true : false);
}
// Prints a list of the nabrs of the neuron that calls it. It identifies itself by the nodes it conects, and then lists the neurons in its nabrs list by their addresses, each on their own line. Used in bugtracking.
void Neuron::printAdjacent() {
	cout << "\nThis neuron connects " << start->getPosition() << " with " << end->getPosition();
	for (int i = 0; i < inabr ; i++) {
		cout << "\n" << nabrs[i]->start->getPosition() << " to " << nabrs[i]->end->getPosition() << " active: " << nabrs[i]->isActive();
	}
}
void Neuron::restart() {
	output = rand()%2;
	prevout = output;
	state = 0;
	prevstate = state;
	updated = false;
}
	
	
	
///////////////////////
//
//		Main
//
//////////////////////
/*
The operation of the code starts here.
*/
int main() {

	// Gets a randomization seed from the time.
	srand (time(NULL));
	// Prompts user for the chess board size. Will not allow values outside the min/max.
	while ((n<MINSIZE)||(n>(MAXSIZE+1))) {
		cout << "Enter chess board length size:  ";
		cin >> n;
	}
	
	// Initializes the board by running initialize(). It creates the nodes, the neurons, and populates the lists of neuron nabrs.
	initialize();
	int epochs;
	// Finishes the general initialization.
	cout << "\nInitialization complete.";
	isdone = false;
	Node* pnode;
	
	/*
		This while loop executes the network updates. It terminates any time a loop completes without a neuron's state changing, or if the executes 1000 times.
		Each time it executes, it calls the resetneurons() method for each node, which in turn resets each neuron that node is connected to via the neuron method reset().
		After resetting all of the neurons, it updates them all in turn, by calling updatenerons() for each node, which in turn calls update() for each adjacent neuron.
	*/
	while(!checkSolution(nodes[0][0])) {
		epochs = 0;
		restartNodes();
		while ((!isdone)&&(epochs < 10000)) {
			epochs ++;
			isdone = true;
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					pnode = nodes[j][i];
					pnode->resetneurons();
				};
			};
			for (int i = 0; i < n; i++) {
				for (int j = 0; j < n; j++) {
					pnode = nodes[j][i];
					pnode->updateneurons();
				};
			};
		};
		// If it expired because the epochs reached 1000, it prints that fact.
		if (epochs > 999) {
			cout << "\nEpochs expired.";
		}
		printSolution();
	};
	// Once the updates have finished, it prints the final layout of the system by calling printSolution(), which calls printActive() for each node.

	cout << "\n\ndone";
};


// This function runs the initialization of the chess board. It is called in main(). It first creates a general Node pointer, and uses that to create n^2 nodes, stored in the nodes[][] array.
// It then runs the createneurons() and populateneurons() methods for each node in this array.
void initialize () {
	cout << "\nInitialization started.";
	cout << "\nNode matrix created.";
	Node* newnode;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			newnode = new Node(i,j);
			nodes[i][j] = newnode;
		};
	};
	cout << "\nNodes complete.";
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			nodes[i][j]->createneurons();
		};
	};
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < nodes[i][j]->iadj; k++) {
				nodes[i][j]->adj[k]->populatenabrs();
			};
		};
	};
	cout << "\nNeurons Complete.";
};

/*
checkSolution looks through the current grid to see if it satisfies the requirements of a knight's tour. It does this by looking at a first element, which happens to be nodes[0][0], when called by main(). When looking at it, it checks all of the important litany of success or failure conditions. First, it checks to see if it has reached the final square on the board, and if it does, it returns true. It does this before it checks that it has visisted that square, so that hamiltonian tours are allowed. It then checks if it has visited that square, and then the number of active neurons leading out of the square. If it has less than 1, or more than 2, than it can't be a possible tour. If everything checks out, it recursively calls itself on the nodes it is connected to.

The short version is that it leap frogs through, and if it is able to reach the last node, it returns true all the way up the recursion.
*/
bool checkSolution(Node* node) {
	int nsq = n*n; // total number of squares on the board
	int icheck = 0; // number of squares checked
	icheck++; // add one, and check if it has reached the final square.
	if(icheck == nsq) {
		return true;
	};
	if (node->visited == true) { // check to see if the square at hand has been visited
		return false;
	} else {
		node->visited = true;
	};
	int ic = 0; // number of active connections out of the node
	Node* connected[2]; // list of active connections out of the node
	for (int i = 0; i < node->iadj; i++) // Checks each connection, adding it to the list if active
		if (node->adj[i]->isActive()) {
			if (ic < 2) { // if there are too many active connections, return false.
				Neuron* pneur = node->adj[i];
				if (pneur->start == node) {
					connected[ic] = pneur->end;
				} else {
					connected[ic] = pneur->start;
				}
				ic++; // if a connection is added, increment number of active connections
			} else {return false;};
		}
	if (ic == 0) { // if there are no active connections, return false
		return false;
	} else if (ic == 1) { // if there are active connections, perform recursive call
		return checkSolution(connected[0]);
	} else {
		return ((checkSolution(connected[0]))||(checkSolution(connected[1])));
	};
}

// Resets the entire collection of neurons, for use when the network either finds a stable solution or runs out of time, and the layout isn't a knight's tour. Calls restart() for each neuron, which resets its state and output. The nodes' visisted boolean is reset, so that the next checkSolution had them all already set to false.
void restartNodes() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			for (int k = 0; k < nodes[i][j]->iadj; k++) {
				nodes[i][j]->adj[k]->restart();
			}
			nodes[i][j]-> visited = false;
		}
	}
}

// Prints the current network lay out by calling printActive for each neuron.
void printSolution() {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			nodes[i][j]->printActive();
		}
	}
}