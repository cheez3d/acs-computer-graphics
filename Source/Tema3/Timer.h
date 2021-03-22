#pragma once

#include "Watch.h"

template<typename T>
class Timer {
    static inline auto GetWatchElapsed = [](Watch<T>* watch) { return watch ? watch->GetElapsed() : 0; };

    T lastElapsed;

    T duration;
    Watch<T>* watch;

public:
    Timer(T duration = 0, bool elapsed = false, Watch<T>* watch = nullptr) {
        lastElapsed = 0;

        SetDuration(duration);
        SetWatch(watch);
        SetElapsed(elapsed);
    }

    T GetDuration() const {
        return duration;
    }

    void SetDuration(T duration) {
        this->duration = duration;
    }

    Watch<T>* GetWatch() const {
        return watch;
    }

    void SetWatch(Watch<T>* watch) {
        lastElapsed += GetWatchElapsed(watch) - GetWatchElapsed(this->watch);

        this->watch = watch;
    }

    bool IsElapsed() const {
        return GetElapsed() >= duration;
    }

    T GetElapsed() const {
        return GetWatchElapsed(watch) - lastElapsed;
    }

    void SetElapsed(bool elapsed) {
        lastElapsed = GetWatchElapsed(watch) - (elapsed ? duration : 0);
    }
};
