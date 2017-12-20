#include <iostream>
#include "ReadData.h"
#include "Graph.h"
#include "RccpFlow.h"

using namespace std;

int main(int argc, char **argv) {
	srand((int)time(0));

	string instance = argv[1];

	Graph &graph = ReadData(instance).read();

	RccpFlow rccpFlow = RccpFlow(graph, instance);

	rccpFlow.rccpFlow();

	return 0;
}


