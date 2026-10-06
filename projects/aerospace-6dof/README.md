# Aerospace Lab — 6-DoF / Orbital Simulation

A research-oriented aerospace simulation stack built around a C++17 / Eigen dynamics core, with Python astrodynamics and verification modules.

This public portfolio snapshot focuses on source and validation logic. Generated telemetry, replay artifacts, large reference outputs, and local build products are intentionally excluded.

## Core areas

- rigid-body 6-DoF dynamics with RK4 integration
- quaternion attitude dynamics and PD control
- quadrotor dynamics and motor lag
- J2 / harmonic gravity research modules
- CR3BP and two-body dynamics
- SGP4/TLE catalog handling
- covariance and conjunction-analysis tooling
- ArduPilot SITL integration in the full working project

## Build the C++ core

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Requires Eigen 3.4.

## Engineering boundaries

This is a research and verification project, not a flight-qualified digital twin or operational conjunction system. The model-status document distinguishes implemented/tested pieces from experimental interfaces and explicitly unimplemented work.

See [STATUS.md](STATUS.md).
