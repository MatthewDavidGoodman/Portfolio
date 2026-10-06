#pragma once
#include "autonomy/types.hpp"
#include <random>
namespace autonomy {
class SensorSuite {
public:
    struct Params { double accel_sigma{0.03}; double gyro_sigma{0.002}; double pos_sigma{0.35}; double gps_dropout_after_s{1.0e9}; };
    SensorSuite();
    explicit SensorSuite(Params p);
    ImuSample imu(const VehicleState&s,const ControlCommand&u,double mass);
    PositionSample position(const VehicleState&s);
private: Params p_; std::mt19937 rng_; std::normal_distribution<double> n01_{0,1};
};
}
