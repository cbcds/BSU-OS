#include <iostream>
#include <fstream>
#include "Matrix.h"
#include "multplc.h"

int main() {
    int threads_num, a_r, a_c, b_r, b_c;

    std::ifstream fin("input.txt");
    fin >> threads_num;
    fin >> a_r >> a_c >> b_r >> b_c;
    Matrix A(a_r, a_c), B(b_r, b_c);
    A.Read(fin);
    B.Read(fin);
    fin.close();

    mul_0(A, B);
    mul_1(A, B, threads_num);
    mul_2(A, B, threads_num);
    mul_3(A, B, threads_num);
}
