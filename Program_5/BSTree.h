#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

struct Bnode {
	string data;
	Bnode *left;
	Bnode *right;

	Bnode()
	{
		data = "";
		left = NULL;
		right = NULL;
	}

	Bnode(string w)
	{
		data = w;
		left = NULL;
		right = NULL;
	}
};

class BSTree
{
private:
	Bnode *root;

	int count(Bnode *);
	void insert(Bnode *&, Bnode *&);
	void print_node(Bnode *, string);
	int height(Bnode *);
	void printGivenLevel(Bnode *, int );
	void GraphVizGetIds(Bnode *, ofstream &);
	void GraphVizMakeConnections(Bnode *, ofstream &);
public:
	BSTree();
	~BSTree();
	int Search(string);
	int count();
	void insert(string );
	int height(string);
	string top();
	void printLevelOrder();
	void GraphVizOut(string);
};