#include "MyCutCallback.h"
#include "Util.h"
#include <set>
extern "C"{
#include "concorde.h"
}

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

	vector<list<int> > originalAdjList = graph.getAdjList();

	for(int f = 0; f < V; f++){

		// monta lista de adjacencia do fluxo
		vector<vector<int> > adjList(V);

		for(int i = 0; i < V; i++){

			for(int j : originalAdjList[i]){

				if(existX(i, j, f, V) && getValue(x[i][j][f]) > 0.0001){
					adjList[i].push_back(j);
				}

			}
		}

		vector<vector<int>> components;
		getComponents(components, adjList);

		if(components.size() == 0){
			continue;
		}

		for(auto &component: components) {
			verifyComponent(component, f);
		}

	}
}

void MyCutCallback::verifyComponent(vector<int> &component, int f) {

	int a, b;

	unsigned int V = graph.N;

	set<pair<int, int>> xvarList;

	for(unsigned int j = 0; j < component.size(); j++){
		a = component[j];

		for(unsigned int k = j + 1; k < component.size(); k++){
			b = component[k];
			if(graph.hasEdge(a, b) && existX(a, b, f, V)){
				if(getValue(x[a][b][f]) > 0.0001) {
					xvarList.insert(pair<int, int>(j, k));
				}
			}
		}
	}

	int nbCustomers = component.size();
	int nbEdges =  xvarList.size();
	int *tsp_edgelist = new int[2*nbEdges];
	double *tsp_x = new double[nbEdges];
	int edgeId = 0;
	for (std::set<pair<int, int>>::const_iterator varIt = xvarList.begin(); varIt != xvarList.end(); ++varIt, ++edgeId){
		int i = varIt->first, j = varIt->second, v = component[i], w = component[j];
		tsp_x[edgeId] = getValue(x[v][w][f]) + getValue(x[w][v][f]);
		tsp_edgelist[2*edgeId] = i;
		tsp_edgelist[2*edgeId + 1] = j;
	}

	CCtsp_lpcut_in *tsp_cuts = NULL;
	int  nbSubtourCuts = 0;

	if(CCtsp_exact_subtours(&tsp_cuts, &nbSubtourCuts, nbCustomers, nbEdges, tsp_edgelist, tsp_x)) {
		//fprintf(stderr, "CCtsp_exact_subtours failed\n");
		//printf("CCtsp_exact_subtours failed\n");
		exit(EXIT_FAILURE);
	}
	if (nbSubtourCuts > 0)
	{
		//printf("Found %2d exact subtours\n", nbSubtourCuts);
		addTspConcordeCuts(&tsp_cuts, component, f);
	}
	tsp_cuts = NULL;

	delete[] tsp_edgelist;
	delete[] tsp_x;
}

void MyCutCallback::addTspConcordeCuts(CCtsp_lpcut_in **tsp_cuts, vector<int> &component, unsigned int f) {

	CCtsp_lpcut_in *tsp_cut;

	for (tsp_cut = *tsp_cuts; tsp_cut; tsp_cut = tsp_cut->next){

		std::vector<int> S;
		for(int j = 0; j < tsp_cut->cliques[0].segcount; j++) {
			for(int k = tsp_cut->cliques[0].nodes[j].lo; k <= tsp_cut->cliques[0].nodes[j].hi; k++) {
				S.push_back(component[k]);
			}
		}

		if(S.size() >= 3) {
			IloExpr expr(getEnv());

			for(unsigned int i = 0; i < S.size(); i++) {
				for(unsigned int j = i+1; j < S.size(); j++) {
					if(graph.hasEdge(S[i], S[j]) && existX(S[i], S[j], f, graph.N)) {
						expr += x[S[i]][S[j]][f];
						expr += x[S[j]][S[i]][f];
					}
				}
			}
			try {
				add(expr <= (int) (S.size() - 1)).end();
			} catch (IloException &e) {
				cout << e << endl;
				exit(1);
			}
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
