#include "autonomy/platform/threading.hpp"
#include <cerrno>
#include <cstring>
#include <pthread.h>
#include <sched.h>
namespace autonomy::platform {
bool pin_current_thread(int cpu,std::string*err){cpu_set_t set;CPU_ZERO(&set);CPU_SET(cpu,&set);int rc=pthread_setaffinity_np(pthread_self(),sizeof(set),&set);if(rc&&err)*err=std::strerror(rc);return rc==0;}
bool set_current_thread_fifo(int priority,std::string*err){sched_param p{};p.sched_priority=priority;int rc=pthread_setschedparam(pthread_self(),SCHED_FIFO,&p);if(rc&&err)*err=std::strerror(rc);return rc==0;}
}
