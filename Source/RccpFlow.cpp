#include "RccpFlow.h"
#include "MyCutCallback.h"
#include "MyLazyCallback.h"
#include <cmath>

RccpFlow::RccpFlow(Graph& graph, string instance) : graph(graph), instance(instance), adjList(graph.getAdjList()), V(graph.N), M(graph.getTrivialWeight()), L(graph.C),
									model(env), x(env, V), Cv(env, V), Sv(env, V) {

		const vector<list<int> >& adjList = graph.getAdjList();

		char var[100];
		for (int i = 0; i < V; i++) {
			sprintf(var, "Cv(%d)", i);
			Cv[i].setName(var);
			model.add(Cv[i]);
		}

		for (int i = 0; i < V; i++) {
			sprintf(var, "Sv(%d)", i);
			Sv[i].setName(var);
			model.add(Sv[i]);
		}

		for(int i=0; i<V; ++i){
			IloArray<IloBoolVarArray> matriz(env, V);
			x[i] = matriz;
			for(int j=0; j<V; ++j){
				IloBoolVarArray vetor(env, V);
				x[i][j] = vetor;

				if(graph.getColor(i, j) != graph.C){
					for(int f = 0; f < V; f++){
						if(existX(i, j, f, V)){
							sprintf(var, "x(%d,%d,%d)", i, j, f);
							x[i][j][f].setName(var);
							model.add(x[i][j][f]);
						}
					}
				}
			}
		}

		IloExpr SumFs(env);
		IloExpr SumFt(env);

		// FUNÇÃO OBJETIVO (1)
		for(int v = 0; v < V; v++){
			SumFs += Cv[v];
			SumFt += Sv[v];
		}
		model.add(IloMinimize(env, SumFs + (M-1)*SumFt));
		// FIM FUNÇÃO OBJETIVO (1)

		// RESTRIÇÃO (2)
		for(int v = 0; v < V; v++){
			IloExpr SumX(env);
			const list<int>& adjListv = adjList[v];
			for(auto i : adjListv){
				if(existX(v, i, v, V))
					SumX += x[v][i][v];
			}
			SumX += Sv[v];
			model.add(SumX == Cv[v]);
		}
		// FIM RESTRIÇÃO (2)

		// RESTRIÇÃO (3)
		for(int f = 0; f < V; f++){
			IloExpr SumXijf(env);
			for(int i = 0; i < V; i++){
				const list<int>& adjListi = adjList[i];

				for(int j : adjListi){
					if(existX(i, j, f, V))
						SumXijf += x[i][j][f];
				}
			}

			int minVL = min(V, L);

			model.add(SumXijf <= (Cv[f] - Sv[f]) * minVL);
		}
		// FIM RESTRIÇÃO (3)

		// RESTRIÇÃO (4)
		for(int v = 0; v < V; v++){
			for(int f = 0; f < v; f++){
				IloExpr SumXivf(env);
				IloExpr SumXvif(env);
				const list<int>& adjListv = adjList[v];
				for(auto i : adjListv){
					if(existX(i, v, f, V)){
						SumXivf += x[i][v][f];
						SumXvif += x[v][i][f];
					}
				}

				model.add(SumXivf == SumXvif);
			}
		}
		// FIM RESTRIÇÃO (4)

		// RESTRIÇÃO (5)
		for(int v = 0; v < V; v++){
			IloExpr SumX(env);
			const list<int>& adjListv = adjList[v];
			for(int f = 0; f <= v; f++){
				for(auto i: adjListv){
					if(existX(v, i, f, V))
						SumX += x[v][i][f];
				}
			}

			SumX += Sv[v];
			model.add(SumX == 1);
		}
		// FIM RESTRIÇÃO (5)

		// RESTRIÇÃO (6)
		vector<vector<pair<int, int> > > El(L);
		for(int i = 0; i < V; i++){
			const list<int>& adjListi = adjList[i];

			for(auto j : adjListi){
				El[graph.getColor(i, j)].push_back(pair<int, int>(i, j));
			}

		}

		for(int f = 0; f < V; f++){
			for(int l = 0; l < L; l++){

				IloExpr Sum(env);

				for(pair<int, int> edge : El[l]){
					if(existX(edge.first, edge.second, f, V)){
						Sum += x[edge.first][edge.second][f];
					}
				}

				model.add(Sum <= 1);

			}
		}
		// FIM RESTRIÇÃO (6)

}

