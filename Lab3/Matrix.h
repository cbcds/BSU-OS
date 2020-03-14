#ifndef MATRIX_H
#define MATRIX_H

class Matrix {
public:
    int rows_;
    int cols_;
    int** matrix_;

    Matrix(int rows, int cols): rows_(rows), cols_(cols) {
        matrix_ = new int*[rows];
        for (int i = 0; i < rows; ++i) {
            matrix_[i] = new int[cols];
        }
    }

    Matrix(int rows, int cols, int def_value): rows_(rows), cols_(cols) {
        matrix_ = new int*[rows];
        for (int i = 0; i < rows; ++i) {
            matrix_[i] = new int[cols];
            for (int j = 0; j < cols; ++j) {
                matrix_[i][j] = def_value;
            }
        }
    }

    void Read(std::ifstream& stream) {
        int x;
        for (int i = 0; i < rows_; ++i) {
            for (int j = 0; j < cols_; ++j) {
                stream >> matrix_[i][j];
            }
        }
    }

    int*& operator[](int index) const {
        return matrix_[index];
    }

    ~Matrix() {
        for (int i = 0; i < rows_; ++i) {
            delete [] matrix_[i];
        }
    }
};

#endif //MATRIX_H
