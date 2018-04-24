#include <iostream>
#include "ReadData.h"
#include "Graph.h"
#include "RccpFlow.h"

using namespace std;

int main(int argc, char **argv) {
	srand(static_cast<unsigned int>(time(nullptr)));

	string instance = argv[1];

	Graph &graph = ReadData(instance).read();

	RccpFlow rccpFlow = RccpFlow(graph, instance);

	rccpFlow.rccpFlow();

	return 0;
}


