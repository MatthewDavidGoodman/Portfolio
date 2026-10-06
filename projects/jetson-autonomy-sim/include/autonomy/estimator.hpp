#pragma once
#include "autonomy/types.hpp"
namespace autonomy {
class StateEstimator {
public:
    void reset(const Estimate&e={});
    void predict(const ImuSample&imu,double dt);
    void update_position(const PositionSample&p);
    const Estimate& estimate() const { return x_; }
private: Estimate x_{}; double pos_gain_{0.18}; double vel_gain_{0.06};
};
}
