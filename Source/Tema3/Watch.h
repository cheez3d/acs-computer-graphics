#pragma once

#include <chrono>

template<typename T>
class Watch {
    std::chrono::time_point<std::chrono::steady_clock> t;
    std::chrono::time_point<std::chrono::steady_clock> ref;

public:
    Watch() {
        using namespace std::chrono;

        t = steady_clock::now();
        ref = t;
    }

    T GetElapsed() const {
        using namespace std::chrono;

        return duration_cast<duration<T>>(t - ref).count();
    }

    void AddElapsed(T dt) {
        using namespace std::chrono;

        t += duration_cast<steady_clock::duration>(duration<T>(dt));
    }
};
