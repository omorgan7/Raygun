# Raygun

![Pew pew!](https://raw.githubusercontent.com/omorgan7/Raygun/master/Results/raytrace.bmp)

A Ray tracer for the VFX coursework, with which the eventual goal will be to render the rotating spaceship scene from 2001 Space Odyssey.

## Methods

Has a phong shading model which is not used. Phong shading is somewhat limited to the 3 main techniques and can't give you global illumination or soft shadowing for free. Instead, Monte Carlo Path tracing is used, which makes this ray tracer (incredibly) slow. Unfortunately I ran out of time with this project, and couldn't implement methods to improve the render time for Monte Carlo in accordance with the project deadline. The above screenshot uses Monte Carlo Path tracing.

## Compilation

Compile all source files, with -fopenmp (or /openmp if you're on Win). This project has no dependencies. Compile with optimisation if you want- I found it actually makes little difference using Clang (YMMV). The Monte Carlo functions are threadsafe, hence compiling with -fopenmp will lead to order of magnitude speedups.


