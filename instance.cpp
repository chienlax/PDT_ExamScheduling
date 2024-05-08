#include "instance.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
using namespace std;

//#include <boost/filesystem.hpp>

Instance::Instance(string instanceFile) {
	read_input(instanceFile);
}

void Instance::read_input(string inputFile)
{
	cout << "--------------------------" << endl;
	cout << "\nStart of instance\n";

	ifstream instream(inputFile);
	if (!instream.is_open()) {
		cout << "Unable to open file\n";
		exit(0);
	}

	// Read file
	string input;

	// First line
	getline(instream, input);
	stringstream ss1(input);
	string line1;
	vector<int> v;
	while (ss1 >> line1) {
		v.push_back(stoi(line1));
	}
	E = v[0]; S = v[1]; R = v[2]; T = v[3], C = v[4];

	//cout << E << " " << S << " " << R << " " << T << " " << C << endl;

	// Second line
	getline(instream, input);
	stringstream ss2(input);
	string line2;

	while (ss2 >> line2) {
		capacity.push_back(stoi(line2));
	}

	// Each student and their respective exam:
	for (int i = 0; i < S; i++) {
		getline(instream, input);
		vector<string> lines;
		stringstream ss(input);
		string line;

		while (getline(ss, line, ':')) {
			lines.push_back(line);
		}
		
		set<int> push_int;
		stringstream ss1(lines[1]);
		string line2;
		
		while (ss1 >> line2) {
			push_int.insert(stoi(line2));
		}
		examOfStudent[i] = push_int;
	}
	
	// The rest of the exam and its number of register
	string input2;
	for (int i = 0; i < E; i++) {
		getline(instream, input2);
		vector<string> lines;
		stringstream ss(input2);
		string line;

		while (ss >> line) {
			lines.push_back(line);
		}
		//cout << lines[0] << " " << lines[1] << endl;

		numberOfStudentInExam[stoi(lines[0])] = stoi(lines[1]);
	}

	// Exam and its name:
	string input3;
	for (int i = 0; i < C; i++) {
		getline(instream, input3);
		vector<string> lines;
		stringstream ss(input3);
		string line;

		while (getline(ss, line, ':')) {
			lines.push_back(line);
		}

		exam_and_infor[lines[0]] = lines[1];
	}
	
	// Students and their name
	string input4;
	for (int i = 0; i < S; i++) {
		getline(instream, input4);
		vector<string> lines;
		stringstream ss(input4);
		string line;

		while (getline(ss, line, ':')) {
			lines.push_back(line);
		}

		student_and_infor[lines[0]] = lines[1];
	}

	// Decoding exam
	string input5;
	for (int i = 0; i < E; i++) {
		getline(instream, input5);
		vector<string> lines;
		stringstream ss(input5);
		string line;

		while (ss >> line) {
			lines.push_back(line);
		}

		decoding_exam[stoi(lines[0])] = lines[1];
	}

	// Decoding student
	string input6;
	for (int i = 0; i < S; i++) {
		getline(instream, input6);
		vector<string> lines;
		stringstream ss(input6);
		string line;

		while (ss >> line) {
			lines.push_back(line);
		}

		decoding_student[stoi(lines[0])] = lines[1];
	}
}