#include "autonomy/platform/tensorrt_engine.hpp"
#ifdef AUTONOMY_HAS_TENSORRT
#include <NvInfer.h>
#include <fstream>
#include <vector>
namespace autonomy::platform {
class Logger final: public nvinfer1::ILogger { void log(Severity s,const char*) noexcept override { if(s>Severity::kWARNING)return; } };
struct TensorRTEngine::Impl { Logger logger; std::unique_ptr<nvinfer1::IRuntime> runtime; std::unique_ptr<nvinfer1::ICudaEngine> engine; };
TensorRTEngine::TensorRTEngine(const std::string&path):impl_(std::make_unique<Impl>()){
 std::ifstream in(path,std::ios::binary);if(!in)return;std::vector<char>d((std::istreambuf_iterator<char>(in)),{});impl_->runtime.reset(nvinfer1::createInferRuntime(impl_->logger));if(impl_->runtime)impl_->engine.reset(impl_->runtime->deserializeCudaEngine(d.data(),d.size()));}
TensorRTEngine::~TensorRTEngine()=default; bool TensorRTEngine::valid()const{return impl_&&impl_->engine!=nullptr;}
}
#endif
