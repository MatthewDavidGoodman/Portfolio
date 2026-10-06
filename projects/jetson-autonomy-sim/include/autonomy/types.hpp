#pragma once
#include "autonomy/math.hpp"
#include <cstdint>

namespace autonomy {
struct VehicleState { double t{0}; Vec3 position{}; Vec3 velocity{}; Quaternion attitude{}; Vec3 body_rates{}; };
struct ControlCommand { double thrust{0}; Vec3 torque{}; };
struct ImuSample { double t{0}; Vec3 accel_body{}; Vec3 gyro_body{}; };
struct PositionSample { double t{0}; Vec3 position{}; bool valid{true}; };
struct Estimate { double t{0}; Vec3 position{}; Vec3 velocity{}; Quaternion attitude{}; Vec3 body_rates{}; Vec3 gyro_bias{}; };
struct Waypoint { Vec3 position{}; double acceptance_radius{2.0}; };
struct PipelineStats { std::uint64_t frames{0}; double capture_ms{0}; double preprocess_ms{0}; double inference_ms{0}; double total_ms{0}; };
}
