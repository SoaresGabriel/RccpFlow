#include "RccpFlow.h"
#include "MyCutCallback.h"
#include "MyLazyCallback.h"
#include <cmath>
#include <utility>
#include <set>

RccpFlow::RccpFlow(Graph& graph, string instance) : graph(graph), instance(move(instance)), adjList(graph.getAdjList()), V(graph.V), M(graph.getTrivialWeight()), L(graph.L),
									model(env), x(env, V), Cv(env, V), Sv(env, V) {

		const vector<list<int> >& adjList(graph.getAdjList());

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

				if(graph.getColor(i, j) != graph.L){
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
		vector<vector<pair<int, int> > > El(static_cast<unsigned long>(L));
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

        vector<set<int> > Vl(static_cast<unsigned long>(L));

        for(int v = 0; v < V; v++) {
            for(int w : adjList[v]) {
                int color = graph.getColor(v, w);
                Vl[color].insert(v);
                Vl[color].insert(w);
            }
        }

        // limit number of edges per color
        for(int l = 0; l < L; l++) {
            IloExpr Sum(env);

            for(int f = 0; f < V - 2; f++) {
                for(pair<int, int> edge : El[l]) {
                    if(edge.first >= f && edge.second >= f) {
                        Sum += x[edge.first][edge.second][f];
                    }
                }
            }

            auto limit = static_cast<int>(Vl[l].size() / 2);
            model.add(Sum <= limit);
        }
}

void RccpFlow::rccpFlow(){

	clock_t initialTime = clock(), finalTime;

	IloCplex RccpFlow(model);

	// callback inteiro
    auto lazyCbk = new (env) MyLazyCallback(env, x, graph);
	RccpFlow.use(lazyCbk);

	// callback fracionario
    auto cutCbk = new (env) MyCutCallback(env, x, graph);
	RccpFlow.use(cutCbk);

	RccpFlow.setOut(env.getNullStream());
	RccpFlow.setWarning(env.getNullStream());

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

}
