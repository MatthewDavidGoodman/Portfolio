#pragma once
#include <memory>
#include <string>
namespace autonomy::platform {
class TensorRTEngine {
public:
    explicit TensorRTEngine(const std::string& engine_path);
    ~TensorRTEngine();
    bool valid() const;
private: struct Impl; std::unique_ptr<Impl> impl_;
};
}
