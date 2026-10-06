#pragma once
#include "aero/types.hpp"
namespace aero {
struct AircraftState {
    Vec3 position_ned_m=Vec3(0,0,-10);
    Vec3 velocity_ned_mps=Vec3::Zero();
    Quat q_body_to_ned=Quat::Identity();
    Vec3 omega_body_radps=Vec3::Zero();
    Eigen::Vector4d motor_thrust_n=Eigen::Vector4d::Zero();
};
class Quadrotor {
public:
    explicit Quadrotor(double mass_kg=2.0);
    AircraftState state() const {return state_;}
    void set_state(const AircraftState& state);
    void step(double dt,const Eigen::Vector4d& commands,const Vec3& wind_ned,const Eigen::Vector4d& effectiveness);
    void enable_ground_contact(bool enabled) {ground_contact_enabled_=enabled;}
    bool on_ground() const {return on_ground_;}
    Vec3 specific_force_body() const {return specific_force_;}
    Eigen::Matrix4d allocation() const {return allocation_;}
private:
    using Packed=Eigen::Matrix<double,17,1>;
    Packed derivative(const Packed&,const Eigen::Vector4d&,const Vec3&,const Eigen::Vector4d&) const;
    Packed pack() const;
    AircraftState state_;
    double mass_;
    bool ground_contact_enabled_=false;
    bool on_ground_=false;
    Mat3 inertia_=Vec3(.035,.035,.06).asDiagonal();
    Eigen::Matrix4d allocation_;
    Vec3 specific_force_=Vec3(0,0,-9.80665);
};
}
