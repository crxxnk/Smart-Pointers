#pragma once
#include <type_traits>
#include <atomic>
#include <memory>
#include "is_allocator.h"

namespace iosp { // implementation of smart pointers
    template<typename Ptr>
    class shared_ptr;
}

struct control_block
{
    std::atomic_size_t strong_ref{1};
    std::atomic_size_t weak_ref{0};
    virtual ~control_block() = default;
    virtual auto destroy() -> void = 0;
    control_block() = default;
    control_block(const control_block&) = delete;
    control_block& operator=(const control_block&) = delete;
};

template<typename Ptr, typename Deleter = std::default_delete<Ptr>, typename Allocator = void>
struct ptr_base : public control_block
{
    Ptr* pointer;
    Deleter deleter;
    
    using Alloc = std::conditional_t<std::is_void_v<Allocator>, std::allocator<ptr_base>, Allocator>;
    Alloc allocator;

    ptr_base(Ptr* p, Deleter d, Alloc a = Alloc() /*Default allocator*/) : pointer(p), deleter(std::move(d)), allocator(std::move(a)) {}
    void destroy() override {
        if(pointer) {
            deleter(pointer);
            pointer = nullptr;
        }

        using _Alloc_Traits = std::allocator_traits<Alloc>;
        _Alloc_Traits::destroy(allocator, this);
        _Alloc_Traits::deallocate(allocator, this, 1);
    }
};

template<typename Ptr>
class iosp::shared_ptr
{
    Ptr* pointer;
    control_block* cb;
public:
    // Constructors && Destructor
    shared_ptr() noexcept;
    shared_ptr(std::nullptr_t) noexcept;

    template<typename Y>
    explicit shared_ptr(Y* _Ptr);
    template<typename Y, typename Deleter>
    shared_ptr(Y* _Ptr, Deleter _Dltr);
    template<typename Deleter>
    shared_ptr(std::nullptr_t _Ptr, Deleter _Dltr);
    template<typename Y, typename Deleter, typename Allocator>
    shared_ptr(Y* _Ptr, Deleter _Dltr, Allocator _Alloc); // Custom allocator for the control block
    shared_ptr(const shared_ptr& s) noexcept;
    shared_ptr(shared_ptr&& u) noexcept;
    ~shared_ptr();
    
    // Operators
    auto operator=(shared_ptr&& u) noexcept -> shared_ptr&;
    auto operator=(std::nullptr_t) noexcept -> shared_ptr&;
    auto operator=(const shared_ptr&) -> shared_ptr& = delete;
    _NODISCARD auto operator*() const noexcept -> Ptr&;
    _NODISCARD auto operator->() const noexcept -> Ptr*;
    explicit operator bool() const noexcept;

    // Members
    _NODISCARD auto get() const noexcept -> Ptr*;
    _NODISCARD auto unique() const noexcept -> bool;
    _NODISCARD auto use_count() const noexcept -> std::size_t;
    // _NODISCARD auto get_deleter() const noexcept -> const Deleter&;
    auto release() noexcept -> Ptr*;
    auto reset(Ptr* _Ptr = nullptr) noexcept -> void;
    auto swap(shared_ptr& other) noexcept -> void;
};

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr() noexcept
{
    pointer = nullptr;
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr(std::nullptr_t) noexcept
{
    pointer = nullptr;
}

template <typename Ptr>
template <typename Y>
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr)
{
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    pointer = _Ptr;

    if(_Ptr)
        cb = new ptr_base<Ptr>(_Ptr, std::default_delete<Ptr>{});
    else
        cb = nullptr;
}

template <typename Ptr>
template <typename Y, typename Deleter>
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr, Deleter _Dltr)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    pointer = _Ptr;
    cb = new ptr_base<Ptr, Deleter>(_Ptr, std::move(_Dltr));
}

template <typename Ptr>
template <typename Deleter>
iosp::shared_ptr<Ptr>::shared_ptr(std::nullptr_t _Ptr, Deleter _Dltr)
{
    static_assert(std::is_nothrow_move_constructible_v<Deleter>);
    pointer = nullptr;
    cb = new ptr_base<Ptr, Deleter>(nullptr, std::move(_Dltr));
}

template <typename Ptr>
template <typename Y, typename Deleter, typename Allocator>
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr, Deleter _Dltr, Allocator _Alloc)
{
    static_assert(iosp::is_allocator<Allocator>::value);

    using _CB = ptr_base<Ptr, Deleter>;
    using _Alloc_CB = typename std::allocator_traits<Allocator>::template rebind_alloc<_CB>; // custom allocator is converted to now allocate the control block
    
    _Alloc_CB alloc_cb(_Alloc);
    _CB* mem = alloc_cb.allocate(1); // allocators only allocate raw memory and do not construct an object
    try {
        cb = new (mem) _CB(_Ptr, std::move(_Dltr)); // constructs a control_block object on top of the allocated memory
        pointer = _Ptr;
    } catch(...) {
        alloc_cb.deallocate(mem, 1);
        throw;
    }
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr(const shared_ptr &s) noexcept
{
    pointer = s.pointer;
    cb = s.cb;
    if(cb)
        cb->strong_ref.fetch_add(1);
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::~shared_ptr()
{
    if(cb && cb->strong_ref.fetch_sub(1) == 1) {
        cb->destroy();
    }
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::get() const noexcept -> Ptr*
{
    return pointer;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::unique() const noexcept -> bool
{
    return cb->strong_ref == 1 ? true : false;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::use_count() const noexcept -> std::size_t
{
    return cb ? cb->strong_ref.load() : 0;
}