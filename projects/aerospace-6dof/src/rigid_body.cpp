#include "aero/rigid_body.hpp"
#include <Eigen/Cholesky>
namespace aero {
RigidBody::RigidBody(double mass, const Mat3& inertia, const State& state,
                     Environment environment, double drag)
    : mass_kg_(mass), inertia_(inertia), environment_(environment),
      drag_cd_area_m2_(drag) {
    if (!std::isfinite(mass) || mass <= 0 || !std::isfinite(drag) || drag < 0)
        throw std::invalid_argument("Mass must be positive; Cd*area nonnegative");
    if (!inertia.allFinite() || !inertia.isApprox(inertia.transpose(), 1e-12))
        throw std::invalid_argument("Inertia must be finite and symmetric");
    Eigen::LLT<Mat3> factor(inertia);
    if (factor.info() != Eigen::Success)
        throw std::invalid_argument("Inertia must be positive definite");
    inverse_inertia_ = factor.solve(Mat3::Identity());
    set_state(state);
}
void RigidBody::set_state(const State& state) {
    validate(state);
    state_ = state;
    state_.q_body_to_ecef = normalized(state.q_body_to_ecef);
}
RigidBody::Packed RigidBody::pack(const State& s) {
    Packed x;
    x.segment<3>(0) = s.position_ecef_m;
    x.segment<3>(3) = s.velocity_ecef_mps;
    x.segment<4>(6) = s.q_body_to_ecef.coeffs();
    x.segment<3>(10) = s.omega_body_radps;
    return x;
}
State RigidBody::unpack(const Packed& x) {
    State s;
    s.position_ecef_m = x.segment<3>(0);
    s.velocity_ecef_mps = x.segment<3>(3);
    s.q_body_to_ecef.coeffs() = x.segment<4>(6);
    s.omega_body_radps = x.segment<3>(10);
    return s;
}
RigidBody::Packed RigidBody::derivative(const Packed& x, const Wrench& u) const {
    const State s = unpack(x);
    const Quat q = normalized(s.q_body_to_ecef);
    const Vec3 earth_rate = environment_.rotation_ecef_radps();
    Vec3 force_ecef = q * u.force_body_n;
    if (drag_cd_area_m2_ > 0) {
        const double rho = Environment::density_kgpm3(
            s.position_ecef_m.norm() - Environment::earth_radius_m);
        force_ecef -= 0.5 * rho * drag_cd_area_m2_ *
            s.velocity_ecef_mps.norm() * s.velocity_ecef_mps;
    }
    Packed dx;
    dx.segment<3>(0) = s.velocity_ecef_mps;
    dx.segment<3>(3) = environment_.gravity_ecef_mps2(s.position_ecef_m) +
        force_ecef / mass_kg_ - 2.0 * earth_rate.cross(s.velocity_ecef_mps) -
        earth_rate.cross(earth_rate.cross(s.position_ecef_m));
    const Vec3 w = s.omega_body_radps;
    const Quat body_rate(0, w.x(), w.y(), w.z());
    const Quat frame_rate(0, earth_rate.x(), earth_rate.y(), earth_rate.z());
    dx.segment<4>(6) = 0.5 * ((s.q_body_to_ecef * body_rate).coeffs() -
                                      (frame_rate * s.q_body_to_ecef).coeffs());
    dx.segment<3>(10) = inverse_inertia_ *
        (u.torque_body_nm - w.cross(inertia_ * w));
    return dx;
}
void RigidBody::step(double dt, const Wrench& u) {
    if (!std::isfinite(dt) || dt <= 0 || !std::isfinite(time_s_ + dt))
        throw std::invalid_argument("dt must be finite and positive");
    if (!u.force_body_n.allFinite() || !u.torque_body_nm.allFinite())
        throw std::invalid_argument("Wrench must be finite");
    const Packed x = pack(state_);
    const Packed k1 = derivative(x, u);
    const Packed k2 = derivative(x + 0.5*dt*k1, u);
    const Packed k3 = derivative(x + 0.5*dt*k2, u);
    const Packed k4 = derivative(x + dt*k3, u);
    State next = unpack(x + (dt/6.0)*(k1 + 2*k2 + 2*k3 + k4));
    validate(next);
    next.q_body_to_ecef = normalized(next.q_body_to_ecef);
    state_ = next;
    time_s_ += dt;
}
}
