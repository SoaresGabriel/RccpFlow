#include "Util.h"
#include <fstream>

bool existX(int i, int j, int f, int V){
	return i >= f && j >= f && f < V-2;
}

string getInstanceName(string instance){

	string::size_type loc = instance.find_last_of(".", instance.size() );
	string::size_type loc2 = instance.find_last_of("/", instance.size() );
	if(loc2 == string::npos)
			loc2 = instance.find_last_of("\\", instance.size() );

	string instanceName;
	if (loc != string::npos) {
		instanceName.append(instance, loc2+1, loc-loc2-1 );
	}
	else {
		instanceName.append(instance, loc2+1, instance.size() );
	}

	return instanceName;
}

void printResult(string instance, vector<vector<int> >& cycles, int trivialWeight, long executionTime, long nodes, bool optimal){

		cout << "CYCLES: " << cycles.size() << endl;

		int trivial = 0, nonTrivial = 0;

		for(unsigned int i = 0; i < cycles.size(); i++){
			if(cycles[i].size() > 1)
				nonTrivial++;
			else
				trivial++;
		}

		int obj = (nonTrivial + trivial*trivialWeight);

		cout << "NON_TRIVIAL: " << nonTrivial << endl;
		cout << "TRIVIAL: " << trivial << endl;
		cout << "OBJ: " << obj << endl;
		cout << "TIME: " << executionTime << endl;

		cout << "NODES: " << nodes << endl;
		cout << "OPTIMAL: " << optimal << endl;

		cout << endl;

		/* imprime resultado no arquivo */
		string outFile = "./out/" + getInstanceName(instance) + ".out";
		ofstream outf(outFile, ios::out);

		outf << "Cover Weight: " << obj << endl;
		outf << "Non Trivial Cycles: " << nonTrivial << endl;
		outf << "Trivial Cycles: " << trivial << endl;
		outf << "Nodes: " << nodes << endl;
		outf << "Optimal: " << (optimal ? "true" : "false") << endl;
		outf << "Execution time: " << executionTime << " ms" << endl;

		outf << "Cycles: " << endl;
		for (unsigned int i = 0; i < cycles.size(); i++) {
			for (unsigned int j = 0; j < cycles[i].size()-1; j++) {
				outf << cycles[i][j] << " -> ";
			}
			outf << cycles[i][cycles[i].size()-1] << endl;
		}


}
