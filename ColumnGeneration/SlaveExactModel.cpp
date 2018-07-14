#include "SlaveExactModel.h"

SlaveExactModel::SlaveExactModel(
        Graph &graph,
        vector<double> weights,
        string instance
) : graph(graph), instance(move(instance)), weights(weights),
        adjList(graph.getAdjList()),
        V(graph.V), M(graph.getTrivialWeight()),
        L(graph.L),
        model(env), x(env, V), y(env, V) {


    // adicionar variaveis no modelo
    char var[100];
    for (int i = 0; i < V; ++i) {
        sprintf(var, "x(%d)", i);
        x[i].setName(var);
        model.add(x[i]);
    }

    for (int i = 0; i < V; ++i) {
        IloBoolVarArray array(env, V);
        y[i] = array;

        for (int j = i + 1; j < V; ++j) {
            if (graph.hasEdge(i, j)) {
                sprintf(var, "x(%d, %d)", i, j);
                y[i][j].setName(var);
                model.add(y[i][j]);
            }
        }
    }

    // Função Objetivo
    IloExpr obj(env);
    for (int v = 0; v < V; ++v) {
        obj += weights[v] * x[v];
    }
    model.add(IloMaximize(env, obj));
    // Fim Função Objetivo

    // Restrição 1 - Formação de ciclos
    for (int v = 0; v < V; ++v) {
        IloExpr sumYvu(env);

        for (int u : adjList[v]) {
            if (v < u) {
                sumYvu += y[v][u];
            } else {
                sumYvu += y[u][v];
            }
        }

        model.add(sumYvu == 2 * x[v]);
    }
    // Fim Restrição 1

    // Restrição 2 - Restrição Rainbow
    vector<vector<pair<int, int> > > El(static_cast<unsigned long>(L));
    for (int v = 0; v < V; v++) { // monta lista com arestas de cada cor
        const list<int> &adjListV = adjList[v];

        for (auto u : adjListV) {
            if (v < u) {
                El[graph.getColor(v, u)].push_back(pair<int, int>(v, u));
            }
        }

    }

    // para cada cor, a soma das arestas deve ser no maximo 1
    for (int l = 0; l < L; l++) {

        IloExpr Sum(env);

        for (pair<int, int> edge : El[l]) {
            Sum += y[edge.first][edge.second];
        }

        model.add(Sum <= 1);

    }
    // Fim Restrição 2

}

void SlaveExactModel::solve() {

    clock_t initialTime = clock(), finalTime;

    IloCplex slave(model);

    slave.solve();

    finalTime = clock();
    long executionTime = ((finalTime - initialTime) / (CLOCKS_PER_SEC / 1000));
    cout << "Execution Time: " << executionTime << "ms" << endl;

    for (int v = 0; v < graph.V; ++v) {
        printf("x[%d] = %lf \n", v, slave.getValue(x[v]));
    }

    for (int v = 0; v < graph.V; ++v) {

        for (int u = 0; u < graph.V; ++u) {
            if (graph.hasEdge(v, u) && v < u) {
                printf("y[%d][%d] = %lf\n", v, u, slave.getValue(y[v][u]));
            }
        }

    }

}