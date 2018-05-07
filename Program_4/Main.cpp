#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "graph.h"

using namespace std;



void filterDups(string, string);
graph loadGraphCSV(string, int max);

void main()
{
	graph G1;
	//filterDups("cities.csv", "filtered_cities.csv");
	G1 = loadGraphCSV("filtered_cities.csv", 0);
	G1.lookupHelp();
	//int test = G1.cityLookup["Lebanon"];
	G1.createForest("Lebanon");
	//G1.connectForest();
	system("pause");
}

/**
* loadGraphCSV - loads a graph with the given csv
* Params:
*     string filename  - filename to open
* Returns
*     graph
*/
graph loadGraphCSV(string filename, int max = 0)
{
	int zip;
	double lat;
	double lon;
	string city;
	string state;
	string county;

	map<string, int> cityCheck;

	int i = 0;

	graph G;
	int vid;
	vertex *temp;

	ifstream file(filename);

	for (CSVIterator loop(file); loop != CSVIterator(); ++loop)
	{
		zip = stoi((*loop)[0]);
		if ((*loop)[1].length() > 0)
		{
			lat = stod((*loop)[1]);
		}
		else
		{
			lat = 0.0;
		}

		if ((*loop)[2].length() > 0)
		{
			lon = stod((*loop)[2]);
		}
		else
		{
			lon = 0.0;
		}

		city = (*loop)[3];
		state = (*loop)[4];
		county = (*loop)[5];

		if (cityCheck.find(city) == cityCheck.end())
		{
			// Add the city as a key to the map.
			cityCheck[city] = 0;
			if ((int)lat == 0 || (int)lon == 0)
			{
				cout << "oops: " << city << ", " << state << ", " << lat << ", " << lon << endl;
			}
			vid = G.addVertex(city, state, lat, lon);
			i++;
		}

		if (i > max && max != 0)
		{
			return G;
		}
	}

	return G;
}

/**
* filterDups - filters out duplicate cities and creates new output file
* Params:
*     string filename  - filename to open
*     string outfile   - filename to write to
* Returns
*     graph
*/
void filterDups(string filename, string outfile)
{
	int zip;
	double lat;
	double lon;
	string city;
	string state;
	string county;

	map<string, int> cityCheck;

	int i = 0;

	ifstream file(filename);
	ofstream out(outfile);

	for (CSVIterator loop(file); loop != CSVIterator(); ++loop)
	{
		zip = stoi((*loop)[0]);
		if ((*loop)[1].length() > 0)
		{
			lat = stod((*loop)[1]);
		}
		else
		{
			lat = 0.0;
		}

		if ((*loop)[2].length() > 0)
		{
			lon = stod((*loop)[2]);
		}
		else
		{
			lon = 0.0;
		}

		city = (*loop)[3];
		state = (*loop)[4];
		county = (*loop)[5];

		if (cityCheck.find(city) == cityCheck.end() && abs(lat) > 0 && abs(lon) > 0)
		{
			// Add the city as a key to the map.
			cityCheck[city] = 0;
			out << zip << "," << lat << "," << lon << "," << city << "," << state << "," << county << "\n";
		}
	}
}