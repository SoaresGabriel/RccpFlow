#include "MyCutCallback.h"
#include "Util.h"
#include "MinCut.h"
#include <set>

MyCutCallback::MyCutCallback(IloEnv env, IloArray< IloArray <IloBoolVarArray> > &x, IloBoolVarArray& Fs, IloBoolVarArray& Ft, Graph& graph) :
		IloCplex::UserCutCallbackI(env),
		x(x),
		Fs(Fs),
		Ft(Ft),
		graph(graph){}

//retorna uma cópia do callback. Este método é uma exigência do CPLEX.
IloCplex::CallbackI* MyCutCallback::duplicateCallback() const {
   return new (getEnv()) MyCutCallback(getEnv(), x, Fs, Ft, graph);
}

void MyCutCallback::main(){

	int V = graph.N;

	vector<list<int> > originalAdjList(graph.getAdjList());

	for(int f = 0; f < V; f++){

		// monta lista de adjacencia do fluxo
		vector<vector<int> > adjList(V);
		vector<vector<double> > adjMatrix(V, vector<double>(V, -1));

		for(int i = 0; i < V; i++){
			if(i == f) continue;

			for(int j : originalAdjList[i]){
				if(j == f) continue;

				if(existX(i, j, f, V)) {
					double value = getValue(x[i][j][f]);
					if(value > 0.0001){
						adjList[i].push_back(j);
						adjMatrix[i][j] = value;
					}
				}
			}
		}

		vector<vector<int>> components;
		getComponents(components, adjList);

		if(components.size() == 0){
			continue;
		}

		for(auto &component: components) {
			verifyComponent(adjMatrix, component, f);
		}

	}
}

void MyCutCallback::verifyComponent(vector<vector<double> > &adjMatrix, vector<int> &component, int f) {

	vector<vector<int> > components = minCut(adjMatrix, component);

	for(vector<int> &S : components) {
		if(S.size() < 3) continue;

		IloExpr expr(getEnv());
		double total = 0;

		for(unsigned int i = 0; i < S.size(); i++) {
			for(unsigned int j = i+1; j < S.size(); j++) {
				if(graph.hasEdge(S[i], S[j]) && existX(S[i], S[j], f, graph.N)) {
					expr += x[S[i]][S[j]][f];
					expr += x[S[j]][S[i]][f];
					total += getValue(x[S[i]][S[j]][f]);
					total += getValue(x[S[i]][S[j]][f]);
				}
			}
		}

		int y = S.size() - 1;

		if(total - 0.01 > y) {
			cout << "new Cut: " << total << " > " << y << endl;
			add(expr <= y).end();
		}

	}


}

void MyCutCallback::getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList){
	vector<int> visited(adjList.size(), false);

	for(unsigned int v = 0; v < adjList.size(); v++){

		if(!visited[v] && adjList[v].size() > 0){
			components.push_back(vector<int>());
			dfs(v, components[components.size() - 1], adjList, visited);
		}

	}
}

void MyCutCallback::dfs(int v, vector<int> &component, vector<vector<int>>& adjList, vector<int>& visited){
	visited[v] = true;
	component.push_back(v);

	for(int w : adjList[v]){
		if(!visited[w]){
			dfs(w, component, adjList, visited);
		}
	}
}
