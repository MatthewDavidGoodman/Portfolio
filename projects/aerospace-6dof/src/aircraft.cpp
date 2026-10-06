#include "aero/aircraft.hpp"
namespace aero {
Quadrotor::Quadrotor(double mass):mass_(mass) {
    if(!std::isfinite(mass)||mass<=0)throw std::invalid_argument("Positive mass required");
    const double a=.23/std::sqrt(2.0);
    allocation_<<1,1,1,1,-a,a,a,-a,a,a,-a,-a,.02,-.02,.02,-.02;
    state_.motor_thrust_n.setConstant(mass_*9.80665/4);
}
void Quadrotor::set_state(const AircraftState& s) {
    if(!s.position_ned_m.allFinite()||!s.velocity_ned_mps.allFinite()||!s.omega_body_radps.allFinite()||
       !s.motor_thrust_n.allFinite()||(s.motor_thrust_n.array()<0).any()||(s.motor_thrust_n.array()>12).any())
        throw std::invalid_argument("Invalid aircraft state");
    state_=s;state_.q_body_to_ned=normalized(s.q_body_to_ned);
}
Quadrotor::Packed Quadrotor::pack() const {
    Packed x;x<<state_.position_ned_m,state_.velocity_ned_mps,state_.q_body_to_ned.coeffs(),
      state_.omega_body_radps,state_.motor_thrust_n;return x;
}
Quadrotor::Packed Quadrotor::derivative(const Packed& x,const Eigen::Vector4d& u,const Vec3& wind,const Eigen::Vector4d& efficiency) const {
    Quat raw;raw.coeffs()=x.segment<4>(6);const Quat q=normalized(raw);
    Vec3 velocity=x.segment<3>(3),omega=x.segment<3>(10),relative=velocity-wind;
    const Eigen::Vector4d actual=x.tail<4>().cwiseProduct(efficiency);
    const Eigen::Vector4d wrench=allocation_*actual;
    double altitude=std::max(0.0,-x[2]),rho=1.225*std::exp(-altitude/8500.0);
    Vec3 force=q*Vec3(0,0,-wrench[0])-.5*rho*.12*relative.norm()*relative;
    Packed d;d.head<3>()=velocity;d.segment<3>(3)=Vec3(0,0,9.80665)+force/mass_;
    d.segment<4>(6)=.5*(raw*Quat(0,omega.x(),omega.y(),omega.z())).coeffs();
    d.segment<3>(10)=inertia_.diagonal().cwiseInverse().cwiseProduct(wrench.tail<3>()-omega.cross(inertia_*omega)-.015*omega);
    d.tail<4>()=(12*u-x.tail<4>())/.06;return d;
}
void Quadrotor::step(double dt,const Eigen::Vector4d& command,const Vec3& wind,const Eigen::Vector4d& efficiency) {
    if(!std::isfinite(dt)||dt<=0||dt>.02||!command.allFinite()||!wind.allFinite()||!efficiency.allFinite()||
      (command.array()<0).any()||(command.array()>1).any()||(efficiency.array()<0).any()||(efficiency.array()>1).any())
       throw std::invalid_argument("Invalid aircraft input; dt in (0,.02], commands/effectiveness in [0,1]");
    const Packed x=pack(),k1=derivative(x,command,wind,efficiency),k2=derivative(x+.5*dt*k1,command,wind,efficiency),
      k3=derivative(x+.5*dt*k2,command,wind,efficiency),k4=derivative(x+dt*k3,command,wind,efficiency);
    Packed next=x+dt/6*(k1+2*k2+2*k3+k4);
    if(!next.allFinite())throw std::runtime_error("Aircraft state diverged");
    state_.position_ned_m=next.head<3>();state_.velocity_ned_mps=next.segment<3>(3);
    state_.q_body_to_ned.coeffs()=next.segment<4>(6);state_.q_body_to_ned.normalize();
    state_.omega_body_radps=next.segment<3>(10);state_.motor_thrust_n=next.tail<4>();
    on_ground_=ground_contact_enabled_ && state_.position_ned_m.z()>=0;
    if(on_ground_) {
        state_.position_ned_m.z()=0;
        state_.velocity_ned_mps.setZero();
        state_.omega_body_radps.setZero();
        state_.q_body_to_ned=Quat::Identity();
        specific_force_=Vec3(0,0,-9.80665);
        return;
    }
    specific_force_=state_.q_body_to_ned.conjugate()*(derivative(next,command,wind,efficiency).segment<3>(3)-Vec3(0,0,9.80665));
}
}
