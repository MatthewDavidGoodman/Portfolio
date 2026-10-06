cmake_minimum_required(VERSION 3.20)
project(aerospace6dof_portfolio VERSION 0.2.0 LANGUAGES CXX)
include(CTest)

find_package(Eigen3 3.4 REQUIRED NO_MODULE)

add_library(aero_core STATIC
    src/environment.cpp
    src/rigid_body.cpp
    src/gnc.cpp
    src/simulation.cpp
    src/astrodynamics.cpp
    src/aircraft.cpp
)
target_compile_features(aero_core PUBLIC cxx_std_17)
target_include_directories(aero_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(aero_core PUBLIC Eigen3::Eigen)
set_target_properties(aero_core PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(aero_core PRIVATE /W4)
else()
  target_compile_options(aero_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(BUILD_TESTING)
  add_executable(test_core tests/test_core.cpp)
  target_link_libraries(test_core PRIVATE aero_core)
  add_test(NAME core COMMAND test_core)
endif()
