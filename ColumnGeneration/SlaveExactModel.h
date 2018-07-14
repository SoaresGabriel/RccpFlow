#ifndef RCCPFLOW_EXACTMODEL_H
#define RCCPFLOW_EXACTMODEL_H

#include <ilcplex/ilocplex.h>
#include "../Common/Graph.h"
#include <list>

class SlaveExactModel {
public:
    SlaveExactModel(Graph &graph, vector<double> weights, string instance);

    void solve();

private:
    Graph &graph;
    vector<double> weights;
    string instance;
    const vector<list<int> > &adjList;
    int V, M, L;
    IloEnv env;
    IloModel model;
    IloBoolVarArray x;
    IloArray<IloBoolVarArray> y;
};


#endif //RCCPFLOW_EXACTMODEL_H
