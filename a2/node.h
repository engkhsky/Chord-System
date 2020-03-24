/*
 * CSCI319 Assignment 2
 * --------------------------
 * Name: Eng Kia Hui
 * 
 */

#include <iostream>
#include <string> 
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>

using namespace std;



//----------------------------------------------------------
// Node.h
//----------------------------------------------------------

class Node;

// Structure for fingertable
struct FingerTable{
	unsigned index;
	unsigned value;
	unsigned successorID;
	Node *successor;
};

class Node 
{
	private:
		unsigned id;
		vector<FingerTable> fingerTable;
        multimap<unsigned int, string> dataList;
		Node *predecessor;
		Node *successor;
		int ftSize;
		unsigned int chordSize;
		// Store last edited id and status
		char status;
		unsigned int updatedId;

	public:
		// Constructor
		Node (unsigned int, int);
		// Destructor
		~Node();  

		// functions
		Node *NewPeer (unsigned int);
		Node *Resolve(unsigned int, bool);
		Node *closestPrecedingNode(unsigned int);
		Node *findPredecessor(unsigned int);
		void updateOthers(Node*);
		void updateFingerTable(Node*);

		void RemovePeer (unsigned int);
		void updateOthersRemove(Node*);
		void updateFingerTableRemove(Node*);

		void Store (string);
		void Delete (string);
		void Print (unsigned int);

		Node* findKey(unsigned int);
		bool withinRange (unsigned int, unsigned int, unsigned int);
		unsigned int Hash(string);	

		//Assessor Function
		unsigned int getId ();		
};

