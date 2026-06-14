#pragma once
#include "shared_ptr.hpp"

template<typename Ptr>
class iosp::weak_ptr
{
    Ptr* pointer;
    control_block* cb;

public:
    constexpr weak_ptr() noexcept;
   
    weak_ptr(const weak_ptr& r) noexcept;

    template<typename Y>
    weak_ptr(const weak_ptr<Y>& r) noexcept;
    
    template<typename Y>
    weak_ptr(const iosp::shared_ptr<Y>& r) noexcept;

    weak_ptr(weak_ptr&& r) noexcept;

    template<typename Y>
    weak_ptr(weak_ptr<Y>&& r) noexcept;

    _NODISCARD auto use_count() -> long;
    _NODISCARD auto expired() -> bool;

    _NODISCARD auto lock() -> iosp::shared_ptr<Ptr>;

    template <typename Y>
    _NODISCARD auto owner_before(const weak_ptr<Y>& other) -> bool;


    template <typename Y>
    _NODISCARD auto owner_before(const iosp::shared_ptr<Y>& other) -> bool;
};

template <typename Ptr>
constexpr iosp::weak_ptr<Ptr>::weak_ptr() noexcept {
    pointer = nullptr;
    cb = nullptr;
}

template <typename Ptr>
template <typename Y>
iosp::weak_ptr<Ptr>::weak_ptr(const weak_ptr<Y>& r) noexcept {
    pointer = r.pointer;
    cb = r.cb;
}

template <typename Ptr>
template <typename Y>
iosp::weak_ptr<Ptr>::weak_ptr(const iosp::shared_ptr<Y>& r) noexcept {
    pointer = r.pointer;
    cb = r.cb;
    if (cb)
        cb->weak_ref.fetch_add(1);
}

template <typename Ptr>
_NODISCARD auto iosp::weak_ptr<Ptr>::use_count() -> long {
    return cb->strong_ref;
}

template <typename Ptr>
_NODISCARD auto iosp::weak_ptr<Ptr>::expired() -> bool {
    return !cb || cb->strong_ref == 0; 
}

template <typename Ptr>
auto iosp::weak_ptr<Ptr>::lock() -> iosp::shared_ptr<Ptr> {
    return expired() ? iosp::shared_ptr<Ptr>() : iosp::shared_ptr<Ptr>(*this);
}

template <typename Ptr>
template <typename Y>
_NODISCARD auto iosp::weak_ptr<Ptr>::owner_before(const iosp::shared_ptr<Y>& other) -> bool {
    return cb < other.cb;
}

template <typename Ptr>
template <typename Y>
_NODISCARD auto iosp::weak_ptr<Ptr>::owner_before(const weak_ptr<Y>& other) -> bool {
    return cb < other.cb;
}
