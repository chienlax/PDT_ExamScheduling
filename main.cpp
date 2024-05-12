#include "preprocess.h"
#include "instance.h"
#include "solver.h"

using namespace std;

int main() {
	Process Process("HCM.csv");
	Instance instance("input.txt");
	Solver solver(&instance);
	solver.Test();
}