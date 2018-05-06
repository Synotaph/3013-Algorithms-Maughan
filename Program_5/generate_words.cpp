#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>

using namespace std;

typedef vector<string> wordlist;

ofstream outfile2;
ifstream infile2;

wordlist loadList(string, int);

void other()
{
	srand(time(NULL));
	wordlist Adjective = loadList("adjectives.txt", 15572);
	wordlist Adverb = loadList("adverbs.txt", 3238);
	wordlist Noun = loadList("nouns.txt", 53738);
	wordlist Verb = loadList("verbs.txt", 12019);
	int adv = 0;
	outfile2.open("tenthousandwords.txt");
	for (int i = 0; i < 10000; i++)
	{
		if (adv > 3238)
			adv = adv % 3238;
		if ((i % 3) == 0)
			outfile2 << Adjective[i] << " " << Noun[i] << " " << Verb[i] << endl;
		else if ((i % 5) == 0)
			outfile2 << Adjective[i] << " " << Noun[i] << " " << Adverb[adv] << " " << Verb[i] << " " << Noun[i * 2] << endl;
		else
			outfile2 << Adjective[i] << " " << Noun[i] << " " << Adverb[adv] << " " << Verb[i] << endl;
	}
	outfile2.close();
}

wordlist loadList(string fileName, int wordTotal)
{
	string input;
	wordlist wList;
	wList.assign(wordTotal, " ");
	int num;
	infile2.open(fileName);
	while (!infile2.eof())
	{
		infile2 >> input;
		num = rand() % wordTotal;
		if (wList[num] == " ")
			wList[num] = input;
		else 
			while (num > -1)
			{
				if (num == wordTotal)
					num = 0;
				else if (wList[num] == " ")
				{
					wList[num] = input;
					num = -2;
				}
				else
					num++;
			}
	}
	infile2.clear();
	infile2.close();
	return wList;
}
	