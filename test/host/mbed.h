#pragma once
#include "drivers/Timeout.h"
namespace mbed {
struct CriticalSectionLock {};
template<class T> std::function<void()> callback(T* object,void(T::*method)()) {
    return [object,method]() {(object->*method)();};
}
}
