#ifndef LINEAR2DVECTOR_HPP
#define LINEAR2DVECTOR_HPP
#include <vector>

#define DECIMAL float 

template <typename T>
class Linear2DVector {

public:
    std::vector<T> data;

    Linear2DVector(int rows, int cols) {
        rows_ = rows;
        cols_ = cols;
        data = std::vector<T>(rows * cols);
    }

    T& get(int i, int j) {
        return data[i* cols_ + j];
    }

private:
    int rows_, cols_;
};

#endif
