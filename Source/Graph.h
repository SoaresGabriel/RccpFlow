#ifndef GRAPH_H_
#define GRAPH_H_

#include <vector>
#include <list>

using namespace std;

class Graph {
private:
	int deleteSingleColor();
	int deleteBridges();
	int deleteBridgesAux(int parent, int vertex, int &time, vector<int> &discoveryTime, vector<int> &low, int &bridges);
	vector<vector<int> > adjMatrix;
	vector<list<int> > adjList;

public:
	const int N;
	const int C;

	Graph(int N, int C);

	const vector<list<int> >& getAdjList();
	
	int getColor(int v, int w);
	unsigned int getTrivialWeight();

	bool hasEdge(int v, int w);

	void insert(int v, int w, int color);

	void reduce();
	
};

#endif /* READDATA_H_ */
