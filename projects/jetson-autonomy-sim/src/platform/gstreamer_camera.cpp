#include "autonomy/platform/gstreamer_camera.hpp"
#ifdef AUTONOMY_HAS_GSTREAMER
#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <chrono>
namespace autonomy::platform {
struct GStreamerCamera::Impl { std::string pipeline; GstElement* pipe{}; GstElement* sink{}; std::uint64_t seq{}; explicit Impl(std::string p):pipeline(std::move(p)){} };
GStreamerCamera::GStreamerCamera(std::string p):impl_(std::make_unique<Impl>(std::move(p))){gst_init(nullptr,nullptr);}
GStreamerCamera::~GStreamerCamera(){if(impl_->pipe){gst_element_set_state(impl_->pipe,GST_STATE_NULL);gst_object_unref(impl_->pipe);}}
bool GStreamerCamera::open(){GError*err=nullptr;impl_->pipe=gst_parse_launch(impl_->pipeline.c_str(),&err);if(!impl_->pipe){if(err)g_error_free(err);return false;}impl_->sink=gst_bin_get_by_name(GST_BIN(impl_->pipe),"appsink");if(!impl_->sink)return false;gst_app_sink_set_drop(GST_APP_SINK(impl_->sink),true);gst_app_sink_set_max_buffers(GST_APP_SINK(impl_->sink),1);return gst_element_set_state(impl_->pipe,GST_STATE_PLAYING)!=GST_STATE_CHANGE_FAILURE;}
bool GStreamerCamera::read(CameraFrame&f,int timeout_ms){auto*s=gst_app_sink_try_pull_sample(GST_APP_SINK(impl_->sink),timeout_ms*GST_MSECOND);if(!s)return false;auto*b=gst_sample_get_buffer(s);GstMapInfo m{};if(!gst_buffer_map(b,&m,GST_MAP_READ)){gst_sample_unref(s);return false;}f.sequence=impl_->seq++;f.timestamp_ns=std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();f.bytes.assign(m.data,m.data+m.size);gst_buffer_unmap(b,&m);gst_sample_unref(s);return true;}
}
#endif
