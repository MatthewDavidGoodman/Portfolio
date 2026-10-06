#pragma once
#include <algorithm>
#include <cmath>
#include <ostream>

namespace autonomy {
struct Vec3 {
    double x{0}, y{0}, z{0};
    Vec3 operator+(const Vec3& o) const { return {x+o.x,y+o.y,z+o.z}; }
    Vec3 operator-(const Vec3& o) const { return {x-o.x,y-o.y,z-o.z}; }
    Vec3 operator*(double s) const { return {x*s,y*s,z*s}; }
    Vec3 operator/(double s) const { return {x/s,y/s,z/s}; }
    Vec3& operator+=(const Vec3& o){ x+=o.x; y+=o.y; z+=o.z; return *this; }
};
inline Vec3 operator*(double s, const Vec3& v){ return v*s; }
inline double dot(const Vec3&a,const Vec3&b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
inline Vec3 cross(const Vec3&a,const Vec3&b){ return {a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
inline double norm(const Vec3&v){ return std::sqrt(dot(v,v)); }
inline Vec3 normalized(const Vec3&v){ const auto n=norm(v); return n>1e-12?v/n:Vec3{}; }
inline double clamp(double x,double lo,double hi){ return std::max(lo,std::min(hi,x)); }

struct Quaternion {
    double w{1}, x{0}, y{0}, z{0};
    Quaternion operator*(const Quaternion& q) const {
        return {w*q.w-x*q.x-y*q.y-z*q.z,
                w*q.x+x*q.w+y*q.z-z*q.y,
                w*q.y-x*q.z+y*q.w+z*q.x,
                w*q.z+x*q.y-y*q.x+z*q.w};
    }
    Quaternion conjugate() const { return {w,-x,-y,-z}; }
    Quaternion normalized_q() const {
        const double n=std::sqrt(w*w+x*x+y*y+z*z);
        return n>1e-12?Quaternion{w/n,x/n,y/n,z/n}:Quaternion{};
    }
};
inline Vec3 rotate(const Quaternion&q,const Vec3&v){
    Quaternion p{0,v.x,v.y,v.z}; auto r=q*p*q.conjugate(); return {r.x,r.y,r.z};
}
inline Quaternion integrate_quaternion(const Quaternion&q,const Vec3&omega,double dt){
    Quaternion wq{0,omega.x,omega.y,omega.z};
    Quaternion qdot=q*wq;
    Quaternion out{q.w+0.5*qdot.w*dt,q.x+0.5*qdot.x*dt,q.y+0.5*qdot.y*dt,q.z+0.5*qdot.z*dt};
    return out.normalized_q();
}
inline double yaw_from_q(const Quaternion&q){
    return std::atan2(2*(q.w*q.z+q.x*q.y),1-2*(q.y*q.y+q.z*q.z));
}
inline std::ostream& operator<<(std::ostream&os,const Vec3&v){return os<<"("<<v.x<<","<<v.y<<","<<v.z<<")";}
}
