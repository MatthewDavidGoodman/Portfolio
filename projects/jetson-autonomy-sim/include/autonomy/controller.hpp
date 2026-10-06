#pragma once
#include "autonomy/types.hpp"
namespace autonomy {
class PositionController {
public:
    struct Gains { double kp_xy{0.55}; double kd_xy{1.35}; double kp_z{0.9}; double kd_z{1.7}; double kp_yaw{1.2}; };
    PositionController() = default;
    explicit PositionController(Gains g):g_(g){}
    ControlCommand compute(const Estimate&e,const Vec3&target,double mass=18.0) const;
private: Gains g_;
};
}
