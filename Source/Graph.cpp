#include "Graph.h"

#include <iostream>

Graph::Graph(int N, int C) : N(N), C(C){
	adjMatrix.resize(N);
	adjList.resize(N);

	for(int i = 0; i < N; i++){
		adjMatrix[i].resize(N);
		for(int j = 0; j < N; j++){
			adjMatrix[i][j] = C;
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
	return (N/3) + 1;
}

bool Graph::hasEdge(int v, int w){
	return v != w && getColor(v, w) < C;
}

void Graph::insert(int v, int w, int color){
	if(color < C){
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

	for(int i = 0; i < N; i++){
		if(adjList[i].size() == 0)
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

	vector<int> discoveryTime(N, -1);
	vector<int> low(N, -1);
	int time = 1;
	int bridges = 0;

	for(int i = 0; i < N; i++){
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
	for(list<int>::iterator it = adjList[vertex].begin(); it != adjList[vertex].end();){
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

void Graph::reduce(){

	int singleColorDeleted = 0, bridgesDeleted = 0, scd, bd;

	do{
		bd = deleteBridges();
		scd = deleteSingleColor();

		bridgesDeleted += bd;
		singleColorDeleted += scd;

	}while(bd > 0 || scd > 0);

	cout << "Pre-processamento: " << (singleColorDeleted + bridgesDeleted) << " arestas deletadas" << endl << endl;

}
