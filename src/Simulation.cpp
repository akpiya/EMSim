#include <numbers>
#include "Simulation.hpp"
#include "Linear2DVector.hpp"

Simulation::Simulation(int m, int n, DECIMAL deltaX, DECIMAL deltaY, DECIMAL deltaT)
    : M(m), N(n), deltaX(deltaX), deltaY(deltaY), deltaT(deltaT), E_z(M, N), H_x(M, N-1), H_y(M-1, N),
        C_eze(M, N), C_ezh(M, N), C_hxh(M, N-1), C_hxe(M, N-1), C_hyh(M-1, N), C_hye(M-1, N), conductorField(M, N){
    initializeCoefficientMatrix();
}

void Simulation::stepElectricField() {
    for (int mm = 1; mm < M-1; ++mm) {
        for (int nn = 1; nn < N-1; ++nn) {
            if (conductorField.get(mm, nn) == 1)
                E_z.get(mm, nn) = 0;
            else
                E_z.get(mm, nn) = C_eze.get(mm, nn) * E_z.get(mm, nn) + 
                    C_ezh.get(mm, nn) * ((H_y.get(mm, nn) - H_y.get(mm-1, nn)) - (H_x.get(mm, nn) - H_x.get(mm, nn-1)));
        }
    }
}

void Simulation::stepMagneticField() {
    for (int mm = 0; mm < M; ++mm) {
        for (int nn = 0; nn < N-1; ++nn) {
            H_x.get(mm, nn) = C_hxh.get(mm, nn) * H_x.get(mm, nn) - 
                C_hxe.get(mm, nn) * (E_z.get(mm, nn+1) - E_z.get(mm,nn));
        }
    } 

    for (int mm = 0; mm < M-1; ++mm) {
        for (int nn = 0; nn < N; ++nn) {
            H_y.get(mm, nn) = C_hyh.get(mm, nn) * H_y.get(mm, nn) +
                C_hye.get(mm, nn) * (E_z.get(mm+1, nn) - E_z.get(mm, nn));
        }
    } 
}

void Simulation::stepRickertSource(DECIMAL time, DECIMAL location) {
    // same source as given in the book
    DECIMAL arg = std::numbers::pi * ((Cdtds * time - location) / 19.0);
    arg *= arg;
    arg = (1.0 - 2.0 * arg) * exp(-arg);
    E_z.get(M/2, N/2) = arg;
}

void Simulation::addConductorAt(int i, int j) {
    conductorField.get(i, j) = 1;
}

void Simulation::removeConductorAt(int i, int j) {
    conductorField.get(i, j) = 0;
}

void Simulation::initializeCoefficientMatrix() {
    for (int mm = 0; mm < M; ++mm) {
        for (int nn = 0; nn < N; ++nn) {
            C_eze.get(mm, nn) = 1.0;
            C_ezh.get(mm, nn) = Cdtds * imp0;
        }
    }

    for (int mm = 0; mm < M; ++mm) {
        for (int nn = 0; nn < N-1; ++nn) {
            C_hxh.get(mm, nn) = 1.0;
            C_hxe.get(mm, nn) = Cdtds / imp0;
        }
    }

    for (int mm = 0; mm < M-1; ++mm) {
        for (int nn = 0; nn < N; ++nn) {
            C_hyh.get(mm, nn) = 1.0;
            C_hye.get(mm, nn) = Cdtds / imp0;
        }
    }
}
