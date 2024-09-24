#include <numbers>
#include <algorithm>
#include <cstring>

#define NS_PRIVATE_IMPLEMENTATION
#define CA_PRIVATE_IMPLEMENTATION
#define MTL_PRIVATE_IMPLEMENTATION

#include "Simulation.hpp"
#include "Linear2DVector.hpp"
#include "Metal/MTLResource.hpp"

const char *computeCode = R"(
    #include <metal_stdlib>
    using namespace metal;
    
    kernel void updateElectricField(
        device const float* C_eze [[ buffer(0) ]],
        device const float* C_ezh [[ buffer(1) ]],
        device const float* H_y [[ buffer(2) ]],
        device const float* H_x [[ buffer(3) ]],
        device const char* conductorField [[ buffer(4) ]],
        device float* E_z [[ buffer(5) ]],
        constant int &M [[ buffer(6) ]],
        constant int &N [[ buffer(7) ]],
        uint idx [[ thread_position_in_grid ]]
    ) {
        int i = idx / N;
        int j = idx % N;
        if (1 <= i && i < M-1 && 1 <= j < N-1) {
            if (conductorField[idx] == 0) {
                E_z[idx] = C_eze[idx] * E_z[idx] + C_ezh[idx] * ((H_y[idx] - H_y[idx - N]) - (H_x[idx] - H_x[idx - 1]));
            } else {
                E_z[idx] = 0.0;
            }
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
    })";


