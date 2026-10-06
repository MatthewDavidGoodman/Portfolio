#pragma once
#include <cstddef>
#include <cstdint>
namespace autonomy::platform {
class MappedBuffer {
public:
    explicit MappedBuffer(std::size_t bytes);
    ~MappedBuffer();
    MappedBuffer(const MappedBuffer&)=delete; MappedBuffer& operator=(const MappedBuffer&)=delete;
    std::byte* host() { return host_; }
    void* device() { return device_; }
    std::size_t size() const { return bytes_; }
    bool gpu_mapped() const { return gpu_mapped_; }
private:
    std::size_t bytes_{}; std::byte* host_{}; void* device_{}; bool gpu_mapped_{false};
};
}
