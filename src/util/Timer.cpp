/* 
 * File:   Timer.cpp
 * Author: tport
 *
 * Created on April 10, 2014, 11:39 AM
 */

#include "util/Timer.h"

util::Timer::Timer()
: current {0.0},
last{0.0},
oneFrameTime{0.0},
period{0.0},
nFrames{0},
fps{0}
{
    reset();
}

void util::Timer::measureTime() {
    nFrames++;
    current = currentTime();
    period = current - last;
    if (period > 1e9) {
        fps = nFrames;
        oneFrameTime = period / double(nFrames * 1e6);
        last = current;
        nFrames = 0;
    }
}

unsigned int util::Timer::getFPS() {
    return fps;
}

double util::Timer::getFrameTime() {
    return oneFrameTime;
}

void util::Timer::reset() {
    timeStamp = std::chrono::system_clock::now();
}

double util::Timer::currentTime() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now = std::chrono::system_clock::now();
    std::chrono::duration<double> fs = now - timeStamp;
    return (double) std::chrono::duration_cast<std::chrono::nanoseconds>(fs).count();
}

double util::Timer::currentTimeMs() {
    std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> now = std::chrono::system_clock::now();
    std::chrono::duration<double> fs = now - timeStamp;
    return (double) std::chrono::duration_cast<std::chrono::milliseconds>(fs).count();
}
