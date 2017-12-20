#include "ReadData.h"

ReadData::ReadData(string fileName) : g(NULL), fileName(fileName), N(0), C(0) {

}

Graph& ReadData::read(){
	
	ifstream in(fileName.c_str(), ios::in);
	
	if(!in){
		std::cout << "Falha ao abrir o arquivo!" << endl;
		exit(1);
	}
	
	string::size_type point = fileName.find_last_of(".");
	string fileExtension = fileName.substr(point + 1, fileName.size() - point - 1);

	if(fileExtension == "rnd"){
		readRCCPEdgesList(in);
	}else{
		readUpperTriangularMatrix(in);
	}

	g->reduce();

	in.close();

	return *g;
}

void ReadData::readUpperTriangularMatrix(ifstream& in){

	in >> N;
	in >> C;
	g = new Graph(N, C);

	int color;

	// Leitura da matriz de adjacencia
	for (int i = 0; i < N; i++) {
		for (int j = i+1; j < N; j++) {
			in >> color;
			g->insert(i, j, color);
		}
	}

}

void ReadData::readRCCPEdgesList(ifstream& in){
	string s = "";

	while(s != "NUMBER_VERTICES:"){
		in >> s;
	}
	in >> N;

	int edges;
	while(s != "NUMBER_EDGES:"){
		in >> s;
	}
	in >> edges;

	while(s != "NUMBER_COLORS:"){
		in >> s;
	}
	in >> C;

	g = new Graph(N, C);

	while(s != "LIST_EDGE_(source_destination_color)"){
		in >> s;
	}

	int v, w, c;
	for(int i = 0; i < edges; i++){
		in >> v >> w >> c;

		g->insert(v-1, w-1, c-1);

	}

}
