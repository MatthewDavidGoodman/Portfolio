#pragma once
#include <string>
namespace autonomy::platform {
bool pin_current_thread(int cpu, std::string* error=nullptr);
bool set_current_thread_fifo(int priority, std::string* error=nullptr);
}
