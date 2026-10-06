#include "aero/astrodynamics.hpp"
#include <unsupported/Eigen/AutoDiff>
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>
namespace aero {
namespace {
void valid(const Eigen::Vector3d& r) {
    if (!r.allFinite() || r.norm()<1e-12) throw std::invalid_argument("Invalid position");
}
void valid_mu(double mu) {
    if (!std::isfinite(mu) || mu<=0 || mu>=0.5) throw std::invalid_argument("Mass ratio must be in (0,0.5)");
}
}
HarmonicGravity::HarmonicGravity(double mu,double radius,const Eigen::MatrixXd& c,const Eigen::MatrixXd& s)
 : mu_(mu),radius_(radius),cosine_(c),sine_(s) {
    if (!std::isfinite(mu) || !std::isfinite(radius) || mu<=0 || radius<=0 || c.rows()<1 ||
        c.rows()>21 || c.rows()!=c.cols() || s.rows()!=c.rows() || s.cols()!=c.cols() ||
        !c.allFinite() || !s.allFinite()) throw std::invalid_argument("Invalid harmonic model (degree 0–20)");
    for(int n=0;n<c.rows();++n) for(int m=0;m<=n;++m) {
        double norm=std::sqrt((m==0?1.0:2.0)*(2*n+1)*std::exp(std::lgamma(n-m+1.0)-std::lgamma(n+m+1.0)));
        cosine_(n,m)*=norm; sine_(n,m)*=norm;
    }
}
Eigen::Vector3d HarmonicGravity::acceleration(const Eigen::Vector3d& p) const {
    valid(p);
    using AD=Eigen::AutoDiffScalar<Eigen::Vector3d>;
    AD x(p.x()),y(p.y()),z(p.z());
    x.derivatives()=Eigen::Vector3d::UnitX(); y.derivatives()=Eigen::Vector3d::UnitY(); z.derivatives()=Eigen::Vector3d::UnitZ();
    AD r2=x*x+y*y+z*z, r=sqrt(r2), f=radius_/r2, g=radius_*radius_/r2;
    int count=static_cast<int>(cosine_.rows());
    std::vector<std::vector<AD>> v(count,std::vector<AD>(count,AD(0))),w=v;
    v[0][0]=radius_/r;
    for(int n=1;n<count;++n) {
        v[n][n]=(2*n-1)*f*(x*v[n-1][n-1]-y*w[n-1][n-1]);
        w[n][n]=(2*n-1)*f*(x*w[n-1][n-1]+y*v[n-1][n-1]);
        for(int m=0;m<n;++m) {
            v[n][m]=(2*n-1.0)/(n-m)*f*z*v[n-1][m];
            w[n][m]=(2*n-1.0)/(n-m)*f*z*w[n-1][m];
            if(n>=m+2) {
                v[n][m]-=(n+m-1.0)/(n-m)*g*v[n-2][m];
                w[n][m]-=(n+m-1.0)/(n-m)*g*w[n-2][m];
            }
        }
    }
    AD potential(0); potential.derivatives().setZero();
    for(int n=0;n<count;++n) for(int m=0;m<=n;++m)
        potential+=mu_/radius_*(cosine_(n,m)*v[n][m]+sine_(n,m)*w[n][m]);
    return potential.derivatives();
}
State6 cr3bp_rhs(const State6& s,double mu) {
    valid_mu(mu); if(!s.allFinite()) throw std::invalid_argument("Nonfinite state");
    Eigen::Vector3d r=s.head<3>(),a=r+Eigen::Vector3d(mu,0,0),b=r-Eigen::Vector3d(1-mu,0,0);
    valid(a);valid(b);
    State6 d; d.head<3>()=s.tail<3>();
    d.tail<3>()=-(1-mu)*a/std::pow(a.norm(),3)-mu*b/std::pow(b.norm(),3);
    d[3]+=s[0]+2*s[4];d[4]+=s[1]-2*s[3]; return d;
}
Eigen::Matrix<double,6,6> cr3bp_jacobian(const State6& s,double mu) {
    cr3bp_rhs(s,mu);
    Eigen::Matrix<double,6,6> a=Eigen::Matrix<double,6,6>::Zero();
    a.block<3,3>(0,3).setIdentity();
    Eigen::Matrix3d h=Eigen::Vector3d(1,1,0).asDiagonal();
    for(int k=0;k<2;++k) {
        Eigen::Vector3d r=s.head<3>()+Eigen::Vector3d(k?mu-1:mu,0,0);
        double mass=k?mu:1-mu, d=r.norm();
        h+=mass*(3*r*r.transpose()/std::pow(d,5)-Eigen::Matrix3d::Identity()/std::pow(d,3));
    }
    a.block<3,3>(3,0)=h; a(3,4)=2;a(4,3)=-2;return a;
}
double jacobi_constant(const State6& s,double mu) {
    cr3bp_rhs(s,mu);
    return s[0]*s[0]+s[1]*s[1]+2*(1-mu)/(s.head<3>()+Eigen::Vector3d(mu,0,0)).norm()
        +2*mu/(s.head<3>()-Eigen::Vector3d(1-mu,0,0)).norm()-s.tail<3>().squaredNorm();
}
Eigen::Vector3d third_body(const Eigen::Vector3d& r,const Eigen::Vector3d& body,double mu) {
    valid(body);Eigen::Vector3d delta=body-r;valid(delta);
    if(!r.allFinite() || !std::isfinite(mu) || mu<=0) throw std::invalid_argument("Invalid third body");
    return mu*(delta/std::pow(delta.norm(),3)-body/std::pow(body.norm(),3));
}
Eigen::Vector3d schwarzschild(const Eigen::Vector3d& r,const Eigen::Vector3d& v,double mu) {
    valid(r); if(!v.allFinite() || !std::isfinite(mu) || mu<=0) throw std::invalid_argument("Invalid relativistic state");
    constexpr double c=299792458.0;
    return mu/(c*c*std::pow(r.norm(),3))*((4*mu/r.norm()-v.squaredNorm())*r+4*r.dot(v)*v);
}
double illumination(const Eigen::Vector3d& r,const Eigen::Vector3d& sun,double re,double rs) {
    valid(r);Eigen::Vector3d to_sun=sun-r; valid(to_sun);
    if(!std::isfinite(re) || !std::isfinite(rs) || re<=0 || rs<=0 || r.norm()<=re || to_sun.norm()<=rs)
        throw std::invalid_argument("Invalid eclipse geometry");
    double a=std::asin(rs/to_sun.norm()),b=std::asin(re/r.norm());
    double d=std::acos(std::clamp((-r).dot(to_sun)/(r.norm()*to_sun.norm()),-1.0,1.0));
    constexpr double pi=3.14159265358979323846;
    if(d>=a+b) return 1;
    if(d<=std::abs(a-b)) return b>=a?0:1-b*b/(a*a);
    double overlap=a*a*std::acos(std::clamp((d*d+a*a-b*b)/(2*d*a),-1.0,1.0))
        +b*b*std::acos(std::clamp((d*d+b*b-a*a)/(2*d*b),-1.0,1.0))
        -0.5*std::sqrt(std::max(0.0,(-d+a+b)*(d+a-b)*(d-a+b)*(d+a+b)));
    return std::clamp(1-overlap/(pi*a*a),0.0,1.0);
}
State6 two_body_rhs(const State6& state, double mu) {
    if(!state.allFinite() || !std::isfinite(mu) || mu<=0 || state.head<3>().norm()<1)
        throw std::invalid_argument("Finite SI state and positive gravitational parameter required");
    State6 result;
    result.head<3>()=state.tail<3>();
    result.tail<3>()=-mu*state.head<3>()/std::pow(state.head<3>().norm(),3);
    return result;
}
Eigen::Matrix<double,6,6> two_body_jacobian(const State6& state, double mu) {
    two_body_rhs(state,mu);
    const Eigen::Vector3d r=state.head<3>();
    const double radius=r.norm();
    Eigen::Matrix<double,6,6> a=Eigen::Matrix<double,6,6>::Zero();
    a.topRightCorner<3,3>().setIdentity();
    a.bottomLeftCorner<3,3>()=mu*(3*r*r.transpose()/std::pow(radius,5)-Eigen::Matrix3d::Identity()/std::pow(radius,3));
    return a;
}
}
