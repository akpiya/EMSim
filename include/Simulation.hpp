#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include <Foundation/Foundation.hpp>
#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <cmath>

#include "Linear2DVector.hpp"


class Simulation {
public:
    Simulation(int m, int n, DECIMAL deltaX, DECIMAL deltaY, DECIMAL deltaT);

    DECIMAL deltaX, deltaY, deltaT;
    int M, N;

    DECIMAL imp0{377.0f};
    DECIMAL Cdtds{1.0f / (DECIMAL) sqrt(2.0f)};
    int maxTime{300};

    // User inputted boundary conditions
    Linear2DVector<char> conductorField;

    Linear2DVector<DECIMAL> E_z;
    Linear2DVector<DECIMAL> H_x;
    Linear2DVector<DECIMAL> H_y;

    void stepElectricField();
    void stepMagneticField();
    void stepRickertSource(DECIMAL time, DECIMAL location);
    void addConductorAt(int i, int j);
    void removeConductorAt(int i, int j);

private:
    Linear2DVector<DECIMAL> C_hxh;
    Linear2DVector<DECIMAL> C_hxe;
    Linear2DVector<DECIMAL> C_hyh;
    Linear2DVector<DECIMAL> C_hye;
    Linear2DVector<DECIMAL> C_eze;
    Linear2DVector<DECIMAL> C_ezh;

    void initializeCoefficientMatrix();
};

#endif
