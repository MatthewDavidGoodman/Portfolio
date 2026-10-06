#pragma once
#include "aero/types.hpp"
#include <cstdint>
#include <random>
namespace aero {
class WaypointGuidance {
public:
    explicit WaypointGuidance(const Vec3& target_ecef_m);
    Quat reference(const State& state) const;
private:
    Vec3 target_;
};
class IdealNavigation {
public:
    IdealNavigation(double position_sigma_m = 0, double velocity_sigma_mps = 0,
                    double attitude_sigma_rad = 0, double rate_sigma_radps = 0,
                    std::uint64_t seed = 0);
    State estimate(const State& truth);
private:
    Vec3 noise(double sigma);
    Eigen::Vector4d sigma_;
    std::mt19937_64 generator_;
    std::normal_distribution<double> gaussian_{0.0, 1.0};
};
class AttitudePD {
public:
    AttitudePD(const Vec3& kp_nm, const Vec3& kd_nms, const Vec3& torque_limit_nm);
    Vec3 command(const State& estimate, const Quat& reference_body_to_ecef,
                 const Vec3& desired_omega_body_radps) const;
    static Vec3 error_rotation_vector(const Quat& current, const Quat& desired);
private:
    Vec3 kp_, kd_, limit_;
};
}
