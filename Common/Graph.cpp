#include "Graph.h"

#include <iostream>
#include <boost/dynamic_bitset.hpp>

Graph::Graph(unsigned int V, unsigned int L) : V(V), L(L){
	adjMatrix.resize(V);
	adjList.resize(V);

	for(int i = 0; i < V; i++){
		adjMatrix[i].resize(V);
		for(int j = 0; j < V; j++){
			adjMatrix[i][j] = L;
		}
	}
}

const vector<list<int> >& Graph::getAdjList(){
	return adjList;
}


int Graph::getColor(int v, int w){
	return adjMatrix[v][w];
}

unsigned int Graph::getTrivialWeight(){
	return ((V / 3) + 1);
}

bool Graph::hasEdge(int v, int w){
	return v != w && getColor(v, w) < L;
}

void Graph::insert(int v, int w, int color){
	if(color < L){
		adjMatrix[v][w] = color;
		adjMatrix[w][v] = color;

		adjList[v].push_back(w);
		adjList[w].push_back(v);
	}
}

int Graph::deleteSingleColor(){

	list<int>::iterator it;

	bool oneColor;
	int color;
	int delOneColor = 0;

	for(int i = 0; i < V; i++){
		if(adjList[i].empty())
			continue;

		/*
		 * Verifica as cores das arestas atuais, se todas forem da mesma cor, deleta o vertice
		 * */
		oneColor = true;
		color = adjMatrix[i][adjList[i].front()]; // cor do vertice atual ate o primeiro na sua lista de adjacencia
		for(it = adjList[i].begin(); it != adjList[i].end(); it++){
			if(adjMatrix[i][*it] != color){
				oneColor = false;
				break;
			}
		}

		// se tiver apenas uma cor, deleta o vertice da lista de adjacencia.
		if(oneColor){
			delOneColor += adjList[i].size();

			for(it = adjList[i].begin(); it != adjList[i].end(); it++){
				adjList[*it].remove(i);
			}

			adjList[i].clear();
		}

	}

	return delOneColor;

}

int Graph::deleteBridges(){

	vector<int> discoveryTime(V, -1);
	vector<int> low(V, -1);
	int time = 1;
	int bridges = 0;

	for(int i = 0; i < V; i++){
		if(discoveryTime[i] == -1){
			deleteBridgesAux(i, i, time, discoveryTime, low, bridges);
		}
	}

	return bridges;
}

int Graph::deleteBridgesAux(int parent, int vertex, int &time, vector<int> &discoveryTime, vector<int> &low, int &bridges){
	discoveryTime[vertex] = time;
	low[vertex] = discoveryTime[vertex];

	time++;

	bool itErased;
	int neighbor;
	for(auto it = adjList[vertex].begin(); it != adjList[vertex].end();){
		neighbor = *it;
		itErased = false;
		if(discoveryTime[neighbor] == -1){

			deleteBridgesAux(vertex, neighbor, time, discoveryTime, low, bridges);
			low[vertex] = low[neighbor];

			if(low[neighbor] == discoveryTime[neighbor]){
				it = adjList[vertex].erase(it);
				itErased = true;
				adjList[neighbor].remove(vertex);
				bridges++;
			}

		}else if(neighbor != parent && discoveryTime[neighbor] < discoveryTime[vertex]){
			discoveryTime[vertex] = discoveryTime[neighbor];
		}

		if(!itErased)
			it++;
	}

	return bridges;
}

int Graph::deleteVertexPairTwoColor() {
    vector<boost::dynamic_bitset<> > vertexColors(V, boost::dynamic_bitset<>(L));

    int deleted = 0;

    for(int v = 0; v < V; v++) {
        for(auto w : adjList[v]) {
            vertexColors[v].set(static_cast<unsigned long>(getColor(v, w)));
        }
    }

    bool itErased;
    int w;
    for(int v = 0; v < V; v++) {
        for(auto it = adjList[v].begin(); it != adjList[v].end();) {
            itErased = false;
            w = *it;

            if((vertexColors[v] | vertexColors[w]).count() == 2) {
                it = adjList[v].erase(it);
                itErased = true;
                adjList[w].remove(v);
                deleted++;
            }

            if(!itErased) it++;
        }
    }

    return deleted;
}

void Graph::reduce(){

	int singleColorDeleted = 0, bridgesDeleted = 0, vertexPairTwoColorDeleted = 0, scd, bd, vptcd;

	do{
		bd = deleteBridges();
		scd = deleteSingleColor();
		vptcd = deleteVertexPairTwoColor();

		bridgesDeleted += bd;
		singleColorDeleted += scd;
		vertexPairTwoColorDeleted += vptcd;

	}while(bd > 0 || scd > 0 || vptcd > 0);

	cout << "Pre-processamento: " << (singleColorDeleted + bridgesDeleted) << " arestas deletadas" << endl;
    cout << bridgesDeleted << " bridges deleted." << endl;
    cout << singleColorDeleted << " single color deleted." << endl;
    cout << vertexPairTwoColorDeleted << " vertex pair with just two color vertices deleted." << endl << endl;

}
