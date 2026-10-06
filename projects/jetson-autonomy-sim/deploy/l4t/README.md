# L4T deployment notes

1. Flash the exact Jetson Linux / JetPack release qualified for the target carrier board.
2. Keep kernel, OOT camera modules, DTB/overlays, CUDA and TensorRT as a tested versioned set.
3. Build sensor drivers as loadable kernel modules where supported by the installed Jetson Linux release.
4. Validate camera registration with `media-ctl`, `v4l2-ctl`, and a minimal capture pipeline before adding inference.
5. Record boot, sensor-link, frame timestamp, GPU, thermal and watchdog telemetry.
6. Deploy the flight runtime using the included systemd pattern after replacing paths and user/group.

Bootloader/device-tree changes can make a target unbootable. Keep a recovery path and a known-good image.
