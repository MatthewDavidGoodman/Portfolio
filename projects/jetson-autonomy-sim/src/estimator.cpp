#include "autonomy/estimator.hpp"
namespace autonomy {
void StateEstimator::reset(const Estimate&e){x_=e;}
void StateEstimator::predict(const ImuSample&imu,double dt){
    x_.body_rates=imu.gyro_body-x_.gyro_bias;
    x_.attitude=integrate_quaternion(x_.attitude,x_.body_rates,dt);
    const Vec3 world_specific=rotate(x_.attitude,imu.accel_body);
    const Vec3 a=world_specific+Vec3{0,0,-9.80665};
    x_.position += x_.velocity*dt + a*(0.5*dt*dt);
    x_.velocity += a*dt;
    x_.t=imu.t;
}
void StateEstimator::update_position(const PositionSample&p){
    if(!p.valid) return;
    const Vec3 r=p.position-x_.position;
    x_.position += pos_gain_*r;
    x_.velocity += vel_gain_*r;
}
}
