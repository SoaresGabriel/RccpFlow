#include <iostream>
#include <cstdio>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <memory>

using namespace std;

string execute(const char *cmd) {
    array<char, 128> buffer{};
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

int main() {

	cout << fixed;
	cout.precision(2);

	vector<int> vertexNumber = { 20, 30, 40, 50 };
	vector<int> nm = { 7, 29, 51, 79, 127 };

	vector<double> ntrivialOtimo = { 0, 1, 2, 2.2, 2.6, 3.4, 2.2, 3.8, 3.6, 2.6,
			3.2, 2.8, 2.6, 4.8, 4.4, 3.2, 6, 5.2, 3.6, 3.6, 3.8, 3.4, 6.2, 6, 4,
			8.6, 6.2, 3.8, 5.6, 5.4, 5.2, 8, 7.2, 5.6, 10.2, 8.2, 4.4 };
	vector<double> trivialOtimo = { 0, 17, 11.6, 8.4, 12.2, 6.4, 4.6, 6.2, 3.2,
			2.2, 18.6, 16.6, 12.8, 13.4, 8.6, 6.4, 9.2, 2.8, 1.8, 27, 21, 17,
			18.8, 9.8, 7, 6, 2.6, 1.4, 30.2, 25, 18.2, 17.4, 11.2, 7.2, 9.6,
			4.6, 2.2 };

	char buffer[512];

	int id = 0;

	ofstream outf("Output/relatorio.txt", ios::out);
	//cout << "ID \tn \tm \tl \tcycles \tdesv \tnon-trvl \tdesv \ttrvl \tdesv \tObj \tdesv \tt(ms)" << endl;
	cout << "ID    n     m     l     cycles  non-trvl  trvl      Obj     t(s)   desv      nodes  optimal" << endl;
	outf << "ID    n     m     l     cycles  non-trvl  trvl      Obj     t(s)   desv      nodes  optimal" << endl;

	for (int i = 0; i < 4; i++) {
		int n = vertexNumber[i];

		for (int j = 1; j <= 3; j++) {

			auto m = static_cast<int>(ceil((n * (n - 1) / 2.0) * j / 10.0 + n));
			double logm = log2(m);

			for (int k = 0; k < 3; k++) {
				auto l = static_cast<int>(k == 0 ? ceil(logm / 2) : ceil(k * logm));
				id++;

				double totalCycles = 0, totalNonTrivial = 0, totalTrivial = 0,
						totalObj = 0, totalTime = 0, totalNodes = 0;
				int optimal = 0;

				for (int a = 0; a < 5; a++) {

					sprintf(buffer,
							"./rccpFlow Instances/Rand_%d_%d_%d_%d.rnd", n, m,
							nm[a], l);

					//cout << buffer << endl;

					stringstream stream(execute(buffer));
					string s;

					int cycles, nonTrivial, trivial, obj, time, nodes;
					int itOptimal;

					while (s != "CYCLES:") {
						stream >> s;
					}
					stream >> cycles;

					while (s != "NON_TRIVIAL:") {
						stream >> s;
					}
					stream >> nonTrivial;

					while (s != "TRIVIAL:") {
						stream >> s;
					}
					stream >> trivial;

					while (s != "OBJ:") {
						stream >> s;
					}
					stream >> obj;

					while (s != "TIME:") {
						stream >> s;
					}
					stream >> time;

					while (s != "NODES:") {
						stream >> s;
					}
					stream >> nodes;

					while (s != "OPTIMAL:") {
						stream >> s;
					}
					stream >> itOptimal;

					totalCycles += cycles;
					totalNonTrivial += nonTrivial;
					totalTrivial += trivial;
					totalObj += obj;
					totalTime += time;
					totalNodes += nodes;
					optimal += itOptimal;
				}

				totalCycles /= 5;
				totalNonTrivial /= 5;
				totalTrivial /= 5;
				totalObj /= 5;
				totalTime /= 5;
				totalNodes /= 5;

				double optnTrivial = ntrivialOtimo[id];
				double optTrivial = trivialOtimo[id];
				double optObj = optnTrivial + optTrivial * ((n / 3) + 1);
				
				double desvioObj = ((totalObj / optObj) - 1) * 100;
				
				sprintf(buffer,
						"%-5d %-5d %-5d %-5d %-8.2f %-8.2f %-8.2f %-8.2f %-5.2f %+5.1f%% %8.2f %5d\n",
						id, n, m, l, totalCycles, totalNonTrivial, totalTrivial,
						totalObj, totalTime/1000, desvioObj, totalNodes, optimal);

				cout << buffer;
				outf << buffer;

				outf.flush();
			}

		}

	}
	outf.close();
	return 0;
}
