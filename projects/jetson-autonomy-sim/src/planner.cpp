#include "autonomy/planner.hpp"
namespace autonomy {
void WaypointPlanner::set_waypoints(std::vector<Waypoint>wps){wps_=std::move(wps);active_=0;}
Vec3 WaypointPlanner::desired_position(const Estimate&e){
    if(wps_.empty()) return e.position;
    if(active_<wps_.size() && norm(wps_[active_].position-e.position)<=wps_[active_].acceptance_radius) ++active_;
    if(active_>=wps_.size()) return wps_.back().position;
    return wps_[active_].position;
}
bool WaypointPlanner::complete() const {return !wps_.empty()&&active_>=wps_.size();}
}
