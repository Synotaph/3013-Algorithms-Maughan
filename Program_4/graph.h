#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include "edge_heap.h"
#include "geo.h"
#include "csv.h"
#include <climits>

using namespace std;

// Used as a lookup from city name to ID.
// And to filter duplicate cities.
//typedef map<string, int> strMapInt;

typedef map<int, int> intint;

/**
 * vertex - represents a vertex in a graph.
 */
struct vertex
{
    int ID;       // integer id
    string city;  // city name
    string state; // state abbr (TX)
    latlon loc;   // latlon point type
    point p;        // xy point type
    vector<edge> E; // vector of outedges
    bool visited;   // flag used for traversals
    vector<int> network;

    /**
     * Constructor: vertex
     * Params:
     *     int    id: integer unique identifier
     *     string  c: city name
     *     string  s: state abbr
     *     latlon ll: city location
     * Returns 
     *     void
     */
    vertex(int id, string c, string s, latlon ll = latlon())
    {
        ID = id;
        city = c;
        state = s;
        loc = ll;
        p.setXY((double)lon2x(ll.lon), (double)lat2y(ll.lat));
        visited = false;
    }

    /**
     * Method: update 
     *     Updates the vertex location
     * Params:
     *     latlon ll: city location
     * Returns 
     *     void
     */
    void update(latlon ll)
    {
        loc = ll; // set location
        p.setXY(lon2x(ll.lon), lat2y(ll.lat));
        // update xy based on new ll
    }

