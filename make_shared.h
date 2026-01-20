#pragma once
#include "shared_ptr.h"

namespace iosp {
    template<typename T, typename... Args>
    _NODISCARD auto make_shared(Args&&... args) -> iosp::shared_ptr<T>;

    template<typename T>
    _NODISCARD auto make_shared(size_t size) -> iosp::shared_ptr<T>;
}

template<typename T, typename... Args>
_NODISCARD auto iosp::make_shared(Args&&... args) -> iosp::shared_ptr<T>
{
    return iosp::shared_ptr<T>(new T(std::forward<Args>(args)...), std::default_delete<T>{});
}