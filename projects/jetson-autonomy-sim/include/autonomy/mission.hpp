#pragma once
#include "autonomy/controller.hpp"
#include "autonomy/estimator.hpp"
#include "autonomy/planner.hpp"
#include "autonomy/sensors.hpp"
#include "autonomy/simulator.hpp"
#include <vector>
namespace autonomy {
class MissionRuntime {
public:
    MissionRuntime();
    void configure(std::vector<Waypoint>wps);
    void step(double dt);
    bool complete() const { return planner_.complete(); }
    const VehicleState& truth() const { return sim_.state(); }
    const Estimate& estimate() const { return estimator_.estimate(); }
    std::size_t waypoint_index() const { return planner_.active_index(); }
private:
    RigidBodySimulator sim_; SensorSuite sensors_; StateEstimator estimator_; WaypointPlanner planner_; PositionController controller_;
    ControlCommand last_u_{}; double imu_accum_{0}; double pos_accum_{0};
};
}
