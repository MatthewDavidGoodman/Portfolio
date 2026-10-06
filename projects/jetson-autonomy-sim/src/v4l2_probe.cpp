#ifdef __linux__
#include "autonomy/platform/v4l2_capture.hpp"
#include <linux/videodev2.h>
#include <iostream>
int main(int argc,char**argv){
    std::string dev=argc>1?argv[1]:"/dev/video0";
    autonomy::platform::V4L2Capture cap(dev,1280,720,V4L2_PIX_FMT_YUYV);
    std::string err;
    if(!cap.open_stream(&err)){std::cerr<<"open_stream failed: "<<err<<"\n";return 2;}
    for(int i=0;i<10;++i){
        autonomy::platform::V4L2Frame f;
        if(!cap.dequeue(f,1000,&err)){std::cerr<<"dequeue: "<<err<<"\n";return 3;}
        std::cout<<"seq="<<f.sequence<<" bytes="<<f.bytes_used<<" dmabuf_fd="<<f.dmabuf_fd
                 <<" timestamp_us="<<f.timestamp_us<<"\n";
        if(!cap.requeue(f.index,&err)){std::cerr<<"requeue: "<<err<<"\n";return 4;}
    }
}
#else
int main(){return 0;}
#endif
