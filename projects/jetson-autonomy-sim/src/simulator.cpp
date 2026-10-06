#include "autonomy/simulator.hpp"
namespace autonomy {
void RigidBodySimulator::reset(const VehicleState&s){s_=s;}
void RigidBodySimulator::step(const ControlCommand&u,double dt){
    const Vec3 body_force{0,0,u.thrust};
    const Vec3 world_force=rotate(s_.attitude,body_force)+Vec3{0,0,-9.80665*p_.mass}-p_.linear_drag*s_.velocity;
    const Vec3 a=world_force/p_.mass;
    s_.position += s_.velocity*dt + a*(0.5*dt*dt);
    s_.velocity += a*dt;
    Vec3 alpha{(u.torque.x-p_.angular_drag*s_.body_rates.x)/p_.inertia.x,
               (u.torque.y-p_.angular_drag*s_.body_rates.y)/p_.inertia.y,
               (u.torque.z-p_.angular_drag*s_.body_rates.z)/p_.inertia.z};
    s_.body_rates += alpha*dt;
    s_.attitude=integrate_quaternion(s_.attitude,s_.body_rates,dt);
    s_.t += dt;
}
}
