#include "aero/gnc.hpp"
namespace aero {
WaypointGuidance::WaypointGuidance(const Vec3& target) : target_(target) {
    if (!target.allFinite()) throw std::invalid_argument("Target must be finite");
}
Quat WaypointGuidance::reference(const State& s) const {
    validate(s);
    const Vec3 delta = target_ - s.position_ecef_m;
    if (delta.norm() < 1e-6) return normalized(s.q_body_to_ecef);
    const Vec3 x = delta.normalized();
    Vec3 up = Vec3::UnitZ();
    if (std::abs(x.dot(up)) > 0.95) up = Vec3::UnitY();
    const Vec3 y = up.cross(x).normalized();
    Mat3 rotation;
    rotation.col(0) = x;
    rotation.col(1) = y;
    rotation.col(2) = x.cross(y);
    return normalized(Quat(rotation));
}
IdealNavigation::IdealNavigation(double p, double v, double a, double w,
                                 std::uint64_t seed)
    : sigma_(p,v,a,w), generator_(seed) {
    if (!sigma_.allFinite() || (sigma_.array() < 0).any())
        throw std::invalid_argument("Noise standard deviations must be nonnegative");
}
Vec3 IdealNavigation::noise(double sigma) {
    Vec3 sample = Vec3::Zero();
    if (sigma != 0) for (int i=0; i<3; ++i) sample[i] = sigma * gaussian_(generator_);
    return sample;
}
State IdealNavigation::estimate(const State& truth) {
    validate(truth);
    State s = truth;
    s.position_ecef_m += noise(sigma_[0]);
    s.velocity_ecef_mps += noise(sigma_[1]);
    s.omega_body_radps += noise(sigma_[3]);
    const Vec3 angle = noise(sigma_[2]);
    const double magnitude = angle.norm();
    s.q_body_to_ecef = normalized(s.q_body_to_ecef);
    if (magnitude > 0)
        s.q_body_to_ecef = normalized(s.q_body_to_ecef *
            Quat(Eigen::AngleAxisd(magnitude, angle/magnitude)));
    return s;
}
AttitudePD::AttitudePD(const Vec3& kp, const Vec3& kd, const Vec3& limit)
    : kp_(kp), kd_(kd), limit_(limit) {
    if (!kp.allFinite() || !kd.allFinite() || !limit.allFinite() ||
        (kp.array()<0).any() || (kd.array()<0).any() || (limit.array()<=0).any())
        throw std::invalid_argument("Gains must be nonnegative; limits positive");
}
Vec3 AttitudePD::error_rotation_vector(const Quat& current, const Quat& desired) {
    Quat error = normalized(current).conjugate() * normalized(desired);
    bool flip = error.w() < 0;
    if (error.w() == 0) {
        for (int i=0; i<3; ++i) if (error.vec()[i] != 0) {
            flip = error.vec()[i] < 0;
            break;
        }
    }
    if (flip) error.coeffs() *= -1;
    const double sine = error.vec().norm();
    if (sine < 1e-12) return 2.0 * error.vec();
    return (2.0 * std::atan2(sine, error.w()) / sine) * error.vec();
}
Vec3 AttitudePD::command(const State& s, const Quat& desired, const Vec3& rate) const {
    validate(s);
    if (!rate.allFinite()) throw std::invalid_argument("Reference rate must be finite");
    const Vec3 torque = kp_.cwiseProduct(error_rotation_vector(s.q_body_to_ecef, desired)) -
        kd_.cwiseProduct(s.omega_body_radps - rate);
    return torque.cwiseMax(-limit_).cwiseMin(limit_);
}
}
