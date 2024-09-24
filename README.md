# EMSim
EMSim is an electromagnetic simulation tool built in SFML. Using the FDTD method it, enables you to see the time evolution of the electric field as it interacts with mirrors and reflectors. To enable a fluid experience, EMSim uses compute kernels with Apple's Metal API (specifically metal-cpp) to offload algorithm-heavy jobs to the GPU. For an introduction to Yee's algorithm see the following [pdf](https://my.ece.utah.edu/~ece6340/LECTURES/lecture%2014/FDTD.pdf).

## Demo
Here is a demo of the EM simulation. There is a source wave packet in the center which transmits outward. The user can draw reflectors with left-clicks and erase them with right-clicks. 

https://github.com/user-attachments/assets/c95801c1-7a99-4b09-b5db-559e00565cba

## Installation
Clone, build with cmake, and run on a Metal-compatible Apple PC.

## Performance
This project went through multiple iterations to optimize performance. This table summarizes a few benchmarks detailing this progress. All benchmarks are done on a 301x301 grid with a simulation time-step up to 100. 
| Version | Benchmark Time |
|-------| ---------------|
| Baseline, single threaded | 259746880 µs |
| Optimized drawing calls | 13429895 µs |
| GPU compute kernels with lots of memcpys | 19651910 µs | 
| GPU compute kernels without memcpys | 14473435 µs |
| GPU compute kernel + multithreaded displaying | 9427397 µs |

Going from the optimized, single threaded simulation to the optimized GPU powered one resulted in a 96% decrease in simulation time.

Benchmarking with a grid size of 1001x1001 here are the results:

| Version | Benchmark Time |
| ------- | -------------- |
| Baseline | 71879879 µs |
| Fully GPU optimized | 19001845 µs | 

At this larger scale, the GPU provides a 4x boost over the baseline. For a 3001x3001 grid, a GPU-enabled simulation step is 20x faster than that of a single-threaded CPU step.

## Usage & Controls
| Key | Control |
| --- | ------- |
| Space | Pause/Unpause |
| Left-click | Place mirror |
| Right-click | Remove mirror |

## Author
Akash Piya
