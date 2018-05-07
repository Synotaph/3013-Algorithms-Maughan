#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "AVLTree.h"
#include "BSTree.h"

using namespace std;

void loadTrees(string, BSTree &, AVLTree &, int);

ofstream outfile;
ifstream infile;

void main()
{
	BSTree BST;
	AVLTree AVL;
	loadTrees("adjectives.txt", BST, AVL, 15572);
	cout << "Adjectives done" << endl;
	loadTrees("adverbs.txt", BST, AVL, 3238);
	cout << "Adverbs done" << endl;
	loadTrees("nouns.txt", BST, AVL, 25000);
	cout << "Nouns done" << endl;
	loadTrees("verbs.txt", BST, AVL, 12019);
	cout << "Verbs done" << endl;
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

void loadTrees(string filename, BSTree &B, AVLTree &A, int limit)
{
	infile.open(filename);
	string input;
	while (limit > 0)
	{
		infile >> input;
		B.insert(input);
		A.insert(input);
		limit--;
	}
	infile.close();
}