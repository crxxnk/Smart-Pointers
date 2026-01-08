#pragma once
#include "unique_ptr.h"

namespace iosp {
    template<typename T, typename... Args>
    _NODISCARD auto make_unique(Args&&... args) -> iosp::unique_ptr<T>;

    template<typename T>
    _NODISCARD auto make_unique(size_t size) -> iosp::unique_ptr<T>;
}

template<typename T, typename... Args>
_NODISCARD auto iosp::make_unique(Args&&... args) -> iosp::unique_ptr<T>
{
    return iosp::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
_NODISCARD auto iosp::make_unique(size_t size) -> iosp::unique_ptr<T>
{
    
}