#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector> 
#include <cmath>

#define DECIMAL float

class Simulation {
public:
    Simulation(int m, int n, DECIMAL deltaX, DECIMAL deltaY, DECIMAL deltaT);

    DECIMAL deltaX, deltaY, deltaT;
    int M, N;
    DECIMAL imp0{377.0f};
    DECIMAL Cdtds{1.0f / (DECIMAL) sqrt(2.0f)};
    int maxTime{300};

    std::vector<std::vector<DECIMAL>> E_z;
    std::vector<std::vector<DECIMAL>> H_x;
    std::vector<std::vector<DECIMAL>> H_y;

    void stepElectricField();
    void stepMagneticField();
    void stepRickertSource(DECIMAL time, DECIMAL location);

private:
    std::vector<std::vector<DECIMAL>> C_hxh;
    std::vector<std::vector<DECIMAL>> C_hxe;
    std::vector<std::vector<DECIMAL>> C_hyh;
    std::vector<std::vector<DECIMAL>> C_hye;
    std::vector<std::vector<DECIMAL>> C_eze;
    std::vector<std::vector<DECIMAL>> C_ezh;

    void initializeCoefficientMatrix();


};

#endif
