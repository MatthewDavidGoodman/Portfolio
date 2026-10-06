#include "autonomy/controller.hpp"
namespace autonomy {
ControlCommand PositionController::compute(const Estimate&e,const Vec3&target,double mass) const {
    const Vec3 ep=target-e.position;
    Vec3 a_cmd{clamp(g_.kp_xy*ep.x-g_.kd_xy*e.velocity.x,-3.5,3.5),
               clamp(g_.kp_xy*ep.y-g_.kd_xy*e.velocity.y,-3.5,3.5),
               clamp(g_.kp_z*ep.z-g_.kd_z*e.velocity.z+9.80665,4.0,15.0)};
    const double thrust=mass*clamp(norm(a_cmd),0.0,20.0);
    const Vec3 z_des=normalized(a_cmd);
    const Vec3 z_body=rotate(e.attitude,{0,0,1});
    const Vec3 attitude_error=cross(z_body,z_des);
    Vec3 torque{15.0*attitude_error.x - 6.0*e.body_rates.x,
                15.0*attitude_error.y - 6.0*e.body_rates.y,
                -g_.kp_yaw*yaw_from_q(e.attitude) - 3.0*e.body_rates.z};
    return {thrust,torque};
}
}
