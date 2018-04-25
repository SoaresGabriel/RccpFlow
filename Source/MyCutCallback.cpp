#include "MyCutCallback.h"
#include "Util.h"
#include "MinCut.h"

MyCutCallback::MyCutCallback(IloEnv env, IloArray< IloArray <IloBoolVarArray> > &x, Graph& graph) :
		IloCplex::UserCutCallbackI(env),
		x(x),
		graph(graph){}

//retorna uma cópia do callback. Este método é uma exigência do CPLEX.
IloCplex::CallbackI* MyCutCallback::duplicateCallback() const {
   return new (getEnv()) MyCutCallback(getEnv(), x, graph);
}

void MyCutCallback::main(){

	vector<list<int> > originalAdjList(graph.getAdjList());

	vector<vector<double> > adjMatrix(graph.V, vector<double>(graph.V, 0));

	vector<unsigned int> vertices = {graph.V - 1, graph.V - 2};

	for(int f = graph.V-3; f >= 0; f--) {
        for(int v = f; v < graph.V; v++) {
            for(int w : originalAdjList[v]) {
                if(w < f) continue; // avoid symmetry

                adjMatrix[v][w] += getValue(x[v][w][f]);
            }
        }

        vertices.push_back(static_cast<unsigned int>(f));
        vector<vector<int> > components = minCut(adjMatrix, vertices);

        for(const vector<int> &component : components) {
        	bool contains = false;
        	for(const int &v : component) {
        		if(v == f) {
        			contains = true;
        			break;
        		}
        	}

        	if(contains) {

				IloExpr expr(getEnv());

				int v, w;

				double total = 0;

				for(int a = f; a < graph.V; a++) {
					for(unsigned int j = 0; j < component.size(); j++){
						v = component[j];
						if(v < a) continue; // avoid symmetry
						for(unsigned int k = j + 1; k < component.size(); k++){
							w = component[k];
							if(w < a) continue; // avoid symmetry

							if(graph.hasEdge(v, w)){
								expr += x[v][w][a];
								expr += x[w][v][a];
								total += getValue(x[v][w][a]);
								total += getValue(x[w][v][a]);
							}
						}
					}
				}

				auto y = static_cast<int>(component.size() - 1);

				if(total - 0.0001 > y) {
					add(expr <= y ).end();
					cout << total << " > " << y << endl;
				}

        	}
        }
	}


	for(int f = 0; f < graph.V; f++){

		// monta lista de adjacencia do fluxo
		vector<vector<int> > adjList(static_cast<unsigned long>(graph.V));
		for(int i = f; i < graph.V; i ++){

			for(int j : originalAdjList[i]){

				if(existX(i, j, f, graph.V) && getValue(x[i][j][f]) > 0.01){
					adjList[i].push_back(j);
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

		if(components.empty()){
			continue;
		}

		for (auto &component : components) {
			if(component.size() < 3)
				continue;

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
				double valor = 0.0;

				int a, b;

				for(unsigned int j = 0; j < component.size(); j++){
					a = component[j];
					for(unsigned int k = j + 1; k < component.size(); k++){
						b = component[k];
						if(graph.hasEdge(a, b) && existX(a, b, f, graph.V)){
							expr += x[a][b][f];
							expr += x[b][a][f];
							valor += getValue(x[a][b][f]);
							valor += getValue(x[b][a][f]);
						}
					}
				}

				// adiciona a restricao se ela foi violada
				auto y = static_cast<int>(component.size() - 1);
				if(valor - 0.01 > y){
					add(expr <= y ).end();
				}

			}

		}

	}
}


void MyCutCallback::getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList){
	vector<int> visited(adjList.size(), false);

	for(unsigned int v = 0; v < adjList.size(); v++){

		if(!visited[v] && !adjList[v].empty()){
			components.emplace_back();
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
