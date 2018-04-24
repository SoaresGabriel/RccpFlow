#include <iostream>
#include <limits.h>
#include <string.h>
#include <queue>
#include <vector>
#include "MinCut.h"
#include <list>
#include "GraphUtils.h"

using namespace std;

/* Returns true if there is a path from source 's' to sink 't' in
  residual graph. Also fills parent[] to store the path */
bool bfs(vector<vector<double> > &rGraph, int s, int t, vector<int>& parent) {
	unsigned int V = rGraph.size();

	parent.resize(V);

    // Create a visited array and mark all vertices as not visited
    vector<bool> visited(V, false);

    // Create a queue, enqueue source vertex and mark source vertex
    // as visited
    queue<int> q;
    q.push(s);
    visited[s] = true;
    parent[s] = -1;

    // Standard BFS Loop
    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (unsigned int v = 0; v < V; v++) {
            if (visited[v]==false && rGraph[u][v] > 0.00001) {
                q.push(v);
                parent[v] = u;
                visited[v] = true;
            }
        }
    }

    // If we reached sink in BFS starting from source, then return
    // true, else false
    return (visited[t] == true);
}

// A DFS based function to find all reachable vertices from s.  The function
// marks visited[i] as true if i is reachable from s.  The initial values in
// visited[] must be false. We can also use BFS to find reachable vertices
void dfs(vector<vector<double> > &rGraph, unsigned int s, vector<bool> &visited) {
    visited[s] = true;
    for (int i = 0; i < rGraph.size(); i++)
       if (rGraph[s][i] > 0.00001 && !visited[i])
           dfs(rGraph, i, visited);
}

// Returns the maximum flow from s to t in the given graph
void minCutMaxFlow(vector<vector<double> > &graph, unsigned int s, unsigned int t, vector<pair<int, int>>& minCut, double &currentMinCutValue) {
    unsigned int u, v;

    unsigned int V = graph.size();

    // Create a residual graph and fill the residual graph with
    // given capacities in the original graph as residual capacities
    // in residual graph
    vector<vector<double> > rGraph(graph); // Residual graph where rGraph[i][j] indicates
                     // residual capacity of edge from i to j (if there
                     // is an edge. If rGraph[i][j] is 0, then there is not)

    vector<int> parent(V);  // This array is filled by BFS and to store path

    double max_flow = 0;  // There is no flow initially

    // Augment the flow while tere is path from source to sink
    while (bfs(rGraph, s, t, parent)) {
        // Find minimum residual capacity of the edges along the
        // path filled by BFS. Or we can say find the maximum flow
        // through the path found.
        double path_flow = INT_MAX;
        for (v = t; v != s; v = parent[v]) {
            u = parent[v];
            path_flow = min(path_flow, rGraph[u][v]);
        }

        // update residual capacities of the edges and reverse edges
        // along the path
        for (v = t; v != s; v = parent[v]) {
            u = parent[v];
            rGraph[u][v] -= path_flow;
            rGraph[v][u] += path_flow;
        }

        // Add path flow to overall flow
        max_flow += path_flow;
    }

    if (max_flow > currentMinCutValue) {
    	currentMinCutValue = max_flow;

    	minCut.clear();

    	// Flow is maximum now, find vertices reachable from s
		vector<bool> visited(V, false);
		dfs(rGraph, s, visited);

		// Print all edges that are from a reachable vertex to
		// non-reachable vertex in the original graph
		for (int i = 0; i < V; i++)
		  for (int j = 0; j < V; j++)
			 if (visited[i] && !visited[j] && graph[i][j])
				  minCut.push_back(pair<int, int>(i, j));
    }

}

vector<vector<int> > minCut(vector<vector<double> > &graph, vector<int> &component) {

	vector<pair<int, int>> minCut;

	double minCutValue = INT_MAX;

	for(int i = 1; i < component.size(); i++) {
		minCutMaxFlow(graph, 0, i, minCut, minCutValue);
	}

	vector<list<int> > adjList(graph.size());

	for(int i = 0; i < graph.size(); i++){

		for(int j = 0; j < graph.size(); j++){

			if(graph[i][j] > 0.0001){
				adjList[i].push_back(j);
				adjList[j].push_back(i);
			}

		}
	}

	for(auto edge : minCut) {
		adjList[edge.first].remove(edge.second);
		adjList[edge.second].remove(edge.first);
	}

	vector<vector<int> > components;

	getComponents(components, adjList);


	return components;
}


