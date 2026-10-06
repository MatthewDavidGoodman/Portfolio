#include "autonomy/mission.hpp"
namespace autonomy {
MissionRuntime::MissionRuntime(){
    VehicleState s; s.position={0,0,0}; sim_.reset(s); Estimate e; e.position=s.position; estimator_.reset(e);
}
void MissionRuntime::configure(std::vector<Waypoint>wps){planner_.set_waypoints(std::move(wps));}
void MissionRuntime::step(double dt){
    imu_accum_+=dt; pos_accum_+=dt;
    if(imu_accum_>=0.005){ auto m=sensors_.imu(sim_.state(),last_u_,18.0); estimator_.predict(m,imu_accum_); imu_accum_=0; }
    if(pos_accum_>=0.10){ estimator_.update_position(sensors_.position(sim_.state())); pos_accum_=0; }
    const auto target=planner_.desired_position(estimator_.estimate());
    last_u_=controller_.compute(estimator_.estimate(),target,18.0);
    sim_.step(last_u_,dt);
}
}
