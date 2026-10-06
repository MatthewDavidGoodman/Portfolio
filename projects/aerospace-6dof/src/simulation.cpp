#include "aero/simulation.hpp"
#include <algorithm>
namespace aero {
SimulationResult simulate_attitude_hold(
    RigidBody vehicle, const Environment& environment, const AttitudePD& controller,
    const Quat& reference, double duration, double dt, int stride, bool enabled) {
    if (!std::isfinite(duration) || duration <= 0 || !std::isfinite(dt) || dt <= 0 ||
        stride < 1 || duration/dt > 400000)
        throw std::invalid_argument("Invalid run length, integration step, or output stride");
    const Quat desired = normalized(reference);
    const int steps = static_cast<int>(std::ceil(duration/dt));
    const double actual_dt = duration / steps;
    SimulationResult result;
    result.samples.reserve(static_cast<std::size_t>(steps/stride + 2));
    for (int index=0; index<=steps; ++index) {
        const State state = vehicle.state();
        Wrench wrench;
        const Vec3 desired_rate = state.q_body_to_ecef.conjugate() *
            environment.rotation_ecef_radps();
        if (enabled) wrench.torque_body_nm = controller.command(state, desired, desired_rate);
        result.maximum_quaternion_norm_error = std::max(result.maximum_quaternion_norm_error,
            std::abs(state.q_body_to_ecef.norm()-1.0));
        result.maximum_torque_nm = std::max(result.maximum_torque_nm,
            wrench.torque_body_nm.cwiseAbs().maxCoeff());
        const bool impact = state.position_ecef_m.norm() <= Environment::earth_radius_m;
        if (index % stride == 0 || index == steps || impact)
            result.samples.push_back({vehicle.time_s(), state,
                AttitudePD::error_rotation_vector(state.q_body_to_ecef, desired),
                wrench.torque_body_nm});
        if (impact) { result.surface_crossing = true; break; }
        if (index < steps) vehicle.step(actual_dt, wrench);
    }
    return result;
}
