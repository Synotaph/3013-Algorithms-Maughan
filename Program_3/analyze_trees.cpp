#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "AVLTree.h"
#include "BSTree.h"

using namespace std;

void loadTrees(string, BSTree, AVLTree);

ofstream outfile;
ifstream infile;

void main()
{
	BSTree BST;
	AVLTree AVL;
	loadTrees("adjectives.txt", BST, AVL);
	loadTrees("adverbs.txt", BST, AVL);
	loadTrees("animals.txt", BST, AVL);
	loadTrees("nouns.txt", BST, AVL);
	loadTrees("verbs.txt", BST, AVL);
	int BSTComp = 0;
	int AVLComp = 0;
	infile.open("tenthousandwords.txt");
	string input;
	while (!infile.eof())
	{
		infile >> input;
		BSTComp += BST.Search(input);
		AVLComp += AVL.Search(input);
	}
	infile.close();
	outfile.open("analysis.out");
	outfile << "BST Comparisons = " << BSTComp << endl;
	outfile << "AVL Comparisons = " << AVLComp << endl;
}

void loadTrees(string filename, BSTree B, AVLTree A)
{
	infile.open(filename);
	string input;
	while (!infile.eof())
	{
		infile >> input;
		B.insert(input);
		A.insert(input);
	}
	infile.close();
}