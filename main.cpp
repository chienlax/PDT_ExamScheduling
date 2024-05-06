#include "preprocess.h"
#include "instance.h"
#include "solver.h"

using namespace std;

int main() {
	
	Process Process("DSSV.csv");
	Instance instance("input.txt");
	Solver solver(&instance);
	solver.run();
}