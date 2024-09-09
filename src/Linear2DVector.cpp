#include "Linear2DVector.hpp"

Linear2DVector::Linear2DVector(int rows, int cols) {
    rows_ = rows;
    cols_ = cols;
    data = std::vector<DECIMAL>(rows * cols);
}

DECIMAL& Linear2DVector::get(int i, int j) {
    return data[i * cols_ + j];
}
