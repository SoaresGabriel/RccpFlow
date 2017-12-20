#ifndef READDATA_H_
#define READDATA_H_

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include "Graph.h"

using namespace std;

class ReadData {
	Graph *g;
	string fileName;
	int N, C;
	void readUpperTriangularMatrix(ifstream& in);
	void readRCCPEdgesList(ifstream& in);
public:
	ReadData(string fileName);
	Graph& read();
};

#endif /* READDATA_H_ */
