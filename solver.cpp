#include "solver.h"
#include <iostream>

// Overleaf: https://www.overleaf.com/project/6624e326e544fed56f8bf846

using namespace std;

Solver::Solver(Instance* instance) : instance(instance) {
	E = instance->E;
	S = instance->S;
	R = instance->R;
	T = instance->T;
	C = instance->C;
	capacity = instance->capacity;
	numberOfStudentInExam = instance->numberOfStudentInExam;
	examOfStudent = instance->examOfStudent;
	exam_and_infor = instance->exam_and_infor;
	student_and_infor = instance->student_and_infor;
	decoding_exam = instance->decoding_exam;
	decoding_student = instance->decoding_student;

	void run();
}

void Solver::fourIndexFormular() {
	cout << "\n-----------------------\n\nStart of solver\n\n";

	IloEnv env;
	IloModel model(env);

	// Declare Variable:
	IloArray<IloArray<IloNumVarArray>> x_jrt(env, E); // 1 nếu môn j được thi tại phòng r và time slot t
	for (int j = 0; j < E; j++) {
		x_jrt[j] = IloArray<IloNumVarArray>(env, R);
		for (int r = 0; r < R; r++) {
			x_jrt[j][r] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				x_jrt[j][r][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "x_jrt(" + to_string(j) + "," + to_string(r) + "," + to_string(t) + ")";
				x_jrt[j][r][t].setName(name.c_str());
			}
		}
	}

	IloArray<IloNumVarArray> x_jt(env, E); // 1 nếu môn j được thi ở time slot t
	for (int j = 0; j < E; j++) {
		x_jt[j] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			x_jt[j][t] = IloNumVar(env, 0, 1, ILOINT);
			string name = "x_jt(" + to_string(j) + "," + to_string(t) + ")";
			x_jt[j][t].setName(name.c_str());
		}
	}

	IloArray<IloArray<IloArray<IloNumVarArray>>> w_ijrt(env, S); // sinh viên i thi môn j ở phòng r time slot t
	for (int i = 0; i < S; i++) {
		w_ijrt[i] = IloArray<IloArray<IloNumVarArray>>(env, E);
		for (int j = 0; j < E; j++) {
			w_ijrt[i][j] = IloArray<IloNumVarArray>(env, R);
			for (int r = 0; r < R; r++) {
				w_ijrt[i][j][r] = IloNumVarArray(env, T);
				for (int t = 0; t < T; t++) {
					w_ijrt[i][j][r][t] = IloNumVar(env, 0, 1, ILOINT);
					string name = "w_ijrt(" + to_string(i) + "," + to_string(j) + "," +
						to_string(r) + "," + to_string(t) + ")";
					w_ijrt[i][j][r][t].setName(name.c_str());
				}
			}
		}
	}


	// sinh viên i thi môn j ở time slot t
	IloArray<IloArray<IloNumVarArray>> w_ijt(env, S);
	for (int i = 0; i < S; i++) {
		w_ijt[i] = IloArray<IloNumVarArray>(env, E);
		for (int j = 0; j < E; j++) {
			w_ijt[i][j] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				w_ijt[i][j][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "w_ijt(" + to_string(i) + "," + to_string(j) + "," + to_string(t) + ")";
				w_ijt[i][j][t].setName(name.c_str());
			}
		}
	}

	IloArray<IloNumVarArray> w_it(env, S);
	for (int i = 0; i < S; i++) {
		w_it[i] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			w_it[i][t] = IloNumVar(env, 0, 1, ILOINT);
			string name = "w_it(" + to_string(i) + "," + to_string(t) + ")";
			w_it[i][t].setName(name.c_str());
		}
	}

	IloArray<IloArray<IloNumVarArray>> wi_pq(env, S); // sinh viên i thi ở trong các time slot p và q
	for (int i = 0; i < S; i++) {
		wi_pq[i] = IloArray<IloNumVarArray>(env, T);
		for (int p = 0; p < T; p++) {
			wi_pq[i][p] = IloNumVarArray(env, T);
			for (int q = p; q < T; q++) {
				wi_pq[i][p][q] = IloNumVar(env, 0, 1, ILOINT);
				string name = "wi_pq[" + to_string(i) + ",(" + to_string(p) + "," + to_string(q) + ")]";
				wi_pq[i][p][q].setName(name.c_str());
			}
		}
	}

	IloArray<IloArray<IloNumVarArray>> zj_pq(env, E); // môn j được tổ chức liên tiếp từ time-slot p đến time slot q
	for (int j = 0; j < E; j++) {
		zj_pq[j] = IloArray<IloNumVarArray>(env, T);
		for (int p = 0; p < T; p++) {
			zj_pq[j][p] = IloNumVarArray(env, T);
			for (int q = p; q < T; q++) {
				zj_pq[j][p][q] = IloNumVar(env, 0, 1, ILOINT);
				string name = "zj_pq[" + to_string(j) + ",(" + to_string(p) + "," + to_string(q) + ")]";
				zj_pq[j][p][q].setName(name.c_str());
			}
		}
	}

	IloArray<IloArray<IloArray<IloNumVarArray>>> Xij_rt(env, E); // hai môn i và j cùng thi tại phòng (r, t)
	for (int j1 = 0; j1 < E; j1++) {
		Xij_rt[j1] = IloArray<IloArray<IloNumVarArray>>(env, E);
		for (int j2 = 0; j2 < E; j2++) {
			Xij_rt[j1][j2] = IloArray<IloNumVarArray>(env, R);
			for (int r = 0; r < R; r++) {
				Xij_rt[j1][j2][r] = IloNumVarArray(env, T);
				for (int t = 0; t < T; t++) {
					Xij_rt[j1][j2][r][t] = IloNumVar(env, 0, 1, ILOINT);
					string name = "Xij_rt[" + to_string(j1) + "," + to_string(j2)
						+ ",(" + to_string(r) + "," + to_string(t) + ")]";
					Xij_rt[j1][j2][r][t].setName(name.c_str());
				}
			}
		}
	}

	for (int i = 0; i < S; i++) {
		for (int j = 0; j < E; j++) {
			if (examOfStudent[i].count(j) == 0) {
				IloExpr expr(env);
				for (int r = 0; r < R; r++) {
					for (int t = 0; t < T; t++) {
						w_ijrt[i][j][r][t].setBounds(0, 0);
					}
				}
			}
		}
	}

	// Constraints:
	// (1) sinh viên i tham gia không quá một môn ở thời điểm t với mọi cặp (i, t)
	for (int i = 0; i < S; i++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr1(env);
			for (auto j : examOfStudent[i]) {
				for (int r = 0; r < R; r++) {
					expr1 += w_ijrt[i][j][r][t];
				}
			}
			model.add(expr1 <= 1).setName("C1");
		}
	}

	// (2) số sinh viên tham gia phòng thi r thỏa mãn điều kiện giới hạn 
	// số sinh viên của phòng tại thời điểm t bất kỳ
	for (int r = 0; r < R; r++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr2(env);
			for (int i = 0; i < S; i++) {
				for (auto j : examOfStudent[i]) {
					expr2 += w_ijrt[i][j][r][t];
				}
			}
			model.add(expr2 <= capacity[r]).setName("C2");
		}
	}

	// (3) sinh viên i phải thi môn j ở phòng r và ở tại thời điểm t
	// nào đó với mọi cặp (i, j) mà sinh viên i thi môn j
	// Với mọi sinh viên, tại 1 thời điểm, chỉ thi 1 môn tại 1 phòng nhất định

	for (int i = 0; i < S; i++) {
		for (auto j : examOfStudent[i]) { // j chỉ chạy trên những exam của sinh viên i
			IloExpr expr3(env);
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					expr3 += w_ijrt[i][j][r][t];
				}
			}
			model.add(expr3 == 1).setName("C3");
		}
	}

	// (4) sinh viên chỉ thi tại phòng mà nó được mở; và môn thi được tổ chức ở timeslot đó

	for (int i = 0; i < S; i++) {
		for (auto j : examOfStudent[i]) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					model.add(w_ijrt[i][j][r][t] <= x_jrt[j][r][t]).setName("C4.1");
				}
			}
		}
	}

	for (int j = 0; j < E; j++) {
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				model.add(x_jrt[j][r][t] <= x_jt[j][t]).setName("C4.2");
			}
		}
	}

	// (5) phải có ít nhất 1 phòng được sử dụng để thi môn j ở thời điểm t
	for (int j = 0; j < E; j++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr4(env);
			for (int r = 0; r < R; r++) {
				expr4 += x_jrt[j][r][t];
			}
			model.add(expr4 >= x_jt[j][t]).setName("C5");
		}
	}

	// (6) phải có ít nhất một sinh viên i thi môn j ở thời điểm t

	/*IloRange cons(env, lb, x, ub, "");
	model.add(cons)*/
	for (int j = 0; j < E; j++) {
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				IloExpr expr5(env);
				for (int i = 0; i < S; i++) {
					expr5 += w_ijrt[i][j][r][t];
				}
				model.add(expr5 >= x_jrt[j][r][t]).setName("C6");
			}
		}
	}

	// (7) số phòng mở phục vụ môn học j phải thỏa mãn số sinh
	// viên cho môn này
	for (int j = 0; j < E; j++) {
		IloExpr expr6(env);
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				expr6 += (x_jrt[j][r][t] * capacity[r]);
			}
		}
		model.add(expr6 >= numberOfStudentInExam[j]).setName("C7");
	}

	// (8) Constraint chia phòng
	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					model.add(Xij_rt[j1][j2][r][t] <= x_jrt[j1][r][t]).setName("C8.1");
				}
			}
		}
	}

	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					model.add(Xij_rt[j1][j2][r][t] <= x_jrt[j2][r][t]).setName("C8.2");
				}
			}
		}
	}

	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					model.add(Xij_rt[j1][j2][r][t] >= x_jrt[j1][r][t] + x_jrt[j2][r][t] - 1).setName("C8.3");
				}
			}
		}
	}

	// (9)  mỗi sinh viên sẽ chỉ chọn thi trong một khoảng time
	// slot[p, q] duy nhất
	for (int i = 0; i < S; i++) {
		IloExpr expr7(env);
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				expr7 += wi_pq[i][p][q];
			}
			model.add(expr7 == 1).setName("C9");
		}
	}

	// (10) đoạn (p,q) nào đó dc chọn thì có một môn sẽ thi ở thời điểm 
	// p với p là thời điểm bắt đầu của của mọi môn thi của sinh viên i
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr8(env);
			for (int q = p; q < T; q++) {
				expr8 += wi_pq[i][p][q];
			}
			model.add(w_it[i][p] >= expr8);
		}
	}

	// (11)  đoạn (p,q) nào đó dc chọn thì có một môn sẽ thi ở thời điểm 
	// q (với q là thời điểm kết thúc mọi môn thi của sv i)
	for (int i = 0; i < S; i++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr9(env);
			for (int p = 0; p <= q; p++) {
				expr9 += wi_pq[i][p][q];
			}
			model.add(w_it[i][q] >= expr9);
		}
	}

	// (12)  p là thời điểm mà bắt đầu các
	// môn thi của sv i thì sv này ko thi bất kỳ môn nào trc thời điểm p
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int t = 0; t < p; t++) {
				IloExpr expr10(env);
				for (int q = p; q < T; q++) {
					expr10 += wi_pq[i][p][q];
				}
				expr10 += w_it[i][t];
				model.add(expr10 <= 1).setName("Constraint 12");
			}
		}
	}

	// (13) q là thời điểm mà kết thúc các
	// môn thi của sv i thì sv này ko thi bất kỳ môn nào sau thời điểm q
	for (int i = 0; i < S; i++) {
		for (int q = 0; q < T; q++) {
			for (int t = q + 1; t < T; t++) {
				IloExpr expr11(env);
				for (int p = 0; p <= q; p++) {
					expr11 += wi_pq[i][p][q];
				}
				expr11 += w_it[i][t];
				model.add(expr11 <= 1).setName("Constraint 13");
			}
		}
	}


	// (14) với mọi bộ (p,q) mà q-p+1 < số lượng môn khác nhau mà sinh
	// viên i tham gia thi
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (q - p + 1 < examOfStudent[i].size()) {
					wi_pq[i][p][q].setBounds(0, 0);
				}
			}
		}
	}


	// (15) với mọi j; có mọt khoảng thời gian p..q được chọn để tổ
	// chức thi môn thi j
	for (int j = 0; j < E; j++) {
		IloExpr expr10(env);
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				expr10 += zj_pq[j][p][q];
			}
		}
		model.add(expr10 == 1).setName("Constraint 15");
	}

	// (16)  nếu đoạn p..q được tổ chức để thi môn j(wj_pq = 1) 
	// thì bất cứ slot nào cũng phải có môn j thi (hay xjt = 1)
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				for (int t = p; t < q; t++) {
					model.add(x_jt[j][t] >= zj_pq[j][p][q]).setName("Constraint 16");
				}
			}
		}
	}

	// (17) p là thời điểm mà bắt đầu các
	// môn thi của môn i thì môn này ko thi bất kỳ môn nào trc thời điểm p
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr12(env);
			for (int q = p; q < T; q++) {
				expr12 += zj_pq[j][p][q];
			}
			for (int t = 0; t < p; t++) {
				model.add(expr12 + x_jt[j][t] <= 1).setName("Constraint 17");
			}
		}
	}

	//------------------- Phụ
	// (18) 1 môn không thi trong 2 ngày
	/*for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (q >= 7 || p >= 6)
			}
		}
	}*/


	/*for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int t = 0; t < p; t++) {
				IloExpr expr12(env);
				for (int q = p; q < T; q++) {
					expr12 += zj_pq[j][p][q];
				}
				expr12 += x_jt[j][t];
				model.add(expr12 <= 1).setName("Constraint 17");
			}
		}
	}*/

	// Thay objective thành constraint 

	// (18) q là thời điểm mà kết thúc các
	// môn thi của môn i thì môn này ko thi bất kỳ môn nào sau thời điểm q
	for (int j = 0; j < E; j++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr13(env);
			for (int p = 0; p <= q; p++) {
				expr13 += zj_pq[j][p][q];
			}
			for (int t = q + 1; t < T; t++) {
				model.add(expr13 += x_jt[j][t] <= 1).setName("Constraint 18");
			}
		}
	}

	// Objective function:
	IloExpr obj1(env);
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				obj1 += zj_pq[j][p][q] * (q - p);
			}
		}
	}
	obj1 *= 1000;

	IloExpr obj2(env);
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (p >= 7 || q <= 6)
					obj2 += wi_pq[i][p][q] * (q - p);
			}
		}
	}
	obj2 *= 1000;

	IloExpr obj3(env);
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (p <= 6 && q >= 7)
					obj3 += wi_pq[i][p][q] * (q - p);
			}
		}
	}
	obj3 *= 1000;

	IloExpr obj4(env);
	for (int j = 0; j < E; j++) {
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				obj4 += x_jrt[j][r][t];
			}
		}
	}
	obj4 *= 10;

	IloExpr obj5(env);
	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					obj5 += Xij_rt[j1][j2][r][t];
				}
			}
		}
	}
	obj5 *= 100000;

	model.add(IloMinimize(env, obj1 + obj2 + obj3 + obj5 + obj4));

	cout << "\nCplex starts here\n";

	IloCplex cplex(model);
	if (!cplex.solve()) {
		cerr << "Failed to solve the problem" << endl;
		throw(-1);
	}

	cout << "Solved\n";

	double objValue = cplex.getObjValue();
	cout << "Objective Value: " << objValue << endl;

	cout << "Solution status: " << cplex.getStatus() << endl;

	ofstream outFile("output.txt", ofstream::trunc);

	cout << "\n\nResult starts here:\n\n";
	cout << "Exam with its respective room and timeslot:\n";

	for (int j = 0; j < E; j++) {
		string examClass = decoding_exam[j];
		stringstream ss(examClass);
		string line; vector<string> lines;
		while (getline(ss, line, '.')) {
			lines.push_back(line);
		}
		string examName = exam_and_infor[lines[1]];
		cout << j + 1 << ": " << examClass << ": " << examName << endl;

		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				if (cplex.getValue(x_jrt[j][r][t])) {
					cout << "Room: " << r << " | " << "Time Slot: " << t << endl;
				}
			}
		}
	}

	cout << "\n\nStudent and their time exam, room, time slot\n";

	for (int i = 0; i < S; i++) {
		string studentID = decoding_student[i];
		string studentName = student_and_infor[studentID];

		cout << i + 1 << ": " << studentID << ": " << studentName << endl;
		for (int j : examOfStudent[i]) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					if (cplex.getValue(w_ijrt[i][j][r][t])) {

						string examClass = decoding_exam[j];
						stringstream ss(examClass);
						string line; vector<string> lines;
						while (getline(ss, line, '.')) {
							lines.push_back(line);
						}
						string examName = exam_and_infor[lines[1]];

						cout << "Exam: " << examName << " | " << "Room: " << r << " | " << "Time Slot: " << t << endl;
					}
				}
			}
		}
		cout << endl;
	}

	int count = 0;

	/*for (int r = 0; r < R; r++) {
		cout << "Room " << r << ":" << endl;
		for (int t = 0; t < T; t++) {
			cout << "Time Slot: " << t << endl;
			for (int j = 0; j < E; j++) {
				for (int i = 0; i < S; i++) {
					if (cplex.getValue(w_ijrt[i][j][r][t])) {
						cout << decoding_student[i] << " ";
					}
				}

			}
			cout << endl;
		}
	}*/
}

