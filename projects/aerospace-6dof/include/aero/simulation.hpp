#pragma once
#include "aero/rigid_body.hpp"
#include "aero/gnc.hpp"
#include <vector>
namespace aero {
struct TelemetrySample {
    double time_s;
    State state;
    Vec3 attitude_error_rad;
    Vec3 commanded_torque_nm;
};
struct SimulationResult {
    std::vector<TelemetrySample> samples;
    bool surface_crossing = false;
    double maximum_quaternion_norm_error = 0;
    double maximum_torque_nm = 0;
};
SimulationResult simulate_attitude_hold(
    RigidBody vehicle, const Environment& environment, const AttitudePD& controller,
    const Quat& reference, double duration_s, double physics_step_s,
    int output_stride, bool control_enabled);
}
