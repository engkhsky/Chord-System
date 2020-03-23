/*
 * CSCI319 Assignment 2
 * --------------------------
 * Name: Eng Kia Hui
 * Student Number: 5986187
 */

#include "node.h"


Node::Node(unsigned int id, int ftSize) {
	this->id = id;
	this->ftSize = ftSize;
	this->chordSize = (unsigned int) pow(2.0, ftSize);
	// Since this is the 1st peer therefore predecessor and successor is itself
	this->predecessor = this; 
	this->successor = this;
	this->dataList = multimap<unsigned, string>();

	// Create fingertable
	for (int i = 0; i < this->ftSize; i++) 
	{
		FingerTable ft;
		ft.index = i+1;
		// succ(p+2^(index-1))
		ft.value = (id + (unsigned int) pow(2.0, i)) % this->chordSize;
		// Assign to itself
		ft.successorID = this->id;
		ft.successor = this;  
		this->fingerTable.push_back(ft);
	}
	// Set update status and id 
	status = 'A'; 
	updatedId = id;
}

Node::~Node() 
{
	
}


//---------------------------------------------------------------------------
// ADD PEER
//---------------------------------------------------------------------------
Node* Node::NewPeer(unsigned int newId)
{
	Node *newINode = new Node(newId, this->ftSize);

	cout << "Peer " << newId << " FingerTable" << endl;

	// Find successor for newINode
	newINode->successor = this->Resolve(newId, false);
	cout << this->id << " > " << newINode->successor->id << endl;

	// 1st ft successor will always be the the newINode's successsor
	newINode->fingerTable[0].successorID = newINode->successor->id;
	newINode->fingerTable[0].successor = newINode->successor;	
	
	// Assign predecessor
	newINode->predecessor = newINode->successor->predecessor;
	newINode->successor->predecessor = newINode;

	// newINode is the only peer in chord
	if (newINode->successor->id == this->id && this->successor->id == this->id)
	{
		this->successor = newINode;
		this->predecessor = newINode;
	}	
	
	// Edit the finger table for the new peer
	for (int i = 1; i < ftSize; i++)
	{
		Node *aSuccessor;
		cout << this->id << " > ";
		// Check new node ft value within new id and prev new node ft successor id
		if (withinRange(newINode->fingerTable[i].value, newId-1, newINode->fingerTable[i-1].successorID))
		{
			newINode->fingerTable[i].successorID = newINode->fingerTable[i - 1].successorID;
			newINode->fingerTable[i].successor = newINode->fingerTable[i - 1].successor;

		} // Check new node ft value within new node pred id and new node id
		else if (withinRange(newINode->fingerTable[i].value, newINode->predecessor->id, newINode->id))
		{
			newINode->fingerTable[i].successorID = newINode->id;
			newINode->fingerTable[i].successor = newINode;
			
		}
		else
		{
			aSuccessor = this->Resolve(newINode->fingerTable[i].value, true);
			
			newINode->fingerTable[i].successor = aSuccessor;
			newINode->fingerTable[i].successorID = newINode->fingerTable[i].successor->id;
		}
		
		cout << newINode->fingerTable[i].successorID;
		cout << endl;
	}

	cout << "PEER " << newINode->id << " ADDED" << endl;

	// Current peer inform all the other peers in its finger table about new peer
    // also update its own fingertable (inside updateOthers func).
	this->updateOthers(newINode);

	return newINode;
}

// Resolve key, printFlag = true is print and false is no print
Node* Node::Resolve (unsigned int key, bool printFlag)
{
	 if (this == this->successor) 
	 {
         return this;
     }

	// ask this current node to find the successor of id
	if (withinRange(key, this->id, this->successor->id))
	{
		if (printFlag)
		{
			cout << this->successor->id << " > ";
		}		
		return this->successor;
	}
	// forward the query around the circle
	Node *closestNode = closestPrecedingNode(key);
	
	if (printFlag)
	{
		cout << closestNode->id << " > ";
	}	

	closestNode = closestNode->Resolve(key, printFlag);
	return closestNode;
}

// Find closest preceding node 
Node* Node::closestPrecedingNode(unsigned int key)
{
	// Search the local table for the highest predecessor of id
	int m = this->ftSize - 1;

	for (int i = m; i >= 0; i--) 
	{	
		// Check ft succ id is within current id and key (not including key)
		if (withinRange (fingerTable[i].successorID, this->id, key-1)) 
		{
			return fingerTable[i].successor;
		}
	}

	return this;
}

// Find predecessor node
Node* Node::findPredecessor (unsigned int id)
{
	// Assign the current node ptr to aNode to find the id's predecessor
	Node *aNode = this;
	
	while (!(withinRange(id, aNode->id, aNode->successor->id)))
	{
		aNode = aNode->closestPrecedingNode(id);
	}
	return aNode;
}

