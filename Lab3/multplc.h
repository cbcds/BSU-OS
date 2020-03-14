#ifndef MULTPLC_H
#define MULTPLC_H

#include <vector>
#include <thread>
#include <iostream>
#include <cmath>

#include "Matrix.h"

void block_mul_1(const Matrix& A, const Matrix& B, const Matrix& res, int beg, int end);
void block_mul_2(const Matrix& A, const Matrix& B, const Matrix& res, int beg, int end);
void block_mul_3(const Matrix& A, const Matrix& B, const Matrix& res, int beg1, int beg2, int end1, int end2);

Matrix mul_0(const Matrix& A, const Matrix& B) {
    auto start =  std::chrono::high_resolution_clock::now();
    Matrix res(A.rows_, B.cols_, 0);
    for (int i = 0; i < A.rows_; ++i) {
        for (int j = 0; j < B.cols_; ++j) {
            for (int k = 0; k < A.cols_; ++k) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    auto end =  std::chrono::high_resolution_clock::now();
    auto ex_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "ordinary multiplication: " << ex_time << " mcs" << std::endl;
    return res;
}

Matrix mul_1(const Matrix& A, const Matrix& B, int threads_num) {
    int len = ceil((double)A.rows_ / threads_num);
    Matrix res(A.rows_, B.cols_, 0);

    std::vector<std::thread> pool;
    pool.reserve(threads_num);
    for (int i = 0; i < threads_num; ++i) {
        int beg = i * len;
        int end = std::min(beg + len, A.rows_);
        pool.emplace_back(block_mul_1, std::ref(A), std::ref(B), std::ref(res), beg, end);
    }

    auto start =  std::chrono::high_resolution_clock::now();
    for (std::thread& th: pool) {
        th.join();
    }
    auto end =  std::chrono::high_resolution_clock::now();
    auto ex_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "horizontal x vertical blocks: " << ex_time << " mcs" << std::endl;
    return res;
}

Matrix mul_2(const Matrix& A, const Matrix& B, int threads_num) {
    int len = ceil((double)B.cols_ / threads_num);
    Matrix res(A.rows_, B.cols_, 0);

    std::vector<std::thread> pool;
    pool.reserve(threads_num);
    for (int i = 0; i < threads_num; ++i) {
        int beg = i * len;
        int end = std::min(beg + len, B.cols_);
        pool.emplace_back(block_mul_2, std::ref(A), std::ref(B), std::ref(res), beg, end);
    }

    auto start =  std::chrono::high_resolution_clock::now();
    for (std::thread& th: pool) {
        th.join();
    }
    auto end =  std::chrono::high_resolution_clock::now();
    auto ex_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "vertical x horizontal blocks: " << ex_time << " mcs" << std::endl;
    return res;
}

Matrix mul_3(const Matrix& A, const Matrix& B, int threads_num) {
    int k = ceil(sqrt(threads_num));
    int len_rows = ceil((double)A.rows_ / k);
    int len_cols = ceil((double)B.cols_ / k);
    Matrix res(A.rows_, B.cols_, 0);

    std::vector<std::thread> pool;
    pool.reserve(threads_num);
    for (int i = 0; i < k; ++i) {
        int beg1 = i * len_rows;
        int end1 = std::min(beg1 + len_rows, A.rows_);
        for (int j = 0; j < k; ++j) {
            int beg2 = j * len_cols;
            int end2 = std::min(beg2 + len_cols, B.cols_);
            pool.emplace_back(block_mul_3, std::ref(A), std::ref(B), std::ref(res), beg1, beg2, end1, end2);
        }
    }

    auto start =  std::chrono::high_resolution_clock::now();
    for (std::thread& th: pool) {
        th.join();
    }
    auto end =  std::chrono::high_resolution_clock::now();
    auto ex_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "division by column and row blocks: " << ex_time << " mcs" << std::endl;
    return res;
}

void block_mul_1(const Matrix& A, const Matrix& B, const Matrix& res, int beg, int end) {
    for (int i = beg; i < end; ++i) {
        for (int j = 0; j < B.cols_; ++j) {
            for (int k = 0; k < A.cols_; ++k) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void block_mul_2(const Matrix& A, const Matrix& B, const Matrix& res, int beg, int end) {
    for (int i = 0; i < A.rows_; ++i) {
        for (int j = beg; j < end; ++j) {
            for (int k = 0; k < A.cols_; ++k) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void block_mul_3(const Matrix& A, const Matrix& B, const Matrix& res, int beg1, int beg2, int end1, int end2) {
    for (int i = beg1; i < end1; ++i) {
        for (int j = 0; j < B.cols_; ++j) {
            for (int k = beg2; k < end2; ++k) {
                res[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

#endif //MULTPLC_H
