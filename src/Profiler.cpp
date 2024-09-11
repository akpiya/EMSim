#include <chrono>


class Profiler {
public:
    int entries;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::microseconds totalTimes;

    Profiler() : entries(0), totalTimes(0) {}

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    void stop() {
        auto stop = std::chrono::high_resolution_clock::now();
        auto dur = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_time);
        totalTimes += std::chrono::duration_cast<std::chrono::microseconds>(dur);
        entries++;
    }

    std::chrono::microseconds getAverageDuration() {
        return totalTimes / entries;
    }

};