// Update other node of the newINode
void Node::updateOthers(Node* newINode)
{
	unsigned int newId = newINode->id;
	
	if (this->status == 'A' && this->updatedId == newINode->id)
	{
		// Do nothing if is already updated
	}
	else
	{
		// Update status of the node so it will repeat updating fingertable
		this->status = 'A';
		this->updatedId = newId;

		// Transfer data to new node if data computed key is lower than new node id
		multimap<unsigned int, string>::iterator it, itLower, itUpper;
		itLower = this->dataList.lower_bound(0);
		itUpper = this->dataList.upper_bound(newId);
		
		for (it = itLower; it != itUpper; )
		{
			// Only transfer the data if data's key value is larger than the current node id
			// those <= to current node id will be remain in the current node
			// Make sure data computed key is witin current id and new id, then transfer data to newINode			
			if (withinRange(it->first, this->id, newId))
			{
				newINode->dataList.insert(pair<unsigned int, string>(it->first, it->second));
				// iterator it becomes invalid after erase, however, other iterators in the dataList will still be valid. 
				// Therefore do post-increment on the iterator it
				this->dataList.erase(it++);
			}	
			else
			{
				it++;
			}
		}		

		// Updating others
		for (int i = 0; i < this->ftSize; i++)
		{
			if (this->fingerTable[i].successorID != this->id)
			{
				this->fingerTable[i].successor->updateOthers(newINode);
			}
		}
		// Now update current node's fingertable
		this->updateFingerTable(newINode);
		this->successor = this->fingerTable[0].successor;
	}	
}

// Update current finger table successor
void Node::updateFingerTable(Node* newINode)
{
	unsigned int newId = newINode->id;

	cout << "Peer " << this->id << " FingerTable" << endl;

	for (int i = 0; i < this->ftSize; i++)
	{
		cout << this->fingerTable[i].successorID << " > ";

		unsigned int sucID = this->fingerTable[i].successorID;

		// If ft succ id is 0, change sucId to chord size
		if (this->fingerTable[i].successorID == 0)
			sucID = this->chordSize;

		// This is for if newid is 0 and the ft value is not 0
		// For the ft value that is not 0 to compare with new id (chordsize)
		if (newId == 0 && this->fingerTable[i].value != newId)
			newId = this->chordSize;
	
		// ft value is <= newId < sucID (old current succ id of ft value) and newNode's succ is not same as ft value	
		// ft value's succ id != ft value, as is alrdy in the right node
		if (this->fingerTable[i].value <= newId && newId < sucID && newINode->successor->id != this->fingerTable[i].value && this->fingerTable[i].successorID != this->fingerTable[i].value)
		{
			this->fingerTable[i].successorID = newINode->id;
			this->fingerTable[i].successor = newINode;
		}
		else if (sucID <= this->id && newId <= sucID)
		{
			// if newId <= sucID <= current node id 
			// meaning new node is smaller than current node, 1st condition will not work
			this->fingerTable[i].successorID = newINode->id;
			this->fingerTable[i].successor = newINode;
		}
		else if (sucID <= this->id && (withinRange(this->fingerTable[i].value, sucID, newId)))
		{
			// if sucId <= current node id, check if ft value is wihtin range of sucID and newId
			this->fingerTable[i].successorID = newINode->id;
			this->fingerTable[i].successor = newINode;
		}

		cout << this->fingerTable[i].successorID << endl;		
	}

	cout << "PEER " << this->id << " FINGERTABLE UPDATED" << endl;
}


//---------------------------------------------------------------------------
// REMOVE PEER
//---------------------------------------------------------------------------
void Node::RemovePeer(unsigned int key)
{
	Node *aNode = findKey(key);
	unsigned int removedID = aNode->id;

	// Only got 1 peer in CHORD
	if (aNode->successor == aNode)
	{
		aNode = NULL;
	}
	else
	{
		// Assign data
		multimap<unsigned int, string>::iterator it = aNode->dataList.begin();
		while (it != aNode->dataList.end()) 
		{
			aNode->successor->dataList.insert(pair<unsigned int, string>(it->first, it->second));
			it++;
		}
		// Clear all data in data List 
		aNode->dataList.clear();

		// Assign the node's pred's succ to be the node's succ
		aNode->predecessor->successor = aNode->successor;
		aNode->successor->predecessor = aNode->predecessor;
	}

	cout << "REMOVE PEER " << removedID << endl;
	this->updateOthersRemove(aNode);

}

// Update other of the removed peer
void Node::updateOthersRemove(Node* aNode)
{
	unsigned int removedID = aNode->id;
	if ((this->status == 'D' && this->updatedId == aNode->id))
	{
		// Do nothing if is already updated
	}
	else
	{
		// Update status of the node so it will repeat updating fingertable
		this->status = 'D';
		this->updatedId = removedID;

		// Updating others
		for (int i = 0; i < this->ftSize; i++)
		{
			if (this->fingerTable[i].successorID != this->id)
			{
				this->fingerTable[i].successor->updateOthersRemove(aNode);
			}
		}
		// Now update current node's fingertable
		this->updateFingerTableRemove(aNode);
		this->successor = this->fingerTable[0].successor;
	}
}