void RccpFlow::rccpFlow(){

	clock_t initialTime = clock(), finalTime;

	IloCplex RccpFlow(model);

	// callback inteiro
	MyLazyCallback* lazyCbk = new (env) MyLazyCallback(env, x, graph);
	RccpFlow.use(lazyCbk);

	// callback fracionario
	MyCutCallback* cutCbk = new (env) MyCutCallback(env, x, graph);
	RccpFlow.use(cutCbk);

	RccpFlow.setOut(env.getNullStream()); // @suppress("Invalid arguments")
	RccpFlow.setWarning(env.getNullStream()); // @suppress("Invalid arguments")

	RccpFlow.setParam(IloCplex::TiLim, 3*60*60); // 3h

	RccpFlow.solve();

	//RccpFlow.exportModel("RccpFlow.lp");

	finalTime = clock();

	// Monta os ciclos da solução
	vector<vector<int> > ciclos;
	for(int f = 0; f < V; f++){

		vector<int> ciclo;

		if(RccpFlow.getValue(Sv[f]) > 0.99){
			ciclo.push_back(f);
			ciclos.push_back(ciclo);
			continue;
		}

		if(RccpFlow.getValue(Cv[f]) < 0.1){
			continue;
		}

		ciclo.push_back(f);

		int atual = f;
		do{
			for(int i : adjList[atual]){
				if(existX(atual, i, f, V) && RccpFlow.getValue(x[atual][i][f]) > 0.99){

					if(i != f)
						ciclo.push_back(i);
					atual = i;
					break;
				}
			}
		}while(atual != f);

		ciclos.push_back(ciclo);

	}

	long executionTime = ((finalTime - initialTime) / (CLOCKS_PER_SEC / 1000));

	bool optimal = abs(RccpFlow.getObjValue() - RccpFlow.getBestObjValue()) < 0.1;

	printResult(instance, ciclos, M, executionTime, RccpFlow.getNnodes(), optimal);

	/*// Imprime os ciclos
	for(unsigned int i = 0; i < ciclos.size(); i++){
		cout << "Ciclo " << ciclos[i].size() << ": ";
		for(unsigned int j = 0; j < ciclos[i].size(); j++){
			cout << ciclos[i][j] << " ";
		}
		cout << endl;
	}

	for(int i = 0; i < V; i++){
		cout << "Vertice" << i << "\tFs: " << RccpFlow.getValue(Fs[i])
				<< "\tFt: " << RccpFlow.getValue(Ft[i]) << endl;
	}*/

}

bool RccpFlow::searchSubcycles(IloCplex& cplex){

	bool containsSubcycle = false;

	for(int f = 0; f < V; f++){

		// monta lista de adjacencia do fluxo
		vector<vector<int> > flowAdjList(V);
		for(int i = 0; i < V; i ++){

			for(int j : adjList[i]){

				if(existX(i, j, f, V) && cplex.getValue(x[i][j][f]) > 0.01){
					flowAdjList[i].push_back(j);
					flowAdjList[j].push_back(i);
				}

			}
		}

		vector<vector<int>> components;
		getComponents(components, flowAdjList);

		if(components.size() <= 1){
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

				containsSubcycle = true;

				IloExpr expr(env);

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
				cplex.addCut(expr <= y );


			}

		}

	}
	return containsSubcycle;
}

void RccpFlow::getComponents(vector<vector<int> >& components, vector<vector<int> >& adjList){
	vector<int> visited(adjList.size(), false);

	for(unsigned int v = 0; v < adjList.size(); v++){

		if(!visited[v] && adjList[v].size() > 0){
			components.push_back(vector<int>());
			dfs(v, components[components.size() - 1], adjList, visited);
		}

	}
}

void RccpFlow::dfs(int v, vector<int> &component, vector<vector<int>>& adjList, vector<int>& visited){
	visited[v] = true;
	component.push_back(v);

	for(int w : adjList[v]){
		if(!visited[w]){
			dfs(w, component, adjList, visited);
		}
	}
}
