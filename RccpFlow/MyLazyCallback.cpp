#include "MyLazyCallback.h"

MyLazyCallback::MyLazyCallback(IloEnv env, IloArray< IloArray <IloBoolVarArray> > &x, Graph& graph) :
		IloCplex::LazyConstraintCallbackI(env),
		x(x),
		graph(graph){}

//retorna uma cópia do callback. Este método é uma exigência do CPLEX.
IloCplex::CallbackI* MyLazyCallback::duplicateCallback() const {
   return new (getEnv()) MyLazyCallback(getEnv(), x, graph);
}

void MyLazyCallback::main(){

	vector<list<int> > originalAdjList = graph.getAdjList();

	for(int f = 0; f < graph.V - 2; f++){

		// monta lista de adjacencia do fluxo
		vector<vector<int> > adjList(graph.V);
		for(int i = f; i < graph.V; i++){

			for(int j : originalAdjList[i]){
				if(j < f) continue; // avoid symmetry

				if(getValue(x[i][j][f]) > 0.0001){
					adjList[i].push_back(j);
				}

			}
		}

		vector<vector<int>> components;
		getComponents(components, adjList);

		if(components.size() < 2){
			continue;
		}

		for (auto &component : components) {
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

				int v, w;

				for(unsigned int j = 0; j < component.size(); j++){
					v = component[j];
					if(v < f) continue; // avoid symmetry
					for(unsigned int k = j + 1; k < component.size(); k++){
						w = component[k];
                        if(w < f) continue; // avoid symmetry

						if(graph.hasEdge(v, w)){
							expr += x[v][w][f];
							expr += x[w][v][f];
						}
					}
				}

                auto y = static_cast<int>(component.size() - 1);
				add(expr <= y ).end();

			}

		}

	}

}


void MyLazyCallback::getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList){
	vector<int> visited(adjList.size(), false);

	for(unsigned int v = 0; v < adjList.size(); v++){

		if(!visited[v] && !adjList[v].empty()){
			components.emplace_back();
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
