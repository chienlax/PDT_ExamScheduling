#include "preprocess.h"
#include "instance.h"
#include "solver.h"
#include <iostream>

//main <tham so formulation> <ten file cvs> <ten file input> <ten file output>
//using namespace std;
//int main(int argc, char* argv[]) {
//
//	cout << argv[1] << " " << argv[2] << " " << argv[3] << " " << argv[4] << endl;
//
//	Process Process(argv[2]);
//	Instance instance(argv[3]);
//	Solver solver(&instance);
//
//	
//	switch(stoi(argv[1])) {
//		case 1: solver.fourIndexFormular(argv[4]);
//		case 2: solver.threeIndexFormular(argv[4]);
//		case 3: solver.Simple3DModel(argv[4]);
//			break;
//		default:
//			cout << "Wrong parameter" << endl;
//	}
//}

using namespace std;

int main() {
	Process Process("FinalData1/HCM_2314-2648.csv");
	Instance instance("input.txt");
	Solver solver(&instance);
	solver.Simple3DModel();
}