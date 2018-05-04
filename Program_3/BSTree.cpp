#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "BSTree.h"

//http://www.webgraphviz.com/

using namespace std;

int BSTree::Search(string word)
{
	Bnode *nodePtr = root;
	int count = 1;
	while (nodePtr)
	{
		if (nodePtr->data == word)
			return count;

		else if (word < nodePtr->data)
		{
			nodePtr = nodePtr->left;
			count++;
		}

		else
		{
			nodePtr = nodePtr->right;
			count++;
		}
	}
	return -1;
}

int BSTree::count(Bnode *root)
	{
		if (!root)
		{
			return 0;
		}

		else
		{
			return 1 + count(root->left) + count(root->right);
		}
	}

void BSTree::insert(Bnode *&root, Bnode *&temp)
	{
		if (!root)
		{
			root = temp;
		}

		else
		{
			if (temp->data < root->data)
			{
				insert(root->left, temp);
			}

			else
			{
				insert(root->right, temp);
			}
		}
	}

void BSTree::print_node(Bnode *n, string label = "")
	{
		if (label != "")
		{
			cout << "[" << label << "]";
		}
		cout << "[[" << n << "][" << n->data << "]]\n";
		if (n->left)
		{
			cout << "\t|-->[L][[" << n->left << "][" << n->left->data << "]]\n";
		}
		else
		{
			cout << "\t\\-->[L][null]\n";
		}
		if (n->right)
		{
			cout << "\t\\-->[R][[" << n->right << "][" << n->right->data << "]]\n";
		}
		else
		{
			cout << "\t\\-->[R][null]\n";
		}
	}

int BSTree::height(Bnode *root)
	{
		if (!root)
		{
			return 0;
		}
		else
		{
			int left = height(root->left);
			int right = height(root->right);
			if (left > right)
			{
				return left + 1;
			}
			else
			{
				return right + 1;
			}
		}
	}

	/* Print nodes at a given level */

void BSTree::printGivenLevel(Bnode *root, int level)
	{
		if (root == NULL)
			return;
		if (level == 1)
		{
			print_node(root);
		}
		else if (level > 1)
		{
			printGivenLevel(root->left, level - 1);
			printGivenLevel(root->right, level - 1);
		}
	}

//************************************************************************
// Method to help create GraphViz code so the expression tree can
// be visualized. This method prints out all the unique node id's
// by traversing the tree.
// Recivies a node pointer to root and performs a simple recursive
// tree traversal.
//************************************************************************

void BSTree::GraphVizGetIds(Bnode *nodePtr, ofstream &VizOut)
	{
		static int NullCount = 0;
		if (nodePtr)
		{
			GraphVizGetIds(nodePtr->left, VizOut);
			VizOut << "node" << nodePtr->data
				<< "[label=\"" << nodePtr->data << "\\n"
				//<<"Add:"<<nodePtr<<"\\n"
				//<<"Par:"<<nodePtr->parent<<"\\n"
				//<<"Rt:"<<nodePtr->right<<"\\n"
				//<<"Lt:"<<nodePtr->left<<"\\n"
				<< "\"]" << endl;
			if (!nodePtr->left)
			{
				NullCount++;
				VizOut << "nnode" << NullCount << "[label=\"X\",shape=point,width=.15]" << endl;
			}
			GraphVizGetIds(nodePtr->right, VizOut);
			if (!nodePtr->right)
			{
				NullCount++;
				VizOut << "nnode" << NullCount << "[label=\"X\",shape=point,width=.15]" << endl;
			}
		}
	}

//************************************************************************
// This method is partnered with the above method, but on this pass it
// writes out the actual data from each node.
// Don't worry about what this method and the above method do, just
// use the output as your told:)
//************************************************************************

void BSTree::GraphVizMakeConnections(Bnode *nodePtr, ofstream &VizOut)
	{
		static int NullCount = 0;
		if (nodePtr)
		{
			GraphVizMakeConnections(nodePtr->left, VizOut);
			if (nodePtr->left)
				VizOut << "node" << nodePtr->data << "->"
				<< "node" << nodePtr->left->data << endl;
			else
			{
				NullCount++;
				VizOut << "node" << nodePtr->data << "->"
					<< "nnode" << NullCount << endl;
			}
			if (nodePtr->right)
				VizOut << "node" << nodePtr->data << "->"
				<< "node" << nodePtr->right->data << endl;
			else
			{
				NullCount++;
				VizOut << "node" << nodePtr->data << "->"
					<< "nnode" << NullCount << endl;
			}
			GraphVizMakeConnections(nodePtr->right, VizOut);
		}
	}

BSTree::BSTree()
	{
		root = NULL;
	}

BSTree::~BSTree()
	{

	}

int BSTree::count()
	{
		return count(root);
	}

void BSTree::insert(string x)
	{
		Bnode *temp = new Bnode(x);
		insert(root, temp);
	}

int BSTree::height(string key = "")
	{
		if (key != "")
		{
			//find node
		}
		else
		{
			return height(root);
		}
		return 0;
	}

string BSTree::top()
	{
		if (root)
			return root->data;
		else
			return 0;
	}

	/* Function to line by line print level order traversal a tree*/

void BSTree::printLevelOrder()
	{
		cout << "Begin Level Order===================\n";
		int h = height(root);
		int i;
		for (i = 1; i <= h; i++)
		{
			printGivenLevel(root, i);
			cout << "\n";
		}
		cout << "End Level Order===================\n";
	}

//************************************************************************
// Recieves a filename to place the GraphViz data into.
// It then calls the above two graphviz methods to create a data file
// that can be used to visualize your expression tree.
//************************************************************************

void BSTree::GraphVizOut(string filename)
	{
		ofstream VizOut;
		VizOut.open(filename);
		VizOut << "Digraph G {\n";
		GraphVizGetIds(root, VizOut);
		GraphVizMakeConnections(root, VizOut);
		VizOut << "}\n";
		VizOut.close();
	}