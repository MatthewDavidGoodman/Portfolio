#pragma once
#ifdef __linux__
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace autonomy::platform {
struct V4L2Frame {
    std::uint32_t index{};
    std::uint32_t sequence{};
    std::uint64_t timestamp_us{};
    std::size_t bytes_used{};
    int dmabuf_fd{-1};
    void* cpu_ptr{nullptr};
};
class V4L2Capture {
public:
    V4L2Capture(std::string device, std::uint32_t width, std::uint32_t height, std::uint32_t fourcc);
    ~V4L2Capture();
    V4L2Capture(const V4L2Capture&)=delete;
    V4L2Capture& operator=(const V4L2Capture&)=delete;
    bool open_stream(std::string* error=nullptr);
    bool dequeue(V4L2Frame& frame, int timeout_ms=100, std::string* error=nullptr);
    bool requeue(std::uint32_t index, std::string* error=nullptr);
    void close_stream();
private:
    struct Buffer { void* ptr{}; std::size_t length{}; int dmabuf_fd{-1}; };
    std::string device_; std::uint32_t width_,height_,fourcc_; int fd_{-1}; bool streaming_{false}; std::vector<Buffer> buffers_;
};
}
#endif
