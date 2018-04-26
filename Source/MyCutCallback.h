#ifndef MYCUTCALLBACK_H_
#define MYCUTCALLBACK_H_

#include <ilcplex/ilocplex.h>
#include "Graph.h"

class MyCutCallback : public IloCplex::UserCutCallbackI {
public:
	MyCutCallback(IloEnv env, IloArray< IloArray <IloBoolVarArray> >& x, Graph& graph);

	//metodo que retorna uma copia do calback. Cplex exige este método.
	IloCplex::CallbackI* duplicateCallback() const override;

	// codigo executado pelo Cplex
	void main() override;

private:
	IloArray< IloArray <IloBoolVarArray> >& x;
	Graph& graph;
};

#endif /* MYCUTCALLBACK_H_ */
