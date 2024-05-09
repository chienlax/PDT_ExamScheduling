#pragma once

#ifndef SOLVER_H
#define SOLVER_H

#include <iostream>
#include "instance.h"
#include <ilcplex/ilocplex.h>

using namespace std;

ILOSTLBEGIN

class Solver {
public:
    Solver(Instance*);
    Instance* instance;

    int E, S, R, T, C;
    vector<int> capacity;
    map<int, int> numberOfStudentInExam;
    map<int, set<int>> examOfStudent;

    map<string, string> exam_and_infor;
    map<string, string> student_and_infor;

    map<int, string> decoding_exam;
    map<int, string> decoding_student;

    void run();
    void test();
};

#endif