// Update finger table
void Node::updateFingerTableRemove(Node* aNode)
{
	unsigned int removedID = aNode->id;

	for (int i = 0; i < this->ftSize; i++)
	{
		// check if current id is removed peer id
		if (this->id != removedID)
		{
			cout << this->fingerTable[i].successorID << " > ";

			unsigned int sucID = this->fingerTable[i].successorID;

			if (sucID == removedID)
			{
				Node *newSucc = this->Resolve(this->fingerTable[i].value, true);
				this->fingerTable[i].successorID = newSucc->id;
				this->fingerTable[i].successor = newSucc;
			}

			cout << this->fingerTable[i].successorID;
			cout << endl;
		}
		else // if curr id is the removed id then do not print out but still update other id
		{
			unsigned int sucID = this->fingerTable[i].successorID;

			if (sucID == removedID)
			{
				Node *newSucc = this->Resolve(this->fingerTable[i].value, false);
				this->fingerTable[i].successorID = newSucc->id;
				this->fingerTable[i].successor = newSucc;
			}
		}

	}

	if (this->id != removedID)
	{
		cout << "Peer " << this->id << " fingerTable Updated" << endl;
	}
}

//---------------------------------------------------------------------------
// INSERT DATA
//---------------------------------------------------------------------------
void Node::Store(string str) {

	// Calculate hash value 
	unsigned int computedKey = this->Hash(str);
	
	// Find the node to store the computedKey 
	Node *dataNode = this->Resolve(computedKey, false);

	// Storing data into dataList
	dataNode->dataList.insert(pair<unsigned int, string>(computedKey, str));
	cout << "INSERTED " << str << " ( key = " << computedKey << " ) AT " << dataNode->id << endl;
}


//---------------------------------------------------------------------------
// REMOVE DATA
//---------------------------------------------------------------------------
void Node::Delete(string str)
{
	// Calculate hash value 
	unsigned int computedKey = this->Hash(str);

	Node *dataNode = this->Resolve(computedKey, false);
	multimap<unsigned int, string>::iterator it;
	pair <multimap<unsigned int, string>::iterator, multimap<unsigned int, string>::iterator> itResult;
	itResult = dataNode->dataList.equal_range(computedKey);

	// Total Elements in the range
	int count = distance(itResult.first, itResult.second);

	if (count == 0)
	{
		cout << "Data ( " << str << " ) don't exist." << endl;
	}
	else
	{
		for (it = itResult.first; it != itResult.second; )
		{
			if (it->second == str) // Found the str
			{
				// iterator it becomes invalid after erase, however, other iterators in the dataList will still be valid. 
				// Therefore do post-increment on the iterator it
				dataNode->dataList.erase(it++);
				cout << "REMOVED " << str << " ( key = " << computedKey << " ) AT " << dataNode->id << endl;
			}
			else // then move next it
			{
				it++;
			}	
		}
	}	
}


//---------------------------------------------------------------------------
// PRINT PEER INFO
//---------------------------------------------------------------------------
void Node::Print(unsigned int key)
{
	Node *aNode = findKey(key);
	cout << "DATA AT NODE " << aNode->id << ":" << endl;
	for (multimap<unsigned int, string>::iterator it = aNode->dataList.begin(); it != aNode->dataList.end(); ++it)
	{
		cout << it->second << endl;
	}
	cout << "\nFINGER TABLE OF NODE " << aNode->id << ":" << endl;
	for (int i = 0; i < aNode->ftSize; i++)
	{
		cout << aNode->fingerTable[i].successorID << " ";
	}
	cout << "\n" << endl;
}


//---------------------------------------------------------------------------
// MISC FUNCTIONS
//---------------------------------------------------------------------------
Node* Node::findKey(unsigned int key)
{
	Node *aNode;
	// Check if is within the range
	if (withinRange(key, this->id, this->successor->id)) {
		aNode = this->successor;
	}
	else
	{
		aNode = closestPrecedingNode(key);
		aNode = aNode->findKey(key);
	}

	return aNode;
}

// Within range function
bool Node::withinRange (unsigned int id, unsigned int curr, unsigned int end)
{
	int max = this->chordSize;
	int min = 0;
	int start = curr + 1;

	 if (start == end && id == start)
	 {
	 	return true;
	 }		
	
	 if (start > end && ((start <= id && id <= max) || (min <= id && id <= end)))
	 {
	 	return true;
	 }

	 if (start < end && start <= id && id <= end)
	 {
	 	return true;
	 }

	 return false;
}

// To calculate computed hash value
unsigned int Node::Hash(string data) 
{
	unsigned int key = 0;
	for (int i = 0; i < data.length(); i++) {
		key = ((key << 5) + key) ^ data[i];
	}
	return key % chordSize;
}


// Assesor function
unsigned int Node::getId ()
{
	return id;
}



