#include "preprocess.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <unordered_set>

using namespace std;

/*
1. Thống nhất index: duyệt từ 0 cho tất cả các thành phần: sinh viên, exam, phòng, time slot
2. Lưu ý: exam_and_infor chứa MÃ LỚP HỌC PHẦN, còn lại các thành phần khác chứa
exam đều là lớp học phần
*/

/*
exam_input: set chứa tất cả sinh viên đọc từ file csv
student_input: set chứa tất cả exam đọc từ file csv

student_and_infor: mã sinh viên + tên sinh viên tương ứng
student_and_exam: map chứa id sinh viên + môn thi của sv đó

exam_and_infor: mã học phần + tên học phần tương ứng
exam_and_student: map chứa id exam + sinh viên thi môn đó

output_student_and_exam: map chứa id sinh viên + set id (số) môn thi cuả sinh viên đó
output_exam_and_student: map chứa id môn thi + id sinh viên thi môn đó

encoding_exam, encoding_student: map chứa id sinh viên và số thứ tự tương ứng, từ 1
decoding_exam, decoding_student: map chứa thứ tự sinh viên và exam cùng với id tương ứng
*/

Process::Process(string instanceFile) {
	process_input(instanceFile);
}

void Process::process_input(string inputFile)
{
	ifstream instream(inputFile);
	if (!instream.is_open()) {
		cout << "Unable to open file\n";
		exit(0);
	}

	set<string> exam_input, student_input;
	map<string, set<string>> student_and_exam;
	map<string, set<string>> exam_and_student;
	map<string, string> student_and_infor;
	map<string, string> exam_and_infor;

	// Read file
	string input;
	int input_line = 5047; cout << "Number of input line: " << input_line << endl;
	getline(instream, input);
	for (int i = 0; i < input_line; i++) {
		getline(instream, input);
		vector<string> lines;
		stringstream ss(input);
		string line;

		while (getline(ss, line, ',')) {
			lines.push_back(line);
		}

		if (lines[4] == "TXGDTC0003" || lines[4] == "TXGDTC0001" || lines[4] == "TXGDTC0004") {
			continue;
		}

		string student_name = lines[2] + " " + lines[3];

		exam_input.insert(lines[5]);
		student_input.insert(lines[1]);

		student_and_exam[lines[1]].insert(lines[5]);
		exam_and_student[lines[5]].insert(lines[1]);

		student_and_infor[lines[1]] = student_name;
		exam_and_infor[lines[4]] = lines[6];
	}

	/*for (auto x : exam_input) {
		cout << x << " ";
	}*/

	/*for (auto it1 : student_and_exam) {
		cout << it1.first << ": ";
		for (auto it2 : x.second) {
			cout << it2 << " ";
		}
		cout << endl;
	}*/

	// Encoding exam and student to the respective number.
	map <string, int> encoding_exam;
	map <string, int> encoding_student;

	int c1 = 0, c2 = 0;
	for (auto it : exam_input) {
		encoding_exam.insert({ it, c1++ });
	}

	for (auto it : student_input) {
		encoding_student.insert({ it, c2++ });
	}

	// Decoding exam and students to their respective ID:
	map <int, string> decoding_exam;
	map <int, string> decoding_student;
	int c3 = 0, c4 = 0;

	for (auto it : exam_input) {
		decoding_exam.insert({ c3++, it });
	}

	for (auto it : student_input) {
		decoding_student.insert({ c4++, it });
	}

	// List of students and their respective exam
	map<int, set<int>> output_student_and_exam;
	for (auto it1 : student_and_exam) {
		for (auto it2 : it1.second) {
			output_student_and_exam[encoding_student[it1.first]].insert(encoding_exam[it2]);
		}
	}

	// List of exams its respective student (to make the conflict matrix)
	map<int, set<int>> output_exam_and_student;
	for (auto it1 : exam_and_student) {
		for (auto it2 : it1.second) {
			output_exam_and_student[encoding_exam[it1.first]].insert(encoding_student[it2]);
		}
	}

	// Print
	/*for (auto it1 : output_exam_and_student) {
		cout << it1.first << ": ";
		for (auto it2 : it1.second) {
			cout << it2 << " ";
		}
		cout << endl;
	}*/

	int num_exam = exam_input.size();
	int num_student = student_input.size();
	int num_room = 15;
	int num_slot = 12;
	int num_course = exam_and_infor.size();

	cout << "Number of exam: " << num_exam << "\n" << "Number of student: " << num_student 
		<< "\nNumber of course: " << num_course << endl;

	// Write file
	cout << "\nWriting File";
	ofstream outstream("input.txt", ofstream::trunc);

	outstream << num_exam << " " << num_student << " " << num_room << " " << num_slot << " " << num_course << endl;

	for (int i = 0; i < num_room; i++) {
		if (i != num_room - 1) {
			outstream << 40 << " ";
		}
		else outstream << 40;
	}
	
	outstream << endl;

	for (auto it1 : output_exam_and_student) {
		outstream << it1.first << " " << it1.second.size() << endl;
	}

	for (auto it : exam_and_infor) {
		outstream << it.first << ":" << it.second << endl;
	}

	for (auto it : student_and_infor) {
		outstream << it.first << ":" << it.second << endl;
	}

	for (auto it : decoding_exam) {
		outstream << it.first << " " << it.second << endl;
	}

	for (auto it : decoding_student) {
		outstream << it.first << " " << it.second << endl;
	}

	cout << "\nDone Writing" << endl;
	outstream.close();
}