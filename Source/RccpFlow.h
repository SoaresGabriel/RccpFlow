#ifndef RCCPFLOW_H_
#define RCCPFLOW_H_

#include "Graph.h"
#include <ilcplex/ilocplex.h>
#include "Util.h"
#include <list>

class RccpFlow {
public:
	RccpFlow(Graph& graph, string instance);

	void rccpFlow();

private:
	Graph& graph;
	string instance;
	const vector<list<int> >& adjList;
	int V, M, L;
	IloEnv env;
	IloModel model;
	IloArray< IloArray <IloBoolVarArray> > x;
	IloBoolVarArray Fs;
	IloBoolVarArray Ft;

	bool searchSubcycles(IloCplex& cplex);
	void getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList);
	void dfs(int v, vector<int> &component, vector<vector<int>>& adjList, vector<int>& visited);
};

#endif /* RCCPFLOW_H_ */
