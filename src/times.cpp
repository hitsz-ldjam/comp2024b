#include "times.h"
#include <cassert>
#include <chrono>

namespace cho    = std::chrono;
using sclock      = cho::steady_clock;
using time_point = cho::time_point<sclock>;
using duration   = time_point::duration;

float duration_to_sec(const duration& dur) {
    return float(dur.count() * duration::period::num) / duration::period::den;
}

struct TimeImpl {
    time_point init_time;
    time_point last_tick;
    float delta_sec = 0;
};

static TimeImpl* s_time_impl = nullptr;

bool Time::init() {
    assert(s_time_impl == nullptr && "time is initialized twice");
    s_time_impl            = new TimeImpl();
    s_time_impl->init_time = sclock::now();
    return true;
}

void Time::quit() {
    delete s_time_impl;
    s_time_impl = nullptr;
}

void Time::tick() {
    time_point curr        = sclock::now();
    s_time_impl->delta_sec = duration_to_sec(curr - s_time_impl->last_tick);
    s_time_impl->last_tick = curr;
}

float Time::delta() {
    return s_time_impl->delta_sec;
}

float Time::real() {
    return duration_to_sec(sclock::now() - s_time_impl->init_time);
}
