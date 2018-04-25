#ifndef GRAPHUTILS_H_
#define GRAPHUTILS_H_

#include <vector>
#include <list>

using namespace std;


void getComponents(vector<vector<int> >& components, vector<list<int> >& adjList);
void dfs(int v, vector<int> &component, const vector<list<int>>& adjList, vector<int>& visited);

#endif /* GRAPHUTILS_H_ */