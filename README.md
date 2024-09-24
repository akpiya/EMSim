# EMSim
EMSim is an electromagnetic simulation tool built in SFML. Built upon the FDTD method it, enables you to see the time evolution of the electric field as it interacts with mirrors and reflectors. To enable a fluid experience, EMSim uses compute kernels with Apple's Metal API (specifically metal-cpp) to offload algorithm-heavy jobs to the GPU.

## Demo
Here is a demo of the EM simulation. There is a source wave packet in the center which transmits outward. The user can draw reflectors with left-clicks and erase them with right-clicks. 

https://github.com/user-attachments/assets/c95801c1-7a99-4b09-b5db-559e00565cba

## Installation
Clone, build with cmake, and run on a Metal-compatible Apple PC.

## Performance
This project went through multiple iterations to optimize performance. This table summarizes a few benchmarks detailing this progress.



## Usage & Controls

## Author
Akash Piya
