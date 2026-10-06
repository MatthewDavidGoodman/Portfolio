#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <cmath>
#include <stdexcept>

namespace aero {
using Vec3 = Eigen::Vector3d;
using Mat3 = Eigen::Matrix3d;
using Quat = Eigen::Quaterniond;

inline Quat normalized(const Quat& q) {
    const double norm = q.norm();
    if (!q.coeffs().allFinite() || !std::isfinite(norm) || norm < 1e-12)
        throw std::invalid_argument("Quaternion must be finite and nonzero");
    Quat result = q;
    result.normalize();
    return result;
}

struct State {
    Vec3 position_ecef_m = Vec3::Zero();
    Vec3 velocity_ecef_mps = Vec3::Zero();
    Quat q_body_to_ecef = Quat::Identity();
    Vec3 omega_body_radps = Vec3::Zero();
};

inline void validate(const State& s) {
    if (!s.position_ecef_m.allFinite() || !s.velocity_ecef_mps.allFinite() ||
        !s.omega_body_radps.allFinite())
        throw std::invalid_argument("State must be finite");
    (void)normalized(s.q_body_to_ecef);
}

struct Wrench {
    Vec3 force_body_n = Vec3::Zero();
    Vec3 torque_body_nm = Vec3::Zero();
};
}
