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

	vector<list<int> > thisAdjList(graph.getAdjList());

	vector<vector<double> > adjMatrix(graph.V, vector<double>(graph.V, 0));
	vector<vector<vector<double> > > xValues(graph.V, vector<vector<double> >(graph.V, vector<double>(graph.V)));

	// initilize vertices vector with all vertices
	vector<unsigned int> vertices;
	for(int v = graph.V - 1; v >= 0; v--){
		vertices.push_back(static_cast<unsigned int &&>(v));
	}

	// add all edges from all flows in the adjMatrix
	for(int f = 0; f < graph.V - 2; f++) {
	    for(int v = f; v < graph.V; v++) {
	        for(const int &w : thisAdjList[v]) {
	            if(w >= f) { // avoid symmetry
	                xValues[v][w][f] = getValue(x[v][w][f]);
	                adjMatrix[v][w] += xValues[v][w][f];
	            }
	        }
	    }
	}

	for(int v = 0; v < graph.V-3; v++) { // v = min{S}

	    // remove edges of the flow v with vertex v
	    for(int w : thisAdjList[v]) {
	        adjMatrix[v][w] -= xValues[v][w][v];
	        adjMatrix[w][v] -= xValues[w][v][v];
	    }

		vector<int> S = minCut(adjMatrix, vertices);

	    if(S.size() > 2) {
            IloExpr expr(getEnv());

            double total = 0;
            int a,b;
            for(int f = 0; f < graph.V - 2; f++) {
                for(unsigned int i = 0; i < S.size(); i++){
                    a = S[i];
                    if(a < f) continue;
                    for(unsigned int j = i + 1; j < S.size(); j++) {
                        b = S[j];
                        if(b < f) continue;

                        if(graph.hasEdge(a, b) && ( f != v || (a != v && b != v) )) {
                            expr += x[a][b][f];
                            expr += x[b][a][f];
                            total += xValues[a][b][f];
                            total += xValues[b][a][f];
                        }

                    }
                }
            }

            auto y = static_cast<int>(S.size() - 1);

            if(total - 0.0001 > y) {
                add(expr <= y ).end();
                cout << total << " > " << y << endl;
            }
	    }

        // restore edges of the flow v with vertex v
        for(int w : thisAdjList[v]) {
            adjMatrix[v][w] += xValues[v][w][v];
            adjMatrix[w][v] += xValues[w][v][v];
        }

        // remove vertex v from adjList and adjMatrix
        for(auto w : thisAdjList[v]) {
	        thisAdjList[w].remove(v);
	        adjMatrix[v][w] = 0;
	        adjMatrix[w][v] = 0;
	    }
	    thisAdjList[v].clear();
	    vertices.pop_back();
	}

}