#include "Simulation.hpp"
#include <numbers>

Simulation::Simulation(int m, int n, double deltaX, double deltaY, double deltaT)
    : M(m), N(n), deltaX(deltaX), deltaY(deltaY), deltaT(deltaT) {
    E_z = std::vector<std::vector<double>>(M, std::vector<double>(N, 0));
    H_x = std::vector<std::vector<double>>(M, std::vector<double>(N-1, 0));
    H_y = std::vector<std::vector<double>>(M-1, std::vector<double>(N, 0));

    initializeCoefficientMatrix();
}

void Simulation::stepElectricField() {
    for (int mm = 1; mm < M-1; ++mm) {
        for (int nn = 1; nn < N-1; ++nn) {
            E_z[mm][nn] = C_eze[mm][nn] * E_z[mm][nn] + 
                C_ezh[mm][nn] * ((H_y[mm][nn] - H_y[mm-1][nn]) - H_x[mm][nn] - H_x[mm][nn-1]);
        }
    }
}

void Simulation::stepMagneticField() {
    for (int mm = 0; mm < M; ++mm) {
        for (int nn = 0; nn < N-1; ++nn) {
            H_x[mm][nn] = C_hxh[mm][nn] * H_x[mm][nn] - 
                C_hxe[mm][nn] * (E_z[mm][nn+1] - E_z[mm][nn]);
        }
    } 
    for (int mm = 0; mm < M-1; ++mm) {
        for (int nn = 0; nn < N; ++nn) {
            H_y[mm][nn] = C_hyh[mm][nn] * H_y[mm][nn] +
                C_hye[mm][nn] * (E_z[mm+1][nn] - E_z[mm][nn]);
        }
    } 
}

void Simulation::stepSource(double time, double location) {
    // same source as given in the book
    double arg = std::numbers::pi * ((Cdtds * time - location) / 19.0);
    arg *= arg;
    arg = (1.0 - 2.0 * arg) * exp(-arg);
    E_z[M/2][N/2] = arg;
}

void Simulation::initializeCoefficientMatrix() {
    C_eze = std::vector<std::vector<double>>(M, std::vector<double>(N, 0));
    C_ezh = std::vector<std::vector<double>>(M, std::vector<double>(N, 0));

    C_hxh = std::vector<std::vector<double>>(M, std::vector<double>(N-1, 0));
    C_hxe = std::vector<std::vector<double>>(M, std::vector<double>(N-1, 0));

    C_hyh = std::vector<std::vector<double>>(M-1, std::vector<double>(N, 0));
    C_hye = std::vector<std::vector<double>>(M-1, std::vector<double>(N, 0));

    for (int mm = 0; mm < M; ++mm) {
        for (int nn = 0; nn < N; ++nn) {
            C_eze[mm][nn] = 1.0;
            C_ezh[mm][nn] = Cdtds * imp0;
        }
    }

    for (int mm = 0; mm < M; ++mm) {
        for (int nn = 0; nn < N-1; ++nn) {
            C_hxh[mm][nn] = 1.0;
            C_hxe[mm][nn] = Cdtds / imp0;
        }
    }

    for (int mm = 0; mm < M-1; ++mm) {
        for (int nn = 0; nn < N; ++nn) {
            C_hyh[mm][nn] = 1.0;
            C_hye[mm][nn] = Cdtds / imp0;
        }
    }
}

