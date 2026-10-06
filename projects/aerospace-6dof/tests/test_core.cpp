#include "aero/rigid_body.hpp"
#include <iostream>
#include <stdexcept>
int main() {
    try {
        aero::Environment environment;
        environment.gravity_enabled = false;
        environment.rotation_enabled = false;
        aero::State initial;
        initial.velocity_ecef_mps = aero::Vec3(1,2,3);
        aero::RigidBody vehicle(2, aero::Mat3::Identity(), initial, environment);
        aero::Wrench command;
        command.force_body_n = aero::Vec3(2,0,0);
        for (int i=0; i<100; ++i) vehicle.step(0.01, command);
        if ((vehicle.state().position_ecef_m - aero::Vec3(1.5,2,3)).norm() > 1e-12)
            throw std::runtime_error("Constant-force propagation failed");
        const auto snapshot = vehicle.state();
        try { vehicle.step(-1); throw std::runtime_error("Invalid dt accepted"); }
        catch (const std::invalid_argument&) {}
        if ((vehicle.state().position_ecef_m-snapshot.position_ecef_m).norm() != 0)
            throw std::runtime_error("Failed step modified state");
        std::cout << "C++ core checks passed\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
