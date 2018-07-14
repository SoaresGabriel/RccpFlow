#ifndef MYCUTCALLBACK_H_
#define MYCUTCALLBACK_H_

#include <ilcplex/ilocplex.h>
#include "../Common/Graph.h"

class MyCutCallback : public IloCplex::UserCutCallbackI {
public:
	MyCutCallback(IloEnv env, IloArray< IloArray <IloBoolVarArray> >& x, Graph& graph);

	//metodo que retorna uma copia do calback. Cplex exige este método.
	IloCplex::CallbackI* duplicateCallback() const;

	// codigo executado pelo Cplex
	void main();

private:
	IloArray< IloArray <IloBoolVarArray> >& x;
	Graph& graph;

	void getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList);

	void dfs(int v, vector<int> &component, vector<vector<int>>& adjList, vector<int>& visited);
};

#endif /* MYCUTCALLBACK_H_ */
