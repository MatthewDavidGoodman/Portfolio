#!/usr/bin/env bash
set -u
echo "== release =="; cat /etc/nv_tegra_release 2>/dev/null || true
echo "== kernel =="; uname -a
echo "== CUDA =="; nvcc --version 2>/dev/null | tail -4 || true
echo "== TensorRT =="; dpkg-query -W 'libnvinfer*' 2>/dev/null | head || true
echo "== cameras =="; v4l2-ctl --list-devices 2>/dev/null || true
echo "== media =="; media-ctl -p 2>/dev/null | head -120 || true
echo "== GStreamer NVIDIA elements =="; gst-inspect-1.0 2>/dev/null | grep -E 'nvargus|nvv4l2|nvinfer' | head -40 || true
echo "== power mode =="; nvpmodel -q 2>/dev/null || true
echo "== clocks =="; jetson_clocks --show 2>/dev/null || true
