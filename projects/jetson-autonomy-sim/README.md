# Jetson Autonomy Simulator

A C++ project designed as a bridge between a **desktop 6-DoF autonomy simulation** and an **NVIDIA Jetson/Tegra mission-computer deployment**.

## Architecture

```text
 simulated / real sensors
          |
          v
 +-------------------+      Jetson path
 | capture + timing  | --> V4L2 / GStreamer / NVMM / DMA-BUF
 +-------------------+
          |
          v
 +-------------------+ --> CUDA preprocessing --> TensorRT
 | perception hook   |
 +-------------------+
          |
          v
 +-------------------+
 | estimator         | IMU + absolute position today; replace with EKF/VIO
 +-------------------+
          |
          v
 +-------------------+
 | waypoint planner  |
 +-------------------+
          |
          v
 +-------------------+
 | controller        |
 +-------------------+
          |
          v
 +-------------------+
 | 6-DoF rigid body  | desktop plant; replace output with flight-control IPC on target
 +-------------------+
```

The flight-control computer boundary is deliberate. The Jetson owns high-bandwidth sensing, perception, localization, planning, health/state management, and inference. A lower-level flight controller can continue to own deterministic actuator I/O and inner-loop safety.

## Build now (x86 or Jetson CPU-only)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/autonomy_sim 20
./build/pipeline_bench
```

## Jetson build

Start from the JetPack/L4T version installed on the target and use the CUDA/TensorRT versions shipped for that release. Do **not** independently upgrade TensorRT/CUDA past the JetPack compatibility matrix.

```bash
cmake -S . -B build-jetson \
  -DCMAKE_BUILD_TYPE=Release \
  -DAUTONOMY_ENABLE_CUDA=ON \
  -DAUTONOMY_ENABLE_GSTREAMER=ON \
  -DAUTONOMY_ENABLE_TENSORRT=ON
cmake --build build-jetson -j$(nproc)
```

## Real Jetson camera pipeline progression

1. **USB / test video**: `v4l2src` or file source -> appsink. Learn timestamps and backpressure.
2. **CSI camera**: sensor driver + device-tree overlay -> V4L2/Argus.
3. **GMSL/FPD-Link**: serializer/deserializer + sensor driver + DT graph + sync.
4. **Avoid copies**: preserve `video/x-raw(memory:NVMM)` as long as possible. Import DMA-BUF/NvBufSurface into CUDA rather than copying to pageable CPU memory.
5. **Inference**: CUDA preprocessing -> TensorRT FP16/INT8 -> publish compact detections/tracks to the autonomy runtime.

A development appsink that copies into `std::vector` is included because it is easy to inspect. It is *not* the final zero-copy flight path.

## Threading and scheduling

Recommended target decomposition:

```text
CPU 0: OS / IRQ housekeeping
CPU 1: camera ingest + timestamping
CPU 2: estimator / sensor fusion
CPU 3: planning + mission state
CPU 4: control / vehicle-interface publisher
GPU : image preprocessing + DNN inference
DLA : optional supported inference engines
```

Use CPU affinity first. Add `SCHED_FIFO` only to genuinely bounded, measured threads.

## Memory model

`MappedBuffer` uses CUDA mapped pinned memory when CUDA is enabled and falls back to normal host memory elsewhere. The desired camera path on Jetson is generally:

```text
CSI/PCIe device -> DMA -> kernel buffer / DMA-BUF -> NVMM/NvBufSurface -> CUDA/TensorRT
```

## Embedded Linux / BSP directory

`deploy/` contains hardware-specific scaffolding for device-tree, kernel, L4T, Yocto, and DeepStream deployment.

## Status

The desktop CPU build has been validated. The Jetson-specific CUDA/TensorRT/zero-copy path is scaffolded and intentionally separated from the portable core.
