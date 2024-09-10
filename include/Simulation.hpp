#ifndef SIMULATION_HPP
#define SIMULATION_HPP

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
    Linear2DVector conductorField;

    Linear2DVector E_z;
    Linear2DVector H_x;
    Linear2DVector H_y;

    void stepElectricField();
    void stepMagneticField();
    void stepRickertSource(DECIMAL time, DECIMAL location);
    void addConductorAt(int i, int j);
    void removeConductorAt(int i, int j);

private:
    Linear2DVector C_hxh;
    Linear2DVector C_hxe;
    Linear2DVector C_hyh;
    Linear2DVector C_hye;
    Linear2DVector C_eze;
    Linear2DVector C_ezh;

    void initializeCoefficientMatrix();
};

#endif
