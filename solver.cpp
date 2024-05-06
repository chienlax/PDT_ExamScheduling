#include "solver.h"
#include <iostream>

using namespace std;

Solver::Solver(Instance* instance) : instance(instance) {
    E = instance->E;
    S = instance->S;
    R = instance->R; 
    T = instance->T;
    C = instance->C;
    capacity = instance->capacity;
    numberOfStudentInExam = instance->numberOfStudentInExam;
    exam_and_infor = instance->exam_and_infor;
    student_and_infor = instance->student_and_infor;
    decoding_exam = instance->decoding_exam;
    decoding_student = instance->decoding_student;

    void run();
}

void Solver::run() {
    cout << "\n-----------------------\n\nStart of solver\n\n";

    IloEnv env;
    IloModel model(env);

    // Declare Variable:
    IloArray<IloArray<IloNumVarArray>> x_jrt (env, E); // 1 nếu môn j được thi tại phòng r và time slot t
    IloArray<IloNumVarArray> x_jt (env, E); // 1 nếu môn j được thi ở time slot t
    IloArray<IloArray<IloArray<IloNumVarArray>>> w_ijrt; // sinh viên i thi môn j ở phòng r time slot t
    IloArray<IloArray<IloNumVarArray>> w_ijt; // sinh viên i thi môn j ở time slot t
    IloArray<IloNumVarArray> w_it; // sinh viên i thi ở time slot t
    IloArray<IloArray<IloNumVarArray>> wi_pq; // sinh viên i thi ở trong các time slot p và q
    IloArray<IloArray<IloNumVarArray>> zj_pq; // môn j được tổ chức liên tiếp từ time-slot p đến time slot q
    IloArray<IloArray<IloArray<IloNumVarArray>>> xij_rt; // hai môn i và j cùng thi tại phòng (r, t)

    for (int j = 0; j < E; j++) {
        x_jrt[j] = IloArray<IloNumVarArray>(env, R);
        for (int r = 0; r < R; r++) {
            x_jrt[j][r] = IloNumVarArray(env, T);
            for (int t = 0; t < T; t++) {
                x_jrt[j][r][t] = IloNumVar(env, 0, 1, ILOINT);
            }
        }
    }

    for (int j = 0; j < E; j++) {
        x_jt[j] = IloNumVarArray(env, T);
        for (int t = 0; t < T; t++) {
            x_jt[j][t] = IloNumVar(env, 0, 1, ILOINT);
        }
    }
    
    for (int i = 0; i < S; i++){
        w_ijrt[i] = IloArray<IloArray<IloNumVarArray>>(env, E);
        for (int j = 0; j < E; j++) {
            w_ijrt[i][j] = IloArray<IloNumVarArray>(env, R);
            for (int r = 0; r < R; r++) {
                w_ijrt[i][j][r] = IloNumVarArray(env, T);
                for (int t = 0; t < T; t++) {
                    w_ijrt[i][j][r][t] = IloNumVar(env, 0, 1, ILOINT);
                }
            }
        }
    }

    for (int i = 0; i < S; i++) {
        w_ijt[i] = IloArray<IloNumVarArray>(env, E);
        for (int j = 0; j < E; j++) {
            w_ijt[i][j] = IloNumVarArray(env, T);
            for (int t = 0; t < T; t++) {
                w_ijt[i][j][t] = IloNumVar(env, 0, 1, ILOINT);
            }
        }
    }

    for (int i = 0; i < S; i++) {
        wi_pq[i] = IloArray<IloNumVarArray>(env, T);
        for (int t1 = 0; t1 < T; t1++) {
            wi_pq[i][t1] = IloNumVarArray(env, T);
            for (int t2 = 0; t2 < T; t2++) {
                wi_pq[i][t1][t2] = IloNumVar(env, 0, 1, ILOINT);
            }
        }
    }

    for (int j = 0; j < E; j++) {
        zj_pq[j] = IloArray<IloNumVarArray>(env, T);
        for (int t1 = 0; t1 < T; t1++) {
            zj_pq[j][t1] = IloNumVarArray(env, T);
            for (int t2 = 0; t2 < T; t2++) {
                zj_pq[j][t1][t2] = IloNumVar(env, 0, 1, ILOINT);
            }
        }
    }

    for (int j1 = 0; j1 < E; j1++) {
        xij_rt[j1] = IloArray<IloArray<IloNumVarArray>>(env, E);
        for (int j2 = 0; j2 < E; j2++) {
            xij_rt[j1][j2] = IloArray<IloNumVarArray>(env, R);
            for (int r = 0; r < R; r++) {
                xij_rt[j1][j2][r] = IloNumVarArray(env, T);
                for (int t = 0; t < T; t++) {
                    xij_rt[j1][j2][r][t] = IloNumVar(env, 0, 1, ILOINT);
                }
            }
        }
    }

    // Constraints:
    // (1) sinh viên i tham gia không quá một môn ở thời điểm t với mọi cặp (i, t)
    for (int i = 0; i < S; i++) {
        IloExpr expr1(env);
        for (int j = 0; j < E; j++) {
            for (int t = 0; t < T; t++) {
                expr1 += w_ijt[i][j][t];
            }
        }
    }

    // (2) số sinh viên tham gia phòng thi r thỏa mãn điều kiện giới hạn 
    // số sinh viên của phòng tại thời điểm t bất kỳ


}