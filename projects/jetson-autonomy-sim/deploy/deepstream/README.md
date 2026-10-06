# GStreamer / DeepStream path

```text
v4l2src / nvarguscamerasrc
  -> caps with hardware format/timestamps
  -> NVMM / NvBufSurface
  -> CUDA preprocessing or nvvideoconvert
  -> TensorRT (nvinfer or custom C++ runtime)
  -> metadata / detections only
  -> autonomy estimator + planner
```

Do not treat `appsink -> std::vector -> cudaMemcpy` as the final flight path. It is useful for bring-up and debugging but adds copies and cache traffic.

Measure at every boundary: sensor exposure timestamp, kernel dequeue, GPU enqueue, inference complete, autonomy publish, flight-controller receive.
