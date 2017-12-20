#include "Util.h"

bool existX(int i, int j, int f, int V){
	return i >= f && j >= f && f < V-2;
}

void printResult(vector<vector<int> >& cycles, int trivialWeight, long executionTime){

		cout << "CYCLES: " << cycles.size() << endl;

		int trivial = 0, nonTrivial = 0;

		for(unsigned int i = 0; i < cycles.size(); i++){
			if(cycles[i].size() > 1)
				nonTrivial++;
			else
				trivial++;
		}

		cout << "NON_TRIVIAL: " << nonTrivial << endl;
		cout << "TRIVIAL: " << trivial << endl;
		cout << "OBJ: " << (nonTrivial + trivial*trivialWeight) << endl;
		cout << endl << "TIME: " << executionTime << endl << endl;

}
