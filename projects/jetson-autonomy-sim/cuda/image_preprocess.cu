#include <cuda_runtime.h>
#include <cstdint>
extern "C" __global__ void rgb8_to_float_chw(const std::uint8_t* src,float* dst,int width,int height){
    const int i=blockIdx.x*blockDim.x+threadIdx.x; const int pixels=width*height; if(i>=pixels)return;
    dst[i]=src[3*i]/255.0f; dst[pixels+i]=src[3*i+1]/255.0f; dst[2*pixels+i]=src[3*i+2]/255.0f;
}