void Solver::test() {
	cout << "\n-----------------------\n\nStart of tESt solver\n\n";

	IloEnv env;
	IloModel model(env);
	IloCplex cplex(model);
	cplex.setParam(IloCplex::Param::MIP::Tolerances::Integrality, 0);
	vector<int> sS;
	vector<int> sE;
	vector<int> sT;
	vector<int> sR;

	for (int i = 0; i < S; i++) {
		sS.push_back(i);
	}
	for (int i = 0; i < E; i++) {
		sE.push_back(i);
	}
	for (int i = 0; i < T; i++) {
		sT.push_back(i);
	}
	for (int i = 0; i < R; i++) {
		sR.push_back(i);
	}

	vector<vector<int>> conflict_matrix(S, vector<int>(E, 0));
	for (auto it1 : examOfStudent) {
		for (auto it2 : it1.second) {
			conflict_matrix[it1.first][it2] = 1;
		}
	}

	// vector<int> sS;
	IloArray<IloArray<IloBoolVarArray>> w_ijt(env, S);
	for (int i : sS) {
		w_ijt[i] = IloArray<IloBoolVarArray>(env, E);
		for (int j : sE) {
			w_ijt[i][j] = IloBoolVarArray(env, T);
			for (int t : sT) {
				string vname = "w_ijt(" + to_string(i) + "," + to_string(j) + "," + to_string(t) + ")";
				// cout << "created " << vname << endl;
				w_ijt[i][j][t] = IloBoolVar(env);
				w_ijt[i][j][t].setName(vname.c_str());
				if (i == 0 && j == 0 && t == 0) {
					cout << "Created 0 0 0" << endl;
				}
			}
		}
	}

	IloArray<IloArray<IloBoolVarArray>> w_irt(env, S);
	for (int i : sS) {
		w_irt[i] = IloArray<IloBoolVarArray>(env, R);
		for (int r : sR) {
			w_irt[i][r] = IloBoolVarArray(env, T);
			for (int t : sT) {
				string vname = "w_irt(" + to_string(i) + "," + to_string(r) + "," + to_string(t) + ")";
				w_irt[i][r][t] = IloBoolVar(env);
				w_irt[i][r][t].setName(vname.c_str());

			}
		}
	}

	IloArray<IloBoolVarArray> student_at_time(env, S);
	for (int i : sS) {
		student_at_time[i] = IloBoolVarArray(env, T);
		for (int t : sT) {
			string vname = "std_time(" + to_string(i) + "," + to_string(t) + ")";
			student_at_time[i][t] = IloBoolVar(env);
			student_at_time[i][t].setName(vname.c_str());
		}
	}

	// Nếu sinh viên i thi môn j thì phải thi trong 1 time slot t nào đó.
	for (int i : sS) {
		for (int j : sE) {
			if (conflict_matrix[i][j]) {
				cout << "Student " << i << " thi mon " << j << endl;
				IloExpr sum(env);
				for (int t : sT) {
					sum += w_ijt[i][j][t];
					model.add(w_ijt[i][j][t] <= student_at_time[i][t]);
				}
				model.add(sum == 1);
			}
		}
	}

	// Với mỗi time slot t thì sinh viên i chỉ được thi tối đa 1 môn.
	for (int i : sS) {
		for (int t : sT) {
			IloExpr sum(env);
			for (int j : sE) {
				if (conflict_matrix[i][j]) {
					sum += w_ijt[i][j][t];
					// model.add(w_ijt[i][j][t] <= student_at_time[i][t]);
				}
			}
			// model.add(sum <= 1);
			model.add(sum == student_at_time[i][t]);
		}
	}

	// Capacity constraint cho phòng r tại time slot t: tổng số sinh viên <= cap(r)
	for (int r : sR) {
		for (int t : sT) {
			IloExpr sum(env);
			for (int i : sS) {
				sum += w_irt[i][r][t];
			}
			model.add(sum <= capacity[r]);
		}
	}

	// Objective: min số slot nhiều nhất của student?
	IloIntVar objective(env);

	for (int i : sS) {
		for (int q = 0; q < T - 1; q++) {
			for (int p = q + 1; p < T; p++) {
				model.add(objective >= p - q - 1e9 * (2 - student_at_time[i][p] - student_at_time[i][q]));
			}
		}
	}

	model.add(IloMinimize(env, objective));
	cplex.exportModel("cpl.lp");
	cplex.solve();

	// for (int i:sS) {
	// 	for (int j:sE) {
	// 		for (int t:sT) {
	// 			if (cplex.getValue(w_ijt[i][j][t]) == 1) {
	// 				cout << "Student " << i << " thi exam " << j << " tai slot " << t << endl;
	// 			}
	// 		}
	// 	}
	// }
	cout << "Objective = " << cplex.getObjValue() << endl;
}

