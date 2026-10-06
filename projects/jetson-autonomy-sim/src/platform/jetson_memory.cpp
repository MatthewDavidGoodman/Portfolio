#include "autonomy/platform/jetson_memory.hpp"
#include <new>
#ifdef AUTONOMY_HAS_CUDA
#include <cuda_runtime.h>
#endif
namespace autonomy::platform {
MappedBuffer::MappedBuffer(std::size_t bytes):bytes_(bytes){
#ifdef AUTONOMY_HAS_CUDA
    void* p=nullptr;
    if(cudaHostAlloc(&p,bytes_,cudaHostAllocMapped)==cudaSuccess){
        host_=static_cast<std::byte*>(p); void* d=nullptr;
        if(cudaHostGetDevicePointer(&d,p,0)==cudaSuccess){device_=d;gpu_mapped_=true;return;}
        cudaFreeHost(p); host_=nullptr;
    }
#endif
    host_=new std::byte[bytes_]; device_=nullptr; gpu_mapped_=false;
}
MappedBuffer::~MappedBuffer(){
#ifdef AUTONOMY_HAS_CUDA
    if(gpu_mapped_){cudaFreeHost(host_);return;}
#endif
    delete[] host_;
}
}
