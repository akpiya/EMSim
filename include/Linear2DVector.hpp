#ifndef LINEAR2DVECTOR_HPP
#define LINEAR2DVECTOR_HPP
#include <vector>

#define DECIMAL float 

class Linear2DVector {

public:
    std::vector<DECIMAL> data;

    Linear2DVector(int rows, int cols);

    DECIMAL& get(int i, int j);

private:
    int rows_, cols_;


};

#endif
