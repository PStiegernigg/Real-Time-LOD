/* 
 * File:   Timer.h
 * Author: tport
 *
 * Created on March 19, 2014, 11:49 AM
 */

#pragma once

#include <chrono>

namespace util {

class Timer {
public:
    Timer();

    void measureTime();
    unsigned int getFPS();
    double getFrameTime();
    double currentTime();
    double currentTimeMs();
    
private:
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> timeStamp;
    double current, last, oneFrameTime, period;
    unsigned int nFrames, fps;
    void reset();
};

} // namespace util
