#pragma once
#include "aero/environment.hpp"
namespace aero {
class RigidBody {
public:
    RigidBody(double mass_kg, const Mat3& inertia_body_kgm2,
              const State& initial_state, Environment environment = {},
              double drag_cd_area_m2 = 0.0);
    State state() const { return state_; }
    double time_s() const { return time_s_; }
    void set_state(const State& state);
    void step(double dt_s, const Wrench& wrench = {});
private:
    using Packed = Eigen::Matrix<double, 13, 1>;
    static Packed pack(const State& state);
    static State unpack(const Packed& value);
    Packed derivative(const Packed& value, const Wrench& wrench) const;
    double mass_kg_;
    Mat3 inertia_;
    Mat3 inverse_inertia_;
    State state_;
    Environment environment_;
    double drag_cd_area_m2_;
    double time_s_ = 0.0;
};
}
