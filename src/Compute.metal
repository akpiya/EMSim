#include <metal_stdlib>
using namespace metal;

kernel void updateElectricField(
    device const float* C_eze [[ buffer(0) ]],
    device const float* C_ezh [[ buffer(1) ]],
    device const float* H_y [[ buffer(2) ]],
    device const float* H_x [[ buffer(3) ]],
    device float* E_z [[ buffer(4) ]],
    constant int &M [[ buffer(5) ]],
    constant int &N [[ buffer(6) ]],
    uint idx [[ thread_position_in_grid ]]
) {
    int i = idx / N;
    int j = idx % N;
    if (1 <= i && i < M-1 && 1 <= j < N-1) {
        E_z[idx] = C_eze[idx] * E_z[idx] + C_ezh[idx] * ((H_y[idx] - H_y[idx - N]) - (H_x[idx] - H_x[idx - 1]));
    }
}


kernel void updateMagneticFieldX(
    device const float* C_hxh [[ buffer(0) ]],
    device const float* C_hxe [[ buffer(1) ]],
    device const float* E_z [[ buffer(2) ]],
    device float* H_x [[ buffer(3) ]],
    constant int &M [[ buffer(4) ]],
    constant int &N [[ buffer(5) ]],
    uint idx [[thread_position_in_grid]]
) {
    H_x[idx] = C_hxh[idx] * H_x[idx] - C_hxe[idx] * (E_z[idx+1] - E_z[idx]);
}


kernel void updateMagneticFieldY(
    device const float* C_hyh [[ buffer(0) ]],
    device const float* C_hye [[ buffer(1) ]],
    device const float* E_z [[ buffer(2) ]],
    device float* H_y [[ buffer(3) ]],
    constant int &M [[ buffer(4) ]],
    constant int &N [[ buffer(5) ]],
    uint idx [[thread_position_in_grid]]
) {
    H_y[idx] = C_hyh[idx] * H_y[idx] + C_hye[idx] * (E_z[idx + N] - E_z[idx]);
}
