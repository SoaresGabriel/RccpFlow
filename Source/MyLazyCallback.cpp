#include "MyLazyCallback.h"
#include "Util.h"

MyLazyCallback::MyLazyCallback(IloEnv env, IloArray< IloArray <IloBoolVarArray> > &x, Graph& graph) :
		IloCplex::LazyConstraintCallbackI(env),
		x(x),
		graph(graph){}

//retorna uma cópia do callback. Este método é uma exigência do CPLEX.
IloCplex::CallbackI* MyLazyCallback::duplicateCallback() const {
   return new (getEnv()) MyLazyCallback(getEnv(), x, graph);
}

void MyLazyCallback::main(){

	int V = graph.N;

	vector<list<int> > originalAdjList = graph.getAdjList();

	for(int f = 0; f < V; f++){

		// monta lista de adjacencia do fluxo
		vector<vector<int> > adjList(V);
		for(int i = 0; i < V; i ++){

			for(int j : originalAdjList[i]){

				if(existX(i, j, f, V) && getValue(x[i][j][f]) > 0.01){
					adjList[i].push_back(j);
					adjList[j].push_back(i);
				}

			}
		}

		/*for(int i = 0; i < V; i++){
			cout << i << ": ";
			for(int j : adjList[i]){
				cout << j << " -> ";
			}
			cout << endl;
		}*/

		vector<vector<int>> components;
		getComponents(components, adjList);

		if(components.size() < 2){
			continue;
		}

		for(unsigned int i = 0; i < components.size(); i++){
			vector<int>& component = components[i];

			// verifica se essa componente contem o vertice desse fluxo
			bool containsF = false;
			for(int v : component){
				if (v == f){
					containsF = true;
					break;
				}
			}

			// se nao contem F, então esse eh um subciclo, e deve ser eliminado da solução
			if(!containsF){

				IloExpr expr(getEnv());

				int a, b;

				for(unsigned int j = 0; j < component.size(); j++){
					a = component[j];
					for(unsigned int k = j + 1; k < component.size(); k++){
						b = component[k];
						if(graph.hasEdge(a, b) && existX(a, b, f, V)){
							expr += x[a][b][f];
							expr += x[b][a][f];
						}
					}
				}

				int y = component.size() - 1;
				add(expr <= y ).end();

			}

		}

	}

}


void MyLazyCallback::getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList){
	vector<int> visited(adjList.size(), false);

	for(unsigned int v = 0; v < adjList.size(); v++){

		if(!visited[v] && adjList[v].size() > 0){
			components.push_back(vector<int>());
			dfs(v, components[components.size() - 1], adjList, visited);
		}

	}
}

void MyLazyCallback::dfs(int v, vector<int> &component, vector<vector<int>>& adjList, vector<int>& visited){
	visited[v] = true;
	component.push_back(v);

	for(int w : adjList[v]){
		if(!visited[w]){
			dfs(w, component, adjList, visited);
		}
	}
}
