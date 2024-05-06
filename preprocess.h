#pragma once

#ifndef ES_PREPROCESS_H
#define ES_PREPROCESS_H
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <string>
#include <set>
using namespace std;

class Process {
public:
    Process(string instanceFile);

private:
    void process_input(const string inputFile);
};

#endif //ES_PREPROCESS_H