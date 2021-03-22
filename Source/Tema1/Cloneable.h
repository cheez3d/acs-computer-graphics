#pragma once

#include <memory>

template<class T>
class Cloneable {
public:
    virtual ~Cloneable() = default;

protected:
    virtual Cloneable* CloneImplementation() const = 0;

public:
    std::unique_ptr<T> Clone() const {
        return std::unique_ptr<T> { static_cast<T*>(CloneImplementation()) };
    }
};
