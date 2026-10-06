cmake_minimum_required(VERSION 3.20)
project(jetson_autonomy_sim LANGUAGES CXX)

option(AUTONOMY_ENABLE_CUDA "Build CUDA preprocessing kernels" OFF)
option(AUTONOMY_ENABLE_GSTREAMER "Build GStreamer camera source" OFF)
option(AUTONOMY_ENABLE_TENSORRT "Build TensorRT runtime wrapper" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(autonomy_core
    src/simulator.cpp
    src/sensors.cpp
    src/estimator.cpp
    src/planner.cpp
    src/controller.cpp
    src/mission.cpp
    src/platform/threading.cpp
    src/platform/jetson_memory.cpp
    src/platform/v4l2_capture.cpp
)
target_include_directories(autonomy_core PUBLIC include)
target_compile_options(autonomy_core PRIVATE -Wall -Wextra -Wpedantic)
target_link_libraries(autonomy_core PUBLIC pthread)

if(AUTONOMY_ENABLE_CUDA)
    enable_language(CUDA)
    find_package(CUDAToolkit REQUIRED)
    target_sources(autonomy_core PRIVATE cuda/image_preprocess.cu)
    target_compile_definitions(autonomy_core PUBLIC AUTONOMY_HAS_CUDA=1)
    target_link_libraries(autonomy_core PUBLIC CUDA::cudart)
endif()

if(AUTONOMY_ENABLE_GSTREAMER)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-app-1.0)
    target_sources(autonomy_core PRIVATE src/platform/gstreamer_camera.cpp)
    target_compile_definitions(autonomy_core PUBLIC AUTONOMY_HAS_GSTREAMER=1)
    target_include_directories(autonomy_core PRIVATE ${GST_INCLUDE_DIRS})
    target_link_directories(autonomy_core PRIVATE ${GST_LIBRARY_DIRS})
    target_link_libraries(autonomy_core PUBLIC ${GST_LIBRARIES})
endif()

if(AUTONOMY_ENABLE_TENSORRT)
    find_path(TENSORRT_INCLUDE_DIR NvInfer.h PATHS /usr/include/aarch64-linux-gnu /usr/include)
    find_library(TENSORRT_LIB nvinfer PATHS /usr/lib/aarch64-linux-gnu /usr/lib)
    if(NOT TENSORRT_INCLUDE_DIR OR NOT TENSORRT_LIB)
        message(FATAL_ERROR "TensorRT requested but NvInfer.h / libnvinfer not found")
    endif()
    target_sources(autonomy_core PRIVATE src/platform/tensorrt_engine.cpp)
    target_compile_definitions(autonomy_core PUBLIC AUTONOMY_HAS_TENSORRT=1)
    target_include_directories(autonomy_core PRIVATE ${TENSORRT_INCLUDE_DIR})
    target_link_libraries(autonomy_core PUBLIC ${TENSORRT_LIB})
endif()

add_executable(autonomy_sim src/main.cpp)
target_link_libraries(autonomy_sim PRIVATE autonomy_core)

add_executable(pipeline_bench src/pipeline_bench.cpp)
target_link_libraries(pipeline_bench PRIVATE autonomy_core)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_executable(v4l2_probe src/v4l2_probe.cpp)
    target_link_libraries(v4l2_probe PRIVATE autonomy_core)
endif()
