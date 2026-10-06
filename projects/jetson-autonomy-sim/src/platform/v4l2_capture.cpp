#ifdef __linux__
#include "autonomy/platform/v4l2_capture.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
namespace autonomy::platform {
namespace {
int xioctl(int fd, unsigned long req, void* arg){ int r; do { r=::ioctl(fd,req,arg); } while(r<0&&errno==EINTR); return r; }
void seterr(std::string* e,const char* what){ if(e)*e=std::string(what)+": "+std::strerror(errno); }
}
V4L2Capture::V4L2Capture(std::string d,std::uint32_t w,std::uint32_t h,std::uint32_t f):device_(std::move(d)),width_(w),height_(h),fourcc_(f){}
V4L2Capture::~V4L2Capture(){close_stream();}
bool V4L2Capture::open_stream(std::string* error){
    fd_=::open(device_.c_str(),O_RDWR|O_NONBLOCK|O_CLOEXEC); if(fd_<0){seterr(error,"open");return false;}
    v4l2_capability cap{}; if(xioctl(fd_,VIDIOC_QUERYCAP,&cap)<0){seterr(error,"VIDIOC_QUERYCAP");close_stream();return false;}
    if(!(cap.device_caps&V4L2_CAP_VIDEO_CAPTURE)||!(cap.device_caps&V4L2_CAP_STREAMING)){if(error)*error="device lacks V4L2 capture/streaming capability";close_stream();return false;}
    v4l2_format fmt{};fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;fmt.fmt.pix.width=width_;fmt.fmt.pix.height=height_;fmt.fmt.pix.pixelformat=fourcc_;fmt.fmt.pix.field=V4L2_FIELD_ANY;
    if(xioctl(fd_,VIDIOC_S_FMT,&fmt)<0){seterr(error,"VIDIOC_S_FMT");close_stream();return false;}
    width_=fmt.fmt.pix.width;height_=fmt.fmt.pix.height;fourcc_=fmt.fmt.pix.pixelformat;
    v4l2_requestbuffers req{};req.count=4;req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;req.memory=V4L2_MEMORY_MMAP;
    if(xioctl(fd_,VIDIOC_REQBUFS,&req)<0||req.count<2){seterr(error,"VIDIOC_REQBUFS");close_stream();return false;}
    buffers_.resize(req.count);
    for(std::uint32_t i=0;i<req.count;++i){
        v4l2_buffer b{};b.type=req.type;b.memory=req.memory;b.index=i;
        if(xioctl(fd_,VIDIOC_QUERYBUF,&b)<0){seterr(error,"VIDIOC_QUERYBUF");close_stream();return false;}
        auto ptr=::mmap(nullptr,b.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd_,b.m.offset);if(ptr==MAP_FAILED){seterr(error,"mmap");close_stream();return false;}
        buffers_[i].ptr=ptr;buffers_[i].length=b.length;
        v4l2_exportbuffer exp{};exp.type=req.type;exp.index=i;exp.flags=O_CLOEXEC;
        if(xioctl(fd_,VIDIOC_EXPBUF,&exp)==0) buffers_[i].dmabuf_fd=exp.fd;
        if(xioctl(fd_,VIDIOC_QBUF,&b)<0){seterr(error,"VIDIOC_QBUF");close_stream();return false;}
    }
    auto type=static_cast<v4l2_buf_type>(V4L2_BUF_TYPE_VIDEO_CAPTURE);if(xioctl(fd_,VIDIOC_STREAMON,&type)<0){seterr(error,"VIDIOC_STREAMON");close_stream();return false;}streaming_=true;return true;
}
bool V4L2Capture::dequeue(V4L2Frame& frame,int timeout_ms,std::string* error){
    pollfd pfd{fd_,POLLIN,0};int pr;do{pr=::poll(&pfd,1,timeout_ms);}while(pr<0&&errno==EINTR);if(pr<=0){if(pr<0)seterr(error,"poll");return false;}
    v4l2_buffer b{};b.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;b.memory=V4L2_MEMORY_MMAP;
    if(xioctl(fd_,VIDIOC_DQBUF,&b)<0){seterr(error,"VIDIOC_DQBUF");return false;}
    const auto& bb=buffers_.at(b.index);frame={b.index,b.sequence,static_cast<std::uint64_t>(b.timestamp.tv_sec)*1000000ull+static_cast<std::uint64_t>(b.timestamp.tv_usec),b.bytesused,bb.dmabuf_fd,bb.ptr};return true;
}
bool V4L2Capture::requeue(std::uint32_t index,std::string* error){v4l2_buffer b{};b.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;b.memory=V4L2_MEMORY_MMAP;b.index=index;if(xioctl(fd_,VIDIOC_QBUF,&b)<0){seterr(error,"VIDIOC_QBUF");return false;}return true;}
void V4L2Capture::close_stream(){
    if(fd_<0) return;
    if(streaming_){ auto type=static_cast<v4l2_buf_type>(V4L2_BUF_TYPE_VIDEO_CAPTURE); xioctl(fd_,VIDIOC_STREAMOFF,&type); streaming_=false; }
    for(auto&b:buffers_){if(b.ptr&&b.ptr!=MAP_FAILED)::munmap(b.ptr,b.length);if(b.dmabuf_fd>=0)::close(b.dmabuf_fd);}buffers_.clear();::close(fd_);fd_=-1;
}
}
#endif
