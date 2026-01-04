#pragma once
#include "unique_ptr.h"

template<typename T, typename... Args>
_NODISCARD auto make_unique(Args&&... args) -> my_unique_ptr<T>
{
    return my_unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template<typename T>
_NODISCARD auto make_unique(size_t size) -> my_unique_ptr<T>
{
    
}