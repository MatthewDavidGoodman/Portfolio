#pragma once
#include "autonomy/types.hpp"
#include <vector>
namespace autonomy {
class WaypointPlanner {
public:
    void set_waypoints(std::vector<Waypoint> wps);
    Vec3 desired_position(const Estimate&e);
    bool complete() const;
    std::size_t active_index() const { return active_; }
private: std::vector<Waypoint> wps_; std::size_t active_{0};
};
}
