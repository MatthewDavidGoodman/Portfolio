#pragma once
#include "aero/types.hpp"
namespace aero {
struct Environment {
    static constexpr double earth_radius_m = 6378137.0;
    static constexpr double earth_mu_m3ps2 = 3.986004418e14;
    static constexpr double earth_j2 = 1.082629821313e-3;
    static constexpr double earth_rate_radps = 7.292115e-5;
    bool gravity_enabled = true;
    bool j2_enabled = true;
    bool rotation_enabled = true;
    Vec3 rotation_ecef_radps() const;
    Vec3 gravity_ecef_mps2(const Vec3& position_ecef_m) const;
    static double density_kgpm3(double spherical_altitude_m);
};
}
