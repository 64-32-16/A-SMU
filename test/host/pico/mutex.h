#pragma once
#include <mutex>
struct mutex_t {std::mutex native;};
inline void mutex_init(mutex_t*) {}
inline void mutex_enter_blocking(mutex_t* value) {value->native.lock();}
inline void mutex_exit(mutex_t* value) {value->native.unlock();}