void Solver::threeIndexFormular() {
	cout << "\n-----------------------\n\nStart of 3D model solver\n\n";

	IloEnv env;
	IloModel model(env);

	// Declare Variable:
	IloArray<IloArray<IloNumVarArray>> x_jrt(env, E); // 1 nếu môn j được thi tại phòng r và time slot t
	for (int j = 0; j < E; j++) {
		x_jrt[j] = IloArray<IloNumVarArray>(env, R);
		for (int r = 0; r < R; r++) {
			x_jrt[j][r] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				x_jrt[j][r][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "x_jrt(" + to_string(j) + "," + to_string(r) + "," + to_string(t) + ")";
				x_jrt[j][r][t].setName(name.c_str());
			}
		}
	}

	IloArray<IloNumVarArray> x_jt(env, E); // 1 nếu môn j được thi ở time slot t
	for (int j = 0; j < E; j++) {
		x_jt[j] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			x_jt[j][t] = IloNumVar(env, 0, 1, ILOINT);
			string name = "x_jt(" + to_string(j) + "," + to_string(t) + ")";
			x_jt[j][t].setName(name.c_str());
		}
	}

	IloArray<IloNumVarArray> x_rt(env, R); // 1 nếu phòng r được thi ở time slot t
	for (int r = 0; r < R; r++) {
		x_rt[r] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			x_rt[r][t] = IloNumVar(env, 0, 1, ILOINT);
			//string name = "x_rt(" + to_string(j) + "," + to_string(t) + ")";
			//x_jt[r][t].setName(name.c_str());
		}
	}

	IloArray<IloArray<IloNumVarArray>> w_irt(env, S); // sinh viên i thi môn j ở phòng r time slot t
	for (int i = 0; i < S; i++) {
		w_irt[i] = IloArray<IloNumVarArray>(env, R);
		for (int r = 0; r < R; r++) {
			w_irt[i][r] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				w_irt[i][r][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "w_irt(" + to_string(i) + "," + to_string(r) + "," + to_string(t) + ")";
				w_irt[i][r][t].setName(name.c_str());
			}
		}
	}

	// sinh viên i thi môn j ở time slot t
	IloArray<IloArray<IloNumVarArray>> w_ijt(env, S);
	for (int i = 0; i < S; i++) {
		w_ijt[i] = IloArray<IloNumVarArray>(env, E);
		for (int j = 0; j < E; j++) {
			w_ijt[i][j] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				w_ijt[i][j][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "w_ijt(" + to_string(i) + "," + to_string(j) + "," + to_string(t) + ")";
				w_ijt[i][j][t].setName(name.c_str());
			}
		}
	}

	IloArray<IloNumVarArray> w_it(env, S);
	for (int i = 0; i < S; i++) {
		w_it[i] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			w_it[i][t] = IloNumVar(env, 0, 1, ILOINT);
			string name = "w_it(" + to_string(i) + "," + to_string(t) + ")";
			w_it[i][t].setName(name.c_str());
		}
	}

	IloArray<IloArray<IloNumVarArray>> wi_pq(env, S); // sinh viên i thi ở trong các time slot p và q
	for (int i = 0; i < S; i++) {
		wi_pq[i] = IloArray<IloNumVarArray>(env, T);
		for (int p = 0; p < T; p++) {
			wi_pq[i][p] = IloNumVarArray(env, T);
			for (int q = p; q < T; q++) {
				wi_pq[i][p][q] = IloNumVar(env, 0, 1, ILOINT);
				string name = "wi_pq[" + to_string(i) + ",(" + to_string(p) + "," + to_string(q) + ")]";
				wi_pq[i][p][q].setName(name.c_str());
			}
		}
	}

	IloArray<IloArray<IloNumVarArray>> zj_pq(env, E); // môn j được tổ chức liên tiếp từ time-slot p đến time slot q
	for (int j = 0; j < E; j++) {
		zj_pq[j] = IloArray<IloNumVarArray>(env, T);
		for (int p = 0; p < T; p++) {
			zj_pq[j][p] = IloNumVarArray(env, T);
			for (int q = p; q < T; q++) {
				zj_pq[j][p][q] = IloNumVar(env, 0, 1, ILOINT);
				string name = "zj_pq[" + to_string(j) + ",(" + to_string(p) + "," + to_string(q) + ")]";
				zj_pq[j][p][q].setName(name.c_str());
			}
		}
	}

	IloArray<IloArray<IloNumVarArray>> Xij_t(env, E); // hai môn i và j cùng thi tại phòng (r, t)
	for (int j1 = 0; j1 < E; j1++) {
		Xij_t[j1] = IloArray<IloNumVarArray>(env, E);
		for (int j2 = 0; j2 < E; j2++) {
			Xij_t[j1][j2] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				Xij_t[j1][j2][t] = IloNumVar(env, 0, 1, ILOINT);
			}
		}
	}

	//---------------------------------------
	//CONSTRAINT

	// (0) Tất cả những môn sinh viên i không thi sẽ đánh 0
	for (int i = 0; i < S; i++) {
		for (int j = 0; j < E; j++) {
			if (examOfStudent[i].count(j) == 0) {
				for (int t = 0; t < T; t++) {
					w_ijt[i][j][t].setBounds(0, 0);
				}
			}
		}
	}

	cout << "Done 0\n";

	// (1) với mọi cặp(i, j) mà sinh viên i thi môn j thì phải thi ở
	// thời điểm t nào đó.
	for (int i = 0; i < S; i++) {
		for (int j : examOfStudent[i]) {
			IloExpr expr1(env);
			for (int t = 0; t < T; t++) {
				expr1 += w_ijt[i][j][t];
			}
			model.add(expr1 == 1).setName("Constraint 1");
		}
	}

	cout << "Done 1\n";

	// (2) sinh viên i thiở thời gian thời gian t thì phải thi một môn nào đó trong danh sách môn thi của họ
	for (int i = 0; i < S; i++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr2(env);
			for (int j : examOfStudent[i]) {
				expr2 += w_ijt[i][j][t];
			}
			model.add(expr2 == w_it[i][t]).setName("Constraint 2");
		}
	}

	cout << "Done 2\n";

	// (3) sinh viên i thi môn gì đó ở thời điểm t thì sinh viên này sẽ phải ở một phòng r nào đó
	for (int i = 0; i < S; i++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr3(env);
			for (int r = 0; r < R; r++) {
				expr3 += w_irt[i][r][t];
			}
			model.add(expr3 == 1).setName("Constraint 3");
		}
	}

	cout << "Done 3\n";

	// (4) số sinh viên tham gia phòng thi r thỏa mãn điều kiện giới hạn số sinh viên của phòng tại thời điểm t bất kỳ.
	for (int r = 0; r < R; r++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr4(env);
			for (int i = 0; i < S; i++) {
				expr4 += w_irt[i][r][t];
			}
			model.add(expr4 <= capacity[r]).setName("Constraint 4");
		}
	}

	cout << "Done 4\n";

	// (5) sinh viên chỉ thi khi môn thi được tổ chức ở timeslot đó
	for (int i = 0; i < S; i++) {
		for (int j : examOfStudent[i]) {
			for (int t = 0; t < T; t++) {
				model.add(w_ijt[i][j][t] <= x_jt[j][t]).setName("Constraint 5");
			}
		}
	}

	cout << "Done 5\n";

	// (6) môn j thi ở phòng r ở thời điểm t chỉ khi môn j được xác định thi ở thời điểm t
	for (int r = 0; r < R; r++) {
		for (int j = 0; j < E; j++) {
			for (int t = 0; t < T; t++) {
				model.add(x_jrt[j][r][t] <= x_jt[j][t]).setName("Contraint 6");
			}
		}
	}

	cout << "Done 6\n";

	// (7) có sinh viên i thi môn j ở phòng r tại thời điểm t thì biến xjrt phải nhận giá trị 1
	for (int i = 0; i < S; i++) {
		for (int j : examOfStudent[i]) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					model.add(w_irt[i][r][t] + w_ijt[i][j][t] <= x_jrt[j][r][t] + 1).setName("Constraint 7");
				}
			}
		}
	}

	cout << "Done 7\n";

	// (8) phải có ít nhất 1 phòng được sử dụng để thi môn j ở thời điểm t
	for (int j = 0; j < E; j++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr5(env);
			for (int r = 0; r < R; r++) {
				expr5 += x_jrt[j][r][t];
			}
			model.add(expr5 >= x_jt[j][t]).setName("Constraint 8");
		}
	}

	cout << "Done 8\n";

	// (9) số phòng mở phục vụ môn học j phải thỏa mãn số sinh viên cho môn này
	for (int j = 0; j < E; j++) {
		IloExpr expr6(env);
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				expr6 += x_jrt[j][r][t] * capacity[r];
			}
		}
		model.add(expr6 >= numberOfStudentInExam[j]).setName("Constraint 9");
	}

	cout << "Done 9\n";

	// (10) Ràng buộc thi chung phòng
	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int t = 0; t < T; t++) {
				model.add(Xij_t[j1][j2][t] <= x_jt[j1][t]).setName("Constraint 10.1");
			}
		}
	}

	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int t = 0; t < T; t++) {
				model.add(Xij_t[j1][j2][t] <= x_jt[j2][t]).setName("Constraint 10.2");
			}
		}
	}

	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int t = 0; t < T; t++) {
				model.add(Xij_t[j1][j2][t]  + 1 >= x_jt[j1][t] + x_jt[j2][t]).setName("Constraint 10.3");
			}
		}
	}

	cout << "Done 10\n";

	// (11) mỗi sinh viên sẻ chỉ chọn thi trong một khoảng timeslot[p, q] duy nhất
	for (int i = 0; i < S; i++) {
		IloExpr expr7(env);
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				expr7 += wi_pq[i][p][q];
			}
		}
		model.add(expr7 == 1).setName("Constraint 11");
	}
	cout << "Done 11\n";

	// (12)  đoạn (p,q) nào đó dc chọn thì có một môn sẽ thi ở thời điểm p với p là 
	// thời điểm bắt đầu của của mọi môn thi của sinh viên i
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr8(env);
			for (int q = p; q < T; q++) {
				expr8 += wi_pq[i][p][q];
			}
			model.add(expr8 <= w_it[i][p]).setName("Constraint 12");
		}
	}

	cout << "Done 12\n";

	// (13) đoạn (p,q) nào đó dc chọn thì có một môn sẽ thi ở thời điểm q
	// (với q là thời điểm kết thúc mọi môn thi của sv i)
	for (int i = 0; i < S; i++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr9(env);
			for (int p = 0; p <= q; p++) {
				expr9 += wi_pq[i][p][q];
			}
			model.add(expr9 <= w_it[i][q]).setName("Constraint 13");
		}
	}

	cout << "Done 13\n";

	// (14) p là thời điểm mà bắt đầu các môn thi của sv i thì sv này ko thi bất kỳ môn nào trc thời điểm p
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr10(env);
			for (int q = p; q < T; q++) {
				expr10 += wi_pq[i][p][q];
			}
			for (int t = 0; t < p; t++) {
				model.add(expr10 + w_it[i][t] <= 1).setName("Constraint 14");
			}
		}
	}

	cout << "Done 14\n";

	// (15) q là thời điểm mà kết thúc các môn thi của sv i thì sv này ko thi bất kỳ môn nào sau thời điểm q
	for (int i = 0; i < S; i++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr11(env);
			for (int p = 0; p <= q; p++) {
				expr11 += wi_pq[i][p][q];
			}
			for (int t = q + 1; t < T; t++) {
				model.add(expr11 + w_it[i][t] <= 1).setName("Constraint 15");
			}
		}
	}
	cout << "Done 15\n";

	// (16) q-p+1 < số lượng môn khác nhau mà sinh viên i tham gia thi
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (q - p + 1 < examOfStudent[i].size()) {
					wi_pq[i][p][q].setBounds(0, 0);
				}
			}
		}
	}
	cout << "Done 16\n";

	// (17) có mọt khoảng thời gian p..q được chọn để tổ chức thi môn thi i
	for (int j = 0; j < E; j++) {
		IloExpr expr12(env);
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				expr12 += zj_pq[j][p][q];
			}
		}
		model.add(expr12 == 1).setName("Constraint 17");
	}

	cout << "Done 17\n";

	// (18) nếu đoạn p..q được tổ chức để thi môn j(wjpq = 1) thì bất cứ slot nào cũng phải có môn j thi (hay xjt = 1)
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				for (int t = p; t <= q; t++) {
					model.add(x_jt[j][t] >= zj_pq[j][p][q]).setName("Constraint 18");
				}
			}
		}
	}

	cout << "Done 18\n";

	// (19) p là thời điểm mà bắt đầu các môn thi của môn i thì môn này 
	// ko thi bất kỳ môn nào trc thời điểm p
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr13(env);
			for (int q = p; q < T; q++) {
				expr13 += zj_pq[j][p][q];
			}
			for (int t = 0; t < p; t++) {
				model.add(expr13 + x_jt[j][t] <= 1).setName("Constraint 19");
			}
		}
	}

	cout << "Done 19\n";

	// (20) q là thời điểm mà kết thúc các môn thi của môn i thì môn này 
	// ko thi bất kỳ môn nào sau thời điểm q
	for (int j = 0; j < E; j++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr14(env);
			for (int p = 0; p <= q; p++) {
				expr14 += zj_pq[j][p][q];
			}
			for (int t = q + 1; t < T; t++) {
				model.add(expr14 + x_jt[j][t] <= 1).setName("Constraint 20");
			}
		}
	}

	cout << "Done 20\n";

	IloExpr obj1(env);
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				obj1 += zj_pq[j][p][q] * (q - p);
			}
		}
	}
	obj1 *= 1000;

	IloExpr obj2(env);
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (p >= 6 || q <= 5)
					obj2 += wi_pq[i][p][q] * (q - p);
			}
		}
	}
	obj2 *= 1000;

	IloExpr obj3(env);
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (p <= 5 && q >= 6)
					obj3 += wi_pq[i][p][q] * (q - p);
			}
		}
	}
	obj3 *= 1000;

	IloExpr obj4(env);
	for (int j = 0; j < E; j++) {
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				obj4 += x_jrt[j][r][t];
			}
		}
	}
	obj4 *= 10;

	IloExpr obj5(env);
	for (int j1 = 0; j1 < E; j1++) {
		for (int j2 = j1 + 1; j2 < E; j2++) {
			for (int t = 0; t < T; t++) {
				obj5 += Xij_t[j1][j2][t];
			}
		}
	}
	obj5 *= 1000;

	model.add(IloMinimize(env, obj1 + obj2 + obj3 + obj5 + obj4));

	cout << "\nCplex starts here\n";

	IloCplex cplex(model);
	if (!cplex.solve()) {
		cerr << "Failed to solve the problem" << endl;
		throw(-1);
	}

	cout << "Solved\n";

	double objValue = cplex.getObjValue();
	cout << "Objective Value: " << objValue << endl;

	cout << "Solution status: " << cplex.getStatus() << endl;

	ofstream outFile("output.txt", ofstream::trunc);

	cout << "\n\nResult starts here:\n\n";
	cout << "Exam with its respective room and timeslot:\n";

	for (int j = 0; j < E; j++) {
		cout << "Exam: " << j << " ";
		for (int r = 0; r < R; r++) {
			cout << "\n\tRoom: " << r + 1;
			for (int t = 0; t < T; t++) {
				if (cplex.getValue(x_jrt[j][r][t])) {
					cout << "\n\t\tTime Slot: " << t + 1;
				}
			}
		}
	}
}