    /**
     * Method: Neighbors 
     *     Are you a neighbor with vertex of id passed in,
     * Params:
     *     int id: potential neighbor id
     * Returns 
     *     bool: true = neighbor false = nope
     */
    bool Neighbors(int id)
    {
        vector<edge>::iterator eit;
        if (E.size() == 0)
        {
            return false;
        }
        else
        {
            for (eit = E.begin(); eit != E.end(); eit++)
            {
                if ((*eit).toID == id)
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool buildNetwork(int id)
    {
        if (find(network.begin(), network.end(), id) != network.end())
        {
        }
        if (E.size() == 0)
        {
            return false;
        }
        else
        {
            for (int e = 0; e < E.size(); e++)
            {
                if (E[e].toID == id)
                {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * operator<< - overload cout for vertex
     * Params:
     *     const vertex v     - vertex to print
     * Returns 
     *     formatted output for a vertex
     */
    friend ostream &operator<<(ostream &output, const vertex &v)
    {
        output << "(ID:" << v.ID << ", " << v.city << ", " << v.state << ", LatLon: " << v.loc << " Point: " << v.p << ", Edges:" << v.E.size() << ")";
        return output;
    }
};

/**
 * graph - set of vertices and edges
 * 
 * Methods (private):
 *     vertex* createVertex(string city,string state)
 * Methods (public):
 *     graph()
 */
class graph
{
  public:
    int id;                      // id counter for new vertices
    int num_edges;               // edge count
    vector<vertex *> vertexList; // vector to hold vertices
	map<string, int> cityLookup;        // dictionary of unique cities
    llBox box;                   // bounding box of coords
    edgePriorityList *E;
    string *colors;
	map<string, vector <string> > mapAdjList;
	map<string, vector <int> > cityByState;

    /**
     * private: createVertex - returns a new vertex with unique id.
     * Params:
     *     string city
     *     string state
     */
    vertex *createVertex(string city, string state, latlon ll)
    {
        return new vertex(id++, city, state, ll);
    }

    /**
     * Constructor
     */
    graph()
    {
        id = 0;
        num_edges = 0;
		stateAdjList();
        colors = new string[9];
        colors[0] = "Black";
        colors[1] = "Blue";
        colors[2] = "Green";
        colors[3] = "Red";
        colors[4] = "Purple";
        colors[5] = "Orange";
        colors[6] = "Yellow";
        colors[7] = "Brown";
        colors[8] = "Pink";
    }

    /**
     * Constructor
     * Params:
     *     graph G: another graph to init with
     */
    graph(const graph &G)
    {
        id = G.id;
        num_edges = G.num_edges;
        vertexList = G.vertexList;
        cityLookup = G.cityLookup;
		mapAdjList = G.mapAdjList;
		cityByState = G.cityByState;
    }

    /**
     * addVertex - adds a vertex to the graph
     * Params:
     *     string   city     - name of city
     *     string   state    - two letter abbr of state
     *     double   lat      - latitude 
     *     double   lon      - longitude 
     * Returns 
     *     void
     */
    int addVertex(string city, string state, double lat = 0.0, double lon = 0.0)
    {

        //create a bounding box of values to help with scaling for drawing.
        box.addLatLon(latlon(lat, lon));

        vertex *temp = createVertex(city, state, latlon(lat, lon));
        vertexList.push_back(temp);

        //update the value that city points to.
        return temp->ID;
    }

    /**
     * Method: getVertex
     * Params:
     *     int id: id of wanted vertex
     * Returns:
     *     vertex* of wanted vertex (if found) NULL otherwise
     */
    vertex *getVertex(int id)
    {
        if (id >= 0 && id < vertexList.size())
        {
            return vertexList[id];
        }
        return NULL;
    }

    /**
     * Method: Connected
     * Params:
     *     void
     * Returns:
     *     False if ANY vertex as zero edges.
     */
    bool Connected()
    {
        vector<vertex *>::iterator i;

        for (i = vertexList.begin(); i != vertexList.end(); i++)
        {
            if ((*i)->E.size() == 0)
            {
                return false;
            }
        }
        return true;
    }

    void Reset()
    {
        vector<vertex *>::iterator i;

        for (i = vertexList.begin(); i != vertexList.end(); i++)
        {
            (*i)->visited = false;
        }
    }

    bool AllVisited()
    {
        vector<vertex *>::iterator i;

        for (i = vertexList.begin(); i != vertexList.end(); i++)
        {
            if((*i)->visited == false){
                return false;
            }
        }
        return true;
    }

    /**
     * addEdge - adds a relationship between two vertices to the graph
     * Params:
     *     int      fromID   - the ID of the vertex in which the edge is leaving
     *     int      toID     - ID of the receiving vertex
     *     double   weight   - weight of the edge if any 
     *     bool     directed - is the edge directed or not
     * Returns 
     *     void
     */
    void addEdge(int fromID, int toID, double weight = 0, bool directed = false, string color = "Black")
    {
        edge e1(fromID, toID, weight, color);
        vertexList[fromID]->E.push_back(e1);
        num_edges++;

        //cout<<"adding "<<fromID<<" to "<<toID<<endl;

        if (!directed)
        {
            edge e2(toID, fromID, weight, color);
            vertexList[toID]->E.push_back(e2);

            //cout<<"adding "<<toID<<" to "<<fromID<<endl;

            num_edges++;
        }
    }

    /**
     * addEdge - adds a relationship between two vertices to the graph
     * Params:
     *     string   fromCity   - the city of the vertex in which the edge is leaving
     *     string   toCity     - city of the receiving vertex
     *     double   weight     - weight of the edge if any 
     *     bool     directed   - is the edge directed or not
     * Returns:
     *     void
     */
    //void addEdge(string fromCity, string toCity, double weight = 0, bool directed = false, string color = "Black")

    /**
     * printGraph - prints the graph out for debugging purposes
     * Params:
     *     void
     */
    void printGraph()
    {

        vector<vertex *>::iterator vit;
        vector<edge>::iterator eit;

        for (vit = vertexList.begin(); vit != vertexList.end(); vit++)
        {

            double brng = bearing((*(*vit)).loc, box.center);

            (*(*vit)).update(geo_destination((*(*vit)).loc, 100, brng));
            cout << (*(*vit)) << endl;

            if ((*vit)->E.size() > 0)
            {
                for (eit = (*vit)->E.begin(); eit != (*vit)->E.end(); eit++)
                {
                    cout << "\t" << (*eit) << endl;
                }
            }
        }
    }
    /**
     * magickGraph - instance of drawGraph included from mymagick.h
     * Params:
     *    int             w: width of image
     *    int             h: height of image
     *    string imageName : name of saved image
     */
    /*
    //NEEDS Magick++
    void magickGraph(int w, int h, string imageName)
    {
        drawGraph dg(w, h, "white");
        vector<vertex *>::iterator vit;
        vector<edge>::iterator eit;
        vector<latlon>::iterator bit;

        vector<latlon> us_border = loadUSA("random_data_files/border.csv");
        point p1;
        point p2;

        
        point cxy;
        point xy1;
        point xy2;

        int buff = 500;

        // For counting how much more
        int vsize = vertexList.size();
        int i = 0;

        //percentages to print completion amounts
        double op = -10.0;
        double np = 0.0;

        // center point of bounding box
        int cx = ((box.c_p.x - box.minx) / (box.maxx - box.minx)) * w;
        int cy = h - (((box.c_p.y - box.miny) / (box.maxy - box.miny)) * h);

        for (vit = vertexList.begin(); vit != vertexList.end(); vit++)
        {

            xy1 = scaleIt((*(*vit)).p,w,h);
            xy1 = shiftIt(xy1,cxy,buff);

            cout << (*(*vit)).city << endl;

            dg.setFontSize(20);
            dg.setFillColor("White");
            dg.setStrokeColor("Blue");
            dg.drawRectangleNode(xy1.x, xy1.y, 150, 60, (*(*vit)).city);

            // dg.setStrokeColor("Black");
            // dg.drawRectangleNode(x1, y1, 150, 60, (*(*vit)).city);

            if ((*vit)->E.size() > 0)
            {
                cout << (*vit)->E.size() << endl;
                for (eit = (*vit)->E.begin(); eit != (*vit)->E.end(); eit++)
                {

                    xy2 = scaleIt((*vertexList[(*eit).toID]).p,w,h);
                    xy2 = shiftIt(xy2,cxy,buff);

                    // if (rx > 0 && ry > 0 && x2 > 0 && y2 > 0)
                    // {
                    //     cout << "stupid color: " << (*eit).color << endl;
                    //     if ((*eit).color == "")
                    //     {
                    (*eit).color = colors[rand() % 9];
                    //     }

                    dg.drawLine(xy1.x, xy1.y, xy2.x, xy2.y, (*eit).color);
                    //     cout << rx << "," << ry << "," << x2 << "," << y2 << endl;
                    // }
                }
            }

            i = i + 1;
            np = (double(i) / double(vsize)) * 100;
            if ((abs(np) - abs(op)) >= 10)
            {
                op = np;
                cout << np << " complete" << endl;
            }
        }

        dg.setStrokeColor("Blue");
        for (int b = 0; b < us_border.size() - 1; b++)
        {
            p1.setXY((double)lon2x(us_border[b].lon), (double)lat2y(us_border[b].lat));
            p2.setXY((double)lon2x(us_border[b + 1].lon), (double)lat2y(us_border[b + 1].lat));

            p1 = scaleIt(p1,w,h);
            p2 = scaleIt(p2,w,h);

            p1 = shiftIt(p1,cxy,buff);
            p2 = shiftIt(p2,cxy,buff);

            dg.drawLine(p1.x, p1.y, p2.x, p2.y, "Blue");
        }

        dg.writeImage(imageName);
    }
    */
    /**
     * expandGraph - calculates a center and moves points away from the center
     *               to "grow" the graph if necessary for visualization.
     * Params:
     *    int distance: distance in miles to expand the graph.
     */
    void expandGraph(int distance)
    {
        vector<vertex *>::iterator vit;

        latlon ll;
        latlon destination;
        latlon center = box.center;

        box.reset();
        for (vit = vertexList.begin(); vit != vertexList.end(); vit++)
        {
            ll = (*(*vit)).loc;

            // get a bearing from the center
            // function in geo.h
            double brng = bearing(center, ll);

            // Calculate destination from center of graph through "this" point
            // moving away a specified distance at a specified bearing.
            destination = geo_destination(ll, distance, brng);

            //update the vertex's location
            (*(*vit)).update(destination);

            //update the bounding box I keep track of
            //encompassing the points in the graph
            box.addLatLon(destination);
        }
    }

    string mylower(string s)
    {
        for (int i = 0; i < s.length(); i++)
        {
            if (s[i] >= 'A' && s[i] <= 'Z')
            {
                s[i] += 32;
            }
        }
        return s;
    }

    string searchGraph(string c)
    {

        vector<vertex *>::iterator i;
        vector<edge>::iterator eit;

        for (i = vertexList.begin(); i != vertexList.end(); i++)
        {
            if (mylower((*i)->city) == mylower(c))
            {
                cout << *(*i) << endl;
                return (*i)->city;
            }
        }
        return "";
    }

    void createSpanningTree(string   = "")
    {
        vector<vertex *>::iterator i;
        vector<vertex *>::iterator j;
        vector<edge>::iterator eit;
        edgeHeap E;
        edge *e;
        vertex *ftemp;
        vertex *ttemp;
        vertex *current;

        double distance = 0;
        double d = 0;
        double minDistance = FLT_MAX;
        int closestID;
        int count = 0;
        int ecount = 0;
        string minCity;
        int c;

        c = 0;

        while (!Connected())
        {
            cout << "Connecting: " << vertexList[c]->city << endl;
            // Inner loop through vertices finds closes neighbors
            for (j = vertexList.begin(); j != vertexList.end(); j++)
            {
                if ((*j)->visited == false)
                {
                    distance = distanceEarth(vertexList[c]->loc.lat, vertexList[c]->loc.lon, (*j)->loc.lat, (*j)->loc.lon);

                    if (distance > 0)
                    {
                        E.Insert(new edge(vertexList[c]->ID, (*j)->ID, distance, colors[rand() % 9]));
                    }
                }
            }

            e = E.Extract();

            addEdge(e->fromID, e->toID, e->weight, false, colors[count % 9]);
            vertexList[e->fromID]->visited = true;
            vertexList[e->toID]->visited = true;

            E.ClearHeap();
            c++;
            if (c == vertexList.size())
            {
                c = 0;
                cout << "looping around\n";
            }
        }
    }

    void OLDcreateForest()
    {
        E = new edgePriorityList[vertexList.size()];
        edge *e;

        double distance = 0;
        double minDistance = FLT_MAX;

        // for every vertex, find its closest neighbors and push on its list
        for (int v1 = 0; v1 < vertexList.size(); v1++)
        {
            cout << "Progress : " << v1 << " / " << vertexList.size() << endl;

            // Inner loop through vertices finds closes neighbors
            for (int v2 = 0; v2 < vertexList.size(); v2++)
            {
                if (vertexList[v2]->visited == false)
                {
                    distance = distanceEarth(vertexList[v1]->loc.lat, vertexList[v1]->loc.lon, vertexList[v2]->loc.lat, vertexList[v2]->loc.lon);

                    if (distance > 0)
                    {
                        // add edge to vertex: c's priority queue
                        // closest edge to "c" in front of list
                        E[v1].Insert(new edge(vertexList[v1]->ID, vertexList[v2]->ID, distance, "Pink"));
                    }
                }
            }
        }

        // Connect close neighbors and create a disconnected forest
        // using the priority queue's for each vertex
        for (int v = 0; v < vertexList.size(); v++)
        {
            e = E[v].Pop();

            e->color = "Green";

            cout << "edge weight: " << e->weight << endl;

            addEdge(e->fromID, e->toID, e->weight, false, "Green");
        }

        for (int v = 0; v < vertexList.size(); v++)
        {
            E[v].ClearList();
        }
        delete E;
    }

	void createForest(string startingCity)
	{
		int startingID = cityLookup[startingCity];
		createForest2(startingID);
		vector <int> next = { vertexList[startingID]->E[0].toID, vertexList[startingID]->E[1].toID, vertexList[startingID]->E[2].toID };
		vertexList[startingID]->visited = true;
		while (!AllVisited())
		{
			if (vertexList[next[0]]->visited != true)
			{
				next.push_back(createForest2(next[0]));
				next.pop_back();
			}
			for (int n1 = 0; n1 < 3; n1++)
				next.push_back(vertexList[next[0]]->E[n1].toID);
			next.erase(next.begin());
			if (next.size() == 0)
			{
				int newID = 0;
				while (vertexList[newID]->visited == true)
					newID++;
				next.push_back(newID);
			}
		}
	}

	int createForest2(int startingID)
	{
		double distance = 0;
		double distArray[3][2] = { 0 };

		for (int v1 = 0; v1 < cityByState[vertexList[startingID]->state].size(); v1++)
		{
			if (vertexList[startingID]->E.size() < 3 && vertexList[cityByState[vertexList[startingID]->state][v1]]->E.size() < 3)
			{
				distance = distanceEarth(vertexList[startingID]->loc.lat, vertexList[startingID]->loc.lon, vertexList[cityByState[vertexList[startingID]->state][v1]]->loc.lat, vertexList[cityByState[vertexList[startingID]->state][v1]]->loc.lon);
				if (distance < distArray[0][0] || distArray[0][0] == 0)
				{
					distArray[0][0] = distance;
					distArray[0][1] = vertexList[cityByState[vertexList[startingID]->state][v1]]->ID;
				}
				else if (distance < distArray[1][0] || distArray[1][0] == 0)
				{
					distArray[1][0] = distance;
					distArray[1][1] = vertexList[cityByState[vertexList[startingID]->state][v1]]->ID;
				}
				else if (distance < distArray[2][0] || distArray[2][0] == 0)
				{
					distArray[2][0] = distance;
					distArray[2][1] = vertexList[cityByState[vertexList[startingID]->state][v1]]->ID;
				}
			}
		}
		if (distArray[0][0] != 0 && distArray[1][0] != 0 && distArray[2][0] != 0)
		{
			for (int a2 = 0; a2 < 3; a2++)
			{
				addEdge(startingID, distArray[a2][1], distArray[a2][0], false, "Green");
			}
			return startingID;
		}
		else
			{
				vector <string> toVisit;
				vector <string> visited;
				visited.push_back(vertexList[startingID]->state);
				for (int t1 = 1; t1 < mapAdjList[vertexList[startingID]->state].size(); t1++)
					toVisit.push_back(mapAdjList[vertexList[startingID]->state][t1]);
				int stateCount = 0;
				string currentState = toVisit[0];
				while (distArray[2][0] == 0)
				{
					if (stateCount >= mapAdjList[currentState].size())
						stateCount = 0;
					if (stateCount == mapAdjList[currentState].size() - 1)
					{
						visited.push_back(currentState);
						for (int w1 = 0; w1 < visited.size(); w1++)
							for (int w2 = 0; w2 < toVisit.size(); w2++)
								if (toVisit[w1] == visited[w2])
									toVisit.erase(toVisit.begin() + w1);
						if (toVisit.size() != 0)
							currentState = toVisit[0];
						else
						{
							for (int a2 = 0; a2 < vertexList[startingID]->E.size(); a2++)
							{
								addEdge(startingID, distArray[a2][1], distArray[a2][0], false, "Green");
							}
							return startingID;
						}
						for (int t1 = 1; t1 < mapAdjList[currentState].size(); t1++)
							toVisit.push_back(mapAdjList[currentState][t1]);
					}
					for (int v2 = 0; v2 < cityByState[currentState].size(); v2++)
					{
						if (vertexList[startingID]->E.size() < 3 && vertexList[cityByState[currentState][v2]]->E.size() < 3)
						{
							distance = distanceEarth(vertexList[startingID]->loc.lat, vertexList[startingID]->loc.lon, vertexList[cityByState[currentState][v2]]->loc.lat, vertexList[cityByState[currentState][v2]]->loc.lon);
							if (distance < distArray[0][0] || distArray[0][0] == 0)
							{
								distArray[0][0] = distance;
								distArray[0][1] = vertexList[cityByState[vertexList[startingID]->state][v2]]->ID;
							}
							else if (distance < distArray[1][0] || distArray[1][0] == 0)
							{
								distArray[1][0] = distance;
								distArray[1][1] = vertexList[cityByState[vertexList[startingID]->state][v2]]->ID;
							}
							else if (distance < distArray[2][0] || distArray[2][0] == 0)
							{
								distArray[2][0] = distance;
								distArray[2][1] = vertexList[cityByState[vertexList[startingID]->state][v2]]->ID;
							}
						}
					}
					stateCount++;
				}
				for (int a3 = 0; a3 < 3; a3++)
				{
					addEdge(startingID, distArray[a3][1], distArray[a3][0], false, "Green");
				}
				return startingID;
			}
		}

		void connectForest()
		{
			bool neighbor;
			double distance;
			edge *e;

			E = new edgePriorityList[vertexList.size()];

			// for every vertex, find its non neighbor
			for (int v1 = 0; v1 < vertexList.size(); v1++)
			{
				// Inner loop through vertices finds closes non neighbors
				for (int v2 = 0; v2 < vertexList.size(); v2++)
				{
					neighbor = vertexList[v1]->Neighbors(vertexList[v2]->ID);
					cout << "neighbor: " << neighbor << endl;
					if (!neighbor)
					{
						distance = distanceEarth(vertexList[v1]->loc.lat, vertexList[v1]->loc.lon, vertexList[v2]->loc.lat, vertexList[v2]->loc.lon);

						if (distance > 0)
						{
							// add edge to vertex: c's priority queue
							// closest edge to "v1" that is a non-neighbor
							E[v1].Insert(new edge(vertexList[v1]->ID, vertexList[v2]->ID, distance, "Purple"));
						}
					}
				}
			}

			// Connect close neighbors and create a disconnected forest
			// using the priority queue's for each vertex
			for (int v = 0; v < vertexList.size(); v++)
			{
				e = E[v].Pop();
				e->color = "Orange";

				cout << "edge weight: " << e->weight << endl;
				cout << "edge color:  " << e->color << endl;

				addEdge(e->fromID, e->toID, e->weight, false, "Orange");
			}
		}

		// vector<vector<int>> BuildNetwork()
		// {
		//     vector<vector<int>> network;

		//     for (int v = 0; v < vertexList.size(); v++)
		//     {
		//         network[0][0] = 0;
		//     }
		//     return network;
		// }

		void printVids()
		{
			vector<vertex *>::iterator vit;
			vector<edge>::iterator eit;

			for (vit = vertexList.begin(); vit != vertexList.end(); vit++)
			{
				cout << (*vit)->ID << endl;
			}
		}

		string graphViz(bool directed = true, int scale = 0)
		{
			vector<vertex *>::iterator vit;
			vector<edge>::iterator eit;

			// [label="hi", weight=100];

			string viz = "";
			string labels = "";
			string conns = "";
			int weight = 0;
			string arrow = "";
			int x;
			int y;

			if (directed)
			{
				viz = "digraph G {\n";
				arrow = "->";
			}
			else
			{
				viz = "graph G {\n";
				arrow = "--";
			}

			for (vit = vertexList.begin(); vit != vertexList.end(); vit++)
			{
				if ((*vit)->E.size() > 0)
				{
					x = (*vit)->p.x;
					y = (*vit)->p.y;

					labels += "\t" + to_string((*vit)->ID) + " [\n\t\tlabel=\"" + (*vit)->city + ", " + (*vit)->state + "\" \n\t\tpos = \"" + to_string(x) + "," + to_string(y) + "!\"\n\t]\n";

					for (eit = (*vit)->E.begin(); eit != (*vit)->E.end(); eit++)
					{
						x = vertexList[eit->toID]->p.x;
						y = vertexList[eit->toID]->p.y;
						labels += "\t" + to_string(eit->toID) + " [\n\t\tlabel=\"" + vertexList[eit->toID]->city + ", " + vertexList[eit->toID]->state + "\"]\n\t\tpos = \"" + to_string(x) + "," + to_string(y) + "!\"\n\t]\n";
						weight = eit->weight;
						conns += "\t" + to_string((*vit)->ID) + arrow + to_string(eit->toID) + " [weight=" + to_string(weight) + ", label=" + to_string(weight) + "]\n";
					}
				}
				else
				{
					x = (*vit)->p.x;
					y = (*vit)->p.y;

					labels += "\t" + to_string((*vit)->ID) + " [\n\t\tlabel=\"" + (*vit)->city + ", " + (*vit)->state + "\" \n\t\tpos = \"" + to_string(x) + "," + to_string(y) + "!\"\n\t]\n";
				}
			}

			viz += labels;
			viz += conns;
			viz += "}\n";

			return viz;
		}

		/**
		 * maxID - returns the max id assigned to any vertex
		 * Params:
		 *     void
		 * Returns:
		 *     int
		 */
		int maxID()
		{
			return id;
		}

		/**
		 * graphSize - returns the number of vertices and edges
		 * Params:
		 *     void
		 * Returns:
		 *     int
		 */
		int *graphSize()
		{
			int *vals = new int[2];
			vals[0] = vertexList.size();
			vals[1] = num_edges;
			return vals;
		}

		/**
		 * operator= - overload assignment for Graph
		 * NOT DONE
		 * Params:
		 *     const latlon ll     - lat lon to assign
		 * Returns
		 *     reference to assignment for chaining (e.g. a = b = c )
		 */
		graph &operator=(const graph &g)
		{
			// do the copy
			vertexList = g.vertexList;
			id = g.id;

			// return the existing object so we can chain this operator
			return *this;
		}

		void stateAdjList()
		{
			ifstream infile;
			ofstream outfile;
			vector <string> States;
			infile.open("states.txt");
			while (!infile.eof())
			{
				string ST;
				infile >> ST;
				States.push_back(ST);
			}
			infile.clear();
			infile.close();
			for (int i = 0; i < States.size(); i++)
			{
				mapAdjList[States[i]] = vector <string>();
				cityByState[States[i]] = vector <int>();
			}
			infile.open("adjacency.txt");
			string ST2;
			while (!infile.eof())
			{
				infile >> ST2;
				if (ST2.length() > 2)
				{
					string ST3, ST4;
					stringstream iss;
					iss << ST2;
					getline(iss, ST3, ',');
					while (getline(iss, ST4, ','))
					{
						mapAdjList[ST3].push_back(ST4);
						mapAdjList[ST4].push_back(ST3);
					}
					iss.clear();
				}
			}
			infile.clear();
			infile.close();
		}

		void lookupHelp()
		{
			for (int v1 = 0; v1 < vertexList.size(); v1++)
			{
				cityLookup[vertexList[v1]->city] = vertexList[v1]->ID;
				cityByState[vertexList[v1]->state].push_back(vertexList[v1]->ID);
			}
		}
};