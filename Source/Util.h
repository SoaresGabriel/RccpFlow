#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <vector>

using namespace std;

bool existX(int i, int j, int f, int V);
void printResult(string instance, vector<vector<int> >& cycles, int trivialWeight, long executionTime, long nodes, bool optimal);

#endif /* UTIL_H_ */
