/*
 * CSCI319 Assignment 2
 * --------------------------
 * Name: Eng Kia Hui
 * 
 */

// g++ node.cpp chord.cpp -o chord
// ./chord test1.dat

#include "node.h"
#include <iostream>
#include <cstdlib>
#include <fstream>

using namespace std;

//---------------------------------------------------------------------------
// CHORD CLASS
//---------------------------------------------------------------------------
class Chord
{
	private:
		
		int ftSize;
		unsigned int chordSize;
		map<unsigned int, Node*> keyList; // store a list of peers

	public:
		// Constructor
		Chord();
		Chord(unsigned int, int);
		// Destructor
		~Chord();
		void Init(int size);
		void Read(char *filename);
		Node *findLowestIDPeer();
		void deleteAll();
		
};

// 1st Constructor
Chord::Chord()
{

}

// 2nd Constructor
Chord::Chord(unsigned int id, int ftSize)
{
	this->ftSize = ftSize;
	this->chordSize = (int)pow(2.0, ftSize);
}

// Destructor
Chord::~Chord()
{
	map<unsigned int, Node*>::iterator it = this->keyList.begin();
	for (it = this->keyList.begin(); it != keyList.end();)
	{
		delete (it->second); // free Memory Space
		keyList.erase(it++);
	}
	this->ftSize = 0;
	this->chordSize = 0;
}

// Init chord
void Chord::Init(int ftSize)
{
	cout << "\nEng Kia Hui\n5986187" << endl;
	cout << "======================" << endl;
	// Set the finger table size and chord size
	this->ftSize = ftSize;
	this->chordSize = (int)pow(2.0, ftSize);
	Node *indexNode = new Node(0, ftSize);

	this->keyList.insert(pair<unsigned int, Node*>(0, indexNode));
}

// Read instruction from text file
void Chord::Read(char *filename)
{
	fstream afile;
	afile.open(filename, ios::in);

	if (!afile)
	{
		cout << filename << " opened for reading failed." << endl;
		afile.close();
		exit(-1);
	}
	cout << "Begin reading of " << filename << endl;

	string line, instruction, content;
	int spacePos;
	
	Node *aNode;
	unsigned int key;

	// Get line by line
	while (getline(afile, line)) 
	{
		spacePos = line.find(" ");
		instruction = line.substr(0, spacePos); // get command
		content = line.substr(spacePos+1); // get the key/string data

		if (instruction == "init")
		{
			char *endPtr; // end pointer for strtol
			int ftsize = strtol(content.c_str(), &endPtr, 0);
			// if there is chord exist when init is called again
			// delete existing chord and peers
			if (!keyList.empty()) 
			{
				this->deleteAll();
			}
			this->Init(ftsize);
		}
		else if (instruction == "newpeer")
		{
			char *endPtr;
			key = strtol(content.c_str(), &endPtr, 0);
			// Always get the lowest id peer
			aNode = findLowestIDPeer();
			Node *newINode = aNode->NewPeer(key); // get new peer
			this->keyList.insert(pair<unsigned int, Node*>(key, newINode)); // insert into keylist
		}
		else if (instruction == "removepeer")
		{
			char *endPtr;
			key = strtol(content.c_str(), &endPtr, 0);
			aNode = findLowestIDPeer();
			aNode->RemovePeer(key);
			// Remove the deleted peer from keylist
			map<unsigned int, Node*>::iterator it = this->keyList.find(key);
			if (it != this->keyList.end())
			{
				delete (it->second); // free Memory Space
				this->keyList.erase(it++);
			}
		}
		else if (instruction == "store")
		{
			aNode = findLowestIDPeer();
			aNode->Store(content);
		}
		else if (instruction == "delete")
		{
			aNode = findLowestIDPeer();
			aNode->Delete(content);
		}
		else if (instruction == "print")
		{
			char *endPtr;
			key = strtol(content.c_str(), &endPtr, 0);
			aNode = findLowestIDPeer(); 
			aNode->Print(key);
		}
	}

	afile.close();
}

// Get lowest id peer
Node* Chord::findLowestIDPeer()
{
	map<unsigned int, Node*>::iterator it = this->keyList.begin();

	return it->second;
}

// Delete all the peers in keylist
void Chord::deleteAll()
{
	map<unsigned int, Node*>::iterator it = this->keyList.begin();
	for (it = this->keyList.begin(); it != keyList.end();)
	{
		delete (it->second); // free Memory Space
		keyList.erase(it++);
	}
	this->ftSize = 0;
	this->chordSize = 0;
}


//---------------------------------------------------------------------------
// MAIN FUNCTION
//---------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	//Chord *chord = new Chord(0, 5);
	Chord *chord = new Chord();
	if (argc < 2)
	{
		cout << "No file input." << endl;
	}
	else if(argc > 1)
	{
		char *filename = argv[1];
		chord->Read(filename);
	}
	
	// Press enter to end program
	cout << "\nPress any key to end program..." << endl;
	cin.get();
}



