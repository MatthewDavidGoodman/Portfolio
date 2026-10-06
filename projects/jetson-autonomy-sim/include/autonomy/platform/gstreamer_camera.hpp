#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
namespace autonomy::platform {
struct CameraFrame { std::uint64_t sequence{}; std::uint64_t timestamp_ns{}; int width{}; int height{}; std::vector<std::uint8_t> bytes; };
class GStreamerCamera {
public:
    explicit GStreamerCamera(std::string pipeline);
    ~GStreamerCamera();
    bool open();
    bool read(CameraFrame& frame,int timeout_ms=100);
private: struct Impl; std::unique_ptr<Impl> impl_;
};
}
