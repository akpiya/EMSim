#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector> 
#include <cmath>

class Simulation {
public:
    Simulation(int m, int n, double deltaX, double deltaY, double deltaT);

    double deltaX, deltaY, deltaT;
    int M, N;
    double imp0{377.0};
    double Cdtds{1.0 / sqrt(2.0)};
    int maxTime{300};

    std::vector<std::vector<double>> E_z;
    std::vector<std::vector<double>> H_x;
    std::vector<std::vector<double>> H_y;

    void stepElectricField();
    void stepMagneticField();
    void stepRickertSource(double time, double location);

private:
    std::vector<std::vector<double>> C_hxh;
    std::vector<std::vector<double>> C_hxe;
    std::vector<std::vector<double>> C_hyh;
    std::vector<std::vector<double>> C_hye;
    std::vector<std::vector<double>> C_eze;
    std::vector<std::vector<double>> C_ezh;

    void initializeCoefficientMatrix();


};

#endif
