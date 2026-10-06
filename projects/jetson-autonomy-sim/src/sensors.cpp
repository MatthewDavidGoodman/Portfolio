#include "autonomy/sensors.hpp"
namespace autonomy {
SensorSuite::SensorSuite():SensorSuite(Params{}){}
SensorSuite::SensorSuite(Params p):p_(p),rng_(42){}
ImuSample SensorSuite::imu(const VehicleState&s,const ControlCommand&u,double mass){
    Vec3 a{0,0,u.thrust/mass};
    a.x+=p_.accel_sigma*n01_(rng_); a.y+=p_.accel_sigma*n01_(rng_); a.z+=p_.accel_sigma*n01_(rng_);
    Vec3 g=s.body_rates; g.x+=p_.gyro_sigma*n01_(rng_); g.y+=p_.gyro_sigma*n01_(rng_); g.z+=p_.gyro_sigma*n01_(rng_);
    return {s.t,a,g};
}
PositionSample SensorSuite::position(const VehicleState&s){
    const bool valid=s.t<p_.gps_dropout_after_s;
    Vec3 p=s.position;
    if(valid){p.x+=p_.pos_sigma*n01_(rng_);p.y+=p_.pos_sigma*n01_(rng_);p.z+=p_.pos_sigma*n01_(rng_);}
    return {s.t,p,valid};
}
}
