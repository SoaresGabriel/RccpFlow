#include "GraphUtils.h"

void getComponents(vector<vector<int> >& components, vector<list<int> >& adjList){
    vector<int> visited(adjList.size(), false);

    for(unsigned int v = 0; v < adjList.size(); v++){

        if(!visited[v] && !adjList[v].empty()){
            components.emplace_back();
            dfs(v, components[components.size() - 1], adjList, visited);
        }

    }
}

void dfs(int v, vector<int> &component, const vector<list<int>>& adjList, vector<int>& visited){
    visited[v] = true;
    component.push_back(v);

    for(int w : adjList[v]){
        if(!visited[w]){
            dfs(w, component, adjList, visited);
        }
    }
}