#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <Foundation/Foundation.hpp>

#include <cmath>

#include "Linear2DVector.hpp"


class Simulation {
public:
    Simulation(int m, int n, DECIMAL deltaX, DECIMAL deltaY, DECIMAL deltaT);

    ~Simulation();

    DECIMAL deltaX, deltaY, deltaT;
    int M, N;

    DECIMAL imp0{377.0f};
    DECIMAL Cdtds{1.0f / (DECIMAL) sqrt(2.0f)};
    int maxTime{300};
    MTL::Buffer *bufferE_z;

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

    void gpuStepElectricField();
    void gpuStepMagneticField();


private:
    Linear2DVector<DECIMAL> C_hxh;
    Linear2DVector<DECIMAL> C_hxe;
    Linear2DVector<DECIMAL> C_hyh;
    Linear2DVector<DECIMAL> C_hye;
    Linear2DVector<DECIMAL> C_eze;
    Linear2DVector<DECIMAL> C_ezh;

    void initializeCoefficientMatrix();

    MTL::Device *device;

    MTL::Buffer *bufferH_x;
    MTL::Buffer *bufferH_y;

    MTL::Buffer *bufferC_hxh;
    MTL::Buffer *bufferC_hxe;
    MTL::Buffer *bufferC_hyh;
    MTL::Buffer *bufferC_hye;
    MTL::Buffer *bufferC_eze;
    MTL::Buffer *bufferC_ezh;
    MTL::Buffer *bufferM;
    MTL::Buffer *bufferN;

    MTL::Buffer *bufferConductorField;

    MTL::Library *library;
    NS::Error *error;
    MTL::Function *eFieldFunction;
    MTL::Function *hxFieldFunction;
    MTL::Function *hyFieldFunction;
};

#endif
