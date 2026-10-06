# Model status and engineering boundaries

## Implemented and exercised

- **Rigid-body 6-DoF:** C++ Eigen RK4, Hamilton quaternions, ECEF forces, attitude PD.
- **Aircraft model:** separate local-NED quadrotor, body-FRD convention, 17-state RK4 including four motor thrust states.
- **Aircraft forces:** constant gravity, first-order motor lag, saturation, quadratic drag, exponential low-altitude density, prescribed wind.
- **GNC:** position/velocity loop, quaternion attitude error, rate damping, clipped motor allocation.
- **Gravity research:** EGM96 coefficients through 10x10 with a Cartesian solid-harmonic formulation in the full project.
- **Atmosphere research:** NRLMSISE-00 density adapter in the full project.
- **Orbital propagation:** adaptive numerical propagation, CR3BP equations/Jacobian/Jacobi diagnostics, state-transition/covariance machinery.
- **Conjunction research:** SGP4 near/deep-space branches, TLE validation, closest-approach refinement, and a 2-D Gaussian encounter-plane collision-probability calculation when covariance is explicitly supplied.
- **SITL integration:** the full working project includes an ArduPilot closed-loop acceptance path.

## Important boundaries

- Aircraft propulsion/aerodynamics are simplified and not calibrated from flight data.
- The navigation path is not yet a full attitude/bias error-state EKF.
- TLEs are **not** treated as covariance sources.
- The conjunction implementation is research tooling, not an operational risk assessment.
- The orbital pipeline is not yet benchmarked end-to-end against Orekit/Tudat.
- Generated run outputs and large replay/reference artifacts are excluded from this public snapshot.

## Not implemented / not claimed

Flight qualification, production deployment, a validated YORP thermal solver, SPH porous-impact physics, autonomous station keeping, decentralized collision-avoidance optimal control, PX4 closed-loop integration, or a calibrated aircraft digital twin.
