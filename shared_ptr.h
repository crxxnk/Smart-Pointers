#pragma once
#include <type_traits>
#include <atomic>
#include <memory>

namespace iosp { // implementation of smart pointers
    template<typename Ptr>
    class shared_ptr;
}

struct ref_count_base
{
    std::atomic_size_t strong_ref{1};
    std::atomic_size_t weak_ref{0};
    virtual ~ref_count_base() = default;
    virtual auto destroy() -> void = 0;
    ref_count_base() = default;
    ref_count_base(const ref_count_base&) = delete;
    ref_count_base& operator=(const ref_count_base&) = delete;
};

template<typename Ptr, typename Deleter = std::default_delete<Ptr>>
struct ptr_base : public ref_count_base
{
    Ptr* pointer;
    Deleter deleter;

    ptr_base(Ptr* p, Deleter d) : pointer(p), deleter(std::move(d)) {}
    void destroy() override { if(pointer) deleter(pointer); };
};

template<typename Ptr>
class iosp::shared_ptr
{
    Ptr* pointer;
    ref_count_base* rc;
public:
    // Constructors && Destructor
    shared_ptr() noexcept;
    shared_ptr(std::nullptr_t) noexcept;
    template<typename Y>
    explicit shared_ptr(Y* _Ptr) noexcept;
    template<typename Y, typename Deleter>
    shared_ptr(Y* _Ptr, Deleter _Dltr) noexcept;
    // shared_ptr(Ptr* _Ptr, Deleter&& _Dltr) noexcept;
    shared_ptr( const shared_ptr& s) noexcept;
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
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr) noexcept
{
    static_assert(std::is_convertible_v<Y*, Ptr*>, "Pointer type must be convertible to Ptr*");
    pointer = _Ptr;

    if(_Ptr)
        rc = new ptr_base<Ptr>(_Ptr, std::default_delete<Ptr>{});
    else
        rc = nullptr;
}

template <typename Ptr>
template <typename Y, typename Deleter>
iosp::shared_ptr<Ptr>::shared_ptr(Y *_Ptr, Deleter _Dltr) noexcept
{
    static_assert(std::is_nothrow_copy_constructible_v<Deleter>);
    pointer = _Ptr;

    if(_Ptr)
        rc = new ptr_base<Ptr, Deleter>(_Ptr, _Dltr);
    else
        rc = nullptr;
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::shared_ptr(const shared_ptr &s) noexcept
{
    pointer = s.pointer;
    rc = s.rc;
    if(rc)
        rc->strong_ref.fetch_add(1);
}

template <typename Ptr>
iosp::shared_ptr<Ptr>::~shared_ptr()
{
    if(rc && rc->strong_ref.fetch_sub(1) == 1) {
        rc->destroy();
        delete rc;
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
    return rc->strong_ref == 1 ? true : false;
}

template <typename Ptr>
auto iosp::shared_ptr<Ptr>::use_count() const noexcept -> std::size_t
{
    return rc ? rc->strong_ref.load() : 0;
}