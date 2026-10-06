#pragma once
#include <Eigen/Core>
namespace aero {
using State6 = Eigen::Matrix<double,6,1>;
class HarmonicGravity {
public:
    HarmonicGravity(double mu, double radius, const Eigen::MatrixXd& cosine,
                    const Eigen::MatrixXd& sine);
    Eigen::Vector3d acceleration(const Eigen::Vector3d& position) const;
private:
    double mu_, radius_;
    Eigen::MatrixXd cosine_, sine_;
};
State6 cr3bp_rhs(const State6& state, double mass_ratio);
Eigen::Matrix<double,6,6> cr3bp_jacobian(const State6& state, double mass_ratio);
double jacobi_constant(const State6& state, double mass_ratio);
Eigen::Vector3d third_body(const Eigen::Vector3d& position,
                           const Eigen::Vector3d& body_position, double mu);
Eigen::Vector3d schwarzschild(const Eigen::Vector3d& position,
                              const Eigen::Vector3d& velocity, double mu);
double illumination(const Eigen::Vector3d& position, const Eigen::Vector3d& sun,
                    double occultor_radius, double sun_radius);
State6 two_body_rhs(const State6& state, double mu);
Eigen::Matrix<double,6,6> two_body_jacobian(const State6& state, double mu);
}