void Solver::Test() {
	cout << "\n-----------------------\n\nStart of Test 3D model solver\n\n";

	IloEnv env;
	IloModel model(env);

	// Declare Variable:
	IloArray<IloArray<IloNumVarArray>> x_jrt(env, E); // 1 nếu môn j được thi tại phòng r và time slot t
	for (int j = 0; j < E; j++) {
		x_jrt[j] = IloArray<IloNumVarArray>(env, R);
		for (int r = 0; r < R; r++) {
			x_jrt[j][r] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				x_jrt[j][r][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "x_jrt(" + to_string(j) + "," + to_string(r) + "," + to_string(t) + ")";
				x_jrt[j][r][t].setName(name.c_str());
			}
		}
	}
	cout << "x_jrt[j][r][t]" << endl;

	IloArray<IloNumVarArray> x_jt(env, E); // 1 nếu môn j được thi ở time slot t
	for (int j = 0; j < E; j++) {
		x_jt[j] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			x_jt[j][t] = IloNumVar(env, 0, 1, ILOINT);
			string name = "x_jt(" + to_string(j) + "," + to_string(t) + ")";
			x_jt[j][t].setName(name.c_str());
		}
	}
	cout << "x_jt[j][t]" << endl;

	IloArray<IloNumVarArray> x_rt(env, R); // 1 nếu phòng r được thi ở time slot t
	for (int r = 0; r < R; r++) {
		x_rt[r] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			x_rt[r][t] = IloNumVar(env, 0, 1, ILOINT);
			//string name = "x_rt(" + to_string(j) + "," + to_string(t) + ")";
			//x_jt[r][t].setName(name.c_str());
		}
	}
	cout << "x_rt[r][t]" << endl;

	IloArray<IloArray<IloNumVarArray>> w_irt(env, S); // sinh viên i thi môn j ở phòng r time slot t
	for (int i = 0; i < S; i++) {
		w_irt[i] = IloArray<IloNumVarArray>(env, R);
		for (int r = 0; r < R; r++) {
			w_irt[i][r] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				w_irt[i][r][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "w_irt(" + to_string(i) + "," + to_string(r) + "," + to_string(t) + ")";
				w_irt[i][r][t].setName(name.c_str());
			}
		}
	}
	cout << "w_irt[i][r][t]" << endl;

	// sinh viên i thi môn j ở time slot t
	IloArray<IloArray<IloNumVarArray>> w_ijt(env, S);
	for (int i = 0; i < S; i++) {
		w_ijt[i] = IloArray<IloNumVarArray>(env, E);
		for (int j = 0; j < E; j++) {
			w_ijt[i][j] = IloNumVarArray(env, T);
			for (int t = 0; t < T; t++) {
				w_ijt[i][j][t] = IloNumVar(env, 0, 1, ILOINT);
				string name = "w_ijt(" + to_string(i) + "," + to_string(j) + "," + to_string(t) + ")";
				w_ijt[i][j][t].setName(name.c_str());
			}
		}
	}
	cout << "w_ijt[i][j][t]" << endl;

	IloArray<IloNumVarArray> w_it(env, S);
	for (int i = 0; i < S; i++) {
		w_it[i] = IloNumVarArray(env, T);
		for (int t = 0; t < T; t++) {
			w_it[i][t] = IloNumVar(env, 0, 1, ILOINT);
			string name = "w_it(" + to_string(i) + "," + to_string(t) + ")";
			w_it[i][t].setName(name.c_str());
		}
	}
	cout << "w_it[i][t]" << endl;

	IloArray<IloArray<IloNumVarArray>> wi_pq(env, S); // sinh viên i thi ở trong các time slot p và q
	for (int i = 0; i < S; i++) {
		wi_pq[i] = IloArray<IloNumVarArray>(env, T);
		for (int p = 0; p < T; p++) {
			wi_pq[i][p] = IloNumVarArray(env, T);
			for (int q = p; q < T; q++) {
				wi_pq[i][p][q] = IloNumVar(env, 0, 1, ILOINT);
				string name = "wi_pq[" + to_string(i) + ",(" + to_string(p) + "," + to_string(q) + ")]";
				wi_pq[i][p][q].setName(name.c_str());
			}
		}
	}
	cout << "wi_pq[i][p][q]" << endl;

	IloArray<IloArray<IloNumVarArray>> zj_pq(env, E); // môn j được tổ chức liên tiếp từ time-slot p đến time slot q
	for (int j = 0; j < E; j++) {
		zj_pq[j] = IloArray<IloNumVarArray>(env, T);
		for (int p = 0; p < T; p++) {
			zj_pq[j][p] = IloNumVarArray(env, T);
			for (int q = p; q < T; q++) {
				zj_pq[j][p][q] = IloNumVar(env, 0, 1, ILOINT);
				string name = "zj_pq[" + to_string(j) + ",(" + to_string(p) + "," + to_string(q) + ")]";
				zj_pq[j][p][q].setName(name.c_str());
				if (q > p) zj_pq[j][p][q].setBounds(0, 0);
			}
		}
	}
	cout << "zj_pq[j][p][q]" << endl;

	//IloArray<IloArray<IloNumVarArray>> Xij_t(env, E); // hai môn i và j cùng thi tại phòng (r, t)
	//for (int j1 = 0; j1 < E; j1++) {
	//	Xij_t[j1] = IloArray<IloNumVarArray>(env, E);
	//	for (int j2 = 0; j2 < E; j2++) {
	//		Xij_t[j1][j2] = IloNumVarArray(env, T);
	//		for (int t = 0; t < T; t++) {
	//			Xij_t[j1][j2][t] = IloNumVar(env, 0, 1, ILOINT);
	//		}
	//	}
	//}

	//---------------------------------------
	//CONSTRAINT

	// (0) Tất cả những môn sinh viên i không thi sẽ đánh 0
	for (int i = 0; i < S; i++) {
		for (int j = 0; j < E; j++) {
			if (examOfStudent[i].count(j) == 0) {
				for (int t = 0; t < T; t++) {
					w_ijt[i][j][t].setBounds(0, 0);
				}
			}
		}
	}

	cout << "Done 0\n";

	// (1) với mọi cặp(i, j) mà sinh viên i thi môn j thì phải thi ở
	// thời điểm t nào đó.
	for (int i = 0; i < S; i++) {
		for (int j : examOfStudent[i]) {
			IloExpr expr1(env);
			for (int t = 0; t < T; t++) {
				expr1 += w_ijt[i][j][t];
			}
			model.add(expr1 == 1).setName(("C1_ij_" + to_string(i) + "." + to_string(j)).c_str());
		}
	}

	cout << "Done 1\n";

	// (2) sinh viên i thiở thời gian thời gian t thì phải thi một môn nào đó trong danh sách môn thi của họ
	for (int i = 0; i < S; i++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr2(env);
			for (int j : examOfStudent[i]) {
				expr2 += w_ijt[i][j][t];
			}
			model.add(expr2 == w_it[i][t]).setName(("C2_it_" + to_string(i) + "." + to_string(t)).c_str());
		}
	}

	cout << "Done 2\n";

	// (3) sinh viên i thi môn gì đó ở thời điểm t thì sinh viên này sẽ phải ở một phòng r nào đó
	for (int i = 0; i < S; i++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr3(env);
			for (int r = 0; r < R; r++) {
				expr3 += w_irt[i][r][t];
			}
			model.add(expr3 == w_it[i][t]).setName(("C3_it_" + to_string(i) + "." + to_string(t)).c_str());
		}
	}

	cout << "Done 3\n";

	// (4) số sinh viên tham gia phòng thi r thỏa mãn điều kiện giới hạn số sinh viên của phòng tại thời điểm t bất kỳ.
	for (int r = 0; r < R; r++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr4(env);
			for (int i = 0; i < S; i++) {
				expr4 += w_irt[i][r][t];
			}
			//cout << r << " " << t << endl;
			model.add(expr4 <= capacity[r]).setName(("C4_rt_" + to_string(r) + "." + to_string(t)).c_str());
		}
	}

	cout << "Done 4\n";

	// (5) sinh viên i thi môn j ở time slot t nếu môn j được tổ chức thi ở time slot t
	for (int i = 0; i < S; i++) {
		for (int j : examOfStudent[i]) {
			for (int t = 0; t < T; t++) {
				model.add(w_ijt[i][j][t] <= x_jt[j][t]).setName(("C5_ijt_"
					+ to_string(i) + "." + to_string(j) + "." + to_string(t)).c_str());
			}
		}
	}

	cout << "Done 5\n";

	// (5.1) N  sinh viên i thi môn j ở time slot t nếu môn j được tổ chức thi ở time slot t
	for (int i = 0; i < S; i++) {
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				model.add(w_irt[i][r][t] <= x_rt[r][t]).setName(("C5.1_irt_"
					+ to_string(i) + "." + to_string(r) + "." + to_string(t)).c_str());
			}
		}
	}

	// (6) môn j thi ở phòng r ở thời điểm t chỉ khi môn j được xác định thi ở thời điểm t
	for (int r = 0; r < R; r++) {
		for (int j = 0; j < E; j++) {
			for (int t = 0; t < T; t++) {
				model.add(x_jrt[j][r][t] <= x_jt[j][t]).setName(("C6_jrt_"
					+ to_string(j) + "." + to_string(r) + "." + to_string(t)).c_str());
			}
		}
	}

	cout << "Done 6\n";

	// (7) có sinh viên i thi môn j ở phòng r tại thời điểm t thì biến xjrt phải nhận giá trị 1
	for (int i = 0; i < S; i++) {
		for (int j : examOfStudent[i]) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					model.add(w_irt[i][r][t] + w_ijt[i][j][t] <= x_jrt[j][r][t] + 1).setName(("C7_ijrt_"
						+ to_string(i) + "." + to_string(j) + "." + to_string(r) + "." + to_string(t)).c_str());
				}
			}
		}
	}

	cout << "Done 7\n";

	// (8) phải có ít nhất 1 phòng được sử dụng để thi môn j ở thời điểm t
	for (int j = 0; j < E; j++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr5(env);
			for (int r = 0; r < R; r++) {
				expr5 += x_jrt[j][r][t];
			}
			model.add(expr5 >= x_jt[j][t]).setName(("C8_jt_"
				+ to_string(j) + "." + to_string(t)).c_str());
		}
	}

	cout << "Done 8\n";

	// (8.1) N  phải có ít nhất 1 môn thi j nào đó được sử dụng để thi
	// ở phòng r tại thời điểm t nếu phòng đó được sử dụng.
	for (int r = 0; r < R; r++) {
		for (int t = 0; t < T; t++) {
			IloExpr expr5_1(env);
			for (int j = 0; j < E; j++) {
				expr5_1 += x_jrt[j][r][t];
			}
			model.add(expr5_1 >= x_rt[r][t]).setName(("C8.1_rt_"
				+ to_string(r) + "." + to_string(t)).c_str());
		}
	}


	// (9) số phòng mở phục vụ môn học j phải thỏa mãn số sinh viên cho môn này
	for (int j = 0; j < E; j++) {
		IloExpr expr6(env);
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				expr6 += x_jrt[j][r][t] * capacity[r];
			}
		}
		model.add(expr6 >= numberOfStudentInExam[j]).setName(("C9_j_" + to_string(j)).c_str());
	}

	cout << "Done 9\n";

	//// (10) Ràng buộc thi chung phòng
	//for (int j1 = 0; j1 < E; j1++) {
	//	for (int j2 = j1 + 1; j2 < E; j2++) {
	//		for (int t = 0; t < T; t++) {
	//			model.add(Xij_t[j1][j2][t] <= x_jt[j1][t]).setName("Constraint 10.1");
	//		}
	//	}
	//}

	//for (int j1 = 0; j1 < E; j1++) {
	//	for (int j2 = j1 + 1; j2 < E; j2++) {
	//		for (int t = 0; t < T; t++) {
	//			model.add(Xij_t[j1][j2][t] <= x_jt[j2][t]).setName("Constraint 10.2");
	//		}
	//	}
	//}

	//for (int j1 = 0; j1 < E; j1++) {
	//	for (int j2 = j1 + 1; j2 < E; j2++) {
	//		for (int t = 0; t < T; t++) {
	//			model.add(Xij_t[j1][j2][t] >= x_jt[j1][t] + x_jt[j2][t]).setName("Constraint 10.3");
	//		}
	//	}
	//}

	//cout << "Done 10\n";

	// (11) mỗi sinh viên sẻ chỉ chọn thi trong một khoảng timeslot[p, q] duy nhất
	for (int i = 0; i < S; i++) {
		IloExpr expr7(env);
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				expr7 += wi_pq[i][p][q];
			}
		}
		model.add(expr7 == 1).setName(("C11_i_" + to_string(i)).c_str());
	}
	cout << "Done 11\n";

	// (12)  đoạn (p,q) nào đó dc chọn thì có một môn sẽ thi ở thời điểm p với p là 
	// thời điểm bắt đầu của của mọi môn thi của sinh viên i
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr8(env);
			for (int q = p; q < T; q++) {
				expr8 += wi_pq[i][p][q];
			}
			model.add(expr8 <= w_it[i][p]).setName(("C12_ip_" + to_string(i) + "." + to_string(p)).c_str());
		}
	}

	cout << "Done 12\n";

	// (13) đoạn (p,q) nào đó dc chọn thì có một môn sẽ thi ở thời điểm q
	// (với q là thời điểm kết thúc mọi môn thi của sv i)
	for (int i = 0; i < S; i++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr9(env);
			for (int p = 0; p <= q; p++) {
				expr9 += wi_pq[i][p][q];
			}
			model.add(expr9 <= w_it[i][q]).setName(("C12_iq_" + to_string(i) + "." + to_string(q)).c_str());
		}
	}

	cout << "Done 13\n";

	// (14) p là thời điểm mà bắt đầu các môn thi của sv i thì sv này ko thi bất kỳ môn nào trc thời điểm p
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr10(env);
			for (int q = p; q < T; q++) {
				expr10 += wi_pq[i][p][q];
			}
			for (int t = 0; t < p; t++) {
				model.add(expr10 + w_it[i][t] <= 1).setName(("C13_ipt_" + to_string(i)
					+ "." + to_string(p) + "." + to_string(t)).c_str());
			}
		}
	}

	cout << "Done 14\n";

	// (15) q là thời điểm mà kết thúc các môn thi của sv i thì sv này ko thi bất kỳ môn nào sau thời điểm q
	for (int i = 0; i < S; i++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr11(env);
			for (int p = 0; p <= q; p++) {
				expr11 += wi_pq[i][p][q];
			}
			for (int t = q + 1; t < T; t++) {
				model.add(expr11 + w_it[i][t] <= 1).setName(("C13_iqt_" + to_string(i)
					+ "." + to_string(q) + "." + to_string(t)).c_str());
			}
		}
	}

	cout << "Done 15\n";

	// (16) q-p+1 < số lượng môn khác nhau mà sinh viên i tham gia thi
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (q - p + 1 < examOfStudent[i].size())
				{
					wi_pq[i][p][q].setBounds(0, 0);
				}
			}
		}
	}

	cout << "Done 16\n";

	// (17) có mọt khoảng thời gian p..q được chọn để tổ chức thi môn thi i
	for (int j = 0; j < E; j++) {
		IloExpr expr12(env);
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				expr12 += zj_pq[j][p][q];
			}
		}
		model.add(expr12 == 1).setName(("C13_j_" + to_string(j)).c_str());
	}

	cout << "Done 17\n";

	// (18) nếu đoạn p..q được tổ chức để thi môn j(wjpq = 1) thì bất cứ slot nào cũng phải có môn j thi (hay xjt = 1)
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				for (int t = p; t <= q; t++) {
					model.add(x_jt[j][t] >= zj_pq[j][p][q]).setName("Constraint 18");
				}
			}
		}
	}
	cout << "Done 18\n";



	// (19) p là thời điểm mà bắt đầu các môn thi của môn i thì môn này 
	// ko thi bất kỳ môn nào trc thời điểm p
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			IloExpr expr13(env);
			for (int q = p; q < T; q++) {
				expr13 += zj_pq[j][p][q];
			}
			for (int t = 0; t < p; t++) {
				model.add(expr13 + x_jt[j][t] <= 1).setName("Constraint 19");
			}
		}
	}

	cout << "Done 19\n";

	// (20) q là thời điểm mà kết thúc các môn thi của môn i thì môn này 
	// ko thi bất kỳ môn nào sau thời điểm q
	for (int j = 0; j < E; j++) {
		for (int q = 0; q < T; q++) {
			IloExpr expr14(env);
			for (int p = 0; p <= q; p++) {
				expr14 += zj_pq[j][p][q];
			}
			for (int t = q + 1; t < T; t++) {
				model.add(expr14 + x_jt[j][t] <= 1).setName("Constraint 20");
			}
		}
	}

	cout << "Done 20\n";


	// Objective function:
	IloExpr obj1(env);
	for (int j = 0; j < E; j++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				obj1 += zj_pq[j][p][q] * (q - p);
			}
		}
	}
	obj1 *= 1000;

	IloExpr obj2(env);
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (p >= 6 || q <= 5)
					obj2 += wi_pq[i][p][q] * (q - p);
			}
		}
	}
	obj2 *= 1000;

	IloExpr obj3(env);
	for (int i = 0; i < S; i++) {
		for (int p = 0; p < T; p++) {
			for (int q = p; q < T; q++) {
				if (p <= 5 && q >= 6)
					obj3 += wi_pq[i][p][q] * (q - p);
			}
		}
	}
	obj3 *= 1000;

	IloExpr obj4(env);
	for (int j = 0; j < E; j++) {
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				obj4 += x_jrt[j][r][t];
			}
		}
	}
	obj4 *= 10000;

	model.add(IloMinimize(env, obj1 + obj2 + obj3 + obj4));

	cout << "\nCplex starts here\n";

	IloCplex cplex(model);
	if (!cplex.solve()) {
		cerr << "Failed to solve the problem" << endl;
		throw(-1);
	}

	cout << "Solved\n";

	double objValue = cplex.getObjValue();
	cout << "Objective Value: " << objValue << endl;

	cout << "Solution status: " << cplex.getStatus() << endl;

	ofstream outFile("output.txt", ofstream::trunc);

	cout << "\n\nResult starts here:\n\n";
	cout << "Exam with its respective room and timeslot:\n";
	outFile << "Exam with its respective room and timeslot:\n";

	for (int j = 0; j < E; j++) {
		cout << "\nExam: " << j;
		outFile << "\nExam: " << j;
		for (int r = 0; r < R; r++) {
			cout << "\n\tRoom: " << r;
			outFile << "\n\tRoom: " << r;
			for (int t = 0; t < T; t++) {
				if (cplex.getValue(x_jrt[j][r][t] == 1))
				cout << "\n\t\tTime slot: " << t << endl;
				outFile << "\n\t\tTime slot: " << t << endl;
			}
		}
	}

	outFile << endl;
	cout << endl;

	cout << "\n\nStudent with their respective room, exam and timeslot:\n";
	outFile << "\n\nStudent with their respective room, exam and timeslot:\n";

	for (int i = 0; i < S; i++) {
		cout << "Student " << i << endl;
		outFile << "Student " << i << endl;
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				if (cplex.getValue(w_irt[i][r][t] == 1)) {
					//cout << i << " ";
					for (int j : examOfStudent[i]) {
						if (cplex.getValue(w_ijt[i][j][t])) {
							//cout << j << " ";
							outFile << "Exam: " << j << " | Room: " << r << " | Time slot: " << t << endl;
							cout << "Exam: " << j << " | Room: " << r << " | Time slot: " << t << endl;
						}
					}
				}
			}
		}
	}

	outFile << endl;
	cout << endl;

	cout << "\n\nRoom -> Timeslot -> Exam -> Student in that exam: \n";
	outFile << "\n\nRoom -> Timeslot -> Exam -> Student in that exam: \n";

	// Exam and its room, timeslot and student
	for (int r = 0; r<R; r++){
		outFile << "Room: " << r << endl;
		cout << "Room: " << r << endl;
		for (int t = 0; t<T; t++){
			for (int j = 0; j < E; j++){
				if (cplex.getValue(x_jrt[j][r][t])){
					outFile << "Time slot: " << t << " | Exam: " << j << endl;
					cout << "Time slot: " << t << " | Exam: " << j << endl;
					for (int i = 0; i<S; i++){
						if (examOfStudent[i].count(j) == 1 && cplex.getValue(w_ijt[i][j][t]) == 1){
							cout << i << " ";
							outFile << i << " ";
						}
					}
					cout << endl;
					outFile << endl;
				}
			}
		}
		cout << endl;
		outFile << endl;
	}
}