Simulation::Simulation(int m, int n, DECIMAL deltaX, DECIMAL deltaY, DECIMAL deltaT)
    : M(m), N(n), deltaX(deltaX), deltaY(deltaY), deltaT(deltaT), E_z(M, N), H_x(M, N-1), H_y(M-1, N),
        C_eze(M, N), C_ezh(M, N), C_hxh(M, N-1), C_hxe(M, N-1), C_hyh(M-1, N), C_hye(M-1, N), conductorField(M, N) {
    initializeCoefficientMatrix();

    device = MTL::CreateSystemDefaultDevice();
    bufferM = device->newBuffer(&M, sizeof(int), MTL::ResourceStorageModeShared);
    bufferN = device->newBuffer(&N, sizeof(int), MTL::ResourceStorageModeShared);

    bufferE_z = device->newBuffer(E_z.data.data(), E_z.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferH_x = device->newBuffer(H_x.data.data(), H_x.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferH_y = device->newBuffer(H_y.data.data(), H_y.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);

    bufferC_eze = device->newBuffer(C_eze.data.data(), C_eze.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferC_ezh = device->newBuffer(C_ezh.data.data(), C_ezh.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferC_hxe = device->newBuffer(C_hxe.data.data(), C_hxe.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferC_hxh = device->newBuffer(C_hxh.data.data(), C_hxh.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferC_hye = device->newBuffer(C_hye.data.data(), C_hye.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);
    bufferC_hyh = device->newBuffer(C_hyh.data.data(), C_hyh.data.size() * sizeof(DECIMAL), MTL::ResourceStorageModeShared);

    bufferConductorField = device->newBuffer(conductorField.data.data(), conductorField.data.size() * sizeof(char), MTL::ResourceStorageModeShared);

    error = nullptr;
    library = device->newLibrary(NS::String::string(computeCode, NS::UTF8StringEncoding), nullptr, &error);
    error = nullptr;

    eFieldFunction = library->newFunction(NS::String::string("updateElectricField", NS::UTF8StringEncoding)); 
    hxFieldFunction = library->newFunction(NS::String::string("updateMagneticFieldX", NS::UTF8StringEncoding)); 
    hyFieldFunction = library->newFunction(NS::String::string("updateMagneticFieldY", NS::UTF8StringEncoding));
}


void Simulation::gpuStepElectricField() {
    error = nullptr;
    MTL::ComputePipelineState *pipelineState = device->newComputePipelineState(eFieldFunction, &error);
    MTL::CommandQueue *commandQueue = device->newCommandQueue();
    MTL::CommandBuffer *commandBuffer = commandQueue->commandBuffer();
    
    MTL::ComputeCommandEncoder *encoder = commandBuffer->computeCommandEncoder();

    encoder->setComputePipelineState(pipelineState);
    encoder->setBuffer(bufferC_eze, 0, 0);
    encoder->setBuffer(bufferC_ezh, 0, 1);
    encoder->setBuffer(bufferH_y, 0, 2);
    encoder->setBuffer(bufferH_x, 0, 3);
    encoder->setBuffer(bufferConductorField, 0, 4);
    encoder->setBuffer(bufferE_z, 0, 5);
    encoder->setBuffer(bufferM, 0, 6);
    encoder->setBuffer(bufferN, 0, 7);

    MTL::Size gridSize = MTL::Size(N * M, 1, 1);
    auto max_threads = (int) pipelineState->maxTotalThreadsPerThreadgroup();
    MTL::Size threadGroupSize = MTL::Size(std::min(max_threads, N * M), 1, 1);
    
    encoder->dispatchThreads(gridSize, threadGroupSize);
    encoder->endEncoding();

    commandBuffer->commit();

    commandBuffer->waitUntilCompleted();

    pipelineState->release();
    commandQueue->release();
    commandBuffer->release();
    encoder->release();
}


void Simulation::gpuStepMagneticField() {
    error = nullptr;
    MTL::ComputePipelineState *xpipelineState = device->newComputePipelineState(hxFieldFunction, &error);
    MTL::ComputePipelineState *ypipelineState = device->newComputePipelineState(hyFieldFunction, &error);

    MTL::CommandQueue *xcommandQueue = device->newCommandQueue();
    MTL::CommandQueue *ycommandQueue = device->newCommandQueue();

    MTL::CommandBuffer *xcommandBuffer = xcommandQueue->commandBuffer();
    MTL::CommandBuffer *ycommandBuffer = ycommandQueue->commandBuffer();

    MTL::ComputeCommandEncoder *xencoder = xcommandBuffer->computeCommandEncoder();
    MTL::ComputeCommandEncoder *yencoder = ycommandBuffer->computeCommandEncoder();

    xencoder->setComputePipelineState(xpipelineState);
    xencoder->setBuffer(bufferC_hxh, 0, 0);
    xencoder->setBuffer(bufferC_hxe, 0, 1);
    xencoder->setBuffer(bufferE_z, 0, 2);
    xencoder->setBuffer(bufferH_x, 0, 3);
    xencoder->setBuffer(bufferM, 0, 4);
    xencoder->setBuffer(bufferN, 0, 5);

    MTL::Size xgridSize = MTL::Size(M*(N-1), 1, 1);
    auto max_threads = (int) xpipelineState->maxTotalThreadsPerThreadgroup();
    MTL::Size xthreadGroupSize = MTL::Size(std::min(max_threads, M*(N-1)), 1, 1);

    xencoder->dispatchThreads(xgridSize, xthreadGroupSize);
    xencoder->endEncoding();

    yencoder->setComputePipelineState(ypipelineState);
    yencoder->setBuffer(bufferC_hyh, 0, 0);
    yencoder->setBuffer(bufferC_hye, 0, 1);
    yencoder->setBuffer(bufferE_z, 0, 2);
    yencoder->setBuffer(bufferH_y, 0, 3);
    yencoder->setBuffer(bufferM, 0, 4);
    yencoder->setBuffer(bufferN, 0, 5);

    MTL::Size ygridSize = MTL::Size((M-1)*N, 1, 1);
    MTL::Size ythreadGroupSize = MTL::Size(std::min(max_threads, (M-1)*N), 1, 1);

    yencoder->dispatchThreads(ygridSize, ythreadGroupSize);
    yencoder->endEncoding();

    xcommandBuffer->commit();
    ycommandBuffer->commit();

    xcommandBuffer->waitUntilCompleted();
    ycommandBuffer->waitUntilCompleted();

    xencoder->release();
    yencoder->release();

    xcommandBuffer->release();
    ycommandBuffer->release();

    xcommandQueue->release();
    ycommandQueue->release();

    xpipelineState->release();
    ypipelineState->release();
}


Simulation::~Simulation() {
    bufferE_z->release();
    bufferH_x->release();
    bufferH_y->release();
    bufferC_hxh->release();
    bufferC_hxe->release();
    bufferC_hyh->release();
    bufferC_hye->release();
    bufferC_eze->release();
    bufferC_ezh->release();
    bufferM->release();
    bufferN->release();
    eFieldFunction->release();
    hxFieldFunction->release();
    hyFieldFunction->release();
    library->release();
    device->release();
};


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
    DECIMAL* gpuE_z = static_cast<DECIMAL*> (bufferE_z->contents());
    gpuE_z[M/2 * N + N/2] = arg;
}

void Simulation::addConductorAt(int i, int j) {
    conductorField.get(i, j) = 1;
    char *p = static_cast<char*>(bufferConductorField->contents());
    p[i * N + j] = 1;
}

void Simulation::removeConductorAt(int i, int j) {
    conductorField.get(i, j) = 0;
    char *p = static_cast<char*>(bufferConductorField->contents());
    p[i * N + j] = 0;
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
