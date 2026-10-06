#include "autonomy/platform/jetson_memory.hpp"
#include "autonomy/platform/threading.hpp"
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
int main(){
    using namespace autonomy::platform;
    std::string err;
    bool pinned=pin_current_thread(0,&err);
    if(!pinned)std::cerr<<"affinity: "<<err<<"\n";
    constexpr std::size_t bytes=1920*1080*3;
    MappedBuffer buf(bytes);
    auto t0=std::chrono::steady_clock::now();
    for(int i=0;i<200;++i)std::memset(buf.host(),i,buf.size());
    auto t1=std::chrono::steady_clock::now();
    double ms=std::chrono::duration<double,std::milli>(t1-t0).count()/200.0;
    std::cout<<"buffer_bytes="<<buf.size()<<" gpu_mapped="<<std::boolalpha<<buf.gpu_mapped()
             <<" avg_touch_ms="<<ms<<" pinned_cpu="<<pinned<<"\n";
}
