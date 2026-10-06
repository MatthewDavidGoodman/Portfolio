#pragma once
#include "autonomy/types.hpp"
namespace autonomy {
class RigidBodySimulator {
public:
    struct Params { double mass{18.0}; Vec3 inertia{2.2,2.6,3.0}; double linear_drag{0.12}; double angular_drag{0.15}; };
    RigidBodySimulator() = default;
    explicit RigidBodySimulator(Params p):p_(p){}
    void reset(const VehicleState&s={});
    void step(const ControlCommand&u,double dt);
    const VehicleState& state() const { return s_; }
private: Params p_; VehicleState s_{};
};
}
