#include "aero/environment.hpp"
#include <algorithm>
namespace aero {
Vec3 Environment::rotation_ecef_radps() const {
    return Vec3(0.0, 0.0, rotation_enabled ? earth_rate_radps : 0.0);
}
Vec3 Environment::gravity_ecef_mps2(const Vec3& p) const {
    if (!p.allFinite()) throw std::invalid_argument("Position must be finite");
    if (!gravity_enabled) return Vec3::Zero();
    const double r = p.norm();
    if (!std::isfinite(r) || r < 1.0)
        throw std::domain_error("Gravity undefined near Earth center");
    Vec3 a = (-earth_mu_m3ps2 / (r*r*r)) * p;
    if (j2_enabled) {
        const double z2_r2 = p.z()*p.z()/(r*r);
        const double factor = 1.5 * earth_j2 * earth_mu_m3ps2 *
            earth_radius_m*earth_radius_m / std::pow(r, 5);
        a += factor * Vec3(p.x()*(5*z2_r2-1), p.y()*(5*z2_r2-1),
                           p.z()*(5*z2_r2-3));
    }
    return a;
}
double Environment::density_kgpm3(double h) {
    if (!std::isfinite(h)) throw std::invalid_argument("Altitude must be finite");
    return 1.225 * std::exp(-std::max(0.0, h) / 8500.0);
}
}
